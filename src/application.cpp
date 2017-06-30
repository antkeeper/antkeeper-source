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
#include "states/splash-state.hpp"
#include "states/title-state.hpp"
#include "states/experiment-state.hpp"
#include <cstdlib>
#include <iostream>
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
	//menuLeft.bindKey(keyboard, SDL_SCANCODE_A);
	menuRight.bindKey(keyboard, SDL_SCANCODE_RIGHT);
	//menuRight.bindKey(keyboard, SDL_SCANCODE_D);
	menuUp.bindKey(keyboard, SDL_SCANCODE_UP);
	//menuUp.bindKey(keyboard, SDL_SCANCODE_W);
	menuDown.bindKey(keyboard, SDL_SCANCODE_DOWN);
	//menuDown.bindKey(keyboard, SDL_SCANCODE_S);
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
	cameraOverheadView = true;
	cameraNestView = false;
	
	// Allocate states
	splashState = new SplashState(this);
	titleState = new TitleState(this);
	experimentState = new ExperimentState(this);
	
	// Setup loaders
	textureLoader = new TextureLoader();
	materialLoader = new MaterialLoader();
	modelLoader = new ModelLoader();
	modelLoader->setMaterialLoader(materialLoader);
	
	// Enter splash state
	state = nextState = splashState;
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
	while (state != nullptr)
	{
		state->execute();
		
		if (nextState != state)
		{
			state->exit();
			
			state = nextState;
			if (nextState != nullptr)
			{
				state->enter();
			}
		}
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
}