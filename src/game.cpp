/*
 * Copyright (C) 2017-2019  Christopher J. Howard
 *
 * This file is part of Antkeeper Source Code.
 *
 * Antkeeper Source Code is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Antkeeper Source Code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Antkeeper Source Code.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "game.hpp"
#include "resources/string-table.hpp"
#include "states/game-state.hpp"
#include "states/sandbox-state.hpp"
#include "filesystem.hpp"
#include "timestamp.hpp"
#include "ui/ui.hpp"
#include "graphics/ui-render-pass.hpp"
#include "graphics/shadow-map-render-pass.hpp"
#include "graphics/clear-render-pass.hpp"
#include "graphics/sky-render-pass.hpp"
#include "graphics/lighting-render-pass.hpp"
#include "graphics/silhouette-render-pass.hpp"
#include "graphics/final-render-pass.hpp"
#include "resources/resource-manager.hpp"
#include "resources/text-file.hpp"
#include "game/camera-rig.hpp"
#include "game/lens.hpp"
#include "game/forceps.hpp"
#include "game/brush.hpp"
#include "entity/component-manager.hpp"
#include "entity/components/transform-component.hpp"
#include "entity/components/model-component.hpp"
#include "entity/components/terrain-patch-component.hpp"
#include "entity/entity-manager.hpp"
#include "entity/entity-template.hpp"
#include "entity/system-manager.hpp"
#include "entity/systems/sound-system.hpp"
#include "entity/systems/collision-system.hpp"
#include "entity/systems/render-system.hpp"
#include "entity/systems/constraint-system.hpp"
#include "entity/systems/tool-system.hpp"
#include "entity/systems/locomotion-system.hpp"
#include "entity/systems/behavior-system.hpp"
#include "entity/systems/steering-system.hpp"
#include "entity/systems/particle-system.hpp"
#include "entity/systems/terrain-system.hpp"
#include "configuration.hpp"
#include "stb/stb_image_write.h"
#include "menu.hpp"
#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <thread>
#include "debug/command-interpreter.hpp"
#include "debug/logger.hpp"
#include "debug/ansi-escape-codes.hpp"

Game::Game(int argc, char* argv[]):
	currentState(nullptr),
	window(nullptr)
{
	// Determine application name
	std::string applicationName;
	#if defined(_WIN32)
		applicationName = "Antkeeper";
	#else
		applicationName = "antkeeper";
	#endif
	
	// Form resource paths
	dataPath = getDataPath(applicationName) + "data/";
	configPath = getConfigPath(applicationName);
	controlsPath = configPath + "controls/";

	// Create nonexistent config directories
	std::vector<std::string> configPaths;
	configPaths.push_back(configPath);
	configPaths.push_back(controlsPath);
	for (const std::string& path: configPaths)
	{
		if (!pathExists(path))
		{
			createDirectory(path);
		}
	}
	
	// Setup resource manager
	resourceManager = new ResourceManager();

	// Include resource search paths in order of priority
	resourceManager->include(controlsPath);
	resourceManager->include(configPath);
	resourceManager->include(dataPath);

	// Subscribe the game to scheduled function events
	eventDispatcher.subscribe<ScheduledFunctionEvent>(this);
	toggleFullscreenDisabled = false;

	sandboxState = new SandboxState(this);

	float3x3 m1;
	float3x3 m2;
	float3x3 m3 = m1 + m2;
	float3x3 m4 = m2 - m3;

	float4x4 projection = perspective(radians(45.0f), 3.0f / 4.0f, -1.0f, 1.0f);
	float4x4 ortho_proj = ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
}

Game::~Game()
{
	if (window)
	{
		windowManager->destroyWindow(window);
	}
}

void Game::changeState(GameState* state)
{
	if (currentState != nullptr)
	{
		currentState->exit();
	}

	currentState = state;
	if (currentState != nullptr)
	{
		currentState->enter();
	}
}

std::string Game::getString(const std::string& name, std::optional<std::size_t> languageIndex) const
{
	std::string value;

	if (languageIndex == std::nullopt)
	{
		languageIndex = currentLanguageIndex;
	}

	auto it = stringTableIndex.find(name);
	if (it != stringTableIndex.end())
	{
		value = (*stringTable)[it->second][(*languageIndex) + 2];
		if (value.empty())
		{
			value = std::string("# EMPTY STRING: ") + name + std::string(" #");
		}
	}
	else
	{
		value = std::string("# MISSING STRING: ") + name + std::string(" #");
	}

	return value;
}

void Game::changeLanguage(std::size_t nextLanguageIndex)
{
	// Get names of fonts
	std::string menuFontFilename = getString("menu-font-filename");

	// Unload fonts
	delete menuFont;
	resourceManager->unload(menuFontFilename);

	// Change current language index
	currentLanguageIndex = nextLanguageIndex;

	// Reload fonts
	loadFonts();

	// Set window title
	window->setTitle(getString("title").c_str());

	// Repopulate UI element strings
	restringUI();

	// Resize the UI
	resizeUI(w, h);

	// Reselect menu item
	if (currentMenuItem)
	{
		menuSelectorSlideAnimation.stop();
		selectMenuItem(menuItemIndex, false);

		uiRootElement->update();
		currentMenu->getContainer()->resetTweens();
	}

	// Save settings
	language = getString("language-code");
	saveSettings();
}

void Game::nextLanguage()
{
	changeLanguage((currentLanguageIndex + 1) % languageCount);
}

void Game::openMenu(Menu* menu, int selectedItemIndex)
{
	if (currentMenu)
	{
		closeCurrentMenu();
	}

	currentMenu = menu;
	uiRootElement->addChild(currentMenu->getContainer());
	currentMenu->getContainer()->addChild(menuSelectorImage);
	currentMenu->getContainer()->setTintColor(Vector4(1.0f));
	
	for (MenuItem* item: *currentMenu->getItems())
	{
		item->getContainer()->setTintColor(menuItemInactiveColor);
	}
	
	selectMenuItem(selectedItemIndex, false);

	uiRootElement->update();
	currentMenu->getContainer()->resetTweens();
}

void Game::closeCurrentMenu()
{
	uiRootElement->removeChild(currentMenu->getContainer());
	currentMenu->getContainer()->removeChild(menuSelectorImage);
	currentMenu->getContainer()->setTintColor(Vector4(1.0f));
	for (MenuItem* item: *currentMenu->getItems())
	{
		item->getContainer()->setTintColor(menuItemInactiveColor);
	}

	currentMenu = nullptr;
	currentMenuItem = nullptr;
	menuItemIndex = -1;

	menuFadeAnimation.stop();
	menuSelectorSlideAnimation.stop();
	menuItemSelectAnimation.stop();
	menuItemDeselectAnimation.stop();

	previousMenu = currentMenu;
	currentMenu = nullptr;
}

void Game::selectMenuItem(int index, bool tween)
{
	bool reselected = false;

	if (index != menuItemIndex)
	{
		if (menuItemSelectAnimation.isPlaying())
		{
			menuItemSelectAnimation.stop();
			currentMenuItem->getContainer()->setTintColor(menuItemActiveColor);
		}

		if (menuItemDeselectAnimation.isPlaying())
		{
			menuItemDeselectAnimation.stop();
			previousMenuItem->getContainer()->setTintColor(menuItemInactiveColor);
		}

		// Save previous menu item
		previousMenuItem = currentMenuItem;

		// Determine current menu item
		menuItemIndex = index;
		currentMenuItem = (*(currentMenu->getItems()))[index];
	}
	else
	{
		reselected = true;
	}

	// Determine target position of menu item selector
	Vector2 itemTranslation = currentMenuItem->getContainer()->getPosition() - currentMenu->getContainer()->getPosition();
	Vector2 itemDimensions = currentMenuItem->getContainer()->getDimensions();
	float spacing = fontSizePX;
	Vector2 translation;
	translation.x = itemTranslation.x - menuSelectorImage->getDimensions().x - spacing;
	translation.y = itemTranslation.y + itemDimensions.y * 0.5f - menuSelectorImage->getDimensions().y * 0.5;

	// Create tween animations
	if (!reselected && tween && previousMenuItem != nullptr)
	{
		float tweenDuration = 0.2f;

		Vector2 oldTranslation = menuSelectorImage->getTranslation();
		Vector2 newTranslation = translation;

		// Slide animation
		{
			menuSelectorSlideClip.removeChannels();
			AnimationChannel<float>* channel = menuSelectorSlideClip.addChannel(0);
			channel->insertKeyframe(0.0f, oldTranslation.y);
			channel->insertKeyframe(tweenDuration, newTranslation.y);
			menuSelectorSlideAnimation.setTimeFrame(menuSelectorSlideClip.getTimeFrame());
			menuSelectorSlideAnimation.rewind();
			menuSelectorSlideAnimation.play();
		}

		// Color animations
		{
			menuItemSelectClip.removeChannels();
			AnimationChannel<Vector4>* channel = menuItemSelectClip.addChannel(0);
			channel->insertKeyframe(0.0f, menuItemInactiveColor);
			channel->insertKeyframe(tweenDuration, menuItemActiveColor);
			menuItemSelectAnimation.setTimeFrame(menuItemSelectClip.getTimeFrame());
			menuItemSelectAnimation.rewind();
			menuItemSelectAnimation.play();

			if (previousMenuItem)
			{
				menuItemDeselectClip.removeChannels();
				channel = menuItemDeselectClip.addChannel(0);
				channel->insertKeyframe(0.0f, menuItemActiveColor);
				channel->insertKeyframe(tweenDuration, menuItemInactiveColor);
				menuItemDeselectAnimation.setTimeFrame(menuItemDeselectClip.getTimeFrame());
				menuItemDeselectAnimation.rewind();
				menuItemDeselectAnimation.play();
			}
		}

		menuSelectorImage->setTranslation(Vector2(newTranslation.x, oldTranslation.y));
	}
	else if (!tween)
	{
		menuSelectorImage->setTranslation(translation);
		currentMenuItem->getContainer()->setTintColor(menuItemActiveColor);

		if (previousMenuItem)
		{
			previousMenuItem->getContainer()->setTintColor(menuItemInactiveColor);
		}
	}
}

void Game::selectNextMenuItem()
{
	int index = (menuItemIndex + 1) % currentMenu->getItems()->size();
	selectMenuItem(index, true);
}

void Game::selectPreviousMenuItem()
{
	int index = (menuItemIndex + (currentMenu->getItems()->size() - 1)) % currentMenu->getItems()->size();
	selectMenuItem(index, true);
}

void Game::activateMenuItem()
{
	currentMenuItem->activate();
}

void Game::activateLastMenuItem()
{
	if (currentMenu)
	{
		(*currentMenu->getItems())[currentMenu->getItems()->size() - 1]->activate();
	}
}

void Game::toggleFullscreen()
{
	if (!toggleFullscreenDisabled)
	{
		fullscreen = !(*fullscreen);
		//window->setFullscreen(*fullscreen);
		
		if (!(*fullscreen))
		{
			const Display* display = deviceManager->getDisplays()->front();
			int displayWidth = std::get<0>(display->getDimensions());
			int displayHeight = std::get<1>(display->getDimensions());

			w = (*windowResolution)[0];
			h = (*windowResolution)[1];

			window->setBordered(true);
			window->setResizable(true);
			window->setDimensions(w, h);
			window->setPosition((*windowPosition)[0], (*windowPosition)[1]);

		}
		else
		{
			const Display* display = deviceManager->getDisplays()->front();
			int displayWidth = std::get<0>(display->getDimensions());
			int displayHeight = std::get<1>(display->getDimensions());

			w = displayWidth;
			h = displayHeight;

			window->setBordered(false);
			window->setResizable(false);
			window->setPosition(0, 0);
			window->setDimensions(w, h);
		}
		
		restringUI();

		// Disable fullscreen toggles for 500ms
		toggleFullscreenDisabled = true;
		ScheduledFunctionEvent event;
		event.caller = static_cast<void*>(this);
		event.function = [this]()
		{
			toggleFullscreenDisabled = false;
		};
		eventDispatcher.schedule(event, time + 0.5f);

		// Save settings
		saveSettings();
	}
}

void Game::toggleVSync()
{
	vsync = !(*vsync);
	window->setVSync(*vsync);
	restringUI();

	// Save settings
	saveSettings();
}

void Game::setUpdateRate(double frequency)
{
	stepScheduler.setStepFrequency(frequency);
}

void Game::disableNonSystemControls()
{
	controls.setCallbacksEnabled(false);
	systemControls.setCallbacksEnabled(true);
}

void Game::setup()
{
	setupDebugging();
	loadSettings();
	setupLocalization();
	setupWindow();
	setupGraphics();
	setupControls();
	setupUI();
	setupGameplay();

	screenshotQueued = false;
	paused = false;

	// Load model resources
	try
	{
		lensModel = resourceManager->load<Model>("lens.mdl");
		forcepsModel = resourceManager->load<Model>("forceps.mdl");
		brushModel = resourceManager->load<Model>("brush.mdl");
		smokeMaterial = resourceManager->load<Material>("smoke.mtl");
	}
	catch (const std::exception& e)
	{
		logger->error("Failed to load one or more models: \"" + std::string(e.what()) + "\"\n");
		close(EXIT_FAILURE);
	}

	time = 0.0f;

	// Tools
	currentTool = nullptr;

	lens = new Lens(lensModel, &animator);
	lens->setOrbitCam(orbitCam);
	worldScene->addObject(lens->getModelInstance());
	worldScene->addObject(lens->getSpotlight());
	lens->setSunDirection(-sunlightCamera.getForward());

	// Forceps
	forceps = new Forceps(forcepsModel, &animator);
	forceps->setOrbitCam(orbitCam);
	worldScene->addObject(forceps->getModelInstance());

	// Brush
	brush = new Brush(brushModel, &animator);
	brush->setOrbitCam(orbitCam);
	worldScene->addObject(brush->getModelInstance());

	// Initialize component manager
	componentManager = new ComponentManager();

	// Initialize entity manager
	entityManager = new EntityManager(componentManager);

	// Initialize systems
	soundSystem = new SoundSystem(componentManager);
	collisionSystem = new CollisionSystem(componentManager);
	constraintSystem = new ConstraintSystem(componentManager);
	renderSystem = new RenderSystem(componentManager, worldScene);
	toolSystem = new ToolSystem(componentManager);
	toolSystem->setPickingCamera(&camera);
	toolSystem->setPickingViewport(Vector4(0, 0, w, h));
	eventDispatcher.subscribe<MouseMovedEvent>(toolSystem);
	behaviorSystem = new BehaviorSystem(componentManager);
	steeringSystem = new SteeringSystem(componentManager);
	locomotionSystem = new LocomotionSystem(componentManager);
	terrainSystem = new TerrainSystem(componentManager);
	terrainSystem->setPatchSize(500.0f);
	particleSystem = new ParticleSystem(componentManager);
	particleSystem->resize(1000);
	particleSystem->setMaterial(smokeMaterial);
	particleSystem->setDirection(Vector3(0, 1, 0));
	lens->setParticleSystem(particleSystem);
	particleSystem->getBillboardBatch()->setAlignment(&camera, BillboardAlignmentMode::SPHERICAL);
	worldScene->addObject(particleSystem->getBillboardBatch());

	// Initialize system manager
	systemManager = new SystemManager();
	systemManager->addSystem(soundSystem);
	systemManager->addSystem(behaviorSystem);
	systemManager->addSystem(steeringSystem);
	systemManager->addSystem(locomotionSystem);
	systemManager->addSystem(collisionSystem);
	systemManager->addSystem(toolSystem);
	systemManager->addSystem(terrainSystem);
	systemManager->addSystem(particleSystem);
	systemManager->addSystem(constraintSystem);
	systemManager->addSystem(renderSystem);

	// Load navmesh
	TriangleMesh* navmesh = resourceManager->load<TriangleMesh>("sidewalk.mesh");

	int highResolutionDiameter = 3;
	int mediumResolutionDiameter = highResolutionDiameter + 2;
	int lowResolutionDiameter = 20;

	float lowResolutionRadius = static_cast<float>(lowResolutionDiameter) / 2.0f;
	float mediumResolutionRadius = static_cast<float>(mediumResolutionDiameter) / 2.0f;
	float highResolutionRadius = static_cast<float>(highResolutionDiameter) / 2.0f;

	for (int i = 0; i < lowResolutionDiameter; ++i)
	{
		for (int j = 0; j < lowResolutionDiameter; ++j)
		{
			EntityID patch;

			int x = i - lowResolutionDiameter / 2;
			int z = j - lowResolutionDiameter / 2;


			if (std::abs(x) < highResolutionRadius && std::abs(z) < highResolutionRadius)
			{
				patch = createInstanceOf("terrain-patch-high-resolution");
			}
			else if (std::abs(x) < mediumResolutionRadius && std::abs(z) < mediumResolutionRadius)
			{
				patch = createInstanceOf("terrain-patch-medium-resolution");
			}
			else
			{
				patch = createInstanceOf("terrain-patch-low-resolution");
			}

			setTerrainPatchPosition(patch, {x, z});
		}
	}

	// Setup state machine states
	languageSelectState =
	{
		std::bind(&Game::enterLanguageSelectState, this),
		std::bind(&Game::exitLanguageSelectState, this)
	};
	splashState =
	{
		std::bind(&Game::enterSplashState, this),
		std::bind(&Game::exitSplashState, this)
	};
	loadingState =
	{
		std::bind(&Game::enterLoadingState, this),
		std::bind(&Game::exitLoadingState, this)
	};
	titleState =
	{
		std::bind(&Game::enterTitleState, this),
		std::bind(&Game::exitTitleState, this)
	};
	playState =
	{
		std::bind(&Game::enterPlayState, this),
		std::bind(&Game::exitPlayState, this)
	};

	// Initialize state machine
	if (firstRun)
	{
		StateMachine::changeState(&languageSelectState);
	}
	else
	{
		#if defined(DEBUG)
			StateMachine::changeState(&titleState);
		#else
			StateMachine::changeState(&splashState);
		#endif
	}

	//changeState(sandboxState);
}

void Game::update(float t, float dt)
{
	this->time = t;

	// Execute current state
	if (currentState != nullptr)
	{
		currentState->execute();
	}

	// Update systems
	systemManager->update(t, dt);

	// Update animations
	animator.animate(dt);

	if (fpsLabel->isVisible())
	{
		std::stringstream stream;
		stream.precision(2);
		stream << std::fixed << (performanceSampler.getMeanFrameDuration() * 1000.0f);
		fpsLabel->setText(stream.str());
	}

	uiRootElement->update();
}

void Game::input()
{
	controls.update();
}

void Game::render()
{
	// Perform sub-frame interpolation on UI elements
	uiRootElement->interpolate(stepScheduler.getScheduledSubsteps());

	// Update and batch UI elements
	uiBatcher->batch(uiBatch, uiRootElement);

	// Perform sub-frame interpolation particles
	particleSystem->getBillboardBatch()->interpolate(stepScheduler.getScheduledSubsteps());
	particleSystem->getBillboardBatch()->batch();

	// Render scene
	renderer.render(*worldScene);
	renderer.render(*uiScene);

	if (screenshotQueued)
	{
		screenshot();
		screenshotQueued = false;
	}

	// Swap window framebuffers
	window->swapBuffers();
}

void Game::exit()
{}

void Game::handleEvent(const WindowResizedEvent& event)
{
	w = event.width;
	h = event.height;

	if (*fullscreen)
	{
		logger->log("Resized fullscreen window to " + std::to_string(w) + "x" + std::to_string(h));
		fullscreenResolution = {event.width, event.height};
	}
	else
	{
		logger->log("Resized window to " + std::to_string(w) + "x" + std::to_string(h));
		windowResolution = {event.width, event.height};
	}

	// Save resolution settings
	saveSettings();

	defaultRenderTarget.width = event.width;
	defaultRenderTarget.height = event.height;
	glViewport(0, 0, event.width, event.height);

	camera.setPerspective(glm::radians(40.0f), static_cast<float>(w) / static_cast<float>(h), 0.1, 100.0f);


	toolSystem->setPickingViewport(Vector4(0, 0, w, h));

	resizeUI(event.width, event.height);

	skipSplash();
}

void Game::handleEvent(const WindowMovedEvent& event)
{
	if (!(*fullscreen))
	{
		windowPosition = {event.x, event.y};
		saveSettings();
	}
}

void Game::handleEvent(const GamepadConnectedEvent& event)
{
	// Unmap all controls
	inputRouter->reset();

	// Reload control profile
	loadControlProfile(*controlProfileName);
}

void Game::handleEvent(const GamepadDisconnectedEvent& event)
{}

void Game::handleEvent(const ScheduledFunctionEvent& event)
{
	if (event.caller == static_cast<void*>(this))
	{
		event.function();
	}
}

void Game::setupDebugging()
{
	// Setup logging
	{
		logger = new Logger();

		// Style log format
		std::string logPrefix = std::string();
		std::string logPostfix = "\n";
		std::string warningPrefix = "Warning: ";
		std::string warningPostfix = std::string();
		std::string errorPrefix = "Error: ";
		std::string errorPostfix = std::string();
		std::string successPrefix = std::string();
		std::string successPostfix = std::string();

		// Enable colored messages on debug builds
		#if defined(DEBUG)
			warningPrefix = std::string(ANSI_CODE_BOLD) + std::string(ANSI_CODE_YELLOW) + std::string("Warning: ") + std::string(ANSI_CODE_RESET) + std::string(ANSI_CODE_YELLOW);
			warningPostfix.append(ANSI_CODE_RESET);
			errorPrefix = std::string(ANSI_CODE_BOLD) + std::string(ANSI_CODE_RED) + std::string("Error: ") + std::string(ANSI_CODE_RESET) + std::string(ANSI_CODE_RED);
			errorPostfix.append(ANSI_CODE_RESET);
			successPrefix.insert(0, ANSI_CODE_GREEN);
			successPostfix.append(ANSI_CODE_RESET);
		#endif

		logger->setLogPrefix(logPrefix);
		logger->setLogPostfix(logPostfix);
		logger->setWarningPrefix(warningPrefix);
		logger->setWarningPostfix(warningPostfix);
		logger->setErrorPrefix(errorPrefix);
		logger->setErrorPostfix(errorPostfix);
		logger->setSuccessPrefix(successPrefix);
		logger->setSuccessPostfix(successPostfix);

		// Redirect logger output to log file on release builds
		#if !defined(DEBUG)
			std::string logFilename = configPath + "log.txt";
			logFileStream.open(logFilename.c_str());
			logger->redirect(&logFileStream);
		#endif
	}

	// Create CLI
	cli = new CommandInterpreter();

	// Register CLI commands
	std::function<void(std::string, std::string)> setCommand = std::bind(&CommandInterpreter::set, cli, std::placeholders::_1, std::placeholders::_2);
	std::function<void(std::string)> unsetCommand = std::bind(&CommandInterpreter::unset, cli, std::placeholders::_1);
	std::function<void()> exitCommand = std::bind(std::exit, EXIT_SUCCESS);
	std::function<void(int, float, float, float)> setScaleCommand = [this](int id, float x, float y, float z) {
		setScale(id, {x, y, z});
	};
	std::function<void(int, float, float, float)> setTranslationCommand = [this](int id, float x, float y, float z) {
		setTranslation(id, {x, y, z});
	};
	std::function<void()> createInstanceCommand = std::bind(&Game::createInstance, this);
	std::function<void(std::string)> createNamedInstanceCommand = std::bind(&Game::createNamedInstance, this, std::placeholders::_1);
	std::function<void(std::string)> createInstanceOfCommand = std::bind(&Game::createInstanceOf, this, std::placeholders::_1);
	std::function<void(std::string, std::string)> createNamedInstanceOfCommand = std::bind(&Game::createNamedInstanceOf, this, std::placeholders::_1, std::placeholders::_2);

	std::function<void(float)> toggleWireframeCommand = [this](float width){ lightingPass->setWireframeLineWidth(width); };
	std::function<void()> helpCommand = [this]()
	{
		auto& helpStrings = cli->help();
		for (auto it = helpStrings.begin(); it != helpStrings.end(); ++it)
		{
			if (it->second.empty())
			{
				continue;
			}

			std::cout << it->second << std::endl;
		}
	};
	std::function<void()> variablesCommand = [this]()
	{
		auto& variables = cli->variables();
		for (auto it = variables.begin(); it != variables.end(); ++it)
		{
			std::cout << it->first << "=\"" << it->second << "\"" << std::endl;
		}
	};

	std::string exitHelp = "exit";
	std::string setHelp = "set <variable name> <value>";
	std::string unsetHelp = "unset <variable name>";
	std::string createInstanceHelp = "createinstance <template name>";
	std::string createNamedInstanceHelp = "createnamedinstance <template name> <instance name>";
	std::string createInstanceOfHelp = "createinstanceof <template name>";
	std::string createNamedInstanceOfHelp = "createnamedinstanceof <template name> <instance name>";
	std::string setTranslationHelp = "settranslation <id> <x> <y> <z>";
	std::string setScaleHelp = "setscale <id> <sx> <sy> <sz>";
	std::string wireframeHelp = "wireframe <width>";
	std::string variablesHelp = "variables";

	cli->registerCommand("exit", exitCommand, exitHelp);
	cli->registerCommand("set", setCommand, setHelp);
	cli->registerCommand("unset", setCommand, unsetHelp);
	cli->registerCommand("createinstance", createInstanceCommand, createInstanceHelp);
	cli->registerCommand("createnamedinstance", createNamedInstanceCommand, createNamedInstanceHelp);
	cli->registerCommand("createinstanceof", createInstanceOfCommand, createInstanceOfHelp);
	cli->registerCommand("createnamedinstanceof", createNamedInstanceOfCommand, createNamedInstanceOfHelp);
	cli->registerCommand("setscale", setScaleCommand, setScaleHelp);
	cli->registerCommand("settranslation", setTranslationCommand, setTranslationHelp);
	cli->registerCommand("wireframe", toggleWireframeCommand, wireframeHelp);
	cli->registerCommand("variables", variablesCommand, variablesHelp);
	cli->registerCommand("help", helpCommand);

	// Start CLI thread
	std::thread cliThread(&Game::interpretCommands, this);
	cliThread.detach();

	// Setup performance sampling
	performanceSampler.setSampleSize(30);
}

void Game::setupLocalization()
{
	// Load strings
	loadStrings();

	// Determine number of available languages
	languageCount = (*stringTable)[0].size() - 2;
	
	// Match language code with language index
	currentLanguageIndex = 0;
	StringTableRow* languageCodes = &(*stringTable)[1];
	for (std::size_t i = 2; i < languageCodes->size(); ++i)
	{
		if (*language == (*languageCodes)[i])
		{
			currentLanguageIndex = i - 2;
			break;
		}
	}
}

void Game::setupWindow()
{
	// Get display resolution
	const Display* display = deviceManager->getDisplays()->front();
	int displayWidth = std::get<0>(display->getDimensions());
	int displayHeight = std::get<1>(display->getDimensions());
	int flags = 0;

	int x;
	int y;
	if (*fullscreen)
	{
		w = displayWidth;
		h = displayHeight;
		x = std::get<0>(display->getPosition());
		y = std::get<1>(display->getPosition());
		flags |= WindowFlag::BORDERLESS;
	}
	else
	{
		w = (*windowResolution)[0];
		h = (*windowResolution)[1];
		x = (*windowPosition)[0];
		y = (*windowPosition)[1];
		flags |= WindowFlag::RESIZABLE;
	}

	// Read title string
	std::string title = getString("title");

	// Create window
	window = windowManager->createWindow(title.c_str(), x, y, w, h, flags);
	if (!window)
	{
		throw std::runtime_error("Game::Game(): Failed to create window.");
	}

	// Set v-sync mode
	window->setVSync(*vsync);

	debugFont = nullptr;
	menuFont = nullptr;
}

void Game::setupGraphics()
{
	// Setup OpenGL
	glEnable(GL_MULTISAMPLE);

	// Setup default render target
	defaultRenderTarget.width = w;
	defaultRenderTarget.height = h;
	defaultRenderTarget.framebuffer = 0;

	// Set shadow map resolution
	shadowMapResolution = 4096;

	// Setup shadow map framebuffer
	glGenFramebuffers(1, &shadowMapFramebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFramebuffer);
	glGenTextures(1, &shadowMapDepthTextureID);
	glBindTexture(GL_TEXTURE_2D, shadowMapDepthTextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, shadowMapResolution, shadowMapResolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMapDepthTextureID, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Setup shadow map render target
	shadowMapRenderTarget.width = shadowMapResolution;
	shadowMapRenderTarget.height = shadowMapResolution;
	shadowMapRenderTarget.framebuffer = shadowMapFramebuffer;

	// Setup shadow map depth texture
	shadowMapDepthTexture.setTextureID(shadowMapDepthTextureID);
	shadowMapDepthTexture.setWidth(shadowMapResolution);
	shadowMapDepthTexture.setHeight(shadowMapResolution);

	// Setup silhouette framebuffer
	glGenTextures(1, &silhouetteRenderTarget.texture);
	glBindTexture(GL_TEXTURE_2D, silhouetteRenderTarget.texture);
	glTexImage2D(GL_TEXTURE_2D, 0,  GL_R8, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glGenFramebuffers(1, &silhouetteRenderTarget.framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, silhouetteRenderTarget.framebuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, silhouetteRenderTarget.texture, 0);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	glReadBuffer(GL_NONE);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Setup silhouette render target
	silhouetteRenderTarget.width = w;
	silhouetteRenderTarget.height = h;

	// Setup shadow map render pass
	shadowMapPass = new ShadowMapRenderPass(resourceManager);
	shadowMapPass->setRenderTarget(&shadowMapRenderTarget);
	shadowMapPass->setViewCamera(&camera);
	shadowMapPass->setLightCamera(&sunlightCamera);
		
	// Setup shadow map compositor
	shadowMapCompositor.addPass(shadowMapPass);
	shadowMapCompositor.load(nullptr);

	// Setup clear render pass
	clearPass = new ClearRenderPass();
	clearPass->setRenderTarget(&defaultRenderTarget);
	clearPass->setClear(true, true, false);
	clearPass->setClearColor(Vector4(0.0f));
	clearPass->setClearDepth(1.0f);

	// Setup sky render pass
	skyPass = new SkyRenderPass(resourceManager);
	skyPass->setRenderTarget(&defaultRenderTarget);

	// Setup lighting pass
	lightingPass = new LightingRenderPass(resourceManager);
	lightingPass->setRenderTarget(&defaultRenderTarget);
	lightingPass->setShadowMapPass(shadowMapPass);
	lightingPass->setShadowMap(&shadowMapDepthTexture);

	// Setup clear silhouette pass
	clearSilhouettePass = new ClearRenderPass();
	clearSilhouettePass->setRenderTarget(&silhouetteRenderTarget);
	clearSilhouettePass->setClear(true, false, false);
	clearSilhouettePass->setClearColor(Vector4(0.0f));

	// Setup silhouette pass
	silhouettePass = new SilhouetteRenderPass(resourceManager);
	silhouettePass->setRenderTarget(&silhouetteRenderTarget);

	// Setup final pass
	finalPass = new FinalRenderPass(resourceManager);
	finalPass->setRenderTarget(&defaultRenderTarget);
	finalPass->setSilhouetteRenderTarget(&silhouetteRenderTarget);

	// Setup default compositor
	defaultCompositor.addPass(clearPass);
	defaultCompositor.addPass(skyPass);
	defaultCompositor.addPass(lightingPass);
	defaultCompositor.addPass(clearSilhouettePass);
	defaultCompositor.addPass(silhouettePass);
	//defaultCompositor.addPass(finalPass);
	defaultCompositor.load(nullptr);

	// Setup UI render pass
	uiPass = new UIRenderPass(resourceManager);
	uiPass->setRenderTarget(&defaultRenderTarget);

	// Setup UI compositor
	uiCompositor.addPass(uiPass);
	uiCompositor.load(nullptr);

	// Create scenes
	worldScene = new Scene(&stepInterpolator);
	uiScene = new Scene(&stepInterpolator);

	// Setup camera
	camera.setPerspective(glm::radians(40.0f), static_cast<float>(w) / static_cast<float>(h), 0.1, 100.0f);
	camera.lookAt(Vector3(0.0f, 4.0f, 2.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f));
	camera.setCompositor(&defaultCompositor);
	camera.setCompositeIndex(1);
	worldScene->addObject(&camera);

	// Setup sun
	sunlight.setDirection(Vector3(0, -1, 0));
	setTimeOfDay(11.0f);
	worldScene->addObject(&sunlight);

	// Setup sunlight camera
	sunlightCamera.setOrthographic(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
	sunlightCamera.setCompositor(&shadowMapCompositor);
	sunlightCamera.setCompositeIndex(0);
	sunlightCamera.setCullingEnabled(true);
	sunlightCamera.setCullingMask(&camera.getViewFrustum());
	worldScene->addObject(&sunlightCamera);
}

void Game::setupUI()
{
	// Get DPI and convert font size to pixels
	const Display* display = deviceManager->getDisplays()->front();
	dpi = display->getDPI();
	fontSizePX = (*fontSize) * (1.0f / 96.0f) * dpi;
	
	logger->log("Detected display DPI as " + std::to_string(dpi) + ".");
	logger->log("Font size = " + std::to_string(*fontSize) + " PT = " + std::to_string(fontSizePX) + " PX.");

	// Load fonts
	loadFonts();

	// Load splash screen texture
	splashTexture = resourceManager->load<Texture2D>("splash.png");

	// Load HUD texture
	hudSpriteSheetTexture = resourceManager->load<Texture2D>("hud.png");

	// Read texture atlas file
	StringTable* atlasTable = resourceManager->load<StringTable>("hud-atlas.csv");

	// Build texture atlas
	for (int row = 0; row < atlasTable->size(); ++row)
	{
		std::stringstream ss;
		float x;
		float y;
		float w;
		float h;

		ss << (*atlasTable)[row][1];
		ss >> x;
		ss.str(std::string());
		ss.clear();
		ss << (*atlasTable)[row][2];
		ss >> y;
		ss.str(std::string());
		ss.clear();
		ss << (*atlasTable)[row][3];
		ss >> w;
		ss.str(std::string());
		ss.clear();
		ss << (*atlasTable)[row][4];
		ss >> h;
		ss.str(std::string());

		y = static_cast<float>(hudSpriteSheetTexture->getHeight()) - y - h;
		x = (int)(x + 0.5f);
		y = (int)(y + 0.5f);
		w = (int)(w + 0.5f);
		h = (int)(h + 0.5f);

		hudTextureAtlas.insert((*atlasTable)[row][0], Rect(Vector2(x, y), Vector2(x + w, y + h)));
	}

	// Setup UI batching
	uiBatch = new BillboardBatch();
	uiBatch->resize(1024);
	uiBatcher = new UIBatcher();

	// Setup root UI element
	uiRootElement = new UIContainer();
	eventDispatcher.subscribe<MouseMovedEvent>(uiRootElement);
	eventDispatcher.subscribe<MouseButtonPressedEvent>(uiRootElement);
	eventDispatcher.subscribe<MouseButtonReleasedEvent>(uiRootElement);

	// Create splash screen background element
	splashBackgroundImage = new UIImage();
	splashBackgroundImage->setLayerOffset(-1);
	splashBackgroundImage->setTintColor(Vector4(0.0f, 0.0f, 0.0f, 1.0f));
	splashBackgroundImage->setVisible(false);
	uiRootElement->addChild(splashBackgroundImage);

	// Create splash screen element
	splashImage = new UIImage();
	splashImage->setTexture(splashTexture);
	splashImage->setVisible(false);
	uiRootElement->addChild(splashImage);

	Rect hudTextureAtlasBounds(Vector2(0), Vector2(hudSpriteSheetTexture->getWidth(), hudSpriteSheetTexture->getHeight()));
	auto normalizeTextureBounds = [](const Rect& texture, const Rect& atlas)
	{
		Vector2 atlasDimensions = Vector2(atlas.getWidth(), atlas.getHeight());
		return Rect(texture.getMin() / atlasDimensions, texture.getMax() / atlasDimensions);
	};

	// Create HUD elements
	hudContainer = new UIContainer();
	hudContainer->setVisible(false);
	uiRootElement->addChild(hudContainer);

	toolIndicatorBGImage = new UIImage();
	toolIndicatorBGImage->setTexture(hudSpriteSheetTexture);
	toolIndicatorBGImage->setTextureBounds(normalizeTextureBounds(hudTextureAtlas.getBounds("tool-indicator"), hudTextureAtlasBounds));
	hudContainer->addChild(toolIndicatorBGImage);

	toolIndicatorsBounds = new Rect[8];
	toolIndicatorsBounds[0] = normalizeTextureBounds(hudTextureAtlas.getBounds("tool-indicator-brush"), hudTextureAtlasBounds);
	toolIndicatorsBounds[1] = normalizeTextureBounds(hudTextureAtlas.getBounds("tool-indicator-spade"), hudTextureAtlasBounds);
	toolIndicatorsBounds[2] = normalizeTextureBounds(hudTextureAtlas.getBounds("tool-indicator-lens"), hudTextureAtlasBounds);
	toolIndicatorsBounds[3] = normalizeTextureBounds(hudTextureAtlas.getBounds("tool-indicator-test-tube"), hudTextureAtlasBounds);
	toolIndicatorsBounds[4] = normalizeTextureBounds(hudTextureAtlas.getBounds("tool-indicator-forceps"), hudTextureAtlasBounds);
	toolIndicatorsBounds[5] = normalizeTextureBounds(hudTextureAtlas.getBounds("tool-indicator"), hudTextureAtlasBounds);
	toolIndicatorsBounds[6] = normalizeTextureBounds(hudTextureAtlas.getBounds("tool-indicator"), hudTextureAtlasBounds);
	toolIndicatorsBounds[7] = normalizeTextureBounds(hudTextureAtlas.getBounds("tool-indicator"), hudTextureAtlasBounds);

	toolIndicatorIconImage = new UIImage();
	toolIndicatorIconImage->setTexture(hudSpriteSheetTexture);
	toolIndicatorIconImage->setTextureBounds(normalizeTextureBounds(hudTextureAtlas.getBounds("tool-icon-brush"), hudTextureAtlasBounds));
	toolIndicatorBGImage->addChild(toolIndicatorIconImage);

	buttonContainer = new UIContainer();
	hudContainer->addChild(buttonContainer);

	playButtonBGImage = new UIImage();
	playButtonBGImage->setTexture(hudSpriteSheetTexture);
	playButtonBGImage->setTextureBounds(normalizeTextureBounds(hudTextureAtlas.getBounds("button-background"), hudTextureAtlasBounds));
	//buttonContainer->addChild(playButtonBGImage);

	pauseButtonBGImage = new UIImage();
	pauseButtonBGImage->setTexture(hudSpriteSheetTexture);
	pauseButtonBGImage->setTextureBounds(normalizeTextureBounds(hudTextureAtlas.getBounds("button-background"), hudTextureAtlasBounds));
	//buttonContainer->addChild(pauseButtonBGImage);

	fastForwardButtonBGImage = new UIImage();
	fastForwardButtonBGImage->setTexture(hudSpriteSheetTexture);
	fastForwardButtonBGImage->setTextureBounds(normalizeTextureBounds(hudTextureAtlas.getBounds("button-background"), hudTextureAtlasBounds));
	//buttonContainer->addChild(fastForwardButtonBGImage);

	playButtonImage = new UIImage();
	playButtonImage->setTexture(hudSpriteSheetTexture);
	playButtonImage->setTextureBounds(normalizeTextureBounds(hudTextureAtlas.getBounds("button-play"), hudTextureAtlasBounds));
	//buttonContainer->addChild(playButtonImage);

	fastForwardButtonImage = new UIImage();
	fastForwardButtonImage->setTexture(hudSpriteSheetTexture);
	fastForwardButtonImage->setTextureBounds(normalizeTextureBounds(hudTextureAtlas.getBounds("button-fast-forward-2x"), hudTextureAtlasBounds));
	//buttonContainer->addChild(fastForwardButtonImage);

	pauseButtonImage = new UIImage();
	pauseButtonImage->setTexture(hudSpriteSheetTexture);
	pauseButtonImage->setTextureBounds(normalizeTextureBounds(hudTextureAtlas.getBounds("button-pause"), hudTextureAtlasBounds));
	//buttonContainer->addChild(pauseButtonImage);

	radialMenuContainer = new UIContainer();
	radialMenuContainer->setVisible(false);
	uiRootElement->addChild(radialMenuContainer);

	radialMenuBackgroundImage = new UIImage();
	radialMenuBackgroundImage->setTintColor(Vector4(Vector3(0.0f), 0.25f));
	radialMenuContainer->addChild(radialMenuBackgroundImage);

	radialMenuImage = new UIImage();
	radialMenuImage->setTexture(hudSpriteSheetTexture);
	radialMenuImage->setTextureBounds(normalizeTextureBounds(hudTextureAtlas.getBounds("radial-menu"), hudTextureAtlasBounds));
	radialMenuContainer->addChild(radialMenuImage);

	radialMenuSelectorImage = new UIImage();
	radialMenuSelectorImage->setTexture(hudSpriteSheetTexture);
	radialMenuSelectorImage->setTextureBounds(normalizeTextureBounds(hudTextureAtlas.getBounds("radial-menu-selector"), hudTextureAtlasBounds));
	radialMenuContainer->addChild(radialMenuSelectorImage);

	toolIconBrushImage = new UIImage();
	toolIconBrushImage->setTexture(hudSpriteSheetTexture);
	toolIconBrushImage->setTextureBounds(normalizeTextureBounds(hudTextureAtlas.getBounds("tool-icon-brush"), hudTextureAtlasBounds));
	radialMenuImage->addChild(toolIconBrushImage);

	toolIconLensImage = new UIImage();
	toolIconLensImage->setTexture(hudSpriteSheetTexture);
	toolIconLensImage->setTextureBounds(normalizeTextureBounds(hudTextureAtlas.getBounds("tool-icon-lens"), hudTextureAtlasBounds));
	radialMenuImage->addChild(toolIconLensImage);

	toolIconForcepsImage = new UIImage();
	toolIconForcepsImage->setTexture(hudSpriteSheetTexture);
	toolIconForcepsImage->setTextureBounds(normalizeTextureBounds(hudTextureAtlas.getBounds("tool-icon-forceps"), hudTextureAtlasBounds));
	radialMenuImage->addChild(toolIconForcepsImage);

	toolIconSpadeImage = new UIImage();
	toolIconSpadeImage->setTexture(hudSpriteSheetTexture);
	toolIconSpadeImage->setTextureBounds(normalizeTextureBounds(hudTextureAtlas.getBounds("tool-icon-spade"), hudTextureAtlasBounds));
	//radialMenuImage->addChild(toolIconSpadeImage);

	toolIconCameraImage = new UIImage();
	toolIconCameraImage->setTexture(hudSpriteSheetTexture);
	toolIconCameraImage->setTextureBounds(normalizeTextureBounds(hudTextureAtlas.getBounds("tool-icon-camera"), hudTextureAtlasBounds));
	radialMenuImage->addChild(toolIconCameraImage);

	toolIconMicrochipImage = new UIImage();
	toolIconMicrochipImage->setTexture(hudSpriteSheetTexture);
	toolIconMicrochipImage->setTextureBounds(normalizeTextureBounds(hudTextureAtlas.getBounds("tool-icon-microchip"), hudTextureAtlasBounds));
	radialMenuImage->addChild(toolIconMicrochipImage);

	toolIconTestTubeImage = new UIImage();
	toolIconTestTubeImage->setTexture(hudSpriteSheetTexture);
	toolIconTestTubeImage->setTextureBounds(normalizeTextureBounds(hudTextureAtlas.getBounds("tool-icon-test-tube"), hudTextureAtlasBounds));
	//radialMenuImage->addChild(toolIconTestTubeImage);

	antTag = new UIContainer();
	antTag->setLayerOffset(-10);
	antTag->setVisible(false);

	uiRootElement->addChild(antTag);

	antLabelContainer = new UIContainer();
	antTag->addChild(antLabelContainer);

	antLabelTL = new UIImage();
	antLabelTR = new UIImage();
	antLabelBL = new UIImage();
	antLabelBR = new UIImage();
	antLabelCC = new UIImage();
	antLabelCT = new UIImage();
	antLabelCB = new UIImage();
	antLabelCL = new UIImage();
	antLabelCR = new UIImage();

	antLabelTL->setTexture(hudSpriteSheetTexture);
	antLabelTR->setTexture(hudSpriteSheetTexture);
	antLabelBL->setTexture(hudSpriteSheetTexture);
	antLabelBR->setTexture(hudSpriteSheetTexture);
	antLabelCC->setTexture(hudSpriteSheetTexture);
	antLabelCT->setTexture(hudSpriteSheetTexture);
	antLabelCB->setTexture(hudSpriteSheetTexture);
	antLabelCL->setTexture(hudSpriteSheetTexture);
	antLabelCR->setTexture(hudSpriteSheetTexture);

	Rect labelTLBounds = normalizeTextureBounds(hudTextureAtlas.getBounds("label-tl"), hudTextureAtlasBounds);
	Rect labelTRBounds = normalizeTextureBounds(hudTextureAtlas.getBounds("label-tr"), hudTextureAtlasBounds);
	Rect labelBLBounds = normalizeTextureBounds(hudTextureAtlas.getBounds("label-bl"), hudTextureAtlasBounds);
	Rect labelBRBounds = normalizeTextureBounds(hudTextureAtlas.getBounds("label-br"), hudTextureAtlasBounds);
	Rect labelCCBounds = normalizeTextureBounds(hudTextureAtlas.getBounds("label-cc"), hudTextureAtlasBounds);
	Rect labelCTBounds = normalizeTextureBounds(hudTextureAtlas.getBounds("label-ct"), hudTextureAtlasBounds);
	Rect labelCBBounds = normalizeTextureBounds(hudTextureAtlas.getBounds("label-cb"), hudTextureAtlasBounds);
	Rect labelCLBounds = normalizeTextureBounds(hudTextureAtlas.getBounds("label-cl"), hudTextureAtlasBounds);
	Rect labelCRBounds = normalizeTextureBounds(hudTextureAtlas.getBounds("label-cr"), hudTextureAtlasBounds);

	Vector2 labelTLMin = labelTLBounds.getMin();
	Vector2 labelTRMin = labelTRBounds.getMin();
	Vector2 labelBLMin = labelBLBounds.getMin();
	Vector2 labelBRMin = labelBRBounds.getMin();
	Vector2 labelCCMin = labelCCBounds.getMin();
	Vector2 labelCTMin = labelCTBounds.getMin();
	Vector2 labelCBMin = labelCBBounds.getMin();
	Vector2 labelCLMin = labelCLBounds.getMin();
	Vector2 labelCRMin = labelCRBounds.getMin();
	Vector2 labelTLMax = labelTLBounds.getMax();
	Vector2 labelTRMax = labelTRBounds.getMax();
	Vector2 labelBLMax = labelBLBounds.getMax();
	Vector2 labelBRMax = labelBRBounds.getMax();
	Vector2 labelCCMax = labelCCBounds.getMax();
	Vector2 labelCTMax = labelCTBounds.getMax();
	Vector2 labelCBMax = labelCBBounds.getMax();
	Vector2 labelCLMax = labelCLBounds.getMax();
	Vector2 labelCRMax = labelCRBounds.getMax();

	antLabelTL->setTextureBounds(labelTLBounds);
	antLabelTR->setTextureBounds(labelTRBounds);
	antLabelBL->setTextureBounds(labelBLBounds);
	antLabelBR->setTextureBounds(labelBRBounds);
	antLabelCC->setTextureBounds(labelCCBounds);
	antLabelCT->setTextureBounds(labelCTBounds);
	antLabelCB->setTextureBounds(labelCBBounds);
	antLabelCL->setTextureBounds(labelCLBounds);
	antLabelCR->setTextureBounds(labelCRBounds);

	antLabelContainer->addChild(antLabelTL);
	antLabelContainer->addChild(antLabelTR);
	antLabelContainer->addChild(antLabelBL);
	antLabelContainer->addChild(antLabelBR);
	antLabelContainer->addChild(antLabelCC);
	antLabelContainer->addChild(antLabelCT);
	antLabelContainer->addChild(antLabelCB);
	antLabelContainer->addChild(antLabelCL);
	antLabelContainer->addChild(antLabelCR);

	antLabel = new UILabel();
	antLabel->setFont(nullptr);
	antLabel->setText("");
	antLabel->setTintColor(Vector4(Vector3(0.0f), 1.0f));
	antLabel->setLayerOffset(1);
	antLabelContainer->addChild(antLabel);

	fpsLabel = new UILabel();
	fpsLabel->setFont(debugFont);
	fpsLabel->setTintColor(Vector4(1, 1, 0, 1));
	fpsLabel->setLayerOffset(50);
	fpsLabel->setAnchor(Anchor::TOP_LEFT);
	uiRootElement->addChild(fpsLabel);

	antPin = new UIImage();
	antPin->setTexture(hudSpriteSheetTexture);
	antPin->setTextureBounds(normalizeTextureBounds(hudTextureAtlas.getBounds("label-pin"), hudTextureAtlasBounds));
	antTag->addChild(antPin);

	antLabelPinHole = new UIImage();
	antLabelPinHole->setTexture(hudSpriteSheetTexture);
	antLabelPinHole->setTextureBounds(normalizeTextureBounds(hudTextureAtlas.getBounds("label-pin-hole"), hudTextureAtlasBounds));
	antLabelContainer->addChild(antLabelPinHole);
	
	// Construct box selection
	boxSelectionImageBackground = new UIImage();
	boxSelectionImageBackground->setAnchor(Anchor::CENTER);
	boxSelectionImageTop = new UIImage();
	boxSelectionImageTop->setAnchor(Anchor::TOP_LEFT);
	boxSelectionImageBottom = new UIImage();
	boxSelectionImageBottom->setAnchor(Anchor::BOTTOM_LEFT);
	boxSelectionImageLeft = new UIImage();
	boxSelectionImageLeft->setAnchor(Anchor::TOP_LEFT);
	boxSelectionImageRight = new UIImage();
	boxSelectionImageRight->setAnchor(Anchor::TOP_RIGHT);
	boxSelectionContainer = new UIContainer();
	boxSelectionContainer->setLayerOffset(80);
	boxSelectionContainer->addChild(boxSelectionImageBackground);
	boxSelectionContainer->addChild(boxSelectionImageTop);
	boxSelectionContainer->addChild(boxSelectionImageBottom);
	boxSelectionContainer->addChild(boxSelectionImageLeft);
	boxSelectionContainer->addChild(boxSelectionImageRight);
	boxSelectionContainer->setVisible(false);
	uiRootElement->addChild(boxSelectionContainer);
	boxSelectionImageBackground->setTintColor(Vector4(1.0f, 1.0f, 1.0f, 0.5f));
	boxSelectionContainer->setTintColor(Vector4(1.0f, 0.0f, 0.0f, 1.0f));
	boxSelectionBorderWidth = 2.0f;

	cameraGridColor = Vector4(1, 1, 1, 0.5f);
	cameraReticleColor = Vector4(1, 1, 1, 0.75f);
	cameraGridY0Image = new UIImage();
	cameraGridY0Image->setAnchor(Vector2(0.5f, (1.0f / 3.0f)));
	cameraGridY0Image->setTintColor(cameraGridColor);
	cameraGridY1Image = new UIImage();
	cameraGridY1Image->setAnchor(Vector2(0.5f, (2.0f / 3.0f)));
	cameraGridY1Image->setTintColor(cameraGridColor);
	cameraGridX0Image = new UIImage();
	cameraGridX0Image->setAnchor(Vector2((1.0f / 3.0f), 0.5f));
	cameraGridX0Image->setTintColor(cameraGridColor);
	cameraGridX1Image = new UIImage();
	cameraGridX1Image->setAnchor(Vector2((2.0f / 3.0f), 0.5f));
	cameraGridX1Image->setTintColor(cameraGridColor);
	cameraReticleImage = new UIImage();
	cameraReticleImage->setAnchor(Anchor::CENTER);
	cameraReticleImage->setTintColor(cameraReticleColor);
	cameraReticleImage->setTexture(hudSpriteSheetTexture);
	cameraReticleImage->setTextureBounds(normalizeTextureBounds(hudTextureAtlas.getBounds("camera-reticle"), hudTextureAtlasBounds));
	cameraGridContainer = new UIContainer();
	cameraGridContainer->addChild(cameraGridY0Image);
	cameraGridContainer->addChild(cameraGridY1Image);
	cameraGridContainer->addChild(cameraGridX0Image);
	cameraGridContainer->addChild(cameraGridX1Image);
	cameraGridContainer->addChild(cameraReticleImage);
	cameraGridContainer->setVisible(false);
	uiRootElement->addChild(cameraGridContainer);

	cameraFlashImage = new UIImage();
	cameraFlashImage->setLayerOffset(99);
	cameraFlashImage->setTintColor(Vector4(1.0f));
	cameraFlashImage->setVisible(false);
	uiRootElement->addChild(cameraFlashImage);

	blackoutImage = new UIImage();
	blackoutImage->setLayerOffset(98);
	blackoutImage->setTintColor(Vector4(0.0f, 0.0f, 0.0f, 1.0f));
	blackoutImage->setVisible(false);
	uiRootElement->addChild(blackoutImage);

	languageSelectBGImage = new UIImage();
	languageSelectBGImage->setLayerOffset(-1);
	languageSelectBGImage->setTintColor(Vector4(0.0f, 0.0f, 0.0f, 1.0f));
	languageSelectBGImage->setVisible(true);

	standardMenuActiveColor = Vector4(Vector3(0.2f), 1.0f);
	standardMenuInactiveColor = Vector4(Vector3(0.2f), 0.5f);
	languageMenuActiveColor = Vector4(Vector3(1.0f), 1.0f);
	languageMenuInactiveColor = Vector4(Vector3(1.0f), 0.5f);
	menuItemActiveColor = standardMenuActiveColor;
	menuItemInactiveColor = standardMenuInactiveColor;

	menuItemIndex = -1;
	currentMenu = nullptr;
	currentMenuItem = nullptr;
	previousMenuItem = nullptr;
	previousMenu = nullptr;

	menuSelectorImage = new UIImage();
	menuSelectorImage->setAnchor(Anchor::TOP_LEFT);
	menuSelectorImage->setTexture(hudSpriteSheetTexture);
	menuSelectorImage->setTextureBounds(normalizeTextureBounds(hudTextureAtlas.getBounds("menu-selector"), hudTextureAtlasBounds));
	menuSelectorImage->setTintColor(menuItemActiveColor);

	// Build main menu
	mainMenu = new Menu();
	mainMenuContinueItem = mainMenu->addItem();
	mainMenuNewGameItem = mainMenu->addItem();
	mainMenuColoniesItem = mainMenu->addItem();
	mainMenuSettingsItem = mainMenu->addItem();
	mainMenuQuitItem = mainMenu->addItem();
	
	// Build settings menu
	settingsMenu = new Menu();
	settingsMenuControlsItem = settingsMenu->addItem();
	settingsMenuFullscreenItem = settingsMenu->addItem();
	settingsMenuVSyncItem = settingsMenu->addItem();
	settingsMenuLanguageItem = settingsMenu->addItem();
	settingsMenuBackItem = settingsMenu->addItem();

	// Build controls menu
	controlsMenu = new Menu();
	controlsMenuMoveForwardItem = controlsMenu->addItem();
	controlsMenuMoveLeftItem = controlsMenu->addItem();
	controlsMenuMoveBackItem = controlsMenu->addItem();
	controlsMenuMoveRightItem = controlsMenu->addItem();
	controlsMenuChangeToolItem = controlsMenu->addItem();
	controlsMenuUseToolItem = controlsMenu->addItem();
	controlsMenuAdjustCameraItem = controlsMenu->addItem();
	controlsMenuPauseItem = controlsMenu->addItem();
	controlsMenuToggleFullscreenItem = controlsMenu->addItem();
	controlsMenuTakeScreenshotItem = controlsMenu->addItem();
	controlsMenuResetToDefaultItem = controlsMenu->addItem();
	controlsMenuBackItem = controlsMenu->addItem();

	// Build pause menu
	pauseMenu = new Menu();
	pauseMenuResumeItem = pauseMenu->addItem();
	pauseMenuSettingsItem = pauseMenu->addItem();
	pauseMenuMainMenuItem = pauseMenu->addItem();
	pauseMenuQuitItem = pauseMenu->addItem();

	// Build language menu
	languageMenu = new Menu();
	for (std::size_t i = 0; i < languageCount; ++i)
	{
		MenuItem* item = languageMenu->addItem();
		item->setActivatedCallback
		(
			[this, i]()
			{
				changeLanguage(i);
				languageSelected();
			}
		);

		item->getContainer()->setTintColor(languageMenuInactiveColor);
		item->getContainer()->setMouseOverCallback(std::bind(&Game::selectMenuItem, this, i, true));
		item->getContainer()->setMouseMovedCallback(std::bind(&Game::selectMenuItem, this, i, true));
		item->getContainer()->setMousePressedCallback(std::bind(&Game::activateMenuItem, this));

		languageMenuItems.push_back(item);
	}

	// Setup main menu callbacks
	mainMenuContinueItem->setActivatedCallback(std::bind(&Game::continueGame, this));
	mainMenuNewGameItem->setActivatedCallback(std::bind(&Game::newGame, this));
	mainMenuSettingsItem->setActivatedCallback(std::bind(&Game::openMenu, this, settingsMenu, 0));
	mainMenuQuitItem->setActivatedCallback(std::bind(&Application::close, this, EXIT_SUCCESS));

	// Setup settings menu callbacks
	settingsMenuControlsItem->setActivatedCallback(std::bind(&Game::openMenu, this, controlsMenu, 0));
	settingsMenuFullscreenItem->setActivatedCallback(std::bind(&Game::toggleFullscreen, this));
	settingsMenuVSyncItem->setActivatedCallback(std::bind(&Game::toggleVSync, this));
	settingsMenuLanguageItem->setActivatedCallback(std::bind(&Game::nextLanguage, this));
	settingsMenuBackItem->setActivatedCallback(std::bind(&Game::openMenu, this, mainMenu, 3));

	// Setup controls menu callbacks
	controlsMenuMoveForwardItem->setActivatedCallback(std::bind(&Game::remapControl, this, &moveForwardControl)); 

	controlsMenuMoveLeftItem->setActivatedCallback(std::bind(&Game::remapControl, this, &moveLeftControl));
	controlsMenuMoveBackItem->setActivatedCallback(std::bind(&Game::remapControl, this, &moveBackControl));
	controlsMenuMoveRightItem->setActivatedCallback(std::bind(&Game::remapControl, this, &moveRightControl));
	controlsMenuChangeToolItem->setActivatedCallback(std::bind(&Game::remapControl, this, &changeToolControl));
	controlsMenuUseToolItem->setActivatedCallback(std::bind(&Game::remapControl, this, &useToolControl));
	controlsMenuAdjustCameraItem->setActivatedCallback(std::bind(&Game::remapControl, this, &adjustCameraControl));
	controlsMenuPauseItem->setActivatedCallback(std::bind(&Game::remapControl, this, &pauseControl));
	controlsMenuToggleFullscreenItem->setActivatedCallback(std::bind(&Game::remapControl, this, &toggleFullscreenControl));
	controlsMenuTakeScreenshotItem->setActivatedCallback(std::bind(&Game::remapControl, this, &takeScreenshotControl));
	controlsMenuResetToDefaultItem->setActivatedCallback(std::bind(&Game::resetControls, this));
	controlsMenuBackItem->setActivatedCallback(std::bind(&Game::openMenu, this, settingsMenu, 0));

	// Setup pause menu callbacks
	pauseMenuResumeItem->setActivatedCallback(std::bind(&Game::togglePause, this));
	pauseMenuSettingsItem->setActivatedCallback(std::bind(&Game::openMenu, this, settingsMenu, 0));
	pauseMenuMainMenuItem->setActivatedCallback(std::bind(&Game::returnToMainMenu, this));
	pauseMenuQuitItem->setActivatedCallback(std::bind(&Application::close, this, EXIT_SUCCESS));

	// Setup standard callbacks for all menu items
	for (std::size_t i = 0; i < mainMenu->getItems()->size(); ++i)
	{
		MenuItem* item = (*mainMenu->getItems())[i];
		item->getContainer()->setTintColor(menuItemInactiveColor);
		item->getContainer()->setMouseOverCallback(std::bind(&Game::selectMenuItem, this, i, true));
		item->getContainer()->setMouseMovedCallback(std::bind(&Game::selectMenuItem, this, i, true));
		item->getContainer()->setMousePressedCallback(std::bind(&Game::activateMenuItem, this));
	}

	for (std::size_t i = 0; i < settingsMenu->getItems()->size(); ++i)
	{
		MenuItem* item = (*settingsMenu->getItems())[i];
		item->getContainer()->setTintColor(menuItemInactiveColor);
		item->getContainer()->setMouseOverCallback(std::bind(&Game::selectMenuItem, this, i, true));
		item->getContainer()->setMouseMovedCallback(std::bind(&Game::selectMenuItem, this, i, true));
		item->getContainer()->setMousePressedCallback(std::bind(&Game::activateMenuItem, this));
	}

	for (std::size_t i = 0; i < controlsMenu->getItems()->size(); ++i)
	{
		MenuItem* item = (*controlsMenu->getItems())[i];
		item->getContainer()->setTintColor(menuItemInactiveColor);
		item->getContainer()->setMouseOverCallback(std::bind(&Game::selectMenuItem, this, i, true));
		item->getContainer()->setMouseMovedCallback(std::bind(&Game::selectMenuItem, this, i, true));
		item->getContainer()->setMousePressedCallback(std::bind(&Game::activateMenuItem, this));
	}

	for (std::size_t i = 0; i < pauseMenu->getItems()->size(); ++i)
	{
		MenuItem* item = (*pauseMenu->getItems())[i];
		item->getContainer()->setTintColor(menuItemInactiveColor);
		item->getContainer()->setMouseOverCallback(std::bind(&Game::selectMenuItem, this, i, true));
		item->getContainer()->setMouseMovedCallback(std::bind(&Game::selectMenuItem, this, i, true));
		item->getContainer()->setMousePressedCallback(std::bind(&Game::activateMenuItem, this));
	}

	// Set fonts for all menus
	mainMenu->setFonts(menuFont);
	settingsMenu->setFonts(menuFont);
	controlsMenu->setFonts(menuFont);
	pauseMenu->setFonts(menuFont);

	AnimationChannel<float>* channel;

	// Setup splash fade-in animation
	splashFadeInClip.setInterpolator(easeOutCubic<float>);
	channel = splashFadeInClip.addChannel(0);
	channel->insertKeyframe(0.0f, 0.0f);
	channel->insertKeyframe(1.0f, 1.0f);
	channel->insertKeyframe(3.0f, 1.0f);
	splashFadeInAnimation.setClip(&splashFadeInClip);
	splashFadeInAnimation.setTimeFrame(splashFadeInClip.getTimeFrame());
	splashFadeInAnimation.setAnimateCallback
	(
		[this](std::size_t id, float opacity)
		{
			Vector3 color = Vector3(splashImage->getTintColor());
			splashImage->setTintColor(Vector4(color, opacity));
		}
	);
	splashFadeInAnimation.setEndCallback
	(
		[this]()
		{
			splashFadeOutAnimation.rewind();
			splashFadeOutAnimation.play();
		}
	);

	// Setup splash fade-out animation
	splashFadeOutClip.setInterpolator(easeOutCubic<float>);
	channel = splashFadeOutClip.addChannel(0);
	channel->insertKeyframe(0.0f, 1.0f);
	channel->insertKeyframe(1.0f, 0.0f);
	channel->insertKeyframe(1.5f, 0.0f);
	splashFadeOutAnimation.setClip(&splashFadeOutClip);
	splashFadeOutAnimation.setTimeFrame(splashFadeOutClip.getTimeFrame());
	splashFadeOutAnimation.setAnimateCallback
	(
		[this](std::size_t id, float opacity)
		{
			Vector3 color = Vector3(splashImage->getTintColor());
			splashImage->setTintColor(Vector4(color, opacity));
		}
	);
	splashFadeOutAnimation.setEndCallback(std::bind(&StateMachine::changeState, this, &titleState));

	// Ant-hill zoom animation
	antHillZoomClip.setInterpolator(easeOutCubic<float>);
	channel = antHillZoomClip.addChannel(0);
	channel->insertKeyframe(0.0f, 0.0f);
	channel->insertKeyframe(3.0f, 40.0f);
	antHillZoomAnimation.setClip(&antHillZoomClip);
	antHillZoomAnimation.setTimeFrame(antHillZoomClip.getTimeFrame());
	antHillZoomAnimation.setAnimateCallback
	(
		[this](std::size_t id, float distance)
		{
			orbitCam->setFocalDistance(distance);
			orbitCam->setTargetFocalDistance(distance);
		}
	);

	// Menu fade animation
	menuFadeInClip.setInterpolator(easeOutCubic<float>);
	channel = menuFadeInClip.addChannel(0);
	channel->insertKeyframe(0.0f, 0.0f);
	channel->insertKeyframe(3.0f, 0.0f);
	channel->insertKeyframe(5.0f, 1.0f);
	menuFadeOutClip.setInterpolator(easeOutCubic<float>);
	channel = menuFadeOutClip.addChannel(0);
	channel->insertKeyframe(0.0f, 1.0f);
	channel->insertKeyframe(0.125f, 0.0f);

	menuFadeAnimation.setClip(&menuFadeInClip);
	menuFadeAnimation.setTimeFrame(menuFadeInClip.getTimeFrame());
	menuFadeAnimation.setAnimateCallback
	(
		[this](std::size_t id, float opacity)
		{
			currentMenu->getContainer()->setTintColor(Vector4(opacity));
		}
	);

	animator.addAnimation(&menuFadeAnimation);

	// Menu selector animation
	menuSelectorSlideClip.setInterpolator(easeOutCubic<float>);
	menuSelectorSlideAnimation.setClip(&menuSelectorSlideClip);
	menuSelectorSlideAnimation.setAnimateCallback
	(
		[this](std::size_t id, float offset)
		{
			Vector2 translation = menuSelectorImage->getTranslation();
			translation.y = offset;
			menuSelectorImage->setTranslation(translation);
		}
	);

	animator.addAnimation(&menuSelectorSlideAnimation);

	// Menu item select animation
	menuItemSelectClip.setInterpolator(easeOutCubic<Vector4>);
	menuItemSelectAnimation.setClip(&menuItemSelectClip);
	menuItemSelectAnimation.setAnimateCallback
	(
		[this](std::size_t id, const Vector4& color)
		{
			currentMenuItem->getContainer()->setTintColor(color);
		}
	);

	// Menu item deselect animation
	menuItemDeselectClip.setInterpolator(easeOutCubic<Vector4>);
	menuItemDeselectAnimation.setClip(&menuItemDeselectClip);
	menuItemDeselectAnimation.setAnimateCallback
	(
		[this](std::size_t id, const Vector4& color)
		{
			previousMenuItem->getContainer()->setTintColor(color);
		}
	);

	animator.addAnimation(&menuItemSelectAnimation);
	animator.addAnimation(&menuItemDeselectAnimation);
	
	// Construct fade-in animation clip
	fadeInClip.setInterpolator(easeOutCubic<float>);
	channel = fadeInClip.addChannel(0);
	channel->insertKeyframe(0.0f, 1.0f);
	channel->insertKeyframe(1.0f, 0.0f);

	// Construct fade-out animation clip
	fadeOutClip.setInterpolator(easeOutCubic<float>);
	channel = fadeOutClip.addChannel(0);
	channel->insertKeyframe(0.0f, 0.0f);
	channel->insertKeyframe(1.0f, 1.0f);

	// Setup fade-in animation callbacks
	fadeInAnimation.setAnimateCallback
	(
		[this](std::size_t id, float opacity)
		{
			Vector3 color = Vector3(blackoutImage->getTintColor());
			blackoutImage->setTintColor(Vector4(color, opacity));
		}
	);
	fadeInAnimation.setEndCallback
	(
		[this]()
		{
			blackoutImage->setVisible(false);
			if (fadeInEndCallback != nullptr)
			{
				fadeInEndCallback();
			}
		}
	);

	// Setup fade-out animation callbacks
	fadeOutAnimation.setAnimateCallback
	(
		[this](std::size_t id, float opacity)
		{
			Vector3 color = Vector3(blackoutImage->getTintColor());
			blackoutImage->setTintColor(Vector4(color, opacity));
		}
	);
	fadeOutAnimation.setEndCallback
	(
		[this]()
		{
			blackoutImage->setVisible(false);
			if (fadeOutEndCallback != nullptr)
			{
				fadeOutEndCallback();
			}
		}
	);

	animator.addAnimation(&fadeInAnimation);
	animator.addAnimation(&fadeOutAnimation);

	// Construct camera flash animation clip
	cameraFlashClip.setInterpolator(easeOutQuad<float>);
	channel = cameraFlashClip.addChannel(0);
	channel->insertKeyframe(0.0f, 1.0f);
	channel->insertKeyframe(1.0f, 0.0f);

	// Setup camera flash animation
	float flashDuration = 0.5f;
	cameraFlashAnimation.setSpeed(1.0f / flashDuration);
	cameraFlashAnimation.setLoop(false);
	cameraFlashAnimation.setClip(&cameraFlashClip);
	cameraFlashAnimation.setTimeFrame(cameraFlashClip.getTimeFrame());
	cameraFlashAnimation.setAnimateCallback
	(
		[this](std::size_t id, float opacity)
		{
			cameraFlashImage->setTintColor(Vector4(Vector3(1.0f), opacity));
		}
	);
	cameraFlashAnimation.setStartCallback
	(
		[this]()
		{
			cameraFlashImage->setVisible(true);
			cameraFlashImage->setTintColor(Vector4(1.0f));
			cameraFlashImage->resetTweens();
		}
	);
	cameraFlashAnimation.setEndCallback
	(
		[this]()
		{
			cameraFlashImage->setVisible(false);
		}
	);
	animator.addAnimation(&cameraFlashAnimation);

	// Setup UI scene
	uiScene->addObject(uiBatch);
	uiScene->addObject(&uiCamera);

	// Setup UI camera
	uiCamera.lookAt(Vector3(0), Vector3(0, 0, -1), Vector3(0, 1, 0));
	uiCamera.resetTweens();
	uiCamera.setCompositor(&uiCompositor);
	uiCamera.setCompositeIndex(0);
	uiCamera.setCullingEnabled(false);
	
	restringUI();
	resizeUI(w, h);
}

void Game::setupControls()
{
	// Get keyboard and mouse
	keyboard = deviceManager->getKeyboards()->front();
	mouse = deviceManager->getMice()->front();

	// Build the master control set
	controls.addControl(&exitControl);
	controls.addControl(&toggleFullscreenControl);
	controls.addControl(&takeScreenshotControl);
	controls.addControl(&menuUpControl);
	controls.addControl(&menuDownControl);
	controls.addControl(&menuLeftControl);
	controls.addControl(&menuRightControl);
	controls.addControl(&menuActivateControl);
	controls.addControl(&menuBackControl);
	controls.addControl(&moveForwardControl);
	controls.addControl(&moveBackControl);
	controls.addControl(&moveLeftControl);
	controls.addControl(&moveRightControl);
	controls.addControl(&zoomInControl);
	controls.addControl(&zoomOutControl);
	controls.addControl(&orbitCCWControl);
	controls.addControl(&orbitCWControl);
	controls.addControl(&adjustCameraControl);
	controls.addControl(&dragCameraControl);
	controls.addControl(&pauseControl);
	controls.addControl(&changeToolControl);
	controls.addControl(&useToolControl);
	controls.addControl(&toggleEditModeControl);

	// Build the system control set
	systemControls.addControl(&exitControl);
	systemControls.addControl(&toggleFullscreenControl);
	systemControls.addControl(&takeScreenshotControl);

	// Build the menu control set
	menuControls.addControl(&menuUpControl);
	menuControls.addControl(&menuDownControl);
	menuControls.addControl(&menuLeftControl);
	menuControls.addControl(&menuRightControl);
	menuControls.addControl(&menuActivateControl);
	menuControls.addControl(&menuBackControl);

	// Build the camera control set
	cameraControls.addControl(&moveForwardControl);
	cameraControls.addControl(&moveBackControl);
	cameraControls.addControl(&moveLeftControl);
	cameraControls.addControl(&moveRightControl);
	cameraControls.addControl(&zoomInControl);
	cameraControls.addControl(&zoomOutControl);
	cameraControls.addControl(&orbitCCWControl);
	cameraControls.addControl(&orbitCWControl);
	cameraControls.addControl(&adjustCameraControl);
	cameraControls.addControl(&dragCameraControl);
	cameraControls.addControl(&pauseControl);

	// Build the tool control set
	toolControls.addControl(&changeToolControl);
	toolControls.addControl(&useToolControl);

	// Build the editor control set
	editorControls.addControl(&toggleEditModeControl);

	// Setup control callbacks
	menuDownControl.setActivatedCallback(std::bind(&Game::selectNextMenuItem, this));
	menuUpControl.setActivatedCallback(std::bind(&Game::selectPreviousMenuItem, this));
	menuActivateControl.setActivatedCallback(std::bind(&Game::activateMenuItem, this));
	menuBackControl.setActivatedCallback(std::bind(&Game::activateLastMenuItem, this));
	pauseControl.setActivatedCallback(std::bind(&Game::togglePause, this));
	exitControl.setActivatedCallback(std::bind(&Application::close, this, EXIT_SUCCESS));
	toggleFullscreenControl.setActivatedCallback(std::bind(&Game::toggleFullscreen, this));
	takeScreenshotControl.setActivatedCallback(std::bind(&Game::queueScreenshot, this));

	// Build map of control names
	controlNameMap["exit"] = &exitControl;
	controlNameMap["toggle-fullscreen"] = &toggleFullscreenControl;
	controlNameMap["take-screenshot"] = &takeScreenshotControl;
	controlNameMap["menu-up"] = &menuUpControl;
	controlNameMap["menu-down"] = &menuDownControl;
	controlNameMap["menu-left"] = &menuLeftControl;
	controlNameMap["menu-right"] = &menuRightControl;
	controlNameMap["menu-activate"] = &menuActivateControl;
	controlNameMap["menu-back"] = &menuBackControl;
	controlNameMap["move-forward"] = &moveForwardControl;
	controlNameMap["move-back"] = &moveBackControl;
	controlNameMap["move-left"] = &moveLeftControl;
	controlNameMap["move-right"] = &moveRightControl;
	controlNameMap["zoom-in"] = &zoomInControl;
	controlNameMap["zoom-out"] = &zoomOutControl;
	controlNameMap["orbit-ccw"] = &orbitCCWControl;
	controlNameMap["orbit-cw"] = &orbitCWControl;
	controlNameMap["adjust-camera"] = &adjustCameraControl;
	controlNameMap["drag-camera"] = &dragCameraControl;
	controlNameMap["pause"] = &pauseControl;
	controlNameMap["change-tool"] = &changeToolControl;
	controlNameMap["use-tool"] = &useToolControl;
	controlNameMap["toggle-edit-mode"] = &toggleEditModeControl;

	// Load control profile
	if (pathExists(controlsPath + (*controlProfileName) + ".csv"))
	{
		loadControlProfile(*controlProfileName);
	}
	else
	{
		loadControlProfile("default-keyboard-controls");
		saveControlProfile(*controlProfileName);
	}

	// Setup input mapper
	inputMapper = new InputMapper(&eventDispatcher);
	inputMapper->setCallback(std::bind(&Game::inputMapped, this, std::placeholders::_1));
	inputMapper->setControl(nullptr);
	inputMapper->setEnabled(false);
}

void Game::setupGameplay()
{
	// Setup step scheduler
	double maxFrameDuration = 0.25;
	double stepFrequency = 60.0;
	stepScheduler.setMaxFrameDuration(maxFrameDuration);
	stepScheduler.setStepFrequency(stepFrequency);
	timestep = stepScheduler.getStepPeriod();

	// Setup camera rigs
	orbitCam = new OrbitCam();
	orbitCam->attachCamera(&camera);
	freeCam = new FreeCam();
	freeCam->attachCamera(&camera);
	cameraRig = orbitCam;
}

void Game::loadSettings()
{
	firstRun = false;

	// Load settings
	try
	{
		settings = resourceManager->load<ParameterDict>("settings.csv");
	}
	catch (const std::exception& e)
	{
		logger->warning("No user settings found. First run assumed.");
		firstRun = true;

		try
		{
			settings = resourceManager->load<ParameterDict>("default-settings.csv");
		}
		catch (const std::exception& e)
		{
			logger->error("Failed to load default settings.");
			settings = new ParameterDict();
		}
	}

	// Read settings
	language = settings->get<std::string>("language");
	windowPosition = settings->get<int, 2>("window-position");
	windowResolution = settings->get<int, 2>("window-resolution");
	fullscreenResolution = settings->get<int, 2>("window-resolution");
	fullscreen = settings->get<bool>("fullscreen");
	vsync = settings->get<bool>("vsync");
	fontSize = settings->get<float>("font-size");
	controlProfileName = settings->get<std::string>("control-profile");

	// Use default values for unset parameters
	{
		if (language == std::nullopt)
		{
			language = "en-us";
		}

		// Get display dimensions
		const Display* display = deviceManager->getDisplays()->front();
		int displayWidth = std::get<0>(display->getDimensions());
		int displayHeight = std::get<1>(display->getDimensions());

		if (windowResolution == std::nullopt)
		{
			const float windowResolutionRatio = 5.0f / 6.0f;
			int windowWidth = static_cast<int>(static_cast<float>(displayWidth) * windowResolutionRatio);
			int windowHeight = static_cast<int>(static_cast<float>(displayHeight) * windowResolutionRatio);

			windowResolution = {windowWidth, windowHeight};
		}

		if (windowPosition == std::nullopt)
		{
			int windowX = std::get<0>(display->getPosition()) + displayWidth / 2 - (*windowResolution)[0] / 2;
			int windowY = std::get<1>(display->getPosition()) + displayHeight / 2 - (*windowResolution)[1] / 2;

			windowPosition = {windowX, windowY};
		}

		if (fullscreen == std::nullopt)
		{
			fullscreen = true;
		}

		if (vsync == std::nullopt)
		{
			vsync = true;
		}

		if (fontSize == std::nullopt)
		{
			fontSize = 14.0f;
		}

		if (controlProfileName == std::nullopt)
		{
			controlProfileName = "controls";
		}
	}
}

void Game::saveSettings()
{
	// Update settings
	settings->set("language", *language);
	settings->set("window-position", *windowPosition);
	settings->set("window-resolution", *windowResolution);
	settings->set("fullscreen-resolution", *fullscreenResolution);
	settings->set("fullscreen", *fullscreen);
	settings->set("vsync", *vsync);
	settings->set("font-size", *fontSize);
	settings->set("control-profile", *controlProfileName);

	// Form full path to settings file
	std::string settingsPath = configPath + "settings.csv";

	// Save settings
	resourceManager->save<ParameterDict>(settings, settingsPath);
}

void Game::loadStrings()
{
	// Read strings file
	stringTable = resourceManager->load<StringTable>("strings.csv");

	// Build string table index
	stringTableIndex = createIndex(*stringTable);
}

void Game::loadFonts()
{
	// If the language selection fonts haven't been loaded
	if (languageSelectionFonts.empty())
	{
		// Load one font for each available language
		for (std::size_t i = 0; i < languageCount; ++i)
		{
			// Get language name and font filename for that language
			std::string languageName = getString("language-name", i);
			std::string fontFilename = getString("menu-font-filename", i);

			// Check if language has a name
			if (languageName.empty())
			{
				logger->error("Language #" + std::to_string(i) + " has no name string.");
				languageSelectionFonts.push_back(nullptr);
				continue;
			}

			// Check if language has a font filename
			if (fontFilename.empty())
			{
				logger->error("No font for language #" + std::to_string(i) + ".");
				languageSelectionFonts.push_back(nullptr);
				continue;
			}

			// Load typeface for the font
			Typeface* typeface = resourceManager->load<Typeface>(fontFilename);

			// Create a font at default font size
			Font* font = typeface->createFont(fontSizePX);

			// Build a character set containing only the characters in the language name
			std::set<char32_t> characterSet;
			std::u32string languageNameUTF32 = toUTF32(languageName);
			for (char32_t charcode: languageNameUTF32)
			{
				characterSet.emplace(charcode);
			}

			// Load glyphs for all characters in the character set
			typeface->loadCharset(font, characterSet);

			// Unload the typeface
			resourceManager->unload(fontFilename);

			// Add the font to the language selection fonts
			languageSelectionFonts.push_back(font);
		}
	}

	// Get filenames of fonts
	std::string menuFontFilename = getString("menu-font-filename");
	std::string debugFontFilename = "inconsolata-bold.ttf";

	// Load debugging font
	if (!debugFont)
	{
		// Load debug font typeface
		Typeface* debugTypeface = resourceManager->load<Typeface>(debugFontFilename);

		// Create debug font
		debugFont = debugTypeface->createFont(fontSizePX);

		// Load basic latin characyer set
		debugTypeface->loadCharset(debugFont, UnicodeRange::BASIC_LATIN);

		// Unload debug font typeface
		resourceManager->unload(debugFontFilename);
	}

	// Load menu font typeface
	Typeface* menuTypeface = resourceManager->load<Typeface>(menuFontFilename);

	// Create menu font
	menuFont = menuTypeface->createFont(fontSizePX);

	// Load basic latin character set
	menuTypeface->loadCharset(menuFont, UnicodeRange::BASIC_LATIN);

	// Build character set for all strings in current language
	std::set<char32_t> characterSet;
	for (const StringTableRow& row: *stringTable)
	{
		// Convert to UTF-8 string to UTF-32
		std::u32string string = toUTF32(row[currentLanguageIndex + 2]);

		// Add each character in the string to the charater set
		for (char32_t charcode: string)
		{
			characterSet.emplace(charcode);
		}
	}

	// Load custom character set
	menuTypeface->loadCharset(menuFont, characterSet);

	// Unload menu typeface
	resourceManager->unload(menuFontFilename);
}

void Game::loadControlProfile(const std::string& profileName)
{
	// Load control profile
	std::string controlProfilePath = profileName + ".csv";
	StringTable* controlProfile = resourceManager->load<StringTable>(controlProfilePath);

	for (const StringTableRow& row: *controlProfile)
	{
		// Skip empty rows and comments
		if (row.empty() || row[0].empty() || row[0][0] == '#')
		{
			continue;
		}

		// Get control name
		const std::string& controlName = row[0];

		// Lookup control in control name map
		auto it = controlNameMap.find(controlName);
		if (it == controlNameMap.end())
		{
			logger->warning("Game::loadControlProfile(): Unknown control name \"" + controlName + "\"\n");
			continue;
		}

		// Get pointer to the control
		Control* control = it->second;

		// Determine type of input mapping
		const std::string& deviceType = row[1];
		if (deviceType == "keyboard")
		{
			const std::string& eventType = row[2];
			const std::string& scancodeName = row[3];

			// Get scancode from string
			Scancode scancode = Keyboard::getScancodeFromName(scancodeName.c_str());

			// Map control
			if (scancode != Scancode::UNKNOWN)
			{
				inputRouter->addMapping(KeyMapping(control, keyboard, scancode));
			}
		}
		else if (deviceType == "mouse")
		{
			const std::string& eventType = row[2];

			if (eventType == "motion")
			{
				const std::string& axisName = row[3];

				// Get axis from string
				MouseMotionAxis axis;
				bool negative = (axisName.find('-') != std::string::npos);
				if (axisName.find('x') != std::string::npos)
				{
					axis = (negative) ? MouseMotionAxis::NEGATIVE_X : MouseMotionAxis::POSITIVE_X;
				}
				else if (axisName.find('y') != std::string::npos)
				{
					axis = (negative) ? MouseMotionAxis::NEGATIVE_Y : MouseMotionAxis::POSITIVE_Y;
				}
				else
				{
					logger->warning("Game::loadControlProfile(): Unknown mouse motion axis \"" + axisName + "\"\n");
					continue;
				}

				// Map control
				inputRouter->addMapping(MouseMotionMapping(control, mouse, axis));
			}
			else if (eventType == "wheel")
			{
				const std::string& axisName = row[3];

				// Get axis from string
				MouseWheelAxis axis;
				bool negative = (axisName.find('-') != std::string::npos);
				if (axisName.find('x') != std::string::npos)
				{
					axis = (negative) ? MouseWheelAxis::NEGATIVE_X : MouseWheelAxis::POSITIVE_X;
				}
				else if (axisName.find('y') != std::string::npos)
				{
					axis = (negative) ? MouseWheelAxis::NEGATIVE_Y : MouseWheelAxis::POSITIVE_Y;
				}
				else
				{
					logger->warning("Game::loadControlProfile(): Unknown mouse wheel axis \"" + axisName + "\"\n");
					continue;
				}

				// Map control
				inputRouter->addMapping(MouseWheelMapping(control, mouse, axis));
			}
			else if (eventType == "button")
			{
				const std::string& buttonName = row[3];

				// Get button from string
				int button;
				std::stringstream stream;
				stream << buttonName;
				stream >> button;

				// Map control
				inputRouter->addMapping(MouseButtonMapping(control, mouse, button));
			}
			else
			{
				logger->warning("Game::loadControlProfile(): Unknown mouse event type \"" + eventType + "\"\n");
				continue;
			}
		}
		else if (deviceType == "gamepad")
		{
			const std::string& eventType = row[2];
			if (eventType == "axis")
			{
				std::string axisName = row[3];

				// Determine whether axis is negative or positive
				bool negative = (axisName.find('-') != std::string::npos);

				// Remove sign from axis name
				std::size_t plusPosition = axisName.find('+');
				std::size_t minusPosition = axisName.find('-');
				if (plusPosition != std::string::npos)
				{
					axisName.erase(plusPosition);
				}
				else if (minusPosition != std::string::npos)
				{
					axisName.erase(minusPosition);
				}

				// Get axis from string
				int axis;
				std::stringstream stream;
				stream << axisName;
				stream >> axis;

				// Map control to each gamepad
				const std::list<Gamepad*>* gamepads = deviceManager->getGamepads();
				for (Gamepad* gamepad: *gamepads)
				{
					inputRouter->addMapping(GamepadAxisMapping(control, gamepad, axis, negative));
				}
			}
			else if (eventType == "button")
			{
				const std::string& buttonName = row[3];

				// Get button from string
				int button;
				std::stringstream stream;
				stream << buttonName;
				stream >> button;

				// Map control to each gamepad
				const std::list<Gamepad*>* gamepads = deviceManager->getGamepads();
				for (Gamepad* gamepad: *gamepads)
				{
					inputRouter->addMapping(GamepadButtonMapping(control, gamepad, button));
				}
			}
			else
			{
				logger->warning("Game::loadControlProfile(): Unknown gamepad event type \"" + eventType + "\"\n");
				continue;
			}
		}
		else
		{
			logger->warning("Game::loadControlProfile(): Unknown input device type \"" + deviceType + "\"\n");
			continue;
		}
	}
}

void Game::saveControlProfile(const std::string& profileName)
{
	// Build control profile string table
	StringTable* table = new StringTable();
	for (auto it = controlNameMap.begin(); it != controlNameMap.end(); ++it)
	{
		// Get control name
		const std::string& controlName = it->first;

		// Get pointer to the control
		Control* control = it->second;

		// Look up list of mappings for the control
		const std::list<InputMapping*>* mappings = inputRouter->getMappings(control);
		if (!mappings)
		{
			continue;
		}

		// For each input mapping
		for (const InputMapping* mapping: *mappings)
		{
			// Add row to the table
			table->push_back(StringTableRow());
			StringTableRow* row = &table->back();

			// Add control name column
			row->push_back(controlName);

			switch (mapping->getType())
			{
				case InputMappingType::KEY:
				{
					const KeyMapping* keyMapping = static_cast<const KeyMapping*>(mapping);
					row->push_back("keyboard");
					row->push_back("key");

					std::string scancodeName = std::string("\"") + std::string(Keyboard::getScancodeName(keyMapping->scancode)) + std::string("\"");
					row->push_back(scancodeName);
					break;
				}

				case InputMappingType::MOUSE_MOTION:
				{
					const MouseMotionMapping* mouseMotionMapping = static_cast<const MouseMotionMapping*>(mapping);
					row->push_back("mouse");
					row->push_back("motion");

					std::string axisName;
					if (mouseMotionMapping->axis == MouseMotionAxis::POSITIVE_X)
					{
						axisName = "+x";
					}
					else if (mouseMotionMapping->axis == MouseMotionAxis::NEGATIVE_X)
					{
						axisName = "-x";
					}
					else if (mouseMotionMapping->axis == MouseMotionAxis::POSITIVE_Y)
					{
						axisName = "+y";
					}
					else
					{
						axisName = "-y";
					}

					row->push_back(axisName);

					break;
				}

				case InputMappingType::MOUSE_WHEEL:
				{
					const MouseWheelMapping* mouseWheelMapping = static_cast<const MouseWheelMapping*>(mapping);

					row->push_back("mouse");
					row->push_back("wheel");

					std::string axisName;
					if (mouseWheelMapping->axis == MouseWheelAxis::POSITIVE_X)
					{
						axisName = "+x";
					}
					else if (mouseWheelMapping->axis == MouseWheelAxis::NEGATIVE_X)
					{
						axisName = "-x";
					}
					else if (mouseWheelMapping->axis == MouseWheelAxis::POSITIVE_Y)
					{
						axisName = "+y";
					}
					else
					{
						axisName = "-y";
					}

					row->push_back(axisName);
					break;
				}

				case InputMappingType::MOUSE_BUTTON:
				{
					const MouseButtonMapping* mouseButtonMapping = static_cast<const MouseButtonMapping*>(mapping);

					row->push_back("mouse");
					row->push_back("button");

					std::string buttonName;
					std::stringstream stream;
					stream << static_cast<int>(mouseButtonMapping->button);
					stream >> buttonName;
					row->push_back(buttonName);
					break;
				}

				case InputMappingType::GAMEPAD_AXIS:
				{
					const GamepadAxisMapping* gamepadAxisMapping = static_cast<const GamepadAxisMapping*>(mapping);

					row->push_back("gamepad");
					row->push_back("axis");

					std::stringstream stream;
					if (gamepadAxisMapping->negative)
					{
						stream << "-";
					}
					else
					{
						stream << "+";
					}
					stream << gamepadAxisMapping->axis;

					std::string axisName;
					stream >> axisName;
					row->push_back(axisName);
					break;
				}

				case InputMappingType::GAMEPAD_BUTTON:
				{
					const GamepadButtonMapping* gamepadButtonMapping = static_cast<const GamepadButtonMapping*>(mapping);

					row->push_back("gamepad");
					row->push_back("button");

					std::string buttonName;
					std::stringstream stream;
					stream << static_cast<int>(gamepadButtonMapping->button);
					stream >> buttonName;
					row->push_back(buttonName);
					break;
				}

				default:
					break;
			}
		}
	}

	// Form full path to control profile file
	std::string controlProfilePath = controlsPath + profileName + ".csv";

	// Save control profile
	resourceManager->save<StringTable>(table, controlProfilePath);

	// Free control profile string table
	delete table;
}

std::array<std::string, 3> Game::getInputMappingStrings(const InputMapping* mapping)
{
	std::string deviceString;
	std::string typeString;
	std::string eventString;

	switch (mapping->getType())
	{
		case InputMappingType::KEY:
		{
			const KeyMapping* keyMapping = static_cast<const KeyMapping*>(mapping);
			deviceString = "keyboard";
			typeString = "key";
			eventString = std::string(Keyboard::getScancodeName(keyMapping->scancode));
			break;
		}

		case InputMappingType::MOUSE_MOTION:
		{
			const MouseMotionMapping* mouseMotionMapping = static_cast<const MouseMotionMapping*>(mapping);
			deviceString = "mouse";
			eventString = "motion";

			if (mouseMotionMapping->axis == MouseMotionAxis::POSITIVE_X)
			{
				eventString = "+x";
			}
			else if (mouseMotionMapping->axis == MouseMotionAxis::NEGATIVE_X)
			{
				eventString = "-x";
			}
			else if (mouseMotionMapping->axis == MouseMotionAxis::POSITIVE_Y)
			{
				eventString = "+y";
			}
			else
			{
				eventString = "-y";
			}

			break;
		}

		case InputMappingType::MOUSE_WHEEL:
		{
			const MouseWheelMapping* mouseWheelMapping = static_cast<const MouseWheelMapping*>(mapping);

			deviceString = "mouse";
			typeString = "wheel";

			if (mouseWheelMapping->axis == MouseWheelAxis::POSITIVE_X)
			{
				eventString = "+x";
			}
			else if (mouseWheelMapping->axis == MouseWheelAxis::NEGATIVE_X)
			{
				eventString = "-x";
			}
			else if (mouseWheelMapping->axis == MouseWheelAxis::POSITIVE_Y)
			{
				eventString = "+y";
			}
			else
			{
				eventString = "-y";
			}

			break;
		}

		case InputMappingType::MOUSE_BUTTON:
		{
			const MouseButtonMapping* mouseButtonMapping = static_cast<const MouseButtonMapping*>(mapping);
			deviceString = "mouse";
			typeString = "button";

			std::stringstream stream;
			stream << static_cast<int>(mouseButtonMapping->button);
			stream >> eventString;
			break;
		}

		case InputMappingType::GAMEPAD_AXIS:
		{
			const GamepadAxisMapping* gamepadAxisMapping = static_cast<const GamepadAxisMapping*>(mapping);
			deviceString = "gamepad";
			typeString = "axis";

			std::stringstream stream;
			if (gamepadAxisMapping->negative)
			{
				stream << "-";
			}
			else
			{
				stream << "+";
			}
			stream << gamepadAxisMapping->axis;

			stream >> eventString;
			break;
		}

		case InputMappingType::GAMEPAD_BUTTON:
		{
			const GamepadButtonMapping* gamepadButtonMapping = static_cast<const GamepadButtonMapping*>(mapping);
			deviceString = "gamepad";
			typeString = "button";

			std::stringstream stream;
			stream << static_cast<int>(gamepadButtonMapping->button);
			stream >> eventString;
			break;
		}

		default:
			break;
	}

	return {deviceString, typeString, eventString};
}

void Game::remapControl(Control* control)
{
	// Remove previously set input mappings for the control
	inputRouter->removeMappings(control);

	// Start mapping new input
	inputMapper->setControl(control);
	inputMapper->setEnabled(true);

	// Restring UI to show control mappings have been removed.
	restringUI();

	// Disable UI callbacks
	uiRootElement->setCallbacksEnabled(false);

	// Disable menu control callbacks
	menuControls.setCallbacksEnabled(false);
}

void Game::resetControls()
{
	inputRouter->reset();
	loadControlProfile("default-keyboard-controls");
	saveControlProfile(*controlProfileName);
	restringUI();
}

void Game::resizeUI(int w, int h)
{
	// Adjust root element dimensions
	uiRootElement->setDimensions(Vector2(w, h));
	uiRootElement->update();

	splashBackgroundImage->setDimensions(Vector2(w, h));
	splashBackgroundImage->setAnchor(Anchor::TOP_LEFT);

	// Resize splash screen image
	splashImage->setAnchor(Anchor::CENTER);
	splashImage->setDimensions(Vector2(splashTexture->getWidth(), splashTexture->getHeight()));

	// Adjust UI camera projection matrix
	uiCamera.setOrthographic(0.0f, w, h, 0.0f, -1.0f, 1.0f);
	uiCamera.resetTweens();

	// Resize camera flash image
	cameraFlashImage->setDimensions(Vector2(w, h));
	cameraFlashImage->setAnchor(Anchor::CENTER);

	// Resize blackout image
	blackoutImage->setDimensions(Vector2(w, h));
	blackoutImage->setAnchor(Anchor::CENTER);

	// Resize language select background image
	languageSelectBGImage->setDimensions(Vector2(w, h));
	languageSelectBGImage->setAnchor(Anchor::CENTER);

	// Resize HUD
	float hudPadding = 20.0f;
	hudContainer->setDimensions(Vector2(w - hudPadding * 2.0f, h - hudPadding * 2.0f));
	hudContainer->setAnchor(Anchor::CENTER);

	// Tool indicator
	Rect toolIndicatorBounds = hudTextureAtlas.getBounds("tool-indicator");
	toolIndicatorBGImage->setDimensions(Vector2(toolIndicatorBounds.getWidth(), toolIndicatorBounds.getHeight()));
	toolIndicatorBGImage->setAnchor(Anchor::TOP_LEFT);

	Rect toolIndicatorIconBounds = hudTextureAtlas.getBounds("tool-indicator-lens");
	toolIndicatorIconImage->setDimensions(Vector2(toolIndicatorIconBounds.getWidth(), toolIndicatorIconBounds.getHeight()));
	toolIndicatorIconImage->setAnchor(Anchor::CENTER);

	// Buttons
	Rect playButtonBounds = hudTextureAtlas.getBounds("button-play");
	Rect fastForwardButtonBounds = hudTextureAtlas.getBounds("button-fast-forward-2x");
	Rect pauseButtonBounds = hudTextureAtlas.getBounds("button-pause");
	Rect buttonBackgroundBounds = hudTextureAtlas.getBounds("button-background");
	Vector2 buttonBGDimensions = Vector2(buttonBackgroundBounds.getWidth(), buttonBackgroundBounds.getHeight());
	float buttonMargin = 10.0f;
	float buttonDepth = 15.0f;

	float buttonContainerWidth = fastForwardButtonBounds.getWidth();
	float buttonContainerHeight = fastForwardButtonBounds.getHeight();
	buttonContainer->setDimensions(Vector2(buttonContainerWidth, buttonContainerHeight));
	buttonContainer->setAnchor(Anchor::TOP_RIGHT);

	playButtonImage->setDimensions(Vector2(playButtonBounds.getWidth(), playButtonBounds.getHeight()));
	playButtonImage->setAnchor(Vector2(0.0f, 0.0f));
	playButtonBGImage->setDimensions(buttonBGDimensions);
	playButtonBGImage->setAnchor(Vector2(0.0f, 1.0f));

	fastForwardButtonImage->setDimensions(Vector2(fastForwardButtonBounds.getWidth(), fastForwardButtonBounds.getHeight()));
	fastForwardButtonImage->setAnchor(Vector2(0.5f, 5.0f));
	fastForwardButtonBGImage->setDimensions(buttonBGDimensions);
	fastForwardButtonBGImage->setAnchor(Vector2(0.5f, 0.5f));

	pauseButtonImage->setDimensions(Vector2(pauseButtonBounds.getWidth(), pauseButtonBounds.getHeight()));
	pauseButtonImage->setAnchor(Vector2(1.0f, 0.0f));
	pauseButtonBGImage->setDimensions(buttonBGDimensions);
	pauseButtonBGImage->setAnchor(Vector2(1.0f, 1.0f));

	// Radial menu
	Rect radialMenuBounds = hudTextureAtlas.getBounds("radial-menu");
	radialMenuContainer->setDimensions(Vector2(w, h));
	radialMenuContainer->setAnchor(Anchor::CENTER);
	radialMenuContainer->setLayerOffset(30);

	radialMenuBackgroundImage->setDimensions(Vector2(w, h));
	radialMenuBackgroundImage->setAnchor(Anchor::CENTER);
	radialMenuBackgroundImage->setLayerOffset(-1);

	//radialMenuImage->setDimensions(Vector2(w * 0.5f, h * 0.5f));
	radialMenuImage->setDimensions(Vector2(radialMenuBounds.getWidth(), radialMenuBounds.getHeight()));
	radialMenuImage->setAnchor(Anchor::CENTER);

	Rect radialMenuSelectorBounds = hudTextureAtlas.getBounds("radial-menu-selector");
	radialMenuSelectorImage->setDimensions(Vector2(radialMenuSelectorBounds.getWidth(), radialMenuSelectorBounds.getHeight()));
	radialMenuSelectorImage->setAnchor(Anchor::CENTER);

	Rect toolIconBrushBounds = hudTextureAtlas.getBounds("tool-icon-brush");
	toolIconBrushImage->setDimensions(Vector2(toolIconBrushBounds.getWidth(), toolIconBrushBounds.getHeight()));
	toolIconBrushImage->setAnchor(Anchor::CENTER);

	Rect toolIconLensBounds = hudTextureAtlas.getBounds("tool-icon-lens");
	toolIconLensImage->setDimensions(Vector2(toolIconLensBounds.getWidth(), toolIconLensBounds.getHeight()));
	toolIconLensImage->setAnchor(Anchor::CENTER);

	Rect toolIconForcepsBounds = hudTextureAtlas.getBounds("tool-icon-forceps");
	toolIconForcepsImage->setDimensions(Vector2(toolIconForcepsBounds.getWidth(), toolIconForcepsBounds.getHeight()));
	toolIconForcepsImage->setAnchor(Anchor::CENTER);

	Rect toolIconSpadeBounds = hudTextureAtlas.getBounds("tool-icon-spade");
	toolIconSpadeImage->setDimensions(Vector2(toolIconSpadeBounds.getWidth(), toolIconSpadeBounds.getHeight()));
	toolIconSpadeImage->setAnchor(Anchor::CENTER);

	Rect toolIconCameraBounds = hudTextureAtlas.getBounds("tool-icon-camera");
	toolIconCameraImage->setDimensions(Vector2(toolIconCameraBounds.getWidth(), toolIconCameraBounds.getHeight()));
	toolIconCameraImage->setAnchor(Anchor::CENTER);

	Rect toolIconMicrochipBounds = hudTextureAtlas.getBounds("tool-icon-microchip");
	toolIconMicrochipImage->setDimensions(Vector2(toolIconMicrochipBounds.getWidth(), toolIconMicrochipBounds.getHeight()));
	toolIconMicrochipImage->setAnchor(Anchor::CENTER);

	Rect toolIconTestTubeBounds = hudTextureAtlas.getBounds("tool-icon-test-tube");
	toolIconTestTubeImage->setDimensions(Vector2(toolIconTestTubeBounds.getWidth(), toolIconTestTubeBounds.getHeight()));
	toolIconTestTubeImage->setAnchor(Anchor::CENTER);

	Rect labelCornerBounds = hudTextureAtlas.getBounds("label-tl");
	Vector2 labelCornerDimensions(labelCornerBounds.getWidth(), labelCornerBounds.getHeight());

	Vector2 antLabelPadding(10.0f, 6.0f);
	antLabelContainer->setDimensions(antLabel->getDimensions() + antLabelPadding * 2.0f);
	antLabelContainer->setTranslation(Vector2(0.0f, (int)(-antPin->getDimensions().y * 0.125f)));
	antLabelTL->setDimensions(labelCornerDimensions);
	antLabelTR->setDimensions(labelCornerDimensions);
	antLabelBL->setDimensions(labelCornerDimensions);
	antLabelBR->setDimensions(labelCornerDimensions);
	antLabelCC->setDimensions(Vector2(antLabel->getDimensions().x - labelCornerDimensions.x * 2.0f + antLabelPadding.x * 2.0f, antLabel->getDimensions().y - labelCornerDimensions.y * 2.0f + antLabelPadding.y * 2.0f));
	antLabelCT->setDimensions(Vector2(antLabel->getDimensions().x - labelCornerDimensions.x * 2.0f + antLabelPadding.x * 2.0f, labelCornerDimensions.y));
	antLabelCB->setDimensions(Vector2(antLabel->getDimensions().x - labelCornerDimensions.x * 2.0f + antLabelPadding.x * 2.0f, labelCornerDimensions.y));

	antLabelCL->setDimensions(Vector2(labelCornerDimensions.x, antLabel->getDimensions().y - labelCornerDimensions.y * 2.0f + antLabelPadding.y * 2.0f));
	antLabelCR->setDimensions(Vector2(labelCornerDimensions.x, antLabel->getDimensions().y - labelCornerDimensions.y * 2.0f + antLabelPadding.y * 2.0f));

	antLabelContainer->setAnchor(Vector2(0.5f, 0.5f));
	antLabelTL->setAnchor(Anchor::TOP_LEFT);
	antLabelTR->setAnchor(Anchor::TOP_RIGHT);
	antLabelBL->setAnchor(Anchor::BOTTOM_LEFT);
	antLabelBR->setAnchor(Anchor::BOTTOM_RIGHT);
	antLabelCC->setAnchor(Anchor::CENTER);
	antLabelCT->setAnchor(Vector2(0.5f, 0.0f));
	antLabelCB->setAnchor(Vector2(0.5f, 1.0f));
	antLabelCL->setAnchor(Vector2(0.0f, 0.5f));
	antLabelCR->setAnchor(Vector2(1.0f, 0.5f));
	antLabel->setAnchor(Anchor::CENTER);

	Rect antPinBounds = hudTextureAtlas.getBounds("label-pin");
	antPin->setDimensions(Vector2(antPinBounds.getWidth(), antPinBounds.getHeight()));
	antPin->setAnchor(Vector2(0.5f, 1.0f));

	Rect pinHoleBounds = hudTextureAtlas.getBounds("label-pin-hole");
	antLabelPinHole->setDimensions(Vector2(pinHoleBounds.getWidth(), pinHoleBounds.getHeight()));
	antLabelPinHole->setAnchor(Vector2(0.5f, 0.0f));
	antLabelPinHole->setTranslation(Vector2(0.0f, -antLabelPinHole->getDimensions().y * 0.5f));
	antLabelPinHole->setLayerOffset(2);


	float pinDistance = 20.0f;
	antTag->setAnchor(Anchor::CENTER);
	antTag->setDimensions(Vector2(antLabelContainer->getDimensions().x, antPin->getDimensions().y));

	float cameraGridLineWidth = 2.0f;
	float cameraReticleDiameter = 6.0f;
	cameraGridContainer->setDimensions(Vector2(w, h));
	cameraGridY0Image->setDimensions(Vector2(w, cameraGridLineWidth));
	cameraGridY1Image->setDimensions(Vector2(w, cameraGridLineWidth));
	cameraGridX0Image->setDimensions(Vector2(cameraGridLineWidth, h));
	cameraGridX1Image->setDimensions(Vector2(cameraGridLineWidth, h));
	cameraReticleImage->setDimensions(Vector2(cameraReticleDiameter));
	cameraGridY0Image->setTranslation(Vector2(0));
	cameraGridY1Image->setTranslation(Vector2(0));
	cameraGridX0Image->setTranslation(Vector2(0));
	cameraGridX1Image->setTranslation(Vector2(0));
	cameraReticleImage->setTranslation(Vector2(0));

	Rect menuSelectorBounds = hudTextureAtlas.getBounds("menu-selector");
	menuSelectorImage->setDimensions(Vector2(menuSelectorBounds.getWidth(), menuSelectorBounds.getHeight()));

	UIImage* icons[] =
	{
		toolIconBrushImage,
		nullptr,
		toolIconLensImage,
		nullptr,
		toolIconForcepsImage,
		toolIconMicrochipImage,
		toolIconCameraImage,
		nullptr
	};

	Rect radialMenuIconRingBounds = hudTextureAtlas.getBounds("radial-menu-icon-ring");
	float iconOffset = radialMenuIconRingBounds.getWidth() * 0.5f;
	float sectorAngle = (2.0f * 3.14159264f) / 8.0f;
	for (int i = 0; i < 8; ++i)
	{
		float angle = sectorAngle * static_cast<float>(i - 4);
		Vector2 translation = Vector2(std::cos(angle), std::sin(angle)) * iconOffset;
		translation.x = (int)(translation.x + 0.5f);
		translation.y = (int)(translation.y + 0.5f);

		if (icons[i] != nullptr)
		{
			icons[i]->setTranslation(translation);
		}
	}

	// Main menu size
	float mainMenuWidth = 0.0f;
	float mainMenuHeight = 0.0f;
	float mainMenuSpacing = 0.5f * fontSizePX;
	float mainMenuPadding = fontSizePX * 4.0f;

	for (const MenuItem* item: *mainMenu->getItems())
	{

		mainMenuHeight += item->getNameLabel()->getFont()->getMetrics().getHeight();
		mainMenuHeight += mainMenuSpacing;
		mainMenuWidth = std::max<float>(mainMenuWidth, item->getNameLabel()->getDimensions().x);
	}
	mainMenuHeight -= mainMenuSpacing;
	mainMenu->getContainer()->setAnchor(Anchor::BOTTOM_RIGHT);
	mainMenu->resize(mainMenuWidth, mainMenuHeight);
	mainMenu->getContainer()->setTranslation(Vector2(-mainMenuPadding));

	// Settings menu size
	float settingsMenuWidth = 0.0f;
	float settingsMenuHeight = 0.0f;
	float settingsMenuSpacing = 0.5f * fontSizePX;
	float settingsMenuPadding = fontSizePX * 4.0f;
	float settingsMenuValueMargin = fontSizePX * 4.0f;

	for (const MenuItem* item: *settingsMenu->getItems())
	{
		settingsMenuHeight += item->getNameLabel()->getFont()->getMetrics().getHeight();
		settingsMenuHeight += settingsMenuSpacing;

		float itemWidth = item->getNameLabel()->getDimensions().x;
		if (!item->getValueLabel()->getText().empty())
		{
			itemWidth += item->getValueLabel()->getDimensions().x + settingsMenuValueMargin;
		}

		settingsMenuWidth = std::max<float>(settingsMenuWidth, itemWidth);
	}

	settingsMenuHeight -= settingsMenuSpacing;

	settingsMenu->getContainer()->setAnchor(Anchor::BOTTOM_RIGHT);
	settingsMenu->resize(settingsMenuWidth, settingsMenuHeight);
	settingsMenu->getContainer()->setTranslation(Vector2(-settingsMenuPadding));

	// Controls menu size
	float controlsMenuWidth = 0.0f;
	float controlsMenuHeight = 0.0f;
	float controlsMenuSpacing = 0.5f * fontSizePX;
	float controlsMenuPadding = fontSizePX * 4.0f;
	float controlsMenuValueMargin = fontSizePX * 4.0f;

	for (const MenuItem* item: *controlsMenu->getItems())
	{
		controlsMenuHeight += item->getNameLabel()->getFont()->getMetrics().getHeight();
		controlsMenuHeight += controlsMenuSpacing;

		float itemWidth = item->getNameLabel()->getDimensions().x;
		if (!item->getValueLabel()->getText().empty())
		{
			itemWidth += item->getValueLabel()->getDimensions().x + controlsMenuValueMargin;
		}

		controlsMenuWidth = std::max<float>(controlsMenuWidth, itemWidth);
	}

	controlsMenuWidth += controlsMenuValueMargin;
	controlsMenuHeight -= controlsMenuSpacing;

	controlsMenu->getContainer()->setAnchor(Anchor::BOTTOM_RIGHT);
	controlsMenu->resize(controlsMenuWidth, controlsMenuHeight);
	controlsMenu->getContainer()->setTranslation(Vector2(-controlsMenuPadding));

	// Pause menu size
	float pauseMenuWidth = 0.0f;
	float pauseMenuHeight = 0.0f;
	float pauseMenuSpacing = 0.5f * fontSizePX;
	float pauseMenuPadding = fontSizePX * 4.0f;

	for (const MenuItem* item: *pauseMenu->getItems())
	{

		pauseMenuHeight += item->getNameLabel()->getFont()->getMetrics().getHeight();
		pauseMenuHeight += pauseMenuSpacing;
		pauseMenuWidth = std::max<float>(pauseMenuWidth, item->getNameLabel()->getDimensions().x);
	}
	pauseMenuHeight -= pauseMenuSpacing;
	pauseMenu->getContainer()->setAnchor(Anchor::BOTTOM_RIGHT);
	pauseMenu->resize(pauseMenuWidth, pauseMenuHeight);
	pauseMenu->getContainer()->setTranslation(Vector2(-pauseMenuPadding));

	// Language menu size
	float languageMenuWidth = 0.0f;
	float languageMenuHeight = 0.0f;
	float languageMenuSpacing = 0.75f * fontSizePX;

	for (const MenuItem* item: *languageMenu->getItems())
	{
		Font* font = item->getNameLabel()->getFont();
		if (font)
		{
			float lineHeight = font->getMetrics().getAscender() - font->getMetrics().getDescender();
			languageMenuHeight += lineHeight + languageMenuSpacing;
			languageMenuWidth = std::max<float>(languageMenuWidth, item->getNameLabel()->getDimensions().x);
		}

		item->getNameLabel()->setAnchor(Vector2(0.5f, 0.0f));
	}
	languageMenuHeight -= languageMenuSpacing;
	languageMenu->getContainer()->setAnchor(Anchor::CENTER);
	languageMenu->resize(languageMenuWidth, languageMenuHeight);
}

void Game::restringUI()
{
	// Reset fonts
	mainMenu->setFonts(menuFont);
	settingsMenu->setFonts(menuFont);
	controlsMenu->setFonts(menuFont);
	pauseMenu->setFonts(menuFont);

	// Get common strings
	std::string offString = getString("off");
	std::string onString = getString("on");
	std::string backString = getString("back");

	// Main menu strings
	mainMenuContinueItem->setName(getString("continue"));
	mainMenuNewGameItem->setName(getString("new-game"));
	mainMenuColoniesItem->setName(getString("colonies"));
	mainMenuSettingsItem->setName(getString("settings"));
	mainMenuQuitItem->setName(getString("quit"));

	// Settings menu strings
	settingsMenuControlsItem->setName(getString("controls"));
	settingsMenuControlsItem->setValue(getString("ellipsis"));
	settingsMenuFullscreenItem->setName(getString("fullscreen"));
	settingsMenuFullscreenItem->setValue((*fullscreen) ? onString : offString);
	settingsMenuVSyncItem->setName(getString("v-sync"));
	settingsMenuVSyncItem->setValue((*vsync) ? onString : offString);
	settingsMenuLanguageItem->setName(getString("language"));
	settingsMenuLanguageItem->setValue(getString("language-name"));
	settingsMenuBackItem->setName(backString);

	// Controls menu strings
	restringControlMenuItem(controlsMenuMoveForwardItem, "move-forward");
	restringControlMenuItem(controlsMenuMoveLeftItem, "move-left");
	restringControlMenuItem(controlsMenuMoveBackItem, "move-back");
	restringControlMenuItem(controlsMenuMoveRightItem, "move-right");
	restringControlMenuItem(controlsMenuChangeToolItem, "change-tool");
	restringControlMenuItem(controlsMenuUseToolItem, "use-tool");
	restringControlMenuItem(controlsMenuAdjustCameraItem, "adjust-camera");
	restringControlMenuItem(controlsMenuPauseItem, "pause");
	restringControlMenuItem(controlsMenuToggleFullscreenItem, "toggle-fullscreen");
	restringControlMenuItem(controlsMenuTakeScreenshotItem, "take-screenshot");
	controlsMenuResetToDefaultItem->setName(getString("reset-to-default"));
	controlsMenuBackItem->setName(backString);

	// Pause menu strings
	pauseMenuResumeItem->setName(getString("resume"));
	pauseMenuSettingsItem->setName(getString("settings"));
	pauseMenuMainMenuItem->setName(getString("main-menu"));
	pauseMenuQuitItem->setName(getString("quit"));

	// Language menu strings
	for (std::size_t i = 0; i < languageCount; ++i)
	{
		languageMenuItems[i]->setName(getString("language-name", i));
	}

	// Reset menu tweens
	uiRootElement->update();
	mainMenu->getContainer()->resetTweens();
	settingsMenu->getContainer()->resetTweens();
	controlsMenu->getContainer()->resetTweens();
	pauseMenu->getContainer()->resetTweens();
	languageMenu->getContainer()->resetTweens();
}

void Game::restringControlMenuItem(MenuItem* item, const std::string& name)
{
	item->setName(getString(name));

	Control* control = controlNameMap.find(name)->second;

	std::string value;
	const std::list<InputMapping*>* mappings = inputRouter->getMappings(control);
	if (mappings != nullptr)
	{
		std::size_t i = 0;
		for (const InputMapping* mapping: *mappings)
		{
			std::array<std::string, 3> mappingStrings = getInputMappingStrings(mapping);

			// keyboard-key, mouse-button, gamepad-axis, etc.
			std::string typeName = mappingStrings[0] + "-" + mappingStrings[1];
			std::string type = getString(typeName);

			if (mapping->getType() != InputMappingType::KEY)
			{
				value += type;
				value += " ";
			}

			value += mappingStrings[2];

			if (i < mappings->size() - 1)
			{
				value += ", ";
			}

			++i;
		}
	}

	item->setValue(value);
}

void Game::setTimeOfDay(float time)
{
	Vector3 midnight = Vector3(0.0f, 1.0f, 0.0f);
	Vector3 sunrise = Vector3(-1.0f, 0.0f, 0.0f);
	Vector3 noon = Vector3(0, -1.0f, 0.0f);
	Vector3 sunset = Vector3(1.0f, 0.0f, 0.0f);

	float angles[4] =
	{
		glm::radians(270.0f), // 00:00
		glm::radians(0.0f),   // 06:00
		glm::radians(90.0f),  // 12:00
		glm::radians(180.0f)  // 18:00
	};

	int index0 = static_cast<int>(fmod(time, 24.0f) / 6.0f);
	int index1 = (index0 + 1) % 4;

	float t = (time - (static_cast<float>(index0) * 6.0f)) / 6.0f;

	Quaternion rotation0 = glm::angleAxis(angles[index0], Vector3(1, 0, 0));
	Quaternion rotation1 = glm::angleAxis(angles[index1], Vector3(1, 0, 0));
	Quaternion rotation = glm::normalize(glm::slerp(rotation0, rotation1, t));

	Vector3 direction = glm::normalize(rotation * Vector3(0, 0, 1));

	sunlight.setDirection(direction);

	Vector3 up = glm::normalize(rotation * Vector3(0, 1, 0));

	sunlightCamera.lookAt(Vector3(0, 0, 0), sunlight.getDirection(), up);
}

void Game::queueScreenshot()
{
	screenshotQueued = true;
	cameraFlashImage->setVisible(false);
	cameraGridContainer->setVisible(false);
	fpsLabel->setVisible(false);

	soundSystem->scrot();
}

void Game::screenshot()
{
	screenshotQueued = false;

	// Read pixel data from framebuffer
	unsigned char* pixels = new unsigned char[w * h * 3];
	glReadBuffer(GL_BACK);
	glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, pixels);

	// Get game title in current language
	std::string title = getString("title");

	// Convert title to lowercase
	std::transform(title.begin(), title.end(), title.begin(), ::tolower);

	// Create screenshot directory if it doesn't exist
	std::string screenshotDirectory = configPath + std::string("screenshots/");
	if (!pathExists(screenshotDirectory))
	{
		createDirectory(screenshotDirectory);
	}

	// Build screenshot file name
	std::string filename = screenshotDirectory + title + "-" + timestamp() + ".png";

	// Write screenshot to file in separate thread
	std::thread screenshotThread(Game::saveScreenshot, filename, w, h, pixels);
	screenshotThread.detach();

	// Play camera flash animation
	cameraFlashAnimation.stop();
	cameraFlashAnimation.rewind();
	cameraFlashAnimation.play();

	// Play camera shutter sound
	
	// Restore camera UI visibility
	//cameraGridContainer->setVisible(true);
	fpsLabel->setVisible(true);

	// Whiteout screen immediately
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void Game::inputMapped(const InputMapping& mapping)
{
	// Skip mouse motion events
	if (mapping.getType() == InputMappingType::MOUSE_MOTION)
	{
		return;
	}

	// Add input mapping to input router
	if (mapping.control != nullptr)
	{
		inputRouter->addMapping(mapping);
	}

	// Disable input mapping generation
	inputMapper->setControl(nullptr);
	inputMapper->setEnabled(false);

	// Restring UI
	restringUI();

	// Schedule callbacks to be enabled in 100ms
	ScheduledFunctionEvent event;
	event.caller = static_cast<void*>(this);
	event.function = [this]()
	{
		// Re-enable UI callbacks
		uiRootElement->setCallbacksEnabled(true);

		// Re-enable menu controls
		menuControls.setCallbacksEnabled(true);
	};
	eventDispatcher.schedule(event, time + 0.1f);

	// Save control profile
	saveControlProfile(*controlProfileName);
}

void Game::languageSelected()
{
	// Disable non-system controls
	disableNonSystemControls();

	// Disable UI callbacks
	uiRootElement->setCallbacksEnabled(false);

	// Begin to fade out
	fadeOut(0.5f, Vector3(0.0f), std::bind(&StateMachine::changeState, this, &titleState));
}

void Game::skipSplash()
{
	if (StateMachine::getCurrentState() == &splashState)
	{
		StateMachine::changeState(&titleState);
	}
}

void Game::togglePause()
{
	paused = !paused;

	if (paused)
	{
		openMenu(pauseMenu, 0);

		// Enable menu controls and UI callbacks
		uiRootElement->setCallbacksEnabled(true);
		menuControls.setCallbacksEnabled(true);
	}
	else
	{
		closeCurrentMenu();

		// Disable menu controls and UI callbacks
		uiRootElement->setCallbacksEnabled(false);
		menuControls.setCallbacksEnabled(false);
	}
}

void Game::continueGame()
{
	// Disable non-system controls
	disableNonSystemControls();

	// Disable UI callbacks
	uiRootElement->setCallbacksEnabled(false);

	// Start fading out main menu
	menuFadeAnimation.setClip(&menuFadeOutClip);
	menuFadeAnimation.setTimeFrame(menuFadeOutClip.getTimeFrame());
	menuFadeAnimation.rewind();
	menuFadeAnimation.play();

	// Close menu and enter play state after it fades out
	menuFadeAnimation.setEndCallback
	(
		[this]()
		{
			closeCurrentMenu();
			StateMachine::changeState(&playState);
		}
	);
}

void Game::newGame()
{
	// Disable non-system controls
	disableNonSystemControls();

	// Disable UI callbacks
	uiRootElement->setCallbacksEnabled(false);

	// Start fading out main menu
	menuFadeAnimation.setClip(&menuFadeOutClip);
	menuFadeAnimation.setTimeFrame(menuFadeOutClip.getTimeFrame());
	menuFadeAnimation.rewind();
	menuFadeAnimation.play();

	// Close menu and enter play state after it fades out
	menuFadeAnimation.setEndCallback
	(
		[this]()
		{
			closeCurrentMenu();
		}
	);

	// Start to play state
	fadeOut(3.0f, Vector3(0.0f), std::bind(&StateMachine::changeState, this, &playState));
}

void Game::returnToMainMenu()
{
	// Disable non-system controls
	disableNonSystemControls();

	// Disable UI callbacks
	uiRootElement->setCallbacksEnabled(false);

	// Close pause menu
	closeCurrentMenu();

	// Fade to title state
	fadeOut(3.0f, Vector3(0.0f), std::bind(&StateMachine::changeState, this, &titleState));
}

void Game::interpretCommands()
{
	while (true)
	{
		std::cout << "> " << std::flush;
		std::string line;
		std::getline(std::cin, line);

		try
		{
			auto [commandName, arguments, call] = cli->interpret(line);

			if (call)
			{

				call();
			}
			else
			{
				if (!commandName.empty())
				{
					std::cout << "Unknown command " << commandName << std::endl;
				}
			}
		}
		catch (const std::invalid_argument& e)
		{
			std::string commandName = line.substr(0, line.find(' '));

			auto& helpStrings = cli->help();
			if (auto it = helpStrings.find(commandName); it != helpStrings.end())
			{
				std::cout << "Usage: " << it->second << std::endl;
			}
			else
			{
				std::cout << commandName << ": Invalid arguments" << std::endl;
			}
		}
	}
}

void Game::boxSelect(float x, float y, float w, float h)
{
	boxSelectionContainer->setTranslation(Vector2(x, y));
	boxSelectionContainer->setDimensions(Vector2(w, h));
	boxSelectionImageBackground->setDimensions(Vector2(w, h));
	boxSelectionImageTop->setDimensions(Vector2(w, boxSelectionBorderWidth));
	boxSelectionImageBottom->setDimensions(Vector2(w, boxSelectionBorderWidth));
	boxSelectionImageLeft->setDimensions(Vector2(boxSelectionBorderWidth, h));
	boxSelectionImageRight->setDimensions(Vector2(boxSelectionBorderWidth, h));
	boxSelectionContainer->setVisible(true);
}

void Game::fadeIn(float duration, const Vector3& color, std::function<void()> callback)
{
	if (fadeInAnimation.isPlaying())
	{
		return;
	}

	fadeOutAnimation.stop();
	this->fadeInEndCallback = callback;
	blackoutImage->setTintColor(Vector4(color, 1.0f));
	blackoutImage->setVisible(true);
	fadeInAnimation.setSpeed(1.0f / duration);
	fadeInAnimation.setLoop(false);
	fadeInAnimation.setClip(&fadeInClip);
	fadeInAnimation.setTimeFrame(fadeInClip.getTimeFrame());
	fadeInAnimation.rewind();
	fadeInAnimation.play();

	blackoutImage->resetTweens();
	uiRootElement->update();
}

void Game::fadeOut(float duration, const Vector3& color, std::function<void()> callback)
{
	if (fadeOutAnimation.isPlaying())
	{
		return;
	}

	fadeInAnimation.stop();
	this->fadeOutEndCallback = callback;
	blackoutImage->setVisible(true);
	blackoutImage->setTintColor(Vector4(color, 0.0f));
	fadeOutAnimation.setSpeed(1.0f / duration);
	fadeOutAnimation.setLoop(false);
	fadeOutAnimation.setClip(&fadeOutClip);
	fadeOutAnimation.setTimeFrame(fadeOutClip.getTimeFrame());
	fadeOutAnimation.rewind();
	fadeOutAnimation.play();

	blackoutImage->resetTweens();
	uiRootElement->update();
}

void Game::stopFade()
{
	fadeInAnimation.stop();
	fadeOutAnimation.stop();
	blackoutImage->setVisible(false);
	uiRootElement->update();
}

void Game::selectTool(int toolIndex)
{
	Tool* tools[] =
	{
		brush,
		nullptr,
		lens,
		nullptr,
		forceps,
		nullptr,
		nullptr,
		nullptr
	};

	Tool* nextTool = tools[toolIndex];
	if (nextTool != currentTool)
	{
		if (currentTool)
		{
			currentTool->setActive(false);
			currentTool->update(0.0f);
		}

		currentTool = nextTool;
		if (currentTool)
		{
			currentTool->setActive(true);
		}
	}

	if (1)
	{
		toolIndicatorIconImage->setTextureBounds(toolIndicatorsBounds[toolIndex]);
		toolIndicatorIconImage->setVisible(true);
	}
	else
	{
		toolIndicatorIconImage->setVisible(false);
	}
}

void Game::nextControlProfile()
{
	
}

EntityID Game::createInstance()
{
	return entityManager->createEntity();
}

EntityID Game::createNamedInstance(const std::string& instanceName)
{
	EntityID entity = entityManager->createEntity();
	cli->set(instanceName, std::to_string(entity));
	return entity;
}

EntityID Game::createInstanceOf(const std::string& templateName)
{

	EntityTemplate* entityTemplate = resourceManager->load<EntityTemplate>(templateName + ".ent");

	EntityID entity = entityManager->createEntity();
	entityTemplate->apply(entity, componentManager);

	return entity;
}

EntityID Game::createNamedInstanceOf(const std::string& templateName, const std::string& instanceName)
{

	EntityTemplate* entityTemplate = resourceManager->load<EntityTemplate>(templateName + ".ent");

	EntityID entity = entityManager->createEntity();
	entityTemplate->apply(entity, componentManager);
	cli->set(instanceName, std::to_string(entity));

	return entity;
}

void Game::destroyInstance(EntityID entity)
{
	entityManager->destroyEntity(entity);
}

void Game::addComponent(EntityID entity, ComponentBase* component)
{
	componentManager->addComponent(entity, component);
}

void Game::removeComponent(EntityID entity, ComponentType type)
{
	ComponentBase* component = componentManager->removeComponent(entity, type);
	delete component;
}

void Game::setTransform(EntityID entity, const Transform& transform)
{
	TransformComponent* component = componentManager->getComponent<TransformComponent>(entity);
	if (!component)
	{
		return;
	}

	component->transform = transform;
}

void Game::setTranslation(EntityID entity, const Vector3& translation)
{
	TransformComponent* component = componentManager->getComponent<TransformComponent>(entity);
	if (!component)
	{
		return;
	}

	component->transform.translation = translation;
}

void Game::setRotation(EntityID entity, const Quaternion& rotation)
{
	TransformComponent* component = componentManager->getComponent<TransformComponent>(entity);
	if (!component)
	{
		return;
	}

	component->transform.rotation = rotation;
}

void Game::setScale(EntityID entity, const Vector3& scale)
{
	TransformComponent* component = componentManager->getComponent<TransformComponent>(entity);
	if (!component)
	{
		return;
	}

	component->transform.scale = scale;
}

void Game::translate(EntityID entity, const Vector3& translation)
{
	TransformComponent* component = componentManager->getComponent<TransformComponent>(entity);
	if (!component)
	{
		return;
	}

	component->transform.translation += translation;
}

void Game::rotate(EntityID entity, const Quaternion& rotation)
{
	TransformComponent* component = componentManager->getComponent<TransformComponent>(entity);
	if (!component)
	{
		return;
	}

	component->transform.rotation = component->transform.rotation * rotation;
}

void Game::scale(EntityID entity, const Vector3& scale)
{
	TransformComponent* component = componentManager->getComponent<TransformComponent>(entity);
	if (!component)
	{
		return;
	}

	component->transform.scale *= scale;
}

void Game::setTerrainPatchPosition(EntityID entity, const std::tuple<int, int>& position)
{
	TerrainPatchComponent* component = componentManager->getComponent<TerrainPatchComponent>(entity);
	if (!component)
	{
		return;
	}

	component->position = position;
}

void Game::saveScreenshot(const std::string& filename, unsigned int width, unsigned int height, unsigned char* pixels)
{
	stbi_flip_vertically_on_write(1);
	stbi_write_png(filename.c_str(), width, height, 3, pixels, width * 3);

	delete[] pixels;
}

