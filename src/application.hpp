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

#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <emergent/emergent.hpp>
using namespace Emergent;

#include "mesh.hpp"
#include "game/terrain.hpp"
#include "game/level.hpp"
#include "game/biome.hpp"
#include "game/terrain.hpp"
#include "input.hpp"
#include "controls.hpp"
#include "settings.hpp"
#include "render-passes.hpp"
#include "ui/ui.hpp"
#include "ui/tween.hpp"

class Menu;
class ApplicationState;
class Colony;
class LoadingState;
class SplashState;
class TitleState;
class MainMenuState;
class CameraController;
class SurfaceCameraController;
class TunnelCameraController;
class LineBatcher;
class ModelLoader;
class MaterialLoader;

/**
 * Encapsulates the state of the application.
 */
class Application
{
public:
	Application(int argc, char* argv[]);
	~Application();
	
	// Executes the application and returns a status code
	int execute();
	
	// Changes the application state
	void changeState(ApplicationState* state);
	
	// Sets the termination code to be returned when the application finishes
	void setTerminationCode(int code);
	
	// Closes the application
	void close(int terminationCode);
	
	void changeFullscreen();
	void changeVerticalSync();
	void saveUserSettings();
	
	bool loadScene();
	bool loadUI();
	bool loadModels();
	bool loadControls();
	bool loadGame();
	
	
	void resizeUI();
	
	void enterMenu(std::size_t index);
	void exitMenu(std::size_t index);
	void selectMenuItem(std::size_t index);
	void activateMenuItem(std::size_t index);
	
	void selectLevel(std::size_t index);
	void activateLevel(std::size_t index);
	
	void enterLevelSelection();
	
	void loadLevel();
	
private:
	ApplicationState* state;
	ApplicationState* nextState;
	int terminationCode;
	
public:
	// SDL
	SDL_Window* window;
	SDL_GLContext context;
	
	// Paths
	std::string appDataPath;
	std::string userDataPath;
	std::string defaultSettingsFilename;
	std::string userSettingsFilename;
	
	// Settings
	ParameterDict settings;
	
	// Window
	bool fullscreen;
	int fullscreenWidth;
	int fullscreenHeight;
	int windowedWidth;
	int windowedHeight;
	int swapInterval;
	int width;
	int height;
	
	// State machine
	LoadingState* loadingState;
	SplashState* splashState;
	TitleState* titleState;
	MainMenuState* mainMenuState;
	
	// Scene
	Scene scene;
	SceneLayer* backgroundLayer;
	SceneLayer* defaultLayer;
	SceneLayer* uiLayer;
	Camera camera;
	Camera sunlightCamera;
	Camera uiCamera;
	Camera bgCamera;
	DirectionalLight sunlight;
	DirectionalLight fillLight;
	DirectionalLight backLight;
	Spotlight lensHotspot;
	Spotlight lensFalloff;
	ModelInstance lensToolObject;
	ModelInstance forcepsToolObject;
	ModelInstance navigatorObject;
	ModelInstance antModelInstance;
	ModelInstance antHillModelInstance;
	
	// Graphics
	Renderer renderer;
	RenderTarget defaultRenderTarget;
	RenderTarget shadowMapRenderTarget;
	GLuint shadowFramebuffer;
	GLuint shadowDepthTexture;
	ShadowMapRenderPass shadowMapPass;
	SoilRenderPass soilPass;
	LightingRenderPass lightingPass;
	DebugRenderPass debugPass;
	Compositor shadowCompositor;
	Compositor defaultCompositor;
	BillboardBatch* uiBatch;
	UIBatcher* uiBatcher;
	UIRenderPass uiPass;
	Compositor uiCompositor;
	BillboardBatch bgBatch;
	Compositor bgCompositor;
	VignetteRenderPass vignettePass;
	TextureLoader* textureLoader;
	MaterialLoader* materialLoader;
	ModelLoader* modelLoader;
	
	// Controls
	InputManager* inputManager;
	Keyboard* keyboard;
	Mouse* mouse;
	ControlProfile* menuControlProfile;
	Control menuLeft;
	Control menuRight;
	Control menuUp;
	Control menuDown;
	Control menuSelect;
	Control menuCancel;
	Control toggleFullscreen;
	Control escape;
	ControlProfile* gameControlProfile;
	Control cameraMoveForward;
	Control cameraMoveBack;
	Control cameraMoveLeft;
	Control cameraMoveRight;
	Control cameraRotateCW;
	Control cameraRotateCCW;
	Control cameraZoomIn;
	Control cameraZoomOut;
	Control cameraToggleOverheadView;
	Control cameraToggleNestView;
	Control walkForward;
	Control walkBack;
	Control turnLeft;
	Control turnRight;
	Arcball arcball;
	
	// Misc
	Timer frameTimer;
	float dt;
	
	// UI text
	ParameterDict strings;
	float dpi;
	float fontSizePT;
	float fontSizePX;
	Font* menuFont;
	
	// UI textures
	Texture* splashTexture;
	Texture* titleTexture;
	Texture* levelActiveTexture;
	Texture* levelInactiveTexture;
	Texture* levelConnectorTexture;
	
	// UI elements
	Vector4 selectedColor;
	Vector4 deselectedColor;
	UIContainer* uiRootElement;
	UIImage* blackoutImage;
	UIImage* splashImage;
	UIImage* titleImage;
	UIImage* copyrightImage;
	UILabel* anyKeyLabel;
	UILabel* menuSelectorLabel;
	UIContainer* mainMenuContainer;
	UIContainer* challengeMenuContainer;
	UIContainer* experimentMenuContainer;
	UIContainer* settingsMenuContainer;
	UILabel* challengeLabel;
	UILabel* experimentLabel;
	UILabel* settingsLabel;
	UILabel* quitLabel;
	UILabel* loadLabel;
	UILabel* newLabel;
	UILabel* experimentBackLabel;
	UILabel* videoLabel;
	UILabel* audioLabel;
	UILabel* controlsLabel;
	UILabel* gameLabel;
	UILabel* settingsBackLabel;
	UIContainer* pauseMenuContainer;
	UILabel* pausedResumeLabel;
	UILabel* pausedSaveLabel;
	UILabel* pausedNewLabel;
	UILabel* pausedSettingsLabel;
	UILabel* returnToMainMenuLabel;
	UILabel* quitToDesktopLabel;
	UIContainer* levelSelectorContainer;
	UIImage* levelSelections[10];
	UIImage* levelConnectors[9];
	
	// Animation
	Tweener* tweener;
	Tween<Vector4>* fadeInTween;
	Tween<Vector4>* fadeOutTween;
	Tween<Vector4>* splashFadeInTween;
	Tween<float>* splashHangTween;
	Tween<Vector4>* splashFadeOutTween;
	Tween<Vector4>* titleFadeInTween;
	Tween<Vector4>* titleFadeOutTween;
	Tween<Vector4>* copyrightFadeInTween;
	Tween<Vector4>* copyrightFadeOutTween;
	Tween<Vector4>* anyKeyFadeInTween;
	Tween<Vector4>* anyKeyFadeOutTween;
	Tween<Vector4>* menuFadeInTween;
	Tween<Vector4>* menuFadeOutTween;
	Tween<Vector2>* menuSlideInTween;
	Tween<Vector2>* levelSelectorSlideInTween;
	
	Tween<float>* antHillZoomInTween;
	Tween<Vector4>* antHillFadeOutTween;
	
	// Menus
	std::size_t menuCount;
	Menu** menus;
	int currentMenuIndex;
	int selectedMenuItemIndex;
	UIContainer** menuContainers;
	Menu* currentMenu;
	Menu* mainMenu;
	Menu* challengeMenu;
	Menu* experimentMenu;
	Menu* settingsMenu;
	Menu* levelSelectorMenu;
	
	// Models
	Model* antModel;
	Model* antHillModel;
	
	// Game variables
	Campaign campaign;
	int currentWorld;
	int currentLevel;
	Biosphere biosphere;
	Terrain terrain;
	Colony* colony;
	SurfaceCameraController* surfaceCam;
	TunnelCameraController* tunnelCam;
	bool cameraOverheadView;
	bool cameraNestView;
	int toolIndex;
	
	// Debug
	LineBatcher* lineBatcher;
};

#endif // APPLICATION_HPP