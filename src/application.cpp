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
	
	/*
	// Check (usable?) display bounds
	SDL_Rect displayBounds;
	if (SDL_GetDisplayBounds(0, &displayBounds) != 0)
	{
		std::cerr << "Failed to get display bounds: \"" << SDL_GetError() << "\"" << std::endl;
		close(EXIT_FAILURE);
		return;
	}
	*/
	
	/*
	SDL_DisplayMode displayMode;
	if (SDL_GetCurrentDisplayMode(0, &displayMode) != 0)
	{
		std::cerr << "Failed to get display mode: \"" << SDL_GetError() << "\"" << std::endl;
		close(EXIT_FAILURE);
		return;
	}
	*/
	
	// Use display resolution if settings request
	if (windowedWidth == -1 || windowedHeight == -1)
	{
		windowedWidth = displayMode.w;
		windowedHeight = displayMode.h;
	}
	if (fullscreenWidth == -1 || fullscreenHeight == -1)
	{
		fullscreenWidth = displayMode.w;
		fullscreenHeight = displayMode.h;
	}
	
	// Determine window parameters
	Uint32 windowFlags = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI;
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
	std::cout << "Creating a " << width << "x" << height;
	std::cout << ((fullscreen) ? " fullscreen" : " windowed");
	std::cout << " window... ";
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
	
	// Setup skybox pass
	skyboxPass.setRenderTarget(&defaultRenderTarget);
	
	// Setup clear depth pass
	clearDepthPass.setRenderTarget(&defaultRenderTarget);
	clearDepthPass.setClear(false, true, false);
	clearDepthPass.setClearDepth(1.0f);
	
	// Setup soil pass
	soilPass.setRenderTarget(&defaultRenderTarget);
	
	// Setup lighting pass
	lightingPass.setRenderTarget(&defaultRenderTarget);
	lightingPass.setShadowMap(shadowMapDepthTexture);
	lightingPass.setShadowCamera(&sunlightCamera);
	lightingPass.setShadowMapPass(&shadowMapPass);
	
	// Setup debug pass
	debugPass.setRenderTarget(&defaultRenderTarget);
	
	defaultCompositor.addPass(&clearDepthPass);
	defaultCompositor.addPass(&skyboxPass);
	defaultCompositor.addPass(&soilPass);
	defaultCompositor.addPass(&lightingPass);
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
	if (!fontLoader->load("data/fonts/Varela-Regular.ttf", static_cast<int>(fontSizePX + 0.5f), menuFont))
	{
		std::cerr << "Failed to load menu font" << std::endl;
	}
	
	copyrightFont = new Font(256, 256);
	if (!fontLoader->load("data/fonts/Varela-Regular.ttf", static_cast<int>(fontSizePX * 0.8f + 0.5f), copyrightFont))
	{
		std::cerr << "Failed to load copyright font" << std::endl;
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
	
	// Get strings
	std::string pressAnyKeyString;
	std::string backString;
	std::string continueString;
	std::string newGameString;
	std::string levelsString;
	std::string sandboxString;
	std::string optionsString;
	std::string exitString;
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
	strings.get("back", &backString);
	strings.get("continue", &continueString);
	strings.get("new-game", &newGameString);
	strings.get("levels", &levelsString);
	strings.get("sandbox", &sandboxString);
	strings.get("options", &optionsString);
	strings.get("exit", &exitString);
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
	
	// Create tweener
	tweener = new Tweener();
	
	// Setup root UI element
	uiRootElement = new UIContainer();
	uiRootElement->setDimensions(Vector2(width, height));
	mouse->addMouseMotionObserver(uiRootElement);
	mouse->addMouseButtonObserver(uiRootElement);
	
	// Create blackout element (for screen transitions)
	blackoutImage = new UIImage();
	blackoutImage->setDimensions(Vector2(width, height));
	blackoutImage->setLayerOffset(ANTKEEPER_UI_LAYER_BLACKOUT);
	blackoutImage->setTintColor(Vector4(0.0f, 0.0f, 0.0f, 1.0f));
	blackoutImage->setVisible(false);
	uiRootElement->addChild(blackoutImage);
	
	// Create darken element (for darkening title screen)
	darkenImage = new UIImage();
	darkenImage->setDimensions(Vector2(width, height));
	darkenImage->setLayerOffset(ANTKEEPER_UI_LAYER_DARKEN);
	darkenImage->setTintColor(Vector4(0.0f, 0.0f, 0.0f, 0.35f));
	darkenImage->setVisible(false);
	uiRootElement->addChild(darkenImage);
	
	// Create splash screen background element
	splashBackgroundImage = new UIImage();
	splashBackgroundImage->setDimensions(Vector2(width, height));
	splashBackgroundImage->setLayerOffset(-1);
	splashBackgroundImage->setTintColor(Vector4(0.0f, 0.0f, 0.0f, 1.0f));
	splashBackgroundImage->setVisible(false);
	uiRootElement->addChild(splashBackgroundImage);
	
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
	titleImage->setLayerOffset(ANTKEEPER_UI_LAYER_MENU);
	uiRootElement->addChild(titleImage);
	
	frameTimeLabel = new UILabel();
	frameTimeLabel->setAnchor(Vector2(0.0f, 0.0f));
	frameTimeLabel->setLayerOffset(99);
	frameTimeLabel->setFont(copyrightFont);
	frameTimeLabel->setTranslation(Vector2(0.0f));
	frameTimeLabel->setTintColor(Vector4(1.0f, 1.0f, 0.0f, 1.0f));
	frameTimeLabel->setText("");
	frameTimeLabel->setVisible(false);
	uiRootElement->addChild(frameTimeLabel);
	
	//bool frameTimeLabelVisible = false;
	//settings.get("show_frame_time", &frameTimeLabelVisible);
	//frameTimeLabel->setVisible(frameTimeLabelVisible);
	
	// Create "Press any key" element
	anyKeyLabel = new UILabel();
	anyKeyLabel->setAnchor(Vector2(0.5f, 1.0f));
	anyKeyLabel->setFont(menuFont);
	anyKeyLabel->setTranslation(Vector2(0.0f, (int)(-height * (1.0f / 4.0f) - menuFont->getMetrics().getHeight() * 0.5f)));
	anyKeyLabel->setText(pressAnyKeyString);
	anyKeyLabel->setVisible(false);
	uiRootElement->addChild(anyKeyLabel);
	
	rectangularPaletteImage = new UIImage();
	rectangularPaletteImage->setAnchor(Vector2(0.0f, 1.0f));
	rectangularPaletteImage->setDimensions(Vector2(rectangularPaletteTexture->getWidth(), rectangularPaletteTexture->getHeight()));
	rectangularPaletteImage->setTranslation(Vector2(16.0f, -16.0f));
	rectangularPaletteImage->setTexture(rectangularPaletteTexture);
	rectangularPaletteImage->setVisible(false);
	rectangularPaletteImage->setActive(false);
	rectangularPaletteImage->setLayerOffset(ANTKEEPER_UI_LAYER_HUD);
	uiRootElement->addChild(rectangularPaletteImage);
	
	contextButtonImage0 = new UIImage();
	contextButtonImage0->setAnchor(Vector2(0.5f, 1.0f));
	contextButtonImage0->setDimensions(Vector2(mouseLeftTexture->getWidth(), mouseLeftTexture->getHeight()));
	contextButtonImage0->setTranslation(Vector2(0.0f, -16.0f));
	contextButtonImage0->setTexture(mouseLeftTexture);
	//uiRootElement->addChild(contextButtonImage0);
	
	foodIndicatorImage = new UIImage();
	foodIndicatorImage->setAnchor(Vector2(1.0f, 0.0f));
	foodIndicatorImage->setDimensions(Vector2(foodIndicatorTexture->getWidth(), foodIndicatorTexture->getHeight()));
	foodIndicatorImage->setTranslation(Vector2(-16.0f, 16.0f));
	foodIndicatorImage->setTexture(foodIndicatorTexture);
	//uiRootElement->addChild(foodIndicatorImage);
	
	depthTextureImage = new UIImage();
	depthTextureImage->setAnchor(Vector2(0.0f, 1.0f));
	depthTextureImage->setDimensions(Vector2(256, 256));
	depthTextureImage->setTranslation(Vector2(0.0f, 0.0f));
	depthTextureImage->setTexture(depthTexture);
	depthTextureImage->setVisible(false);
	uiRootElement->addChild(depthTextureImage);
	
	// Create level ID
	levelIDLabel = new UILabel();
	levelIDLabel->setAnchor(Vector2(0.5f, 0.0f));
	levelIDLabel->setFont(menuFont);
	levelIDLabel->setTranslation(Vector2(0.0f, (int)(height * (1.0f / 4.0f) - menuFont->getMetrics().getHeight() * 1.5f)));
	levelIDLabel->setText("");
	levelIDLabel->setVisible(false);
	uiRootElement->addChild(levelIDLabel);
	
	// Create level name label
	levelNameLabel = new UILabel();
	levelNameLabel->setAnchor(Vector2(0.5f, 0.0f));
	levelNameLabel->setFont(menuFont);
	levelNameLabel->setTranslation(Vector2(0.0f, (int)(height * (1.0f / 4.0f) - menuFont->getMetrics().getHeight() * 0.5f)));
	levelNameLabel->setText("");
	levelNameLabel->setVisible(false);
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
	uiRootElement->addChild(toolbar->getContainer());
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
	fadeInTween = new Tween<Vector4>(EaseFunction::IN_CUBIC, 0.0f, 2.0f, Vector4(0.0f, 0.0f, 0.0f, 1.0f), Vector4(0.0f, 0.0f, 0.0f, -1.0f));
	fadeInTween->setUpdateCallback(std::bind(&UIElement::setTintColor, blackoutImage, std::placeholders::_1));
	tweener->addTween(fadeInTween);
	fadeOutTween = new Tween<Vector4>(EaseFunction::OUT_CUBIC, 0.0f, 2.0f, Vector4(0.0f, 0.0f, 0.0f, 0.0f), Vector4(0.0f, 0.0f, 0.0f, 1.0f));
	fadeOutTween->setUpdateCallback(std::bind(&UIElement::setTintColor, blackoutImage, std::placeholders::_1));
	tweener->addTween(fadeOutTween);
	
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
	
	float menuFadeInDuration = 0.15f;
	Vector4 menuFadeInStartColor = Vector4(1.0f, 1.0f, 1.0f, 0.0f);
	Vector4 menuFadeInDeltaColor = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
	float menuFadeOutDuration = 0.15f;
	Vector4 menuFadeOutStartColor = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	Vector4 menuFadeOutDeltaColor = Vector4(0.0f, 0.0f, 0.0f, -1.0f);
	float menuSlideInDuration = 0.35f;
	Vector2 menuSlideInStartTranslation = Vector2(-64.0f, 0.0f);
	Vector2 menuSlideInDeltaTranslation = Vector2((int)(64.0f + width / 8.0f), 0.0f);
	
	// Setup main menu tween
	menuFadeInTween = new Tween<Vector4>(EaseFunction::OUT_QUINT, 0.0f, menuFadeInDuration, menuFadeInStartColor, menuFadeInDeltaColor);
	tweener->addTween(menuFadeInTween);
	menuFadeOutTween = new Tween<Vector4>(EaseFunction::OUT_QUINT, 0.0f, menuFadeOutDuration, menuFadeOutStartColor, menuFadeOutDeltaColor);
	tweener->addTween(menuFadeOutTween);
	menuSlideInTween = new Tween<Vector2>(EaseFunction::OUT_QUINT, 0.0f, menuSlideInDuration, menuSlideInStartTranslation, menuSlideInDeltaTranslation);
	tweener->addTween(menuSlideInTween);
	
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
	pauseMenu = new Menu();
	
	// Main menu
	{	
		mainMenu->setFont(menuFont);
		mainMenu->getUIContainer()->setAnchor(Vector2(0.5f, 0.8f));
		mainMenu->getUIContainer()->setLayerOffset(ANTKEEPER_UI_LAYER_MENU);
		mainMenu->setLineSpacing(1.0f);
		
		MenuItem* continueItem = mainMenu->addItem();
		continueItem->setActivatedCallback(std::bind(&Application::continueGame, this));
		continueItem->setLabel(continueString);
		
		MenuItem* newGameItem = mainMenu->addItem();
		newGameItem->setActivatedCallback(std::bind(&Application::newGame, this));
		newGameItem->setLabel(newGameString);
		
		MenuItem* levelsItem = mainMenu->addItem();
		levelsItem->setActivatedCallback(std::bind(&Application::openMenu, this, levelsMenu));
		levelsItem->setLabel(levelsString);
		
		MenuItem* sandboxItem = mainMenu->addItem();
		sandboxItem->setActivatedCallback(std::bind(&std::printf, "1\n"));
		sandboxItem->setLabel(sandboxString);
		
		MenuItem* optionsItem = mainMenu->addItem();
		optionsItem->setActivatedCallback(std::bind(&Application::openMenu, this, optionsMenu));
		optionsItem->setLabel(optionsString);
		
		MenuItem* exitItem = mainMenu->addItem();
		exitItem->setActivatedCallback(std::bind(&Application::close, this, EXIT_SUCCESS));
		exitItem->setLabel(exitString);
		
		mainMenu->getUIContainer()->setActive(false);
		mainMenu->getUIContainer()->setVisible(false);
		uiRootElement->addChild(mainMenu->getUIContainer());
	}
	
	// Levels menu
	{
		levelsMenu->setFont(menuFont);
		levelsMenu->getUIContainer()->setAnchor(Vector2(0.5f, 0.8f));
		levelsMenu->getUIContainer()->setLayerOffset(ANTKEEPER_UI_LAYER_MENU);
		levelsMenu->setLineSpacing(1.0f);
		
		for (std::size_t world = 0; world < campaign.getWorldCount(); ++world)
		{
			for (std::size_t level = 0; level < campaign.getLevelCount(world); ++level)
			{
				// Form level ID string
				char levelIDBuffer[6];
				std::sprintf(levelIDBuffer, "%02d-%02d", static_cast<int>(world + 1), static_cast<int>(level + 1));
				std::string levelID(levelIDBuffer);
				
				// Look up level name
				std::string levelName;
				strings.get(levelIDBuffer, &levelName);
				
				// Create label
				std::string label = levelID + ": " + levelName;
				
				MenuItem* levelItem = levelsMenu->addItem();
				levelItem->setActivatedCallback
				(
					[this, world, level]()
					{
						closeMenu();
						loadWorld(world);
						loadLevel(level);
						changeState(gameState);
					}
				);
				levelItem->setLabel(label);
			}
		}
		
		MenuItem* backItem = levelsMenu->addItem();
		backItem->setActivatedCallback
		(
			[this]()
			{
				openMenu(previousActiveMenu);
			}
		);
		backItem->setLabel(backString);
		
		levelsMenu->getUIContainer()->setActive(false);
		levelsMenu->getUIContainer()->setVisible(false);
		uiRootElement->addChild(levelsMenu->getUIContainer());
	}
	
	// Options menu
	{
		optionsMenu->setFont(menuFont);
		optionsMenu->getUIContainer()->setAnchor(Vector2(0.5f, 0.8f));
		optionsMenu->getUIContainer()->setLayerOffset(ANTKEEPER_UI_LAYER_MENU);
		optionsMenu->setLineSpacing(1.0f);
		
		MenuItem* resolutionItem = optionsMenu->addItem();
		resolutionItem->setLabel("Resolution");
		
		MenuItem* fullscreenItem = optionsMenu->addItem();
		fullscreenItem->setLabel("Fullscreen");
		
		MenuItem* backItem = optionsMenu->addItem();
		backItem->setActivatedCallback
		(
			[this]()
			{
				openMenu(previousActiveMenu);
			}
		);
		backItem->setLabel(backString);
		
		optionsMenu->getUIContainer()->setActive(false);
		optionsMenu->getUIContainer()->setVisible(false);
		uiRootElement->addChild(optionsMenu->getUIContainer());
	}
	
	// Pause menu
	{
		pauseMenu->setFont(menuFont);
		pauseMenu->getUIContainer()->setAnchor(Vector2(0.5f, 0.5f));
		pauseMenu->getUIContainer()->setLayerOffset(ANTKEEPER_UI_LAYER_MENU);
		pauseMenu->setLineSpacing(1.0f);
		
		MenuItem* resumeItem = pauseMenu->addItem();
		resumeItem->setActivatedCallback(std::bind(&Application::unpauseSimulation, this));
		resumeItem->setLabel("Resume");
		
		MenuItem* levelsItem = pauseMenu->addItem();
		levelsItem->setActivatedCallback(std::bind(&Application::openMenu, this, levelsMenu));
		levelsItem->setLabel(levelsString);
		
		MenuItem* optionsItem = pauseMenu->addItem();
		optionsItem->setActivatedCallback(std::bind(&Application::openMenu, this, optionsMenu));
		optionsItem->setLabel(optionsString);
		
		MenuItem* mainMenuItem = pauseMenu->addItem();
		mainMenuItem->setActivatedCallback
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
		mainMenuItem->setLabel("Main Menu");
		
		MenuItem* exitItem = pauseMenu->addItem();
		exitItem->setActivatedCallback(std::bind(&Application::close, this, EXIT_SUCCESS));
		exitItem->setLabel(exitString);
		
		pauseMenu->getUIContainer()->setActive(false);
		pauseMenu->getUIContainer()->setVisible(false);
		uiRootElement->addChild(pauseMenu->getUIContainer());
	}
	
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
	
	defaultRenderTarget.width = width;
	defaultRenderTarget.height = height;
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
	
	brush = new Brush(brushModel);
	brush->setCameraController(surfaceCam);
	
	loadWorld(0);
	loadLevel(0);
	
	return true;
}

void Application::resizeUI()
{
	// Adjust UI dimensions
	uiRootElement->setDimensions(Vector2(width, height));
	uiRootElement->update();
	
	// Adjust UI camera projection
	uiCamera.setOrthographic(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, -1.0f, 1.0f);
}

void Application::openMenu(Menu* menu)
{
	if (activeMenu != nullptr)
	{
		closeMenu();
	}
	
	activeMenu = menu;
	activeMenu->getUIContainer()->setActive(true);
	activeMenu->getUIContainer()->setVisible(true);
	
	if (activeMenu->getSelectedItem() == nullptr)
	{
		activeMenu->select(0);
	}
}

void Application::closeMenu()
{
	if (activeMenu != nullptr)
	{
		activeMenu->getUIContainer()->setActive(false);
		activeMenu->getUIContainer()->setVisible(false);
		
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
	
	changeState(gameState);
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

		// Save continue world and level indices
		settings.set("continue_world", currentWorldIndex);
		settings.set("continue_level", currentLevelIndex);
		saveUserSettings();
		
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
	
	darkenImage->setVisible(true);
	
	openMenu(pauseMenu);
	pauseMenu->select(0);
}

void Application::unpauseSimulation()
{
	simulationPaused = false;
	
	darkenImage->setVisible(false);
	
	closeMenu();
}

void Application::setDisplayDebugInfo(bool display)
{
	displayDebugInfo = display;

	frameTimeLabel->setVisible(displayDebugInfo);
	depthTextureImage->setVisible(displayDebugInfo);
}