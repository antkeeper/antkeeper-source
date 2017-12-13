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
class MenuItem;
class ApplicationState;
class Colony;
class LoadingState;
class SplashState;
class TitleState;
class GameState;
class CameraRig;
class OrbitCam;
class FreeCam;
class LineBatcher;
class ModelLoader;
class MaterialLoader;
class Toolbar;
class PieMenu;
class Tool;
class Forceps;
class Lens;
class Brush;

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
	void restringUI();
	
	void openMenu(Menu* menu);
	void closeMenu();
	void selectMenuItem(std::size_t index);
	void activateMenuItem();
	void incrementMenuItem();
	void decrementMenuItem();
	
	void loadWorld(std::size_t index);
	void loadLevel(std::size_t index);
	void continueGame();
	void newGame();
	
	void deselectTool(Tool* tool);
	void selectTool(Tool* tool);
	

	
	void pauseSimulation();
	void unpauseSimulation();
	
	void openPauseMenu();
	void closePauseMenu();
	
	void setDisplayDebugInfo(bool display);
	
	std::u32string getLevelName(std::size_t world, std::size_t level) const;
	
	// Options menu functions
	void selectWindowedResolution(std::size_t index);
	void selectFullscreenResolution(std::size_t index);
	void selectFullscreenMode(std::size_t index);
	void selectVSyncMode(std::size_t index);
	void selectLanguage(std::size_t index);
	
	void bindControl(Control* control);
	
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
	
	// State machine
	LoadingState* loadingState;
	SplashState* splashState;
	TitleState* titleState;
	GameState* gameState;
	
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
	ModelInstance forcepsModelInstance;
	ModelInstance navigatorObject;
	ModelInstance antModelInstance;
	ModelInstance antHillModelInstance;
	ModelInstance nestModelInstance;
	ModelInstance sidewalkPanelInstance;
	ModelInstance sidewalkPanelInstance1;
	ModelInstance sidewalkPanelInstance2;
	ModelInstance sidewalkPanelInstance3;
	ModelInstance sidewalkPanelInstance4;
	ModelInstance soilInstance;
	
	// Graphics
	Renderer renderer;
	RenderTarget defaultRenderTarget;
	
	int shadowMapResolution;
	GLuint shadowMapDepthTexture;
	GLuint shadowMapFramebuffer;
	RenderTarget shadowMapRenderTarget;
	ShadowMapRenderPass shadowMapPass;
	Compositor shadowMapCompositor;
	
	GLuint framebufferAColorTexture;
	GLuint framebufferADepthTexture;
	GLuint framebufferA;
	RenderTarget framebufferARenderTarget;
	
	GLuint framebufferBColorTexture;
	GLuint framebufferBDepthTexture;
	GLuint framebufferB;
	RenderTarget framebufferBRenderTarget;
	
	GLuint pheromonePBO;
	GLuint pheromoneTextureID;
	Texture pheromoneTexture;
	
	ClearRenderPass clearDepthPass;
	LightingRenderPass lightingPass;
	DebugRenderPass debugPass;
	Compositor defaultCompositor;
	BillboardBatch* uiBatch;
	UIBatcher* uiBatcher;
	UIRenderPass uiPass;
	Compositor uiCompositor;
	BillboardBatch bgBatch;
	Compositor bgCompositor;
	VignetteRenderPass vignettePass;
	SkyboxRenderPass skyboxPass;
	TextureLoader* textureLoader;
	MaterialLoader* materialLoader;
	ModelLoader* modelLoader;
	BlurRenderPass horizontalBlurPass;
	BlurRenderPass verticalBlurPass;
	BlurRenderPass horizontalBlurPass2;
	BlurRenderPass verticalBlurPass2;
	
	// Controls
	Control* bindingControl;
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
	Control toggleDebugDisplay;
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
	Control togglePause;
	Control togglePauseMenu;
	Control fastForward;
	Control switchRig;
	Arcball arcball;
	
	// Misc
	Timer frameTimer;
	float t;
	float dt;
	
	// UI text
	ParameterDict strings;
	float dpi;
	float fontSizePT;
	float fontSizePX;
	Font* menuFont;
	Font* copyrightFont;
	Font* levelNameFont;
	
	// UI textures
	Texture* splashTexture;
	Texture* titleTexture;
	Texture* rectangularPaletteTexture;
	Texture* foodIndicatorTexture;
	Texture* toolBrushTexture;
	Texture* toolLensTexture;
	Texture* toolForcepsTexture;
	Texture* toolTrowelTexture;
	
	Texture* toolbarTopTexture;
	Texture* toolbarBottomTexture;
	Texture* toolbarMiddleTexture;
	Texture* toolbarButtonRaisedTexture;
	Texture* toolbarButtonDepressedTexture;
	
	Texture* arcNorthTexture;
	Texture* arcEastTexture;
	Texture* arcSouthTexture;
	Texture* arcWestTexture;
	Texture* mouseLeftTexture;
	Texture* mouseRightTexture;
	
	Texture* depthTexture;
	
	// UI elements
	Vector4 selectedColor;
	Vector4 deselectedColor;
	UIContainer* uiRootElement;
	UIImage* blackoutImage;
	UIImage* splashBackgroundImage;
	UIImage* splashImage;
	UIImage* titleImage;
	UIImage* darkenImage;
	UILabel* frameTimeLabel;
	UILabel* anyKeyLabel;
	UILabel* copyrightLabel;
	UIImage* rectangularPaletteImage;
	UIImage* foodIndicatorImage;
	UIImage* contextButtonImage0;
	UIImage* contextButtonImage1;
	UIImage* depthTextureImage;
	UILabel* levelNameLabel;
	Toolbar* toolbar;
	PieMenu* pieMenu;
	
	// Animation
	Tweener* tweener;
	Tween<Vector4>* fadeInTween;
	Tween<Vector4>* fadeOutTween;
	Tween<Vector4>* darkenFadeInTween;
	Tween<Vector4>* darkenFadeOutTween;
	Tween<float>* blurFadeInTween;
	Tween<float>* blurFadeOutTween;
	Tween<Vector4>* splashFadeInTween;
	Tween<Vector4>* splashFadeOutTween;
	Tween<float>* splashHangTween;
	Tween<Vector4>* titleFadeInTween;
	Tween<Vector4>* titleFadeOutTween;
	Tween<Vector4>* anyKeyFadeInTween;
	Tween<Vector4>* anyKeyFadeOutTween;
	Tween<Vector4>* menuFadeInTween;
	Tween<Vector4>* menuFadeOutTween;
	Tween<float>* menuActivateTween;
	Tween<Vector3>* cameraTranslationTween;
	Tween<float>* forcepsSwoopTween;

	
	// Menus
	Menu* activeMenu;
	Menu* previousActiveMenu;
	
	Menu* mainMenu;
	MenuItem* mainMenuContinueItem;
	MenuItem* mainMenuLevelsItem;
	MenuItem* mainMenuNewGameItem;
	MenuItem* mainMenuSandboxItem;
	MenuItem* mainMenuOptionsItem;
	MenuItem* mainMenuExitItem;
	
	Menu* levelsMenu;
	MenuItem* levelsMenuBackItem;
	
	Menu* optionsMenu;
	MenuItem* optionsMenuWindowedResolutionItem;
	MenuItem* optionsMenuFullscreenResolutionItem;
	MenuItem* optionsMenuFullscreenItem;
	MenuItem* optionsMenuVSyncItem;
	MenuItem* optionsMenuLanguageItem;
	MenuItem* optionsMenuControlsItem;
	MenuItem* optionsMenuBackItem;
	
	Menu* controlsMenu;
	MenuItem* controlsMenuResetToDefaultItem;
	MenuItem* controlsMenuMoveForwardItem;
	MenuItem* controlsMenuMoveBackItem;
	MenuItem* controlsMenuMoveLeftItem;
	MenuItem* controlsMenuMoveRightItem;
	MenuItem* controlsMenuBackItem;
	
	Menu* graphicsMenu;
	Menu* audioMenu;
	Menu* gameplayMenu;
	
	Menu* pauseMenu;
	MenuItem* pauseMenuResumeItem;
	MenuItem* pauseMenuLevelsItem;
	MenuItem* pauseMenuOptionsItem;
	MenuItem* pauseMenuMainMenuItem;
	MenuItem* pauseMenuExitItem;
	
	// Models
	Model* antModel;
	Model* antHillModel;
	Model* nestModel;
	Model* forcepsModel;
	Model* lensModel;
	Model* brushModel;
	Model* sidewalkPanelModel;
	Model* soilModel;
	
	// Game variables
	Biosphere biosphere;
	Campaign campaign;
	
	int currentWorldIndex;
	int currentLevelIndex;
	Level* currentLevel;
	
	Colony* colony;
	OrbitCam* orbitCam;
	FreeCam* freeCam;
	CameraRig* activeRig;
	bool cameraOverheadView;
	bool cameraNestView;
	int toolIndex;
	Tool* currentTool;
	Forceps* forceps;
	Lens* lens;
	Brush* brush;
	bool simulationPaused;
	
	// Debug
	LineBatcher* lineBatcher;
	bool displayDebugInfo;
	
	// Options menu values
	bool fullscreen;
	int swapInterval;
	Vector2 resolution;
	std::vector<Vector2> resolutions;
	std::size_t windowedResolutionIndex;
	std::size_t fullscreenResolutionIndex;
	int* fullscreenModes;
	int* vsyncModes;
	std::vector<std::string> languages;
	std::size_t languageIndex;
};

#endif // APPLICATION_HPP