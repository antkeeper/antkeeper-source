/*
 * Copyright (C) 2017  Christopher J. Howard
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

#include "application.hpp"
#include "application-state.hpp"
#include "model-loader.hpp"
#include "material-loader.hpp"
#include "states/loading-state.hpp"
#include "states/splash-state.hpp"
#include "states/title-state.hpp"
#include "states/main-menu-state.hpp"
#include "states/play-state.hpp"
#include "game/colony.hpp"
#include "debug.hpp"
#include "camera-controller.hpp"
#include <cstdlib>
#include <iostream>
#include <cstdio>
#include <sstream>
#include <SDL.h>

#define OPENGL_VERSION_MAJOR 3
#define OPENGL_VERSION_MINOR 3

Application::Application(int argc, char* argv[]):
	state(nullptr),
	nextState(nullptr),
	terminationCode(EXIT_SUCCESS)
{
	window = nullptr;
	context = nullptr;
	
	// Initialize SDL
	std::cout << "Initializing SDL... ";
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_GAMECONTROLLER) < 0)
	{
		std::cout << "failed: \"" << SDL_GetError() << "\"" << std::endl;
		close(EXIT_FAILURE);
		return;
	}
	else
	{
		std::cout << "success" << std::endl;
	}

	// Print SDL version strings
	SDL_version compiled;
	SDL_version linked;
	SDL_VERSION(&compiled);
	SDL_GetVersion(&linked);
	std::cout << "Compiled with SDL " << (int)compiled.major << "." << (int)compiled.minor << "." << (int)compiled.patch << std::endl;
	std::cout << "Linking to SDL " << (int)linked.major << "." << (int)linked.minor << "." << (int)linked.patch << std::endl;
	
	// Find app and user data paths
	appDataPath = std::string(SDL_GetBasePath()) + "data/";
	userDataPath = SDL_GetPrefPath("cjhoward", "antkeeper");
	std::cout << "Application data path: \"" << appDataPath << "\"" << std::endl;
	std::cout << "User data path: \"" << userDataPath << "\"" << std::endl;
	
	// Form pathes to settings files
	defaultSettingsFilename = appDataPath + "default-settings.txt";
	userSettingsFilename = userDataPath + "settings.txt";

	// Load default settings
	std::cout << "Loading default settings from \"" << defaultSettingsFilename << "\"... ";
	if (!settings.load(defaultSettingsFilename))
	{
		std::cout << "failed" << std::endl;
		close(EXIT_FAILURE);
		return;
	}
	else
	{
		std::cout << "success" << std::endl;
	}
	
	// Load user settings
	std::cout << "Loading user settings from \"" << userSettingsFilename << "\"... ";
	if (!settings.load(userSettingsFilename))
	{
		// Failed, save default settings as user settings
		std::cout << "failed" << std::endl;
		saveUserSettings();
	}
	else
	{
		std::cout << "success" << std::endl;
	}
	
	// Get values of required settings
	settings.get("fullscreen", &fullscreen);
	settings.get("fullscreen_width", &fullscreenWidth);
	settings.get("fullscreen_height", &fullscreenHeight);
	settings.get("windowed_width", &windowedWidth);
	settings.get("windowed_height", &windowedHeight);
	settings.get("swap_interval", &swapInterval);
	
	// Load strings
	std::string language;
	settings.get("language", &language);
	std::string stringsFile = appDataPath + "strings/" + language + ".txt";
	std::cout << "Loading strings from \"" << stringsFile << "\"... ";
	if (!strings.load(stringsFile))
	{
		std::cout << "failed" << std::endl;
	}
	else
	{
		std::cout << "success" << std::endl;
	}

	// Select OpenGL version
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, OPENGL_VERSION_MAJOR);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, OPENGL_VERSION_MINOR);
	
	// Set OpenGL buffer attributes
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

	// Check desktop display mode
	SDL_DisplayMode displayMode;
	if (SDL_GetDesktopDisplayMode(0, &displayMode) != 0)
	{
		std::cerr << "Failed to get desktop display mode: \"" << SDL_GetError() << "\"" << std::endl;
		close(EXIT_FAILURE);
		return;
	}
	
	// Check (usable?) display bounds
	SDL_Rect displayBounds;
	if (SDL_GetDisplayBounds(0, &displayBounds) != 0)
	{
		std::cerr << "Failed to get display bounds: \"" << SDL_GetError() << "\"" << std::endl;
		close(EXIT_FAILURE);
		return;
	}
	
	// Use display resolution if settings request
	if (windowedWidth == -1 || windowedHeight == -1)
	{
		windowedWidth = displayBounds.w;
		windowedHeight = displayBounds.h;
	}
	if (fullscreenWidth == -1 || fullscreenHeight == -1)
	{
		fullscreenWidth = displayMode.w;
		fullscreenHeight = displayMode.h;
	}
	
	// Determine window parameters
	Uint32 windowFlags = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
	if (fullscreen)
	{
		width = fullscreenWidth;
		height = fullscreenHeight;
		windowFlags |= SDL_WINDOW_FULLSCREEN;
	}
	else
	{
		width = windowedWidth;
		height = windowedHeight;
	}
	
	// Get window title string
	std::string title;
	strings.get("title", &title);
	
	// Create window
	std::cout << "Creating a window... ";
	window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, windowFlags);
	if (window == nullptr)
	{
		std::cout << "failed: \"" << SDL_GetError() << "\"" << std::endl;
		close(EXIT_FAILURE);
		return;
	}
	else
	{
		std::cout << "success" << std::endl;
	}
	
	// Get actual window size
	SDL_GetWindowSize(window, &width, &height);
	if (fullscreen)
	{
		fullscreenWidth = width;
		fullscreenHeight = height;
	}
	else
	{
		windowedWidth = width;
		windowedHeight = height;
	}
	
	// Print video driver
	const char* videoDriver = SDL_GetCurrentVideoDriver();
	if (!videoDriver)
	{
		std::cout << "Unable to determine video driver" << std::endl;
	}
	else
	{
		std::cout << "Using video driver \"" << videoDriver << "\"" << std::endl;
	}
	
	// Create an OpenGL context
	std::cout << "Creating an OpenGL context... ";
	context = SDL_GL_CreateContext(window);
	if (context == nullptr)
	{
		std::cout << "failed: \"" << SDL_GetError() << "\"" << std::endl;
		close(EXIT_FAILURE);
		return;
	}
	else
	{
		std::cout << "success" << std::endl;
	}

	// Initialize GL3W
	std::cout << "Initializing GL3W... ";
	if (gl3wInit())
	{
		std::cout << "failed" << std::endl;
		close(EXIT_FAILURE);
		return;
	}
	else
	{
		std::cout << "success" << std::endl;
	}
	
	// Check if OpenGL version is supported
	if (!gl3wIsSupported(OPENGL_VERSION_MAJOR, OPENGL_VERSION_MINOR))
	{
		std::cout << "OpenGL " << OPENGL_VERSION_MAJOR << "." << OPENGL_VERSION_MINOR << " not supported" << std::endl;
		close(EXIT_FAILURE);
		return;
	}
	
	// Print OpenGL and GLSL version strings
	std::cout << "Using OpenGL " << glGetString(GL_VERSION) << ", GLSL " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
	
	// Set swap interval (vsync)
	if (swapInterval)
	{
		std::cout << "Enabling vertical sync... ";
	}
	else
	{
		std::cout << "Disabling vertical sync... ";
	}
	if (SDL_GL_SetSwapInterval(swapInterval) != 0)
	{
		std::cout << "failed: \"" << SDL_GetError() << "\"" << std::endl;
		swapInterval = SDL_GL_GetSwapInterval();
	}
	else
	{
		std::cout << "success" << std::endl;
	}
	
	// Clear screen to black
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	SDL_GL_SwapWindow(window);
	
	// Get display DPI
	std::cout << "Getting DPI of display 0... ";
	if (SDL_GetDisplayDPI(0, &dpi, nullptr, nullptr) != 0)
	{
		std::cerr << "failed: \"" << SDL_GetError() << "\"" << std::endl;
		
		std::cout << "Reverting to default DPI" << std::endl;
		settings.get("default_dpi", &dpi);
	}
	else
	{
		std::cout << "success" << std::endl;
	}
	
	// Print DPI
	std::cout << "Rendering at " << dpi << " DPI" << std::endl;
	
	// Determine base font size
	settings.get("font_size", &fontSizePT);
	fontSizePX = fontSizePT * (1.0f / 72.0f) * dpi;
	
	// Print font size
	std::cout << "Base font size is " << fontSizePT << "pt (" << fontSizePX << "px)" << std::endl;
	
	// Setup input
	inputManager = new SDLInputManager();
	keyboard = (*inputManager->getKeyboards()).front();
	mouse = (*inputManager->getMice()).front();
	
	// Allocate states
	loadingState = new LoadingState(this);
	splashState = new SplashState(this);
	titleState = new TitleState(this);
	mainMenuState = new MainMenuState(this);
	playState = new PlayState(this);
	
	// Setup loaders
	textureLoader = new TextureLoader();
	materialLoader = new MaterialLoader();
	modelLoader = new ModelLoader();
	modelLoader->setMaterialLoader(materialLoader);
	
	// Allocate game variables
	surfaceCam = new SurfaceCameraController();
	tunnelCam = new TunnelCameraController();
	
	// Enter loading state
	state = nextState = loadingState;
	state->enter();
}

Application::~Application()
{
	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

int Application::execute()
{
	// Fixed timestep
	// @see http://gafferongames.com/game-physics/fix-your-timestep/
	t = 0.0f;
	dt = 1.0f / 60.0f;
	float accumulator = 0.0f;
	float maxFrameTime = 0.25f;
	
	int performanceSampleSize = 15;     // Number of frames to sample
	int performanceSampleFrame = 0;     // Current sample frame
	float performanceSampleTime = 0.0f; // Current sample time
	
	// Start frame timer
	frameTimer.start();
	
	while (state != nullptr)
	{
		// Calculate frame time (in milliseconds) then reset frame timer
		float frameTime = static_cast<float>(frameTimer.microseconds().count()) / 1000.0f;
		frameTimer.reset();
		
		// Add frame time (in seconds) to accumulator
		accumulator += std::min(frameTime / 1000.0f, maxFrameTime);
		
		// If the user tried to close the application
		if (inputManager->wasClosed() || escape.isTriggered())
		{
			// Close the application
			close(EXIT_SUCCESS);
		}
		else
		{
			// Execute current state
			while (accumulator >= dt)
			{
				state->execute();
				
				// Perform tweening
				tweener->update(dt);
				
				accumulator -= dt;
				t += dt;
			}
		}
		
		// Check for state change
		if (nextState != state)
		{
			// Exit current state
			state->exit();
			
			// Enter next state (if valid)
			state = nextState;
			if (nextState != nullptr)
			{
				state->enter();
				tweener->update(0.0f);
				
				// Reset frame timer to counteract frames eaten by state exit() and enter() functions
				frameTimer.reset();
			}
		}
		
		// Update controls
		menuControlProfile->update();
		gameControlProfile->update();
		
		// Update input
		inputManager->update();
		
		// Check if fullscreen was toggled
		if (toggleFullscreen.isTriggered() && !toggleFullscreen.wasTriggered())
		{
			changeFullscreen();
		}
		
		// Add frame time to performance sample time and increment the frame count
		performanceSampleTime += frameTime;
		++performanceSampleFrame;
		
		// If performance sample is complete
		if (performanceSampleFrame >= performanceSampleSize)
		{
			// Calculate mean frame time
			float meanFrameTime = performanceSampleTime / static_cast<float>(performanceSampleSize);
			
			// Reset perform sample timers
			performanceSampleTime = 0.0f;
			performanceSampleFrame = 0;
			
			// Update frame time label
			if (frameTimeLabel->isVisible())
			{
				std::string frameTimeString;
				std::stringstream stream;
				stream.precision(2);
				stream << std::fixed << meanFrameTime;
				stream >> frameTimeString;
				frameTimeLabel->setText(frameTimeString);
			}
		}
		
		// Update UI
		uiRootElement->update();
		uiBatcher->batch(uiBatch, uiRootElement);
		
		// Clear depth and stencil buffers
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		
		// Render scene
		renderer.render(scene);
		
		// Swap buffers
		SDL_GL_SwapWindow(window);
	}
	
	return terminationCode;
}

void Application::changeState(ApplicationState* state)
{
	nextState = state;
}

void Application::setTerminationCode(int code)
{
	terminationCode = code;
}

void Application::close(int terminationCode)
{
	setTerminationCode(terminationCode);
	changeState(nullptr);
}

void Application::changeFullscreen()
{
	fullscreen = !fullscreen;
	
	if (fullscreen)
	{
		width = fullscreenWidth;
		height = fullscreenHeight;
		
		SDL_SetWindowSize(window, width, height);
		if (SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN) != 0)
		{
			std::cerr << "Failed to set fullscreen mode: \"" << SDL_GetError() << "\"" << std::endl;
			fullscreen = false;
		}
	}
	else
	{
		width = windowedWidth;
		height = windowedHeight;
		
		if (SDL_SetWindowFullscreen(window, 0) != 0)
		{
			std::cerr << "Failed to set windowed mode: \"" << SDL_GetError() << "\"" << std::endl;
			fullscreen = true;
		}
		else
		{
			SDL_SetWindowSize(window, width, height);
			SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
		}
	}
	
	// Get actual window size
	SDL_GetWindowSize(window, &width, &height);
	
	// Print mode and resolution
	if (fullscreen)
	{
		std::cout << "Changed to fullscreen mode at resolution " << width << "x" << height << std::endl;
	}
	else
	{
		std::cout << "Changed to windowed mode at resolution " << width << "x" << height << std::endl;
	}
	
	// Save settings
	settings.set("fullscreen", fullscreen);
	saveUserSettings();
	
	// Notify window observers
	inputManager->update();
}

void Application::changeVerticalSync()
{
	swapInterval = (swapInterval == 1) ? 0 : 1;
	
	if (swapInterval == 1)
	{
		std::cout << "Enabling vertical sync... ";
	}
	else
	{
		std::cout << "Disabling vertical sync... ";
	}
	
	if (SDL_GL_SetSwapInterval(swapInterval) != 0)
	{
		std::cout << "failed: \"" << SDL_GetError() << "\"" << std::endl;
		swapInterval = SDL_GL_GetSwapInterval();
	}
	else
	{
		std::cout << "success" << std::endl;
	}
	
	// Save settings
	settings.set("swap_interval", swapInterval);
	saveUserSettings();
}

void Application::saveUserSettings()
{
	std::cout << "Saving user setttings to \"" << userSettingsFilename << "\"... ";
	if (!settings.save(userSettingsFilename))
	{
		std::cout << "failed" << std::endl;
	}
	else
	{
		std::cout << "success" << std::endl;
	}
}

bool Application::loadModels()
{
	antModel = modelLoader->load("data/models/debug-worker.mdl");
	antHillModel = modelLoader->load("data/models/ant-hill.mdl");
	nestModel = modelLoader->load("data/models/nest.mdl");
	
	if (!antModel || !antHillModel || !nestModel)
	{
		return false;
	}
	
	antModelInstance.setModel(antModel);
	antModelInstance.setTransform(Transform::getIdentity());	
	antHillModelInstance.setModel(antHillModel);
	antHillModelInstance.setRotation(glm::angleAxis(glm::radians(90.0f), Vector3(1, 0, 0)));
	nestModelInstance.setModel(nestModel);
	
	// Create terrain
	terrain.create(255, 255, Vector3(50, 20, 50));
	
	return true;
}

bool Application::loadScene()
{
	// Create scene layers
	backgroundLayer = scene.addLayer();
	defaultLayer = scene.addLayer();
	uiLayer = scene.addLayer();
	
	// BG
	bgBatch.resize(1);
	BillboardBatch::Range* bgRange = bgBatch.addRange();
	bgRange->start = 0;
	bgRange->length = 1;
	Billboard* bgBillboard = bgBatch.getBillboard(0);
	bgBillboard->setDimensions(Vector2(1.0f, 1.0f));
	bgBillboard->setTranslation(Vector3(0.5f, 0.5f, 0.0f));
	bgBillboard->setTintColor(Vector4(1, 1, 1, 1));
	bgBatch.update();
	
	vignettePass.setRenderTarget(&defaultRenderTarget);
	bgCompositor.addPass(&vignettePass);
	bgCompositor.load(nullptr);
	bgCamera.setOrthographic(0, 1.0f, 1.0f, 0, -1.0f, 1.0f);
	bgCamera.lookAt(glm::vec3(0), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));
	bgCamera.setCompositor(&bgCompositor);
	bgCamera.setCompositeIndex(0);
	
	// Setup soil pass
	soilPass.setRenderTarget(&defaultRenderTarget);
	defaultCompositor.addPass(&soilPass);
	
	// Setup lighting pass
	lightingPass.setRenderTarget(&defaultRenderTarget);
	lightingPass.setShadowMap(0);
	lightingPass.setShadowCamera(&camera);
	defaultCompositor.addPass(&lightingPass);
	
	// Setup debug pass
	debugPass.setRenderTarget(&defaultRenderTarget);
	defaultCompositor.addPass(&debugPass);
	

	// Load compositor
	defaultCompositor.load(nullptr);
	
	// Setup camera
	camera.lookAt(
	glm::vec3(0.0f, 0.0f, 10.0f),
	glm::vec3(0.0f, 0.0f, 0.0f),
	glm::vec3(0.0f, 1.0f, 0.0f));
	camera.setCompositor(&defaultCompositor);
	camera.setCompositeIndex(0);
	defaultLayer->addObject(&camera);
	
	// Debug
	lineBatcher = new LineBatcher(4096);
	BillboardBatch* lineBatch = lineBatcher->getBatch();
	lineBatch->setAlignment(&camera, BillboardAlignmentMode::CYLINDRICAL);
	lineBatch->setAlignmentVector(Vector3(1, 0, 0));
	defaultLayer->addObject(lineBatch);
	
	return true;
}

bool Application::loadUI()
{
	// Load fonts
	FontLoader* fontLoader = new FontLoader();
	
	menuFont = new Font(512, 512);
	if (!fontLoader->load("data/fonts/Varela-Regular.ttf", fontSizePX, menuFont))
	{
		std::cerr << "Failed to load menu font" << std::endl;
	}
	
	copyrightFont = new Font(256, 256);
	if (!fontLoader->load("data/fonts/Varela-Regular.ttf", (int)(fontSizePX * 0.8f + 0.5f), copyrightFont))
	{
		std::cerr << "Failed to load copyright font" << std::endl;
	}
	
	delete fontLoader;
	
	// Load UI textures
	textureLoader->setGamma(1.0f);
	textureLoader->setCubemap(false);
	textureLoader->setMipmapChain(false);
	textureLoader->setMaxAnisotropy(1.0f);
	splashTexture = textureLoader->load("data/textures/ui-splash.png");
	titleTexture = textureLoader->load("data/textures/ui-title.png");
	levelActiveTexture = textureLoader->load("data/textures/ui-level-active.png");
	levelInactiveTexture = textureLoader->load("data/textures/ui-level-inactive.png");
	levelConnectorTexture = textureLoader->load("data/textures/ui-level-connector.png");
	pauseButtonTexture = textureLoader->load("data/textures/pause-button.png");
	playButtonTexture = textureLoader->load("data/textures/play-button.png");
	
	// Get strings
	std::string pressAnyKeyString;
	std::string copyrightString;
	std::string versionString;
	std::string backString;
	std::string challengeString;
	std::string experimentString;
	std::string settingsString;
	std::string quitString;
	std::string loadString;
	std::string newString;
	std::string videoString;
	std::string audioString;
	std::string controlsString;
	std::string gameString;
	std::string resumeString;
	std::string returnToMainMenuString;
	std::string quitToDesktopString;
	strings.get("press-any-key", &pressAnyKeyString);
	strings.get("copyright", &copyrightString);
	strings.get("version", &versionString);
	strings.get("back", &backString);
	strings.get("challenge", &challengeString);
	strings.get("experiment", &experimentString);
	strings.get("settings", &settingsString);
	strings.get("quit", &quitString);
	strings.get("load", &loadString);
	strings.get("new", &newString);
	strings.get("video", &videoString);
	strings.get("audio", &audioString);
	strings.get("controls", &controlsString);
	strings.get("game", &gameString);
	strings.get("resume", &resumeString);
	strings.get("return-to-main-menu", &returnToMainMenuString);
	strings.get("quit-to-desktop", &quitToDesktopString);
	
	// Set colors
	selectedColor = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	deselectedColor = Vector4(1.0f, 1.0f, 1.0f, 0.35f);
	
	// Setup root UI element
	uiRootElement = new UIContainer();
	uiRootElement->setDimensions(Vector2(width, height));
	mouse->addMouseMotionObserver(uiRootElement);
	mouse->addMouseButtonObserver(uiRootElement);
	
	// Create blackout element (for screen transitions)
	blackoutImage = new UIImage();
	blackoutImage->setDimensions(Vector2(width, height));
	blackoutImage->setLayerOffset(98);
	blackoutImage->setTintColor(Vector4(0.0f, 0.0f, 0.0f, 1.0f));
	blackoutImage->setVisible(false);
	uiRootElement->addChild(blackoutImage);
	
	// Create splash screen element
	splashImage = new UIImage();
	splashImage->setAnchor(Anchor::CENTER);
	splashImage->setDimensions(Vector2(splashTexture->getWidth(), splashTexture->getHeight()));
	splashImage->setTexture(splashTexture);
	splashImage->setVisible(false);
	uiRootElement->addChild(splashImage);
	
	// Create game title element
	titleImage = new UIImage();
	titleImage->setAnchor(Vector2(0.5f, 0.0f));
	titleImage->setDimensions(Vector2(titleTexture->getWidth(), titleTexture->getHeight()));
	titleImage->setTranslation(Vector2(0.0f, (int)(height * (1.0f / 4.0f) - titleTexture->getHeight() * 0.5f)));
	titleImage->setTexture(titleTexture);
	titleImage->setVisible(false);
	uiRootElement->addChild(titleImage);
	
	// Create Title screen info element
	titleScreenInfoContainer = new UIContainer();
	titleScreenInfoContainer->setDimensions(Vector2(width, height));
	titleScreenInfoContainer->setVisible(false);
	uiRootElement->addChild(titleScreenInfoContainer);
	
	// Create copyright element
	copyrightLabel = new UILabel();
	copyrightLabel->setAnchor(Vector2(0.0f, 1.0f));
	copyrightLabel->setFont(copyrightFont);
	copyrightLabel->setTranslation(Vector2((int)(width * 0.025f), (int)(-height * 0.025f)));
	copyrightLabel->setText(copyrightString);
	titleScreenInfoContainer->addChild(copyrightLabel);
	
	// Create version element
	versionLabel = new UILabel();
	versionLabel->setAnchor(Vector2(1.0f, 1.0f));
	versionLabel->setFont(copyrightFont);
	versionLabel->setTranslation(Vector2((int)(-width * 0.025f), (int)(-height * 0.025f)));
	versionLabel->setText(versionString);
	titleScreenInfoContainer->addChild(versionLabel);
	
	frameTimeLabel = new UILabel();
	frameTimeLabel->setAnchor(Vector2(0.0f, 0.0f));
	frameTimeLabel->setLayerOffset(99);
	frameTimeLabel->setFont(copyrightFont);
	frameTimeLabel->setTranslation(Vector2(0.0f));
	frameTimeLabel->setTintColor(Vector4(1.0f, 1.0f, 0.0f, 1.0f));
	frameTimeLabel->setText("");
	uiRootElement->addChild(frameTimeLabel);
	
	bool frameTimeLabelVisible = false;
	settings.get("show_frame_time", &frameTimeLabelVisible);
	frameTimeLabel->setVisible(frameTimeLabelVisible);
	
	// Create "Press any key" element
	anyKeyLabel = new UILabel();
	anyKeyLabel->setAnchor(Vector2(0.5f, 1.0f));
	anyKeyLabel->setFont(menuFont);
	anyKeyLabel->setTranslation(Vector2(0.0f, (int)(-height * (1.0f / 4.0f) - menuFont->getMetrics().getHeight() * 0.5f)));
	anyKeyLabel->setText(pressAnyKeyString);
	anyKeyLabel->setVisible(false);
	uiRootElement->addChild(anyKeyLabel);
	
	// Create main menu selector element
	menuSelectorLabel = new UILabel();
	menuSelectorLabel->setAnchor(Anchor::TOP_LEFT);
	menuSelectorLabel->setFont(menuFont);
	menuSelectorLabel->setText("<");
	menuSelectorLabel->setTintColor(selectedColor);
	menuSelectorLabel->setVisible(false);
	uiRootElement->addChild(menuSelectorLabel);
	
	// Create main menu elements
	mainMenuContainer = new UIContainer();
	mainMenuContainer->setDimensions(Vector2(width, menuFont->getMetrics().getHeight() * 4));
	mainMenuContainer->setAnchor(Vector2(0.0f, 0.5f));
	mainMenuContainer->setVisible(false);
	mainMenuContainer->setActive(false);
	uiRootElement->addChild(mainMenuContainer);
	challengeLabel = new UILabel();
	challengeLabel->setFont(menuFont);
	challengeLabel->setText(challengeString);
	challengeLabel->setTintColor(deselectedColor);
	experimentLabel = new UILabel();
	experimentLabel->setFont(menuFont);
	experimentLabel->setText(experimentString);
	experimentLabel->setTranslation(Vector2(0.0f, menuFont->getMetrics().getHeight()));
	experimentLabel->setTintColor(deselectedColor);
	settingsLabel = new UILabel();
	settingsLabel->setFont(menuFont);
	settingsLabel->setText(settingsString);
	settingsLabel->setTranslation(Vector2(0.0f, menuFont->getMetrics().getHeight() * 2));
	settingsLabel->setTintColor(deselectedColor);
	quitLabel = new UILabel();
	quitLabel->setFont(menuFont);
	quitLabel->setText(quitString);
	quitLabel->setTintColor(deselectedColor);
	quitLabel->setTranslation(Vector2(0.0f, menuFont->getMetrics().getHeight() * 3));
	mainMenuContainer->addChild(challengeLabel);
	mainMenuContainer->addChild(experimentLabel);
	mainMenuContainer->addChild(settingsLabel);
	mainMenuContainer->addChild(quitLabel);
	
	// Create challenge menu elements
	challengeMenuContainer = new UIContainer();
	challengeMenuContainer->setDimensions(Vector2(width, menuFont->getMetrics().getHeight() * 4));
	challengeMenuContainer->setAnchor(Vector2(0.0f, 0.5f));
	challengeMenuContainer->setVisible(false);
	challengeMenuContainer->setActive(false);
	uiRootElement->addChild(challengeMenuContainer);
	
	// Create experiment menu elements
	experimentMenuContainer = new UIContainer();
	experimentMenuContainer->setDimensions(Vector2(width, menuFont->getMetrics().getHeight() * 3));
	experimentMenuContainer->setAnchor(Vector2(0.0f, 0.5f));
	experimentMenuContainer->setVisible(false);
	experimentMenuContainer->setActive(false);
	uiRootElement->addChild(experimentMenuContainer);
	loadLabel = new UILabel();
	loadLabel->setFont(menuFont);
	loadLabel->setText(loadString);
	loadLabel->setTintColor(deselectedColor);
	loadLabel->setTranslation(Vector2(0.0f, menuFont->getMetrics().getHeight() * 0));
	experimentMenuContainer->addChild(loadLabel);
	newLabel = new UILabel();
	newLabel->setFont(menuFont);
	newLabel->setText(newString);
	newLabel->setTintColor(deselectedColor);
	newLabel->setTranslation(Vector2(0.0f, menuFont->getMetrics().getHeight() * 1));
	experimentMenuContainer->addChild(newLabel);
	experimentBackLabel = new UILabel();
	experimentBackLabel->setFont(menuFont);
	experimentBackLabel->setText(backString);
	experimentBackLabel->setTintColor(deselectedColor);
	experimentBackLabel->setTranslation(Vector2(0.0f, menuFont->getMetrics().getHeight() * 2));
	experimentMenuContainer->addChild(experimentBackLabel);
	
	// Create settings menu elements
	settingsMenuContainer = new UIContainer();
	settingsMenuContainer->setDimensions(Vector2(width, menuFont->getMetrics().getHeight() * 5));
	settingsMenuContainer->setAnchor(Vector2(0.0f, 0.5f));
	settingsMenuContainer->setVisible(false);
	settingsMenuContainer->setActive(false);
	uiRootElement->addChild(settingsMenuContainer);
	videoLabel = new UILabel();
	videoLabel->setFont(menuFont);
	videoLabel->setText(videoString);
	videoLabel->setTintColor(deselectedColor);
	videoLabel->setTranslation(Vector2(0.0f, menuFont->getMetrics().getHeight() * 0));
	settingsMenuContainer->addChild(videoLabel);
	audioLabel = new UILabel();
	audioLabel->setFont(menuFont);
	audioLabel->setText(audioString);
	audioLabel->setTintColor(deselectedColor);
	audioLabel->setTranslation(Vector2(0.0f, menuFont->getMetrics().getHeight() * 1));
	settingsMenuContainer->addChild(audioLabel);
	controlsLabel = new UILabel();
	controlsLabel->setFont(menuFont);
	controlsLabel->setText(controlsString);
	controlsLabel->setTintColor(deselectedColor);
	controlsLabel->setTranslation(Vector2(0.0f, menuFont->getMetrics().getHeight() * 2));
	settingsMenuContainer->addChild(controlsLabel);
	gameLabel = new UILabel();
	gameLabel->setFont(menuFont);
	gameLabel->setText(gameString);
	gameLabel->setTintColor(deselectedColor);
	gameLabel->setTranslation(Vector2(0.0f, menuFont->getMetrics().getHeight() * 3));
	settingsMenuContainer->addChild(gameLabel);
	settingsBackLabel = new UILabel();
	settingsBackLabel->setFont(menuFont);
	settingsBackLabel->setText(backString);
	settingsBackLabel->setTintColor(deselectedColor);
	settingsBackLabel->setTranslation(Vector2(0.0f, menuFont->getMetrics().getHeight() * 4));
	settingsMenuContainer->addChild(settingsBackLabel);
	
	// Create pause menu elements
	pauseMenuContainer = new UIContainer();
	pauseMenuContainer->setDimensions(Vector2(width, menuFont->getMetrics().getHeight() * 6));
	pauseMenuContainer->setAnchor(Anchor::CENTER);
	pauseMenuContainer->setVisible(false);
	pauseMenuContainer->setActive(false);
	uiRootElement->addChild(pauseMenuContainer);
	pausedResumeLabel = new UILabel();
	pausedResumeLabel->setFont(menuFont);
	pausedResumeLabel->setText(resumeString);
	pausedResumeLabel->setTintColor(deselectedColor);
	pausedResumeLabel->setTranslation(Vector2(0.0f, menuFont->getMetrics().getHeight() * 0));
	pauseMenuContainer->addChild(pausedResumeLabel);
	returnToMainMenuLabel = new UILabel();
	returnToMainMenuLabel->setFont(menuFont);
	returnToMainMenuLabel->setText(returnToMainMenuString);
	returnToMainMenuLabel->setTintColor(deselectedColor);
	returnToMainMenuLabel->setTranslation(Vector2(0.0f, menuFont->getMetrics().getHeight() * 1));
	pauseMenuContainer->addChild(returnToMainMenuLabel);
	quitToDesktopLabel = new UILabel();
	quitToDesktopLabel->setFont(menuFont);
	quitToDesktopLabel->setText(quitToDesktopString);
	quitToDesktopLabel->setTintColor(deselectedColor);
	quitToDesktopLabel->setTranslation(Vector2(0.0f, menuFont->getMetrics().getHeight() * 2));
	pauseMenuContainer->addChild(quitToDesktopLabel);
	
	// Create level selector elements
	levelSelectorContainer = new UIContainer();
	levelSelectorContainer->setDimensions(Vector2(levelActiveTexture->getWidth() * 10 + 48 * 9, levelActiveTexture->getHeight()));
	levelSelectorContainer->setAnchor(Vector2(0.5f, 1.0f));
	levelSelectorContainer->setTranslation(Vector2(0.0f, -levelActiveTexture->getHeight()));
	levelSelectorContainer->setVisible(false);
	levelSelectorContainer->setActive(false);
	uiRootElement->addChild(levelSelectorContainer);
	for (int i = 0; i < 10; ++i)
	{
		levelSelections[i] = new UIImage();
		levelSelections[i]->setAnchor(Vector2(0.0f, 0.5f));
		levelSelections[i]->setDimensions(Vector2(levelActiveTexture->getWidth(), levelActiveTexture->getHeight()));
		levelSelections[i]->setTranslation(Vector2(i * 96.0f, 0.0f));
		levelSelections[i]->setTexture(levelInactiveTexture);
		levelSelections[i]->setVisible(true);
		levelSelectorContainer->addChild(levelSelections[i]);
				
		if (i < 9)
		{
			levelConnectors[i] = new UIImage();
			levelConnectors[i]->setAnchor(Vector2(0.0f, 0.5f));
			levelConnectors[i]->setDimensions(Vector2(levelConnectorTexture->getWidth(), levelConnectorTexture->getHeight()));
			levelConnectors[i]->setTranslation(Vector2((i + 1) * 96.0f - 50.0f, 0.0f));
			levelConnectors[i]->setTexture(levelConnectorTexture);
			levelConnectors[i]->setVisible(true);
			levelSelectorContainer->addChild(levelConnectors[i]);
		}
	}
	
	// Create pause/play button elements
	pauseButtonImage = new UIImage();
	pauseButtonImage->setAnchor(Vector2(0.0f, 1.0f));
	pauseButtonImage->setDimensions(Vector2(pauseButtonTexture->getWidth(), pauseButtonTexture->getHeight()));
	pauseButtonImage->setTranslation(Vector2(16.0f, -16.0f));
	pauseButtonImage->setTexture(pauseButtonTexture);
	pauseButtonImage->setVisible(false);
	pauseButtonImage->setActive(false);
	uiRootElement->addChild(pauseButtonImage);
	
	playButtonImage = new UIImage();
	playButtonImage->setAnchor(Vector2(0.0f, 1.0f));
	playButtonImage->setDimensions(Vector2(playButtonTexture->getWidth(), playButtonTexture->getHeight()));
	playButtonImage->setTranslation(Vector2(16.0f, -16.0f));
	playButtonImage->setTexture(playButtonTexture);
	playButtonImage->setVisible(false);
	playButtonImage->setActive(false);
	uiRootElement->addChild(playButtonImage);
	
	// Create tweener
	tweener = new Tweener();
	
	// Setup screen fade in/fade out tween
	fadeInTween = new Tween<Vector4>(EaseFunction::IN_CUBIC, 0.0f, 1.5f, Vector4(0.0f, 0.0f, 0.0f, 1.0f), Vector4(0.0f, 0.0f, 0.0f, -1.0f));
	fadeInTween->setUpdateCallback(std::bind(UIElement::setTintColor, blackoutImage, std::placeholders::_1));
	tweener->addTween(fadeInTween);
	fadeOutTween = new Tween<Vector4>(EaseFunction::OUT_CUBIC, 0.0f, 1.5f, Vector4(0.0f, 0.0f, 0.0f, 0.0f), Vector4(0.0f, 0.0f, 0.0f, 1.0f));
	fadeOutTween->setUpdateCallback(std::bind(UIElement::setTintColor, blackoutImage, std::placeholders::_1));
	tweener->addTween(fadeOutTween);
	
	// Setup splash screen tween
	splashFadeInTween = new Tween<Vector4>(EaseFunction::IN_CUBIC, 0.0f, 0.5f, Vector4(1.0f, 1.0f, 1.0f, 0.0f), Vector4(0.0f, 0.0f, 0.0f, 1.0f));
	splashFadeInTween->setUpdateCallback(std::bind(UIElement::setTintColor, splashImage, std::placeholders::_1));
	tweener->addTween(splashFadeInTween);
	
	splashHangTween = new Tween<float>(EaseFunction::OUT_CUBIC, 0.0f, 1.0f, 0.0f, 1.0f);
	tweener->addTween(splashHangTween);
	
	splashFadeOutTween = new Tween<Vector4>(EaseFunction::OUT_CUBIC, 0.0f, 0.5f, Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector4(0.0f, 0.0f, 0.0f, -1.0f));
	splashFadeOutTween->setUpdateCallback(std::bind(UIElement::setTintColor, splashImage, std::placeholders::_1));
	tweener->addTween(splashFadeOutTween);
	
	splashFadeInTween->setEndCallback(std::bind(TweenBase::start, splashHangTween));
	splashHangTween->setEndCallback(std::bind(TweenBase::start, splashFadeOutTween));
	splashFadeOutTween->setEndCallback(std::bind(Application::changeState, this, titleState));
	
	// Setup game title tween
	titleFadeInTween = new Tween<Vector4>(EaseFunction::IN_CUBIC, 0.0f, 2.0f, Vector4(1.0f, 1.0f, 1.0f, 0.0f), Vector4(0.0f, 0.0f, 0.0f, 1.0f));
	titleFadeInTween->setUpdateCallback(std::bind(UIElement::setTintColor, titleImage, std::placeholders::_1));
	tweener->addTween(titleFadeInTween);
	titleFadeOutTween = new Tween<Vector4>(EaseFunction::OUT_CUBIC, 0.0f, 0.25f, Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector4(0.0f, 0.0f, 0.0f, -1.0f));
	titleFadeOutTween->setUpdateCallback(std::bind(UIElement::setTintColor, titleImage, std::placeholders::_1));
	tweener->addTween(titleFadeOutTween);
	
	// Setup copyright tween
	copyrightFadeInTween = new Tween<Vector4>(EaseFunction::IN_CUBIC, 0.0f, 1.0f, Vector4(0.0f, 0.0f, 0.0f, 0.0f), Vector4(0.0f, 0.0f, 0.0f, 0.5f));
	copyrightFadeInTween->setUpdateCallback(std::bind(UIElement::setTintColor, titleScreenInfoContainer, std::placeholders::_1));
	tweener->addTween(copyrightFadeInTween);
	copyrightFadeOutTween = new Tween<Vector4>(EaseFunction::OUT_CUBIC, 0.0f, 0.25f, Vector4(0.0f, 0.0f, 0.0f, 0.5f), Vector4(0.0f, 0.0f, 0.0f, -0.5f));
	copyrightFadeOutTween->setUpdateCallback(std::bind(UIElement::setTintColor, titleScreenInfoContainer, std::placeholders::_1));
	tweener->addTween(copyrightFadeOutTween);
	
	// Setup "Press any key" tween
	anyKeyFadeInTween = new Tween<Vector4>(EaseFunction::LINEAR, 0.0f, 1.5f, Vector4(0.0f, 0.0f, 0.0f, 0.0f), Vector4(0.0f, 0.0f, 0.0f, 1.0f));
	anyKeyFadeInTween->setUpdateCallback(std::bind(UIElement::setTintColor, anyKeyLabel, std::placeholders::_1));
	tweener->addTween(anyKeyFadeInTween);
	anyKeyFadeOutTween = new Tween<Vector4>(EaseFunction::LINEAR, 0.0f, 1.5f, Vector4(0.0f, 0.0f, 0.0f, 1.0f), Vector4(0.0f, 0.0f, 0.0f, -1.0f));
	anyKeyFadeOutTween->setUpdateCallback(std::bind(UIElement::setTintColor, anyKeyLabel, std::placeholders::_1));
	anyKeyFadeInTween->setEndCallback(std::bind(TweenBase::start, anyKeyFadeOutTween));
	anyKeyFadeOutTween->setEndCallback(std::bind(TweenBase::start, anyKeyFadeInTween));
	tweener->addTween(anyKeyFadeOutTween);
	
	float menuFadeInDuration = 0.15f;
	Vector4 menuFadeInStartColor = Vector4(1.0f, 1.0f, 1.0f, 0.0f);
	Vector4 menuFadeInDeltaColor = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
	float menuFadeOutDuration = 0.15f;
	Vector4 menuFadeOutStartColor = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	Vector4 menuFadeOutDeltaColor = Vector4(0.0f, 0.0f, 0.0f, -1.0f);
	float menuSlideInDuration = 0.35f;
	Vector2 menuSlideInStartTranslation = Vector2(-64.0f, 0.0f);
	Vector2 menuSlideInDeltaTranslation = Vector2((int)(64.0f + width / 8.0f), 0.0f);
	
	float levelSelectorSlideInDuration = 0.35f;
	Vector2 levelSelectorSlideInStartTranslation = Vector2(0.0f, levelActiveTexture->getHeight());
	Vector2 levelSelectorSlideInDeltaTranslation = Vector2(0.0f, -levelActiveTexture->getHeight() * 2.0f);
	
	// Setup main menu tween
	menuFadeInTween = new Tween<Vector4>(EaseFunction::OUT_QUINT, 0.0f, menuFadeInDuration, menuFadeInStartColor, menuFadeInDeltaColor);
	tweener->addTween(menuFadeInTween);
	menuFadeOutTween = new Tween<Vector4>(EaseFunction::OUT_QUINT, 0.0f, menuFadeOutDuration, menuFadeOutStartColor, menuFadeOutDeltaColor);
	tweener->addTween(menuFadeOutTween);
	menuSlideInTween = new Tween<Vector2>(EaseFunction::OUT_QUINT, 0.0f, menuSlideInDuration, menuSlideInStartTranslation, menuSlideInDeltaTranslation);
	tweener->addTween(menuSlideInTween);
	
	// Setup level selector tween
	levelSelectorSlideInTween = new Tween<Vector2>(EaseFunction::OUT_QUINT, 0.0f, levelSelectorSlideInDuration, levelSelectorSlideInStartTranslation, levelSelectorSlideInDeltaTranslation);
	tweener->addTween(levelSelectorSlideInTween);
	
	// Title screen zoom in tween
	antHillZoomInTween = new Tween<float>(EaseFunction::LINEAR, 0.0f, 2.0f, 50.0f, -49.9f);
	antHillZoomInTween->setUpdateCallback(std::bind(SurfaceCameraController::setTargetFocalDistance, surfaceCam, std::placeholders::_1));
	tweener->addTween(antHillZoomInTween);
	
	antHillFadeOutTween = new Tween<Vector4>(EaseFunction::IN_CUBIC, 0.0f, 2.0f, Vector4(0.0f, 0.0f, 0.0f, 0.0f), Vector4(0.0f, 0.0f, 0.0f, 1.0f));
	antHillFadeOutTween->setUpdateCallback(std::bind(UIElement::setTintColor, blackoutImage, std::placeholders::_1));
	antHillFadeOutTween->setEndCallback(std::bind(Application::changeState, this, mainMenuState));
	tweener->addTween(antHillFadeOutTween);
	
	playButtonFadeTween = new Tween<Vector4>(EaseFunction::OUT_CUBIC, 0.0f, 1.0f, Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector4(0.0f, 0.0f, 0.0f, -1.0f));
	playButtonFadeTween->setUpdateCallback(std::bind(UIElement::setTintColor, playButtonImage, std::placeholders::_1));
	playButtonFadeTween->setEndCallback(std::bind(UIElement::setVisible, playButtonImage, false));
	tweener->addTween(playButtonFadeTween);
	
	// Build menu system
	selectedMenuItemIndex = 0;
	mainMenu = new Menu();
	MenuItem* challengeItem = mainMenu->addItem();
	challengeItem->setSelectedCallback(std::bind(UIElement::setTintColor, challengeLabel, selectedColor));
	challengeItem->setDeselectedCallback(std::bind(UIElement::setTintColor, challengeLabel, deselectedColor));
	challengeItem->setActivatedCallback(std::bind(Application::enterLevelSelection, this));
	challengeLabel->setMouseOverCallback(std::bind(Application::selectMenuItem, this, challengeItem->getIndex()));
	challengeLabel->setMouseMovedCallback(std::bind(Application::selectMenuItem, this, challengeItem->getIndex()));
	challengeLabel->setMousePressedCallback(std::bind(Application::activateMenuItem, this, challengeItem->getIndex()));
	MenuItem* experimentItem = mainMenu->addItem();
	experimentItem->setSelectedCallback(std::bind(UIElement::setTintColor, experimentLabel, selectedColor));
	experimentItem->setDeselectedCallback(std::bind(UIElement::setTintColor, experimentLabel, deselectedColor));
	experimentItem->setActivatedCallback(std::bind(Application::enterMenu, this, 2));
	experimentLabel->setMouseOverCallback(std::bind(Application::selectMenuItem, this, experimentItem->getIndex()));
	experimentLabel->setMouseMovedCallback(std::bind(Application::selectMenuItem, this, experimentItem->getIndex()));
	experimentLabel->setMousePressedCallback(std::bind(Application::activateMenuItem, this, experimentItem->getIndex()));
	MenuItem* settingsItem = mainMenu->addItem();
	settingsItem->setSelectedCallback(std::bind(UIElement::setTintColor, settingsLabel, selectedColor));
	settingsItem->setDeselectedCallback(std::bind(UIElement::setTintColor, settingsLabel, deselectedColor));
	settingsItem->setActivatedCallback(std::bind(Application::enterMenu, this, 3));
	settingsLabel->setMouseOverCallback(std::bind(Application::selectMenuItem, this, settingsItem->getIndex()));
	settingsLabel->setMouseMovedCallback(std::bind(Application::selectMenuItem, this, settingsItem->getIndex()));
	settingsLabel->setMousePressedCallback(std::bind(Application::activateMenuItem, this, settingsItem->getIndex()));
	MenuItem* quitItem = mainMenu->addItem();
	quitItem->setSelectedCallback(std::bind(UIElement::setTintColor, quitLabel, selectedColor));
	quitItem->setDeselectedCallback(std::bind(UIElement::setTintColor, quitLabel, deselectedColor));
	quitItem->setActivatedCallback(std::bind(Application::close, this, EXIT_SUCCESS));
	quitLabel->setMouseOverCallback(std::bind(Application::selectMenuItem, this, quitItem->getIndex()));
	quitLabel->setMouseMovedCallback(std::bind(Application::selectMenuItem, this, quitItem->getIndex()));
	quitLabel->setMousePressedCallback(std::bind(Application::activateMenuItem, this, quitItem->getIndex()));
	
	experimentMenu = new Menu();
	MenuItem* loadItem = experimentMenu->addItem();
	loadItem->setSelectedCallback(std::bind(UIElement::setTintColor, loadLabel, selectedColor));
	loadItem->setDeselectedCallback(std::bind(UIElement::setTintColor, loadLabel, deselectedColor));
	loadItem->setActivatedCallback(std::bind(std::printf, "0\n"));
	loadLabel->setMouseOverCallback(std::bind(Application::selectMenuItem, this, loadItem->getIndex()));
	loadLabel->setMouseMovedCallback(std::bind(Application::selectMenuItem, this, loadItem->getIndex()));
	loadLabel->setMousePressedCallback(std::bind(Application::activateMenuItem, this, loadItem->getIndex()));
	MenuItem* newItem = experimentMenu->addItem();
	newItem->setSelectedCallback(std::bind(UIElement::setTintColor, newLabel, selectedColor));
	newItem->setDeselectedCallback(std::bind(UIElement::setTintColor, newLabel, deselectedColor));
	newItem->setActivatedCallback(std::bind(std::printf, "bla\n"));
	newLabel->setMouseOverCallback(std::bind(Application::selectMenuItem, this, newItem->getIndex()));
	newLabel->setMouseMovedCallback(std::bind(Application::selectMenuItem, this, newItem->getIndex()));
	newLabel->setMousePressedCallback(std::bind(Application::activateMenuItem, this, newItem->getIndex()));
	MenuItem* experimentBackItem = experimentMenu->addItem();
	experimentBackItem->setSelectedCallback(std::bind(UIElement::setTintColor, experimentBackLabel, selectedColor));
	experimentBackItem->setDeselectedCallback(std::bind(UIElement::setTintColor, experimentBackLabel, deselectedColor));
	experimentBackItem->setActivatedCallback(std::bind(Application::enterMenu, this, 0));
	experimentBackLabel->setMouseOverCallback(std::bind(Application::selectMenuItem, this, experimentBackItem->getIndex()));
	experimentBackLabel->setMouseMovedCallback(std::bind(Application::selectMenuItem, this, experimentBackItem->getIndex()));
	experimentBackLabel->setMousePressedCallback(std::bind(Application::activateMenuItem, this, experimentBackItem->getIndex()));
	
	settingsMenu = new Menu();
	MenuItem* videoItem = settingsMenu->addItem();
	videoItem->setSelectedCallback(std::bind(UIElement::setTintColor, videoLabel, selectedColor));
	videoItem->setDeselectedCallback(std::bind(UIElement::setTintColor, videoLabel, deselectedColor));
	videoItem->setActivatedCallback(std::bind(std::printf, "0\n"));
	videoLabel->setMouseOverCallback(std::bind(Application::selectMenuItem, this, videoItem->getIndex()));
	videoLabel->setMouseMovedCallback(std::bind(Application::selectMenuItem, this, videoItem->getIndex()));
	videoLabel->setMousePressedCallback(std::bind(Application::activateMenuItem, this, videoItem->getIndex()));
	MenuItem* audioItem = settingsMenu->addItem();
	audioItem->setSelectedCallback(std::bind(UIElement::setTintColor, audioLabel, selectedColor));
	audioItem->setDeselectedCallback(std::bind(UIElement::setTintColor, audioLabel, deselectedColor));
	audioItem->setActivatedCallback(std::bind(std::printf, "1\n"));
	audioLabel->setMouseOverCallback(std::bind(Application::selectMenuItem, this, audioItem->getIndex()));
	audioLabel->setMouseMovedCallback(std::bind(Application::selectMenuItem, this, audioItem->getIndex()));
	audioLabel->setMousePressedCallback(std::bind(Application::activateMenuItem, this, audioItem->getIndex()));
	MenuItem* controlsItem = settingsMenu->addItem();
	controlsItem->setSelectedCallback(std::bind(UIElement::setTintColor, controlsLabel, selectedColor));
	controlsItem->setDeselectedCallback(std::bind(UIElement::setTintColor, controlsLabel, deselectedColor));
	controlsItem->setActivatedCallback(std::bind(std::printf, "2\n"));
	controlsLabel->setMouseOverCallback(std::bind(Application::selectMenuItem, this, controlsItem->getIndex()));
	controlsLabel->setMouseMovedCallback(std::bind(Application::selectMenuItem, this, controlsItem->getIndex()));
	controlsLabel->setMousePressedCallback(std::bind(Application::activateMenuItem, this, controlsItem->getIndex()));
	MenuItem* gameItem = settingsMenu->addItem();
	gameItem->setSelectedCallback(std::bind(UIElement::setTintColor, gameLabel, selectedColor));
	gameItem->setDeselectedCallback(std::bind(UIElement::setTintColor, gameLabel, deselectedColor));
	gameItem->setActivatedCallback(std::bind(std::printf, "3\n"));
	gameLabel->setMouseOverCallback(std::bind(Application::selectMenuItem, this, gameItem->getIndex()));
	gameLabel->setMouseMovedCallback(std::bind(Application::selectMenuItem, this, gameItem->getIndex()));
	gameLabel->setMousePressedCallback(std::bind(Application::activateMenuItem, this, gameItem->getIndex()));
	MenuItem* settingsBackItem = settingsMenu->addItem();
	settingsBackItem->setSelectedCallback(std::bind(UIElement::setTintColor, settingsBackLabel, selectedColor));
	settingsBackItem->setDeselectedCallback(std::bind(UIElement::setTintColor, settingsBackLabel, deselectedColor));
	settingsBackItem->setActivatedCallback(std::bind(Application::enterMenu, this, 0));
	settingsBackLabel->setMouseOverCallback(std::bind(Application::selectMenuItem, this, settingsBackItem->getIndex()));
	settingsBackLabel->setMouseMovedCallback(std::bind(Application::selectMenuItem, this, settingsBackItem->getIndex()));
	settingsBackLabel->setMousePressedCallback(std::bind(Application::activateMenuItem, this, settingsBackItem->getIndex()));
	
	menuCount = 4;
	menus = new Menu*[menuCount];
	menus[0] = mainMenu;
	menus[1] = challengeMenu;
	menus[2] = experimentMenu;
	menus[3] = settingsMenu;
	
	menuContainers = new UIContainer*[menuCount];
	menuContainers[0] = mainMenuContainer;
	menuContainers[1] = challengeMenuContainer;
	menuContainers[2] = experimentMenuContainer;
	menuContainers[3] = settingsMenuContainer;
	
	currentMenu = mainMenu;
	currentMenuIndex = 0;
	selectedMenuItemIndex = 0;
	selectMenuItem(selectedMenuItemIndex);
	
	currentLevel = 0;
	levelSelectorMenu = new Menu();
	for (int i = 0; i < 10; ++i)
	{
		MenuItem* levelSelectionItem = levelSelectorMenu->addItem();
		levelSelectionItem->setSelectedCallback(std::bind(UIImage::setTexture, levelSelections[i], levelActiveTexture));
		levelSelectionItem->setDeselectedCallback(std::bind(UIImage::setTexture, levelSelections[i], levelInactiveTexture));
		levelSelectionItem->setActivatedCallback(std::bind(Application::loadLevel, this));
		
		levelSelections[i]->setMouseOverCallback(std::bind(Application::selectLevel, this, levelSelectionItem->getIndex()));
		levelSelections[i]->setMouseMovedCallback(std::bind(Application::selectLevel, this, levelSelectionItem->getIndex()));
		levelSelections[i]->setMousePressedCallback(std::bind(Application::activateLevel, this, levelSelectionItem->getIndex()));
	}
	
	// Setup UI batch
	uiBatch = new BillboardBatch();
	uiBatch->resize(256);
	uiBatcher = new UIBatcher();
	
	// Setup UI render pass and compositor
	uiPass.setRenderTarget(&defaultRenderTarget);
	uiCompositor.addPass(&uiPass);
	uiCompositor.load(nullptr);
	
	// Setup UI camera
	uiCamera.lookAt(glm::vec3(0), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));
	uiCamera.setCompositor(&uiCompositor);
	uiCamera.setCompositeIndex(0);
	
	// Setup UI scene
	uiLayer->addObject(uiBatch);
	uiLayer->addObject(&uiCamera);
	
	defaultRenderTarget.width = width;
	defaultRenderTarget.height = height;
	resizeUI();
	
	return true;
}

bool Application::loadControls()
{
	// Setup menu navigation controls
	menuControlProfile = new ControlProfile(inputManager);
	menuControlProfile->registerControl("menu_left", &menuLeft);
	menuControlProfile->registerControl("menu_right", &menuRight);
	menuControlProfile->registerControl("menu_up", &menuUp);
	menuControlProfile->registerControl("menu_down", &menuDown);
	menuControlProfile->registerControl("menu_select", &menuSelect);
	menuControlProfile->registerControl("menu_cancel", &menuCancel);
	menuControlProfile->registerControl("toggle_fullscreen", &toggleFullscreen);
	menuControlProfile->registerControl("escape", &escape);
	menuLeft.bindKey(keyboard, SDL_SCANCODE_LEFT);
	menuLeft.bindKey(keyboard, SDL_SCANCODE_A);
	menuRight.bindKey(keyboard, SDL_SCANCODE_RIGHT);
	menuRight.bindKey(keyboard, SDL_SCANCODE_D);
	menuUp.bindKey(keyboard, SDL_SCANCODE_UP);
	menuUp.bindKey(keyboard, SDL_SCANCODE_W);
	menuDown.bindKey(keyboard, SDL_SCANCODE_DOWN);
	menuDown.bindKey(keyboard, SDL_SCANCODE_S);
	menuSelect.bindKey(keyboard, SDL_SCANCODE_RETURN);
	menuSelect.bindKey(keyboard, SDL_SCANCODE_SPACE);
	menuSelect.bindKey(keyboard, SDL_SCANCODE_Z);
	menuCancel.bindKey(keyboard, SDL_SCANCODE_BACKSPACE);
	menuCancel.bindKey(keyboard, SDL_SCANCODE_X);
	toggleFullscreen.bindKey(keyboard, SDL_SCANCODE_F11);
	escape.bindKey(keyboard, SDL_SCANCODE_ESCAPE);
	
	// Setup in-game controls
	gameControlProfile = new ControlProfile(inputManager);
	gameControlProfile->registerControl("camera-move-forward", &cameraMoveForward);
	gameControlProfile->registerControl("camera-move-back", &cameraMoveBack);
	gameControlProfile->registerControl("camera-move-left", &cameraMoveLeft);
	gameControlProfile->registerControl("camera-move-right", &cameraMoveRight);
	gameControlProfile->registerControl("camera-rotate-cw", &cameraRotateCW);
	gameControlProfile->registerControl("camera-rotate-ccw", &cameraRotateCCW);
	gameControlProfile->registerControl("camera-zoom-in", &cameraZoomIn);
	gameControlProfile->registerControl("camera-zoom-out", &cameraZoomOut);
	gameControlProfile->registerControl("camera-toggle-nest-view", &cameraToggleNestView);
	gameControlProfile->registerControl("camera-toggle-overhead-view", &cameraToggleOverheadView);
	gameControlProfile->registerControl("walk-forward", &walkForward);
	gameControlProfile->registerControl("walk-back", &walkBack);
	gameControlProfile->registerControl("turn-left", &turnLeft);
	gameControlProfile->registerControl("turn-right", &turnRight);
	gameControlProfile->registerControl("toggle-pause", &togglePause);
	
	cameraMoveForward.bindKey(keyboard, SDL_SCANCODE_W);
	cameraMoveBack.bindKey(keyboard, SDL_SCANCODE_S);
	cameraMoveLeft.bindKey(keyboard, SDL_SCANCODE_A);
	cameraMoveRight.bindKey(keyboard, SDL_SCANCODE_D);
	cameraRotateCW.bindKey(keyboard, SDL_SCANCODE_Q);
	cameraRotateCCW.bindKey(keyboard, SDL_SCANCODE_E);
	cameraZoomIn.bindKey(keyboard, SDL_SCANCODE_EQUALS);
	cameraZoomOut.bindKey(keyboard, SDL_SCANCODE_MINUS);
	cameraZoomIn.bindMouseWheelAxis(mouse, MouseWheelAxis::POSITIVE_Y);
	cameraZoomOut.bindMouseWheelAxis(mouse, MouseWheelAxis::NEGATIVE_Y);
	cameraToggleOverheadView.bindKey(keyboard, SDL_SCANCODE_R);
	cameraToggleNestView.bindKey(keyboard, SDL_SCANCODE_F);
	walkForward.bindKey(keyboard, SDL_SCANCODE_UP);
	walkBack.bindKey(keyboard, SDL_SCANCODE_DOWN);
	turnLeft.bindKey(keyboard, SDL_SCANCODE_LEFT);
	turnRight.bindKey(keyboard, SDL_SCANCODE_RIGHT);
	togglePause.bindKey(keyboard, SDL_SCANCODE_SPACE);
	
	return true;
}

bool Application::loadGame()
{

	
	// Load biosphere
	biosphere.load("data/biomes/");
	
	// Load campaign
	campaign.load("data/levels/");
	currentWorld = 1;
	currentLevel = 1;
	simulationPaused = false;
	
	// Create colony
	colony = new Colony();
	colony->setAntModel(antModel);
	
	return true;
}

void Application::resizeUI()
{
	// Adjust UI dimensions
	uiRootElement->setDimensions(Vector2(width, height));
	uiRootElement->update();
	
	// Adjust UI camera projection
	uiCamera.setOrthographic(0, width, height, 0, -1.0f, 1.0f);
}

void Application::enterMenu(std::size_t index)
{
	if (index != currentMenuIndex)
	{
		exitMenu(currentMenuIndex);
	}
	
	// Select next menu
	currentMenuIndex = index;
	selectedMenuItemIndex = 0;
	currentMenu = menus[currentMenuIndex];
	menus[currentMenuIndex]->getItem(selectedMenuItemIndex)->select();
	
	// Start menu fade-in tween
	menuFadeInTween->setUpdateCallback(std::bind(UIElement::setTintColor, menuContainers[currentMenuIndex], std::placeholders::_1));
	menuFadeInTween->setEndCallback(std::bind(UIElement::setActive, menuContainers[currentMenuIndex], true));
	menuFadeInTween->reset();
	menuFadeInTween->start();
	
	// Start menu slide-in tween
	menuSlideInTween->setUpdateCallback(std::bind(UIElement::setTranslation, menuContainers[currentMenuIndex], std::placeholders::_1));
	menuSlideInTween->reset();
	menuSlideInTween->start();
	
	// Make menu visible
	menuContainers[currentMenuIndex]->setVisible(true);
	
	// Make menu selector visible
	menuSelectorLabel->setVisible(true);
}

void Application::exitMenu(std::size_t index)
{
	// Deactivate previous menu
	menuContainers[currentMenuIndex]->setActive(false);
	
	// Fade out previous menu
	menuFadeOutTween->setUpdateCallback(std::bind(UIElement::setTintColor, menuContainers[currentMenuIndex], std::placeholders::_1));
	menuFadeOutTween->setEndCallback(std::bind(UIElement::setVisible, menuContainers[currentMenuIndex], false));
	menuFadeOutTween->reset();
	menuFadeOutTween->start();
	
	// Make menu selector invisible
	menuSelectorLabel->setVisible(false);
}

void Application::selectMenuItem(std::size_t index)
{
	if (currentMenu == nullptr || index > currentMenu->getItemCount())
	{
		std::cout << "Selected invalid menu item" << std::endl;
		return;
	}
	
	MenuItem* previousItem = currentMenu->getItem(selectedMenuItemIndex);
	previousItem->deselect();
	
	selectedMenuItemIndex = index;
	
	MenuItem* nextItem = currentMenu->getItem(selectedMenuItemIndex);
	nextItem->select();
}

void Application::activateMenuItem(std::size_t index)
{
	if (index > menus[currentMenuIndex]->getItemCount())
	{
		std::cout << "Activated invalid menu item" << std::endl;
		return;
	}
	
	menus[currentMenuIndex]->getItem(index)->deselect();
	menus[currentMenuIndex]->getItem(index)->activate();
}

void Application::enterLevelSelection()
{
	exitMenu(0);
	
	currentWorld = 1;
	currentLevel = 1;
	
	// Start menu slide-in tween
	levelSelectorSlideInTween->setUpdateCallback(std::bind(UIElement::setTranslation, levelSelectorContainer, std::placeholders::_1));
	levelSelectorSlideInTween->reset();
	levelSelectorSlideInTween->start();
	
	// Make menu visible and active
	levelSelectorContainer->setVisible(true);
	levelSelectorContainer->setActive(true);
}

void Application::selectLevel(std::size_t index)
{
	if (index > levelSelectorMenu->getItemCount())
	{
		std::cout << "Selected invalid level" << std::endl;
		return;
	}
	
	MenuItem* previousItem = levelSelectorMenu->getItem(currentLevel - 1);
	previousItem->deselect();
	
	currentLevel = index + 1;
	
	MenuItem* nextItem = levelSelectorMenu->getItem(currentLevel - 1);
	nextItem->select();
}

void Application::activateLevel(std::size_t index)
{
	if (index > levelSelectorMenu->getItemCount())
	{
		std::cout << "Activated invalid level" << std::endl;
		return;
	}
	
	//levelSelectorMenu->getItem(currentLevel - 1)->deselect();
	levelSelectorMenu->getItem(currentLevel - 1)->activate();
}

void Application::loadLevel()
{
	if (currentLevel < 1 || currentLevel >= campaign.levels[currentWorld].size())
	{
		std::cout << "Attempted to load invalid level" << std::endl;
		return;
	}
	
	const Level* level = &campaign.levels[currentWorld][currentLevel];
	const Biome* biome = &biosphere.biomes[level->biome];
	
	soilPass.setHorizonOTexture(biome->soilHorizonO);
	soilPass.setHorizonATexture(biome->soilHorizonA);
	soilPass.setHorizonBTexture(biome->soilHorizonB);
	soilPass.setHorizonCTexture(biome->soilHorizonC);
	
	std::string heightmap = std::string("data/textures/") + level->heightmap;
	terrain.load(heightmap);
	
	changeState(playState);
}

void Application::pauseSimulation()
{
	simulationPaused = true;
	playButtonImage->setVisible(false);
	playButtonImage->setActive(false);
	pauseButtonImage->setVisible(true);
	pauseButtonImage->setActive(true);
	playButtonFadeTween->stop();
}

void Application::unpauseSimulation()
{
	simulationPaused = false;
	pauseButtonImage->setVisible(false);
	pauseButtonImage->setActive(false);
	playButtonImage->setTintColor(Vector4(1.0f));
	playButtonImage->setVisible(true);
	playButtonImage->setActive(true);
	playButtonFadeTween->start();
}