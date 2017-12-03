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
#include "states/game-state.hpp"
#include "game/colony.hpp"
#include "game/tool.hpp"
#include "ui/menu.hpp"
#include "ui/toolbar.hpp"
#include "ui/pie-menu.hpp"
#include "debug.hpp"
#include "camera-controller.hpp"
#include "configuration.hpp"
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <cstdio>
#include <sstream>
#include <SDL2/SDL.h>
#include <dirent.h>

#define OPENGL_VERSION_MAJOR 3
#define OPENGL_VERSION_MINOR 3

#undef max

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
	settings.get("swap_interval", &swapInterval);

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
	
	// Get all possible display modes for the default display
	int displayModeCount = SDL_GetNumDisplayModes(0);
	for (int i = displayModeCount - 1; i >= 0; --i)
	{
		SDL_DisplayMode displayMode;
		
		if (SDL_GetDisplayMode(0, i, &displayMode) != 0)
		{
			std::cerr << "Failed to get display mode: \"" << SDL_GetError() << "\"" << std::endl;
			close(EXIT_FAILURE);
			return;
		}
		
		resolutions.push_back(Vector2(displayMode.w, displayMode.h));		
	}
	
	// Read requested windowed and fullscreen resolutions from settings
	Vector2 requestedWindowedResolution;
	Vector2 requestedFullscreenResolution;
	settings.get("windowed_width", &requestedWindowedResolution.x);
	settings.get("windowed_height", &requestedWindowedResolution.y);
	settings.get("fullscreen_width", &requestedFullscreenResolution.x);
	settings.get("fullscreen_height", &requestedFullscreenResolution.y);
	
	// Determine desktop resolution
	SDL_DisplayMode desktopDisplayMode;
	if (SDL_GetDesktopDisplayMode(0, &desktopDisplayMode) != 0)
	{
		std::cerr << "Failed to get desktop display mode: \"" << SDL_GetError() << "\"" << std::endl;
		close(EXIT_FAILURE);
		return;
	}
	Vector2 desktopResolution;
	desktopResolution.x = static_cast<float>(desktopDisplayMode.w);
	desktopResolution.y = static_cast<float>(desktopDisplayMode.h);
	
	// Replace requested resolutions of -1 with native resolution
	requestedWindowedResolution.x = (requestedWindowedResolution.x == -1.0f) ? desktopResolution.x : requestedWindowedResolution.x;
	requestedWindowedResolution.y = (requestedWindowedResolution.y == -1.0f) ? desktopResolution.y : requestedWindowedResolution.y;
	requestedFullscreenResolution.x = (requestedFullscreenResolution.x == -1.0f) ? desktopResolution.x : requestedFullscreenResolution.x;
	requestedFullscreenResolution.y = (requestedFullscreenResolution.y == -1.0f) ? desktopResolution.y : requestedFullscreenResolution.y;
	
	// Find indices of closest resolutions to requested windowed and fullscreen resolutions
	windowedResolutionIndex = 0;
	fullscreenResolutionIndex = 0;
	float minWindowedResolutionDistance = std::numeric_limits<float>::max();
	float minFullscreenResolutionDistance = std::numeric_limits<float>::max();
	for (std::size_t i = 0; i < resolutions.size(); ++i)
	{
		Vector2 windowedResolutionDifference = resolutions[i] - requestedWindowedResolution;
		float windowedResolutionDistance = glm::dot(windowedResolutionDifference, windowedResolutionDifference);
		if (windowedResolutionDistance <= minWindowedResolutionDistance)
		{
			minWindowedResolutionDistance = windowedResolutionDistance;
			windowedResolutionIndex = i;
		}
		
		Vector2 fullscreenResolutionDifference = resolutions[i] - requestedFullscreenResolution;
		float fullscreenResolutionDistance = glm::dot(fullscreenResolutionDifference, fullscreenResolutionDifference);
		if (fullscreenResolutionDistance <= minFullscreenResolutionDistance)
		{
			minFullscreenResolutionDistance = fullscreenResolutionDistance;
			fullscreenResolutionIndex = i;
		}
	}
	
	// Determine window parameters and current resolution
	Uint32 windowFlags = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI;
	if (fullscreen)
	{
		resolution = resolutions[fullscreenResolutionIndex];
		windowFlags |= SDL_WINDOW_FULLSCREEN;
	}
	else
	{
		resolution = resolutions[windowedResolutionIndex];
	}
	
	// Get requested language
	languageIndex = 0;
	std::string requestedLanguage;
	settings.get("language", &requestedLanguage);
	
	// Find available languages
	{
		std::string stringsDirectory = appDataPath + "strings/";
		
		// Open strings directory
		DIR* dir = opendir(stringsDirectory.c_str());
		if (dir == nullptr)
		{
			std::cout << "Failed to open strings directory \"" << stringsDirectory << "\"" << std::endl;
			close(EXIT_FAILURE);
			return;
		}
		
		// Scan directory for .txt files
		for (struct dirent* entry = readdir(dir); entry != nullptr; entry = readdir(dir))
		{
			if (entry->d_type == DT_DIR || *entry->d_name == '.')
			{
				continue;
			}
			
			std::string filename = entry->d_name;
			std::string::size_type delimeter = filename.find_last_of('.');
			if (delimeter == std::string::npos)
			{
				continue;
			}
			
			std::string extension = filename.substr(delimeter + 1);
			if (extension != "txt")
			{
				continue;
			}
			
			// Add language
			std::string language = filename.substr(0, delimeter);
			languages.push_back(language);
			
			if (language == requestedLanguage)
			{
				languageIndex = languages.size() - 1;
			}
		}
		
		// Close biomes directory
		closedir(dir);
	}
	
	// Load strings
	std::string stringsFile = appDataPath + "strings/" + languages[languageIndex] + ".txt";
	std::cout << "Loading strings from \"" << stringsFile << "\"... ";
	if (!strings.load(stringsFile))
	{
		std::cout << "failed" << std::endl;
	}
	else
	{
		std::cout << "success" << std::endl;
	}
	
	// Get window title string
	std::string title;
	strings.get("title", &title);
	
	// Create window
	std::cout << "Creating a " << resolution.x << "x" << resolution.y;
	std::cout << ((fullscreen) ? " fullscreen" : " windowed");
	std::cout << " window... ";
	window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, static_cast<int>(resolution.x), static_cast<int>(resolution.y), windowFlags);
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
	bindingControl = nullptr;
	
	// Allocate states
	loadingState = new LoadingState(this);
	splashState = new SplashState(this);
	titleState = new TitleState(this);
	gameState = new GameState(this);
	
	// Setup loaders
	textureLoader = new TextureLoader();
	materialLoader = new MaterialLoader();
	modelLoader = new ModelLoader();
	modelLoader->setMaterialLoader(materialLoader);
	
	// Allocate game variables
	surfaceCam = new SurfaceCameraController();
	
	// Enter loading state
	state = nextState = loadingState;
	state->enter();
	
	displayDebugInfo = false;
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
		accumulator += std::min<float>(frameTime / 1000.0f, maxFrameTime);
		
		// If the user tried to close the application
		if (inputManager->wasClosed())
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
				
				// Update controls
				menuControlProfile->update();
				gameControlProfile->update();
				
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
			else
			{
				break;
			}
		}
		
		// Bind controls
		if (bindingControl != nullptr)
		{
			InputEvent event;
			inputManager->listen(&event);
			
			if (event.type != InputEvent::Type::NONE)
			{
				bindingControl->bind(event);
				bindingControl = nullptr;
				
				if (activeMenu != nullptr)
				{
					MenuItem* item = activeMenu->getSelectedItem();
					if (item != nullptr)
					{
						if (event.type == InputEvent::Type::KEY)
						{
							const char* keyName = SDL_GetKeyName(SDL_GetKeyFromScancode(static_cast<SDL_Scancode>(event.key.second)));
							std::stringstream stream;
							stream << keyName;
							std::string streamstring = stream.str();
							std::u32string label;
							label.assign(streamstring.begin(), streamstring.end());
							
							item->setValueName(item->getValueIndex(), label);
						}
					}
				}
			}
		}
		
		// Update input
		inputManager->update();
		
		// Check if fullscreen was toggled
		if (toggleFullscreen.isTriggered() && !toggleFullscreen.wasTriggered())
		{
			changeFullscreen();
		}
		
		// Check if debug display was toggled
		if (toggleDebugDisplay.isTriggered() && !toggleDebugDisplay.wasTriggered())
		{
			setDisplayDebugInfo(!displayDebugInfo);
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
				std::u32string label;
				std::stringstream stream;
				stream.precision(2);
				stream << std::fixed << meanFrameTime;
				std::string streamstring = stream.str();
				label.assign(streamstring.begin(), streamstring.end());
				frameTimeLabel->setText(label);
			}
		}
		
		// Update UI
		if (activeMenu != nullptr)
		{
			activeMenu->update(dt);
		}
		
		uiRootElement->update();
		uiBatcher->batch(uiBatch, uiRootElement);
		
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
		resolution = resolutions[fullscreenResolutionIndex];
		
		SDL_SetWindowSize(window, static_cast<int>(resolution.x), static_cast<int>(resolution.y));
		if (SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN) != 0)
		{
			std::cerr << "Failed to set fullscreen mode: \"" << SDL_GetError() << "\"" << std::endl;
			fullscreen = false;
		}
	}
	else
	{
		resolution = resolutions[windowedResolutionIndex];
			
		if (SDL_SetWindowFullscreen(window, 0) != 0)
		{
			std::cerr << "Failed to set windowed mode: \"" << SDL_GetError() << "\"" << std::endl;
			fullscreen = true;
		}
		else
		{
			SDL_SetWindowSize(window, static_cast<int>(resolution.x), static_cast<int>(resolution.y));
			SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
		}
	}
	
	// Print mode and resolution
	if (fullscreen)
	{
		std::cout << "Changed to fullscreen mode at resolution " << resolution.x << "x" << resolution.y << std::endl;
	}
	else
	{
		std::cout << "Changed to windowed mode at resolution " << resolution.x << "x" << resolution.y << std::endl;
	}
	
	// Save settings
	settings.set("fullscreen", fullscreen);
	saveUserSettings();
	
	// Resize UI
	resizeUI();
	
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
	forcepsModel = modelLoader->load("data/models/forceps.mdl");
	lensModel = modelLoader->load("data/models/lens.mdl");
	brushModel = modelLoader->load("data/models/brush.mdl");
	biomeFloorModel = modelLoader->load("data/models/desert-floor.mdl");
	
	if (!antModel || !antHillModel || !nestModel || !forcepsModel || !lensModel || !brushModel)
	{
		return false;
	}
	
	antModelInstance.setModel(antModel);
	antModelInstance.setTransform(Transform::getIdentity());	
	antHillModelInstance.setModel(antHillModel);
	antHillModelInstance.setRotation(glm::angleAxis(glm::radians(90.0f), Vector3(1, 0, 0)));
	nestModelInstance.setModel(nestModel);
	biomeFloorModelInstance.setModel(biomeFloorModel);
	
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
	//bgCompositor.addPass(&vignettePass);
	bgCompositor.load(nullptr);
	bgCamera.setOrthographic(0, 1.0f, 1.0f, 0, -1.0f, 1.0f);
	bgCamera.lookAt(glm::vec3(0), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));
	bgCamera.setCompositor(&bgCompositor);
	bgCamera.setCompositeIndex(0);
	
	// Set shadow map resolution
	shadowMapResolution = 4096;
	
	// Generate shadow map framebuffer
	glGenFramebuffers(1, &shadowMapFramebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFramebuffer);

	// Generate shadow map depth texture
	glGenTextures(1, &shadowMapDepthTexture);
	glBindTexture(GL_TEXTURE_2D, shadowMapDepthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, shadowMapResolution, shadowMapResolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
	
	// Attach depth texture to framebuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMapDepthTexture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	
	// Unbind shadow map depth texture
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	// Setup shadow map render target
	shadowMapRenderTarget.width = shadowMapResolution;
	shadowMapRenderTarget.height = shadowMapResolution;
	shadowMapRenderTarget.framebuffer = shadowMapFramebuffer;
	
	// Setup shadow map render pass
	shadowMapPass.setRenderTarget(&shadowMapRenderTarget);
	shadowMapPass.setViewCamera(&camera);
	shadowMapPass.setLightCamera(&sunlightCamera);
	
	// Setup shadow map compositor
	shadowMapCompositor.addPass(&shadowMapPass);
	shadowMapCompositor.load(nullptr);
	
	// Post-processing framebuffers
	{
		// Generate color texture
		glGenTextures(1, &framebufferAColorTexture);
		glBindTexture(GL_TEXTURE_2D, framebufferAColorTexture);
		glTexImage2D(GL_TEXTURE_2D, 0,  GL_RGB, static_cast<GLsizei>(resolution.x), static_cast<GLsizei>(resolution.y), 0,  GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		
		// Generate depth texture
		glGenTextures(1, &framebufferADepthTexture);
		glBindTexture(GL_TEXTURE_2D, framebufferADepthTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, static_cast<GLsizei>(resolution.x), static_cast<GLsizei>(resolution.y), 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
		
		// Generate framebuffer
		glGenFramebuffers(1, &framebufferA);
		glBindFramebuffer(GL_FRAMEBUFFER, framebufferA);

		// Attach textures to framebuffer
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebufferAColorTexture, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, framebufferADepthTexture, 0);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		//glReadBuffer(GL_COLOR_ATTACHMENT0);
		
		// Unbind framebuffer and texture
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		
		// Setup render target
		framebufferARenderTarget.width = static_cast<int>(resolution.x);
		framebufferARenderTarget.height = static_cast<int>(resolution.y);
		framebufferARenderTarget.framebuffer = framebufferA;
	}
	
	{
		// Generate color texture
		glGenTextures(1, &framebufferBColorTexture);
		glBindTexture(GL_TEXTURE_2D, framebufferBColorTexture);
		glTexImage2D(GL_TEXTURE_2D, 0,  GL_RGB, static_cast<GLsizei>(resolution.x), static_cast<GLsizei>(resolution.y), 0,  GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		
		// Generate framebuffer
		glGenFramebuffers(1, &framebufferA);
		glBindFramebuffer(GL_FRAMEBUFFER, framebufferA);

		// Attach textures to framebuffer
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebufferBColorTexture, 0);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		//glReadBuffer(GL_COLOR_ATTACHMENT0);
		
		// Unbind framebuffer and texture
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		
		// Setup render target
		framebufferBRenderTarget.width = static_cast<int>(resolution.x);
		framebufferBRenderTarget.height = static_cast<int>(resolution.y);
		framebufferBRenderTarget.framebuffer = framebufferA;
	}
	
	// Setup skybox pass
	skyboxPass.setRenderTarget(&framebufferARenderTarget);
	
	// Setup clear depth pass
	clearDepthPass.setRenderTarget(&framebufferARenderTarget);
	clearDepthPass.setClear(false, true, false);
	clearDepthPass.setClearDepth(1.0f);
	
	// Setup soil pass
	soilPass.setRenderTarget(&framebufferARenderTarget);
	
	// Setup lighting pass
	lightingPass.setRenderTarget(&framebufferARenderTarget);
	lightingPass.setShadowMap(shadowMapDepthTexture);
	lightingPass.setShadowCamera(&sunlightCamera);
	lightingPass.setShadowMapPass(&shadowMapPass);
	
	// Setup blur passes
	horizontalBlurPass.setRenderTarget(&framebufferBRenderTarget);
	horizontalBlurPass.setTexture(framebufferAColorTexture);
	horizontalBlurPass.setDirection(Vector2(0.0f, 0.0f));
	verticalBlurPass.setRenderTarget(&framebufferARenderTarget);
	verticalBlurPass.setTexture(framebufferBColorTexture);
	verticalBlurPass.setDirection(Vector2(0.0f, 0.0f));
	horizontalBlurPass2.setRenderTarget(&framebufferBRenderTarget);
	horizontalBlurPass2.setTexture(framebufferAColorTexture);
	horizontalBlurPass2.setDirection(Vector2(0.0f, 0.0f));
	verticalBlurPass2.setRenderTarget(&defaultRenderTarget);
	verticalBlurPass2.setTexture(framebufferBColorTexture);
	verticalBlurPass2.setDirection(Vector2(0.0f, 0.0f));

	
	// Setup debug pass
	debugPass.setRenderTarget(&defaultRenderTarget);
	
	defaultCompositor.addPass(&clearDepthPass);
	defaultCompositor.addPass(&skyboxPass);
	defaultCompositor.addPass(&soilPass);
	defaultCompositor.addPass(&lightingPass);
	defaultCompositor.addPass(&horizontalBlurPass);
	defaultCompositor.addPass(&verticalBlurPass);
	defaultCompositor.addPass(&horizontalBlurPass2);
	defaultCompositor.addPass(&verticalBlurPass2);
	//defaultCompositor.addPass(&debugPass);
	defaultCompositor.load(nullptr);
	
	// Setup sunlight camera
	sunlightCamera.lookAt(Vector3(0.5f, 2.0f, 2.0f), Vector3(0, 0, 0), Vector3(0, 1, 0));
	sunlightCamera.setOrthographic(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
	sunlightCamera.setCompositor(&shadowMapCompositor);
	sunlightCamera.setCompositeIndex(0);
	sunlightCamera.setCullingMask(nullptr);
	defaultLayer->addObject(&sunlightCamera);
	
	// Setup camera
	camera.lookAt(Vector3(0.0f, 0.0f, 10.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f));
	camera.setCompositor(&defaultCompositor);
	camera.setCompositeIndex(1);
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
	if (!fontLoader->load("data/fonts/NotoSansCJKsc-Regular.otf", static_cast<int>(fontSizePX + 0.5f), {UnicodeRange::BASIC_LATIN}, menuFont))
	{
		std::cerr << "Failed to load menu font" << std::endl;
	}
	
	copyrightFont = new Font(256, 256);
	if (!fontLoader->load("data/fonts/Varela-Regular.ttf", static_cast<int>(fontSizePX * 0.8f + 0.5f), {UnicodeRange::BASIC_LATIN}, copyrightFont))
	{
		std::cerr << "Failed to load copyright font" << std::endl;
	}
	
	levelNameFont = new Font(512, 512);
	if (!fontLoader->load("data/fonts/Vollkorn-Regular.ttf", static_cast<int>(fontSizePX * 2.0f + 0.5f), {UnicodeRange::BASIC_LATIN}, levelNameFont))
	{
		std::cerr << "Failed to load level name font" << std::endl;
	}
	
	delete fontLoader;
	
	// Load UI textures
	textureLoader->setGamma(1.0f);
	textureLoader->setCubemap(false);
	textureLoader->setMipmapChain(false);
	textureLoader->setMaxAnisotropy(1.0f);
	textureLoader->setWrapS(false);
	textureLoader->setWrapT(false);
	
	splashTexture = textureLoader->load("data/textures/ui-splash.png");
	titleTexture = textureLoader->load("data/textures/ui-title.png");
	rectangularPaletteTexture = textureLoader->load("data/textures/rectangular-palette.png");
	foodIndicatorTexture = textureLoader->load("data/textures/food-indicator.png");
	toolBrushTexture = textureLoader->load("data/textures/tool-brush.png");
	toolLensTexture = textureLoader->load("data/textures/tool-lens.png");
	toolForcepsTexture = textureLoader->load("data/textures/tool-forceps.png");
	toolTrowelTexture = textureLoader->load("data/textures/tool-trowel.png");
	
	toolbarTopTexture = textureLoader->load("data/textures/toolbar-top.png");
	toolbarBottomTexture = textureLoader->load("data/textures/toolbar-bottom.png");
	toolbarMiddleTexture = textureLoader->load("data/textures/toolbar-middle.png");
	toolbarButtonRaisedTexture = textureLoader->load("data/textures/toolbar-button-raised.png");
	toolbarButtonDepressedTexture = textureLoader->load("data/textures/toolbar-button-depressed.png");
	
	arcNorthTexture = textureLoader->load("data/textures/pie-menu-arc-north.png");
	arcEastTexture = textureLoader->load("data/textures/pie-menu-arc-east.png");
	arcSouthTexture = textureLoader->load("data/textures/pie-menu-arc-south.png");
	arcWestTexture = textureLoader->load("data/textures/pie-menu-arc-west.png");
	
	mouseLeftTexture = textureLoader->load("data/textures/mouse-left.png");
	mouseRightTexture = textureLoader->load("data/textures/mouse-right.png");
	
	depthTexture = new Texture();
	depthTexture->setTextureID(shadowMapDepthTexture);
	depthTexture->setWidth(shadowMapResolution);
	depthTexture->setHeight(shadowMapResolution);
	
	// Set colors
	selectedColor = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	deselectedColor = Vector4(1.0f, 1.0f, 1.0f, 0.35f);
	
	// Create tweener
	tweener = new Tweener();
	
	// Setup root UI element
	uiRootElement = new UIContainer();
	uiRootElement->setDimensions(resolution);
	mouse->addMouseMotionObserver(uiRootElement);
	mouse->addMouseButtonObserver(uiRootElement);
	
	// Create blackout element (for screen transitions)
	blackoutImage = new UIImage();
	blackoutImage->setLayerOffset(ANTKEEPER_UI_LAYER_BLACKOUT);
	blackoutImage->setTintColor(Vector4(0.0f, 0.0f, 0.0f, 1.0f));
	blackoutImage->setVisible(false);
	uiRootElement->addChild(blackoutImage);
	
	// Create darken element (for darkening title screen)
	darkenImage = new UIImage();
	darkenImage->setLayerOffset(ANTKEEPER_UI_LAYER_DARKEN);
	darkenImage->setTintColor(Vector4(0.0f, 0.0f, 0.0f, 0.35f));
	darkenImage->setVisible(false);
	uiRootElement->addChild(darkenImage);
	
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
	
	// Create game title element
	titleImage = new UIImage();
	titleImage->setTexture(titleTexture);
	titleImage->setVisible(false);
	titleImage->setLayerOffset(ANTKEEPER_UI_LAYER_MENU);
	uiRootElement->addChild(titleImage);
	
	frameTimeLabel = new UILabel();
	frameTimeLabel->setLayerOffset(99);
	frameTimeLabel->setTintColor(Vector4(1.0f, 1.0f, 0.0f, 1.0f));
	frameTimeLabel->setVisible(false);
	uiRootElement->addChild(frameTimeLabel);
	
	//bool frameTimeLabelVisible = false;
	//settings.get("show_frame_time", &frameTimeLabelVisible);
	//frameTimeLabel->setVisible(frameTimeLabelVisible);
	
	// Create "Press any key" element
	anyKeyLabel = new UILabel();
	anyKeyLabel->setLayerOffset(ANTKEEPER_UI_LAYER_MENU);
	anyKeyLabel->setVisible(false);
	uiRootElement->addChild(anyKeyLabel);
	
	// Create copyright element
	copyrightLabel = new UILabel();
	copyrightLabel->setLayerOffset(ANTKEEPER_UI_LAYER_MENU);
	copyrightLabel->setVisible(false);
	copyrightLabel->setTintColor(Vector4(1.0f, 1.0f, 1.0f, 0.15f));
	uiRootElement->addChild(copyrightLabel);
	
	rectangularPaletteImage = new UIImage();
	rectangularPaletteImage->setTexture(rectangularPaletteTexture);
	rectangularPaletteImage->setVisible(false);
	rectangularPaletteImage->setActive(false);
	rectangularPaletteImage->setLayerOffset(ANTKEEPER_UI_LAYER_HUD);
	uiRootElement->addChild(rectangularPaletteImage);
	
	contextButtonImage0 = new UIImage();
	contextButtonImage0->setTexture(mouseLeftTexture);
	//uiRootElement->addChild(contextButtonImage0);
	
	foodIndicatorImage = new UIImage();
	foodIndicatorImage->setTexture(foodIndicatorTexture);
	//uiRootElement->addChild(foodIndicatorImage);
	
	depthTextureImage = new UIImage();
	depthTextureImage->setTexture(depthTexture);
	depthTextureImage->setVisible(false);
	uiRootElement->addChild(depthTextureImage);
	
	// Create level name label
	levelNameLabel = new UILabel();
	levelNameLabel->setVisible(false);
	levelNameLabel->setLayerOffset(ANTKEEPER_UI_LAYER_HUD);
	uiRootElement->addChild(levelNameLabel);
	
	// Create toolbar
	toolbar = new Toolbar();
	toolbar->setToolbarTopTexture(toolbarTopTexture);
	toolbar->setToolbarBottomTexture(toolbarBottomTexture);
	toolbar->setToolbarMiddleTexture(toolbarMiddleTexture);
	toolbar->setButtonRaisedTexture(toolbarButtonRaisedTexture);
	toolbar->setButtonDepressedTexture(toolbarButtonDepressedTexture);
	toolbar->addButton(toolBrushTexture, std::bind(&std::printf, "0\n"), std::bind(&std::printf, "0\n"));
	toolbar->addButton(toolLensTexture, std::bind(&std::printf, "1\n"), std::bind(&std::printf, "1\n"));
	toolbar->addButton(toolForcepsTexture, std::bind(&std::printf, "2\n"), std::bind(&std::printf, "2\n"));
	toolbar->addButton(toolTrowelTexture, std::bind(&std::printf, "3\n"), std::bind(&std::printf, "3\n"));
	toolbar->resize();
	//uiRootElement->addChild(toolbar->getContainer());
	toolbar->getContainer()->setVisible(false);
	toolbar->getContainer()->setActive(false);
	
	// Create pie menu
	pieMenu = new PieMenu(tweener);
	pieMenu->addOption(arcNorthTexture, toolLensTexture, std::bind(&Application::selectTool, this, lens), std::bind(&Application::deselectTool, this, lens));
	pieMenu->addOption(arcEastTexture, toolForcepsTexture, std::bind(&Application::selectTool, this, forceps), std::bind(&Application::deselectTool, this, forceps));
	pieMenu->addOption(arcSouthTexture, toolTrowelTexture, std::bind(&Application::selectTool, this, nullptr), std::bind(&Application::deselectTool, this, nullptr));
	pieMenu->addOption(arcWestTexture, toolBrushTexture, std::bind(&Application::selectTool, this, brush), std::bind(&Application::deselectTool, this, brush));
	uiRootElement->addChild(pieMenu->getContainer());
	pieMenu->resize();
	pieMenu->getContainer()->setVisible(false);
	pieMenu->getContainer()->setActive(true);
	
	// Setup screen fade in/fade out tween
	fadeInTween = new Tween<Vector4>(EaseFunction::IN_QUINT, 0.0f, 2.0f, Vector4(0.0f, 0.0f, 0.0f, 1.0f), Vector4(0.0f, 0.0f, 0.0f, -1.0f));
	fadeInTween->setUpdateCallback(std::bind(&UIElement::setTintColor, blackoutImage, std::placeholders::_1));
	tweener->addTween(fadeInTween);
	fadeOutTween = new Tween<Vector4>(EaseFunction::OUT_QUINT, 0.0f, 2.0f, Vector4(0.0f, 0.0f, 0.0f, 0.0f), Vector4(0.0f, 0.0f, 0.0f, 1.0f));
	fadeOutTween->setUpdateCallback(std::bind(&UIElement::setTintColor, blackoutImage, std::placeholders::_1));
	tweener->addTween(fadeOutTween);
	
	// Setup darken fade in/fade out tweens
	darkenFadeInTween = new Tween<Vector4>(EaseFunction::OUT_CUBIC, 0.0f, 0.15f, Vector4(0.0f, 0.0f, 0.0f, 0.0f), Vector4(0.0f, 0.0f, 0.0f, 0.4f));
	darkenFadeInTween->setStartCallback(std::bind(&UIElement::setVisible, darkenImage, true));
	darkenFadeInTween->setUpdateCallback(std::bind(&UIElement::setTintColor, darkenImage, std::placeholders::_1));
	tweener->addTween(darkenFadeInTween);
	darkenFadeOutTween = new Tween<Vector4>(EaseFunction::OUT_CUBIC, 0.0f, 0.15f, Vector4(0.0f, 0.0f, 0.0f, 0.4f), Vector4(0.0f, 0.0f, 0.0f, -0.4f));
	darkenFadeOutTween->setUpdateCallback(std::bind(&UIElement::setTintColor, darkenImage, std::placeholders::_1));
	darkenFadeOutTween->setEndCallback(std::bind(&UIElement::setVisible, darkenImage, false));
	tweener->addTween(darkenFadeOutTween);
	
	// Setup blur fade in/fade out tweens
	blurFadeInTween = new Tween<float>(EaseFunction::OUT_CUBIC, 0.0f, 0.15f, 0.0f, 1.0f);
	blurFadeInTween->setUpdateCallback
	(
		[this](float t)
		{
			float factor = blurFadeInTween->getTweenValue();
			horizontalBlurPass.setDirection(Vector2(1.0f, 0.0f) * t);
			horizontalBlurPass2.setDirection(Vector2(3.0f, 0.0f) * t);
			verticalBlurPass.setDirection(Vector2(0.0f, 1.0f) * t);
			verticalBlurPass2.setDirection(Vector2(0.0f, 3.0f) * t);
		}
	);
	tweener->addTween(blurFadeInTween);
	
	blurFadeOutTween = new Tween<float>(EaseFunction::OUT_CUBIC, 0.0f, 0.15f, 1.0f, -1.0f);
	blurFadeOutTween->setUpdateCallback
	(
		[this](float t)
		{
			float factor = blurFadeInTween->getTweenValue();
			horizontalBlurPass.setDirection(Vector2(1.0f, 0.0f) * t);
			horizontalBlurPass2.setDirection(Vector2(3.0f, 0.0f) * t);
			verticalBlurPass.setDirection(Vector2(0.0f, 1.0f) * t);
			verticalBlurPass2.setDirection(Vector2(0.0f, 3.0f) * t);
		}
	);
	tweener->addTween(blurFadeOutTween);
	
	// Setup splash screen tween
	splashFadeInTween = new Tween<Vector4>(EaseFunction::IN_CUBIC, 0.0f, 0.5f, Vector4(1.0f, 1.0f, 1.0f, 0.0f), Vector4(0.0f, 0.0f, 0.0f, 1.0f));
	splashFadeInTween->setUpdateCallback(std::bind(&UIElement::setTintColor, splashImage, std::placeholders::_1));
	tweener->addTween(splashFadeInTween);
	
	splashHangTween = new Tween<float>(EaseFunction::OUT_CUBIC, 0.0f, 1.0f, 0.0f, 1.0f);
	tweener->addTween(splashHangTween);
	
	splashFadeOutTween = new Tween<Vector4>(EaseFunction::OUT_CUBIC, 0.0f, 0.5f, Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector4(0.0f, 0.0f, 0.0f, -1.0f));
	splashFadeOutTween->setUpdateCallback(std::bind(&UIElement::setTintColor, splashImage, std::placeholders::_1));
	tweener->addTween(splashFadeOutTween);
	
	splashFadeInTween->setEndCallback(std::bind(&TweenBase::start, splashHangTween));
	splashHangTween->setEndCallback(std::bind(&TweenBase::start, splashFadeOutTween));
	splashFadeOutTween->setEndCallback(std::bind(&Application::changeState, this, titleState));
	
	// Setup game title tween
	titleFadeInTween = new Tween<Vector4>(EaseFunction::IN_CUBIC, 0.0f, 2.0f, Vector4(1.0f, 1.0f, 1.0f, 0.0f), Vector4(0.0f, 0.0f, 0.0f, 1.0f));
	titleFadeInTween->setUpdateCallback(std::bind(&UIElement::setTintColor, titleImage, std::placeholders::_1));
	tweener->addTween(titleFadeInTween);
	titleFadeOutTween = new Tween<Vector4>(EaseFunction::OUT_CUBIC, 0.0f, 0.25f, Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector4(0.0f, 0.0f, 0.0f, -1.0f));
	titleFadeOutTween->setUpdateCallback(std::bind(&UIElement::setTintColor, titleImage, std::placeholders::_1));
	tweener->addTween(titleFadeOutTween);
	
	// Setup "Press any key" tween
	anyKeyFadeInTween = new Tween<Vector4>(EaseFunction::LINEAR, 0.0f, 1.5f, Vector4(1.0f, 1.0f, 1.0f, 0.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	anyKeyFadeInTween->setUpdateCallback(std::bind(&UIElement::setTintColor, anyKeyLabel, std::placeholders::_1));
	tweener->addTween(anyKeyFadeInTween);
	anyKeyFadeOutTween = new Tween<Vector4>(EaseFunction::LINEAR, 0.0f, 1.5f, Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector4(1.0f, 1.0f, 1.0f, -1.0f));
	anyKeyFadeOutTween->setUpdateCallback(std::bind(&UIElement::setTintColor, anyKeyLabel, std::placeholders::_1));
	anyKeyFadeInTween->setEndCallback(std::bind(&TweenBase::start, anyKeyFadeOutTween));
	anyKeyFadeOutTween->setEndCallback(std::bind(&TweenBase::start, anyKeyFadeInTween));
	tweener->addTween(anyKeyFadeOutTween);
	
	float menuFadeInDuration = 0.5f;
	Vector4 menuFadeInStartColor = Vector4(1.0f, 1.0f, 1.0f, 0.0f);
	Vector4 menuFadeInDeltaColor = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
	float menuFadeOutDuration = 0.25f;
	Vector4 menuFadeOutStartColor = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	Vector4 menuFadeOutDeltaColor = Vector4(0.0f, 0.0f, 0.0f, -1.0f);
	
	// Setup main menu tween
	menuFadeInTween = new Tween<Vector4>(EaseFunction::OUT_QUINT, 0.0f, menuFadeInDuration, menuFadeInStartColor, menuFadeInDeltaColor);
	tweener->addTween(menuFadeInTween);
	menuActivateTween = new Tween<float>(EaseFunction::OUT_QUINT, 0.0f, 0.01f, 0.0f, 0.0f);
	tweener->addTween(menuActivateTween);
	menuFadeOutTween = new Tween<Vector4>(EaseFunction::OUT_QUINT, 0.0f, menuFadeOutDuration, menuFadeOutStartColor, menuFadeOutDeltaColor);
	tweener->addTween(menuFadeOutTween);
	
	// Camera translation tween
	cameraTranslationTween = new Tween<Vector3>(EaseFunction::OUT_CUBIC, 0.0f, 0.0f, Vector3(0.0f), Vector3(0.0f));
	tweener->addTween(cameraTranslationTween);
	
	// Tool tweens
	forcepsSwoopTween = new Tween<float>(EaseFunction::OUT_CUBIC, 0.0f, 1.0f, 0.0f, 0.5f);
	tweener->addTween(forcepsSwoopTween);
	
	// Build menu system
	activeMenu = nullptr;
	previousActiveMenu = nullptr;
	
	// Allocate menus
	mainMenu = new Menu();
	levelsMenu = new Menu();
	optionsMenu = new Menu();
	controlsMenu = new Menu();
	pauseMenu = new Menu();
	
	// Main menu
	{	
		mainMenu->getUIContainer()->setAnchor(Vector2(0.5f, 0.8f));
		mainMenu->getUIContainer()->setLayerOffset(ANTKEEPER_UI_LAYER_MENU);
		mainMenu->setLineSpacing(1.0f);
		mainMenu->getUIContainer()->setActive(false);
		mainMenu->getUIContainer()->setVisible(false);
		uiRootElement->addChild(mainMenu->getUIContainer());
		
		mainMenuContinueItem = mainMenu->addItem();
		mainMenuContinueItem->setActivatedCallback(std::bind(&Application::continueGame, this));
		
		mainMenuLevelsItem = mainMenu->addItem();
		mainMenuLevelsItem->setActivatedCallback(std::bind(&Application::openMenu, this, levelsMenu));
		
		mainMenuNewGameItem = mainMenu->addItem();
		mainMenuNewGameItem->setActivatedCallback(std::bind(&Application::newGame, this));
		
		mainMenuSandboxItem = mainMenu->addItem();
		mainMenuSandboxItem->setActivatedCallback(std::bind(&std::printf, "1\n"));
		
		mainMenuOptionsItem = mainMenu->addItem();
		mainMenuOptionsItem->setActivatedCallback
		(
			[this]()
			{
				optionsMenuBackItem->setActivatedCallback(std::bind(&Application::openMenu, this, mainMenu));
				openMenu(optionsMenu);
			}
		);
		
		mainMenuExitItem = mainMenu->addItem();
		mainMenuExitItem->setActivatedCallback(std::bind(&Application::close, this, EXIT_SUCCESS));
	}
		
	// Levels menu
	{
		levelsMenu->getUIContainer()->setAnchor(Vector2(0.5f, 0.8f));
		levelsMenu->getUIContainer()->setLayerOffset(ANTKEEPER_UI_LAYER_MENU);
		levelsMenu->setLineSpacing(1.0f);
		
		for (std::size_t world = 0; world < campaign.getWorldCount(); ++world)
		{
			for (std::size_t level = 0; level < campaign.getLevelCount(world); ++level)
			{
				MenuItem* levelItem = levelsMenu->addItem();
				levelItem->setActivatedCallback
				(
					[this, world, level]()
					{
						loadWorld(world);
						loadLevel(level);
						
						// Close levels menu
						closeMenu();
						
						// Begin title fade-out
						titleFadeOutTween->reset();
						titleFadeOutTween->start();
						
						// Begin fade-out
						fadeOutTween->setEndCallback(std::bind(&Application::changeState, this, gameState));
						fadeOutTween->reset();
						fadeOutTween->start();
					}
				);
			}
		}
		
		levelsMenuBackItem = levelsMenu->addItem();
		levelsMenuBackItem->setActivatedCallback
		(
			[this]()
			{
				openMenu(previousActiveMenu);
			}
		);
		
		levelsMenu->getUIContainer()->setActive(false);
		levelsMenu->getUIContainer()->setVisible(false);
		uiRootElement->addChild(levelsMenu->getUIContainer());
	}
	
	// Options menu
	{
		optionsMenu->getUIContainer()->setAnchor(Vector2(0.5f, 0.8f));
		optionsMenu->getUIContainer()->setLayerOffset(ANTKEEPER_UI_LAYER_MENU);
		optionsMenu->setLineSpacing(1.0f);
		optionsMenu->setColumnMargin(menuFont->getWidth(U"MM"));
		
		optionsMenuWindowedResolutionItem = optionsMenu->addItem();
		optionsMenuFullscreenResolutionItem = optionsMenu->addItem();
		for (const Vector2& resolution: resolutions)
		{
			optionsMenuWindowedResolutionItem->addValue();
			optionsMenuFullscreenResolutionItem->addValue();
		}
		optionsMenuWindowedResolutionItem->setValueIndex(windowedResolutionIndex);
		optionsMenuWindowedResolutionItem->setActivatedCallback(std::bind(&Application::incrementMenuItem, this));
		optionsMenuWindowedResolutionItem->setValueChangedCallback(std::bind(&Application::selectWindowedResolution, this, std::placeholders::_1));
		optionsMenuFullscreenResolutionItem->setValueIndex(fullscreenResolutionIndex);
		optionsMenuFullscreenResolutionItem->setActivatedCallback(std::bind(&Application::incrementMenuItem, this));
		optionsMenuFullscreenResolutionItem->setValueChangedCallback(std::bind(&Application::selectFullscreenResolution, this, std::placeholders::_1));
		
		optionsMenuFullscreenItem = optionsMenu->addItem();
		optionsMenuFullscreenItem->addValue();
		optionsMenuFullscreenItem->addValue();
		optionsMenuFullscreenItem->setValueIndex((fullscreen == 0) ? 0 : 1);
		optionsMenuFullscreenItem->setActivatedCallback(std::bind(&Application::incrementMenuItem, this));
		optionsMenuFullscreenItem->setValueChangedCallback(std::bind(&Application::selectFullscreenMode, this, std::placeholders::_1));
		
		optionsMenuVSyncItem = optionsMenu->addItem();
		optionsMenuVSyncItem->addValue();
		optionsMenuVSyncItem->addValue();
		optionsMenuVSyncItem->setValueIndex((swapInterval == 0) ? 0 : 1);
		optionsMenuVSyncItem->setActivatedCallback(std::bind(&Application::incrementMenuItem, this));
		optionsMenuVSyncItem->setValueChangedCallback(std::bind(&Application::selectVSyncMode, this, std::placeholders::_1));
		
		optionsMenuLanguageItem = optionsMenu->addItem();
		for (std::size_t i = 0; i < languages.size(); ++i)
		{
			optionsMenuLanguageItem->addValue();
		}
		optionsMenuLanguageItem->setValueIndex(languageIndex);
		optionsMenuLanguageItem->setActivatedCallback(std::bind(&Application::incrementMenuItem, this));
		optionsMenuLanguageItem->setValueChangedCallback(std::bind(&Application::selectLanguage, this, std::placeholders::_1));
		
		optionsMenuControlsItem = optionsMenu->addItem();
		optionsMenuControlsItem->setActivatedCallback
		(
			[this]()
			{
				controlsMenuBackItem->setActivatedCallback(std::bind(&Application::openMenu, this, optionsMenu));
				openMenu(controlsMenu);
			}
		);
		
		optionsMenuBackItem = optionsMenu->addItem();
		optionsMenuBackItem->setActivatedCallback
		(
			[this]()
			{
				openMenu(previousActiveMenu);
			}
		);
		
		optionsMenu->getUIContainer()->setActive(false);
		optionsMenu->getUIContainer()->setVisible(false);
		uiRootElement->addChild(optionsMenu->getUIContainer());
	}
	
	// Controls menu
	{
		controlsMenu->getUIContainer()->setAnchor(Vector2(0.5f, 0.8f));
		controlsMenu->getUIContainer()->setLayerOffset(ANTKEEPER_UI_LAYER_MENU);
		controlsMenu->setLineSpacing(1.0f);
		controlsMenu->setColumnMargin(menuFont->getWidth(U"MM"));
		controlsMenu->getUIContainer()->setActive(false);
		controlsMenu->getUIContainer()->setVisible(false);
		uiRootElement->addChild(controlsMenu->getUIContainer());
		
		controlsMenuResetToDefaultItem = controlsMenu->addItem();
		
		controlsMenuMoveForwardItem = controlsMenu->addItem();
		controlsMenuMoveForwardItem->addValue();
		controlsMenuMoveForwardItem->setActivatedCallback(std::bind(&Application::bindControl, this, &cameraMoveForward));
		
		controlsMenuMoveBackItem = controlsMenu->addItem();
		controlsMenuMoveBackItem->addValue();
		controlsMenuMoveBackItem->setActivatedCallback(std::bind(&Application::bindControl, this, &cameraMoveBack));
		
		controlsMenuMoveLeftItem = controlsMenu->addItem();
		controlsMenuMoveLeftItem->addValue();
		controlsMenuMoveLeftItem->setActivatedCallback(std::bind(&Application::bindControl, this, &cameraMoveLeft));
		
		controlsMenuMoveRightItem = controlsMenu->addItem();
		controlsMenuMoveRightItem->addValue();
		controlsMenuMoveRightItem->setActivatedCallback(std::bind(&Application::bindControl, this, &cameraMoveRight));
		
		controlsMenuBackItem = controlsMenu->addItem();
		controlsMenuBackItem->setActivatedCallback
		(
			[this]()
			{
				openMenu(optionsMenu);
			}
		);
	}
	
	// Pause menu
	{
		pauseMenu->getUIContainer()->setAnchor(Vector2(0.5f, 0.5f));
		pauseMenu->getUIContainer()->setLayerOffset(ANTKEEPER_UI_LAYER_MENU);
		pauseMenu->setLineSpacing(1.0f);
		
		pauseMenuResumeItem = pauseMenu->addItem();
		pauseMenuResumeItem->setActivatedCallback(std::bind(&Application::unpauseSimulation, this));
		
		pauseMenuLevelsItem = pauseMenu->addItem();
		pauseMenuLevelsItem->setActivatedCallback(std::bind(&Application::openMenu, this, levelsMenu));
		
		pauseMenuOptionsItem = pauseMenu->addItem();
		pauseMenuOptionsItem->setActivatedCallback
		(
			[this]()
			{
				optionsMenuBackItem->setActivatedCallback(std::bind(&Application::openMenu, this, pauseMenu));
				openMenu(optionsMenu);
			}
		);
		
		pauseMenuMainMenuItem = pauseMenu->addItem();
		pauseMenuMainMenuItem->setActivatedCallback
		(
			[this]()
			{
				// Close pause menu
				closeMenu();
				
				// Begin fade-out to title state
				fadeOutTween->setEndCallback(std::bind(&Application::changeState, this, titleState));
				fadeOutTween->reset();
				fadeOutTween->start();
			}
		);
		
		pauseMenuExitItem = pauseMenu->addItem();
		pauseMenuExitItem->setActivatedCallback(std::bind(&Application::close, this, EXIT_SUCCESS));
		
		pauseMenu->getUIContainer()->setActive(false);
		pauseMenu->getUIContainer()->setVisible(false);
		uiRootElement->addChild(pauseMenu->getUIContainer());
	}
	
	// Set UI strings
	restringUI();
	resizeUI();
	
	// Setup UI batch
	uiBatch = new BillboardBatch();
	uiBatch->resize(512);
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
	
	defaultRenderTarget.width = static_cast<int>(resolution.x);
	defaultRenderTarget.height = static_cast<int>(resolution.y);
	defaultRenderTarget.framebuffer = 0;
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
	menuControlProfile->registerControl("toggle_debug_display", &toggleDebugDisplay);
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
	toggleDebugDisplay.bindKey(keyboard, SDL_SCANCODE_GRAVE);
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
	currentWorldIndex = 0;
	currentLevelIndex = 0;
	simulationPaused = false;
	
	// Allocate level
	currentLevel = new Level();
	
	// Create colony
	colony = new Colony();
	colony->setAntModel(antModel);
	
	currentTool = nullptr;
	
	// Create tools
	forceps = new Forceps(forcepsModel);
	forceps->setColony(colony);
	forceps->setCameraController(surfaceCam);
	
	lens = new Lens(lensModel);
	lens->setCameraController(surfaceCam);
	lens->setSunDirection(glm::normalize(-sunlightCamera.getTranslation()));
	
	brush = new Brush(brushModel);
	brush->setCameraController(surfaceCam);
	
	loadWorld(0);
	loadLevel(0);
	
	return true;
}

void Application::resizeUI()
{
	// Adjust render target dimensions
	defaultRenderTarget.width = static_cast<int>(resolution.x);
	defaultRenderTarget.height = static_cast<int>(resolution.y);
	
	// Adjust UI dimensions
	uiRootElement->setDimensions(resolution);
	uiRootElement->update();
	
	// Adjust title
	titleImage->setAnchor(Vector2(0.5f, 0.0f));
	titleImage->setDimensions(Vector2(titleTexture->getWidth(), titleTexture->getHeight()));
	titleImage->setTranslation(Vector2(0.0f, (int)(resolution.y * (1.0f / 4.0f) - titleTexture->getHeight() * 0.5f)));
	blackoutImage->setDimensions(resolution);
	darkenImage->setDimensions(resolution);
	splashBackgroundImage->setDimensions(resolution);
	splashImage->setAnchor(Anchor::CENTER);
	splashImage->setDimensions(Vector2(splashTexture->getWidth(), splashTexture->getHeight()));
	frameTimeLabel->setAnchor(Vector2(0.0f, 0.0f));
	frameTimeLabel->setTranslation(Vector2(0.0f));
	anyKeyLabel->setAnchor(Vector2(0.5f, 1.0f));
	anyKeyLabel->setTranslation(Vector2(0.0f, (int)(-resolution.y * (1.0f / 4.0f) - menuFont->getMetrics().getHeight() * 0.5f)));
	
	copyrightLabel->setAnchor(Vector2(0.0f, 1.0f));
	copyrightLabel->setTranslation(Vector2(resolution.x, -resolution.y) * 0.02f);
	
	rectangularPaletteImage->setAnchor(Vector2(0.0f, 1.0f));
	rectangularPaletteImage->setDimensions(Vector2(rectangularPaletteTexture->getWidth(), rectangularPaletteTexture->getHeight()));
	rectangularPaletteImage->setTranslation(Vector2(16.0f, -16.0f));
	contextButtonImage0->setAnchor(Vector2(0.5f, 1.0f));
	contextButtonImage0->setDimensions(Vector2(mouseLeftTexture->getWidth(), mouseLeftTexture->getHeight()));
	contextButtonImage0->setTranslation(Vector2(0.0f, -16.0f));
	foodIndicatorImage->setAnchor(Vector2(1.0f, 0.0f));
	foodIndicatorImage->setDimensions(Vector2(foodIndicatorTexture->getWidth(), foodIndicatorTexture->getHeight()));
	foodIndicatorImage->setTranslation(Vector2(-16.0f, 16.0f));
	depthTextureImage->setAnchor(Vector2(0.0f, 1.0f));
	depthTextureImage->setDimensions(Vector2(256, 256));
	depthTextureImage->setTranslation(Vector2(0.0f, 0.0f));
	levelNameLabel->setAnchor(Vector2(0.5f, 0.5f));
	
	// Adjust UI camera projection
	uiCamera.setOrthographic(0.0f, resolution.x, resolution.y, 0.0f, -1.0f, 1.0f);
}

void Application::restringUI()
{
	// Build map of UTF-8 string names to UTF-32 string values
	std::map<std::string, std::u32string> stringMap;
	const std::map<std::string, std::string>* stringParameters = strings.getParameters();
	for (auto it = stringParameters->begin(); it != stringParameters->end(); ++it)
	{
		std::u32string u32value;
		strings.get(it->first, &stringMap[it->first]);
	}
	
	// Build set of Unicode characters which encompass all strings
	std::set<char32_t> unicodeSet;
	for (auto it = stringMap.begin(); it != stringMap.end(); ++it)
	{
		for (char32_t charcode: it->second)
		{
			unicodeSet.insert(charcode);
		}
	}
	
	// Insert basic latin Unicode block
	for (char32_t charcode = UnicodeRange::BASIC_LATIN.start; charcode <= UnicodeRange::BASIC_LATIN.end; ++charcode)
	{
		unicodeSet.insert(charcode);
	}
	
	// Transform character set into character ranges
	std::vector<UnicodeRange> unicodeRanges;
	for (auto it = unicodeSet.begin(); it != unicodeSet.end(); ++it)
	{
		char32_t charcode = *it;
		unicodeRanges.push_back(UnicodeRange(charcode));
	}
	
	// Delete previously loaded fonts
	delete menuFont;
	delete copyrightFont;
	delete levelNameFont;
	
	// Determine fonts for current language
	std::string menuFontBasename;
	std::string copyrightFontBasename;
	std::string levelNameFontBasename;
	strings.get("menu-font", &menuFontBasename);
	strings.get("copyright-font", &copyrightFontBasename);
	strings.get("level-name-font", &levelNameFontBasename);
	std::string fontsDirectory = appDataPath + "fonts/";
	
	// Load fonts with the custom Unicode ranges
	FontLoader* fontLoader = new FontLoader();
	
	menuFont = new Font(512, 512);
	if (!fontLoader->load(fontsDirectory + menuFontBasename, static_cast<int>(fontSizePX + 0.5f), unicodeRanges, menuFont))
	{
		std::cerr << "Failed to load menu font" << std::endl;
	}
	
	copyrightFont = new Font(256, 256);
	if (!fontLoader->load(fontsDirectory + copyrightFontBasename, static_cast<int>(fontSizePX * 0.8f + 0.5f), unicodeRanges, copyrightFont))
	{
		std::cerr << "Failed to load copyright font" << std::endl;
	}
	
	levelNameFont = new Font(512, 512);
	if (!fontLoader->load(fontsDirectory + levelNameFontBasename, static_cast<int>(fontSizePX * 2.0f + 0.5f), unicodeRanges, levelNameFont))
	{
		std::cerr << "Failed to load level name font" << std::endl;
	}
	
	delete fontLoader;
	
	// Set fonts
	levelNameLabel->setFont(levelNameFont);
	frameTimeLabel->setFont(copyrightFont);
	anyKeyLabel->setFont(menuFont);
	copyrightLabel->setFont(copyrightFont);
	mainMenu->setFont(menuFont);
	levelsMenu->setFont(menuFont);
	optionsMenu->setFont(menuFont);
	controlsMenu->setFont(menuFont);
	pauseMenu->setFont(menuFont);
	
	// Title screen
	anyKeyLabel->setText(stringMap["press-any-key"]);
	copyrightLabel->setText(stringMap["copyright"]);
	
	// Main menu
	mainMenuContinueItem->setName(stringMap["continue"]);
	mainMenuLevelsItem->setName(stringMap["levels"]);
	mainMenuNewGameItem->setName(stringMap["new-game"]);
	mainMenuSandboxItem->setName(stringMap["sandbox"]);
	mainMenuOptionsItem->setName(stringMap["options"]);
	mainMenuExitItem->setName(stringMap["exit"]);
	
	// Levels menu
	std::size_t levelItemIndex = 0;
	for (std::size_t world = 0; world < campaign.getWorldCount(); ++world)
	{
		for (std::size_t level = 0; level < campaign.getLevelCount(world); ++level)
		{
			// Look up level name
			std::u32string levelName = getLevelName(world, level);
			
			// Create label
			/*
			std::u32string label;
			std::stringstream stream;
			stream << (world + 1) << "-" << (level + 1) << ": ";
			label = std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>().from_bytes(stream.str()) + levelName;
			*/
			
			// Set item name
			MenuItem* levelItem = levelsMenu->getItem(levelItemIndex);
			levelItem->setName(levelName);
			
			++levelItemIndex;
		}
	}
	levelsMenuBackItem->setName(stringMap["back"]);
	
	// Options menu
	optionsMenuWindowedResolutionItem->setName(stringMap["windowed-resolution"]);
	optionsMenuFullscreenResolutionItem->setName(stringMap["fullscreen-resolution"]);
	std::size_t resolutionIndex = 0;
	for (std::size_t i = 0; i < resolutions.size(); ++i)
	{
		
		std::u32string label;
		std::stringstream stream;
		stream << resolutions[i].x << "x" << resolutions[i].y;
		std::string streamstring = stream.str();
		label.assign(streamstring.begin(), streamstring.end());
		
		optionsMenuWindowedResolutionItem->setValueName(i, label);
		optionsMenuFullscreenResolutionItem->setValueName(i, label);
	}
	optionsMenuFullscreenItem->setName(stringMap["fullscreen"]);
	optionsMenuFullscreenItem->setValueName(0, stringMap["off"]);
	optionsMenuFullscreenItem->setValueName(1, stringMap["on"]);
	optionsMenuVSyncItem->setName(stringMap["vertical-sync"]);
	optionsMenuVSyncItem->setValueName(0, stringMap["off"]);
	optionsMenuVSyncItem->setValueName(1, stringMap["on"]);
	
	optionsMenuLanguageItem->setName(stringMap["language"]);
	for (std::size_t i = 0; i < languages.size(); ++i)
	{
		optionsMenuLanguageItem->setValueName(i, stringMap[languages[i]]);
	}
	
	optionsMenuControlsItem->setName(stringMap["controls"]);
	optionsMenuBackItem->setName(stringMap["back"]);
	
	// Controls menu
	controlsMenuResetToDefaultItem->setName(stringMap["reset-to-default"]);
	controlsMenuMoveForwardItem->setName(stringMap["move-forward"]);
	controlsMenuMoveForwardItem->setValueName(0, U"W");
	controlsMenuMoveBackItem->setName(stringMap["move-back"]);
	controlsMenuMoveBackItem->setValueName(0, U"S");
	controlsMenuMoveLeftItem->setName(stringMap["move-left"]);
	controlsMenuMoveLeftItem->setValueName(0, U"A");
	controlsMenuMoveRightItem->setName(stringMap["move-right"]);
	controlsMenuMoveRightItem->setValueName(0, U"D");
	controlsMenuBackItem->setName(stringMap["back"]);
	
	// Pause menu
	pauseMenuResumeItem->setName(stringMap["resume"]);
	pauseMenuLevelsItem->setName(stringMap["levels"]);
	pauseMenuOptionsItem->setName(stringMap["options"]);
	pauseMenuMainMenuItem->setName(stringMap["main-menu"]);
	pauseMenuExitItem->setName(stringMap["exit"]);
}

void Application::openMenu(Menu* menu)
{
	if (activeMenu != nullptr)
	{
		closeMenu();
	}
	
	activeMenu = menu;
	activeMenu->select(0);
	activeMenu->getUIContainer()->setVisible(true);
	activeMenu->getUIContainer()->setActive(false);
	activeMenu->getUIContainer()->setTintColor(Vector4(1.0f, 1.0f, 1.0f, 0.0f));
	/*
	if (activeMenu->getSelectedItem() == nullptr)
	{
		activeMenu->select(0);
	}
	*/
	
	// Delay menu activation
	menuActivateTween->setEndCallback(std::bind(&UIElement::setActive, activeMenu->getUIContainer(), true));
	menuActivateTween->reset();
	menuActivateTween->start();
	
	// Begin menu fade-in
	menuFadeInTween->setUpdateCallback(std::bind(&UIElement::setTintColor, activeMenu->getUIContainer(), std::placeholders::_1));
	menuFadeInTween->reset();
	menuFadeInTween->start();
}

void Application::closeMenu()
{
	if (activeMenu != nullptr)
	{
		// Deactivate menu
		activeMenu->getUIContainer()->setActive(false);
		activeMenu->getUIContainer()->setVisible(false);
		
		// Begin menu fade-out
		/*
		menuFadeOutTween->setUpdateCallback(std::bind(&UIElement::setTintColor, activeMenu->getUIContainer(), std::placeholders::_1));
		menuFadeOutTween->setEndCallback(std::bind(&UIElement::setVisible, activeMenu->getUIContainer(), false));
		menuFadeOutTween->reset();
		menuFadeOutTween->start();
		*/
		
		previousActiveMenu = activeMenu;
		activeMenu = nullptr;
	}
}

void Application::selectMenuItem(std::size_t index)
{
	if (activeMenu != nullptr)
	{
		activeMenu->select(index);
	}
}

void Application::activateMenuItem()
{
	if (activeMenu != nullptr)
	{
		activeMenu->activate();
	}
}

void Application::incrementMenuItem()
{
	if (activeMenu != nullptr)
	{
		MenuItem* item = activeMenu->getSelectedItem();
		if (item != nullptr)
		{
			if (item->getValueCount() != 0)
			{
				item->setValueIndex((item->getValueIndex() + 1) % item->getValueCount());
			}
		}
	}
}

void Application::decrementMenuItem()
{
	if (activeMenu != nullptr)
	{
		MenuItem* item = activeMenu->getSelectedItem();
		if (item != nullptr)
		{
			if (item->getValueCount() != 0)
			{
				if (!item->getValueIndex())
				{
					item->setValueIndex(item->getValueCount() - 1);
				}
				else
				{
					item->setValueIndex(item->getValueIndex() - 1);
				}
			}
		}
	}
}

void Application::continueGame()
{
	closeMenu();
	
	int world = 0;
	int level = 0;
	
	settings.get("continue_world", &world);
	settings.get("continue_level", &level);
	
	if (world != currentWorldIndex)
	{
		loadWorld(world);
	}
	if (level != currentLevelIndex)
	{
		loadLevel(level);
	}

	// Begin title fade-out
	titleFadeOutTween->reset();
	titleFadeOutTween->start();
	
	// Begin fade-out
	fadeOutTween->setEndCallback(std::bind(&Application::changeState, this, gameState));
	fadeOutTween->reset();
	fadeOutTween->start();
}

void Application::newGame()
{
	// Close main menu
	closeMenu();
	
	// Begin title fade-out
	titleFadeOutTween->reset();
	titleFadeOutTween->start();
	
	if (currentWorldIndex != 0 || currentLevelIndex != 0)
	{
		// Select first level of the first world
		currentWorldIndex = 0;
		currentLevelIndex = 0;
		
		// Begin fade-out
		fadeOutTween->setEndCallback(std::bind(&Application::changeState, this, gameState));
		fadeOutTween->reset();
		fadeOutTween->start();
	}
	else
	{
		// Begin fade-out
		fadeOutTween->setEndCallback(std::bind(&Application::changeState, this, gameState));
		fadeOutTween->reset();
		fadeOutTween->start();
		
		// Change state
		//changeState(gameState);
	}
}

void Application::deselectTool(Tool* tool)
{
	if (tool != nullptr)
	{
		tool->setActive(false);
		return;
	}
}

void Application::selectTool(Tool* tool)
{
	if (tool != nullptr)
	{
		tool->setActive(true);
	}
	
	currentTool = tool;
}

void Application::loadWorld(std::size_t index)
{
	// Set current world
	currentWorldIndex = index;
	
	// Get world biome
	const LevelParameterSet* levelParams = campaign.getLevelParams(currentWorldIndex, 0);
	const Biome* biome = &biosphere.biomes[levelParams->biome];
	
	// Setup rendering passes
	soilPass.setHorizonOTexture(biome->soilHorizonO);
	soilPass.setHorizonATexture(biome->soilHorizonA);
	soilPass.setHorizonBTexture(biome->soilHorizonB);
	soilPass.setHorizonCTexture(biome->soilHorizonC);
	lightingPass.setDiffuseCubemap(biome->diffuseCubemap);
	lightingPass.setSpecularCubemap(biome->specularCubemap);
	skyboxPass.setCubemap(biome->specularCubemap);
}

void Application::loadLevel(std::size_t index)
{
	// Set current level
	currentLevelIndex = index;
	
	// Load level
	const LevelParameterSet* levelParams = campaign.getLevelParams(currentWorldIndex, currentLevelIndex);
	currentLevel->load(*levelParams);
	currentLevel->terrain.getSurfaceModel()->getGroup(0)->material = materialLoader->load("data/materials/debug-terrain-surface.mtl");
}

/*
void Application::loadLevel()
{
	if (currentLevel < 1 || currentLevel >= campaign.levels[currentWorld].size())
	{
		std::cout << "Attempted to load invalid level" << std::endl;
		return;
	}
	
	const LevelParameterSet* levelParams = campaign.getLevelParams(currentWorld, currentLevel);
	const Biome* biome = &biosphere.biomes[levelParams->biome];
	
	soilPass.setHorizonOTexture(biome->soilHorizonO);
	soilPass.setHorizonATexture(biome->soilHorizonA);
	soilPass.setHorizonBTexture(biome->soilHorizonB);
	soilPass.setHorizonCTexture(biome->soilHorizonC);
	
	std::string heightmap = std::string("data/textures/") + level->heightmap;
	currentLevelTerrain->load(heightmap);
	
	// Set skybox
	skyboxPass.setCubemap(biome->specularCubemap);
	
	//changeState(playState);
}
*/

void Application::pauseSimulation()
{
	simulationPaused = true;
	
	darkenFadeOutTween->stop();
	darkenFadeInTween->reset();
	darkenFadeInTween->start();
	
	blurFadeOutTween->stop();
	blurFadeInTween->reset();
	blurFadeInTween->start();
	
	openMenu(pauseMenu);
	pauseMenu->select(0);
}

void Application::unpauseSimulation()
{
	simulationPaused = false;
	
	darkenFadeInTween->stop();
	darkenFadeOutTween->reset();
	darkenFadeOutTween->start();
	
	blurFadeInTween->stop();
	blurFadeOutTween->reset();
	blurFadeOutTween->start();
	
	closeMenu();
}

void Application::setDisplayDebugInfo(bool display)
{
	displayDebugInfo = display;

	frameTimeLabel->setVisible(displayDebugInfo);
	depthTextureImage->setVisible(displayDebugInfo);
}

std::u32string Application::getLevelName(std::size_t world, std::size_t level) const
{
	// Form level ID string
	char levelIDBuffer[6];
	std::sprintf(levelIDBuffer, "%02d-%02d", static_cast<int>(world + 1), static_cast<int>(level + 1));
	std::string levelID(levelIDBuffer);
	
	// Look up level name
	std::u32string levelName;
	strings.get(levelIDBuffer, &levelName);
	
	return levelName;
}

void Application::selectWindowedResolution(std::size_t index)
{
	windowedResolutionIndex = index;
	
	if (!fullscreen)
	{
		// Select resolution
		resolution = resolutions[windowedResolutionIndex];
		
		// Resize window
		SDL_SetWindowSize(window, static_cast<int>(resolution.x), static_cast<int>(resolution.y));
		SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
		
		// Resize UI
		resizeUI();
		
		// Notify window observers
		inputManager->update();
	}
	
	// Save settings
	settings.set("windowed_width", resolutions[windowedResolutionIndex].x);
	settings.set("windowed_height", resolutions[windowedResolutionIndex].y);
	saveUserSettings();
}

void Application::selectFullscreenResolution(std::size_t index)
{
	fullscreenResolutionIndex = index;
	
	if (fullscreen)
	{
		// Select resolution
		resolution = resolutions[fullscreenResolutionIndex];
		
		// Resize window
		SDL_SetWindowSize(window, static_cast<int>(resolution.x), static_cast<int>(resolution.y));
		SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
		
		// Resize UI
		resizeUI();
		
		// Notify window observers
		inputManager->update();
	}
	
	// Save settings
	settings.set("fullscreen_width", resolutions[fullscreenResolutionIndex].x);
	settings.set("fullscreen_height", resolutions[fullscreenResolutionIndex].y);
	saveUserSettings();
}

void Application::selectFullscreenMode(std::size_t index)
{
	fullscreen = (index == 1);
	
	if (fullscreen)
	{
		resolution = resolutions[fullscreenResolutionIndex];
		
		SDL_SetWindowSize(window, static_cast<int>(resolution.x), static_cast<int>(resolution.y));
		if (SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN) != 0)
		{
			std::cerr << "Failed to set fullscreen mode: \"" << SDL_GetError() << "\"" << std::endl;
			fullscreen = false;
		}
	}
	else
	{
		resolution = resolutions[windowedResolutionIndex];
			
		if (SDL_SetWindowFullscreen(window, 0) != 0)
		{
			std::cerr << "Failed to set windowed mode: \"" << SDL_GetError() << "\"" << std::endl;
			fullscreen = true;
		}
		else
		{
			SDL_SetWindowSize(window, static_cast<int>(resolution.x), static_cast<int>(resolution.y));
			SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
		}
	}
	
	// Print mode and resolution
	if (fullscreen)
	{
		std::cout << "Changed to fullscreen mode at resolution " << resolution.x << "x" << resolution.y << std::endl;
	}
	else
	{
		std::cout << "Changed to windowed mode at resolution " << resolution.x << "x" << resolution.y << std::endl;
	}
	
	// Save settings
	settings.set("fullscreen", fullscreen);
	saveUserSettings();
	
	// Resize UI
	resizeUI();
	
	// Notify window observers
	inputManager->update();
}

// index: 0 = off, 1 = on
void Application::selectVSyncMode(std::size_t index)
{
	swapInterval = (index == 0) ? 0 : 1;
	
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

void Application::selectLanguage(std::size_t index)
{
	// Set language index
	languageIndex = index;
	
	// Clear strings
	strings.clear();
	
	// Load strings
	std::string stringsFile = appDataPath + "strings/" + languages[languageIndex] + ".txt";
	std::cout << "Loading strings from \"" << stringsFile << "\"... ";
	if (!strings.load(stringsFile))
	{
		std::cout << "failed" << std::endl;
	}
	else
	{
		std::cout << "success" << std::endl;
	}
	
	// Save settings
	settings.set("language", languages[languageIndex]);
	saveUserSettings();
	
	// Change window title
	std::string title;
	strings.get("title", &title);
	SDL_SetWindowTitle(window, title.c_str());
	
	// Restring UI
	restringUI();
}

void Application::bindControl(Control* control)
{
	bindingControl = control;
	bindingControl->unbind();
}
