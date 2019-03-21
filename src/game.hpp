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

#ifndef GAME_HPP
#define GAME_HPP

#include <emergent/emergent.hpp>
using namespace Emergent;

#include "state-machine.hpp"
#include "entity/entity-id.hpp"
#include "scheduled-function-event.hpp"
#include "resources/string-table.hpp"
#include <array>
#include <map>
#include <optional>
#include <string>
#include <vector>
#include <fstream>

class GameState;
class SplashState;
class SandboxState;
class UIContainer;
class UIBatcher;
class UIImage;
class UILabel;
class UIRenderPass;
class ClearRenderPass;
class SkyRenderPass;
class ShadowMapRenderPass;
class LightingRenderPass;
class SilhouetteRenderPass;
class FinalRenderPass;
class ResourceManager;
class CameraRig;
class OrbitCam;
class FreeCam;
class Tool;
class Lens;
class Forceps;
class Brush;
class ParticleSystem;
class EntityManager;
class ComponentManager;
class SystemManager;
class SoundSystem;
class CollisionSystem;
class RenderSystem;
class CameraSystem;
class ToolSystem;
class BehaviorSystem;
class SteeringSystem;
class LocomotionSystem;
class TerrainSystem;
class ComponentBase;
class Menu;
class MenuItem;
class CommandInterpreter;
class Logger;
enum class ComponentType;

class Game:
	public Application,
	public StateMachine,
	public EventHandler<ScheduledFunctionEvent>
{
public:
	/**
	 * Creates a game instance.
	 *
	 * @param argc Argument count
	 * @param argv Argument list
	 */
	Game(int argc, char* argv[]);

	/// Destroys a game instance.
	virtual ~Game();

	/**
	 * Gets a string in the current language.
	 *
	 * @param name Name of the string.
	 * @return String in the current language.
	 */
	std::string getString(const std::string& name, std::optional<std::size_t> languageIndex = std::nullopt) const;

	/**
	 * Changes the current language.
	 *
	 * @param languageIndex Index of the language to use.
	 */
	void changeLanguage(std::size_t nextLanguageIndex);

	void nextLanguage();

	/// Returns the number of available languages.
	std::size_t getLanguageCount() const;

	/// Returns the index of the current language.
	std::size_t getCurrentLanguageIndex() const;

	void openMenu(Menu* menu, int selectedItemIndex);
	void closeCurrentMenu();
	void selectMenuItem(int index, bool tween);
	void selectNextMenuItem();
	void selectPreviousMenuItem();
	void activateMenuItem();
	void activateLastMenuItem();

	void toggleFullscreen();
	void toggleVSync();
	void setUpdateRate(double frequency);

	void disableNonSystemControls();

	/**
	 * Changes the game state.
	 *
	 * @param state New game state.
	 */
	void changeState(GameState* state);

	const EventDispatcher* getEventDispatcher() const;
	EventDispatcher* getEventDispatcher();

	const Animator* getAnimator() const;
	Animator* getAnimator();

	void fadeIn(float duration, const Vector3& color, std::function<void()> callback);
	void fadeOut(float duration, const Vector3& color, std::function<void()> callback);
	void stopFade();

	void selectTool(int toolIndex);



private:
	virtual void setup();
	virtual void input();
	virtual void update(float t, float dt);
	virtual void render();
	virtual void exit();
	virtual void handleEvent(const WindowResizedEvent& event);
	virtual void handleEvent(const GamepadConnectedEvent& event);
	virtual void handleEvent(const GamepadDisconnectedEvent& event);
	virtual void handleEvent(const ScheduledFunctionEvent& event);

	#if defined(DEBUG)
		void setupDebugging();
	#endif
	void setupLocalization();
	void setupWindow();
	void setupGraphics();
	void setupUI();
	void setupControls();
	void setupGameplay();

	void resetSettings();
	void loadSettings();
	void saveSettings();
	void loadStrings();
	void loadFonts();
	void loadControlProfile(const std::string& profileName);
	void saveControlProfile(const std::string& profileName);
	std::array<std::string, 3> getInputMappingStrings(const InputMapping* mapping);
	void remapControl(Control* control);
	void resetControls();
	// Callback for the input mapper
	void inputMapped(const InputMapping& mapping);

	void resizeUI(int w, int h);
	void restringUI();
	void restringControlMenuItem(MenuItem* item, const std::string& name);
	void resizeRenderTargets();

	void setTimeOfDay(float time);
	void queueScreenshot();
	void screenshot();

	// State functions
	void enterLanguageSelectState();
	void exitLanguageSelectState();
	void enterSplashState();
	void exitSplashState();
	void enterLoadingState();
	void exitLoadingState();
	void enterTitleState();
	void exitTitleState();
	void enterPlayState();
	void exitPlayState();

	void languageSelected();
	void skipSplash();

	void togglePause();

	void continueGame();
	void newGame();
	void returnToMainMenu();

	void interpretCommands();

public:
	EntityID createInstance();
	EntityID createNamedInstance(const std::string& instanceName);
	EntityID createInstanceOf(const std::string& templateName);
	EntityID createNamedInstanceOf(const std::string& templateName, const std::string& instanceName);
	void selectInstance(EntityID entity);
	void selectNamedInstance(const std::string& instanceName);

	void destroyInstance(EntityID entity);
	void addComponent(EntityID entity, ComponentBase* component);
	void removeComponent(EntityID entity, ComponentType type);
	void setTranslation(EntityID entity, const Vector3& translation);
	void setRotation(EntityID entity, const Quaternion& rotation);
	void setScale(EntityID entity, const Vector3& scale);
	void setTerrainPatchPosition(EntityID entity, const std::tuple<int, int>& position);

	void boxSelect(float x, float y, float w, float h);

	template <typename T>
	bool readSetting(const std::string& name, T* value) const;


public:
	// Game states
	StateMachine::State languageSelectState;
	StateMachine::State splashState;
	StateMachine::State loadingState;
	StateMachine::State titleState;
	StateMachine::State playState;

	GameState* currentState;
	SandboxState* sandboxState;

	// Paths
	std::string dataPath;
	std::string configPath;
	std::string controlsPath;

	// Settings
	bool firstRun;
	StringTable* settingsTable;
	StringTableIndex settingsTableIndex;

	// Localization
	StringTable* stringTable;
	StringTableIndex stringTableIndex;
	std::size_t languageCount;
	std::size_t currentLanguageIndex;

	// Window management
	Window* window;
	int w, h;
	float dpi;
	float fontSizePX;

	// Input
	Mouse* mouse;
	Keyboard* keyboard;

	// Master control set
	ControlSet controls;

	// System control set
	ControlSet systemControls;
	Control exitControl;
	Control toggleFullscreenControl;
	Control takeScreenshotControl;
	
	// Menu control set
	ControlSet menuControls;
	Control menuUpControl;
	Control menuDownControl;
	Control menuLeftControl;
	Control menuRightControl;
	Control menuActivateControl;
	Control menuBackControl;

	// Camera control set
	ControlSet cameraControls;
	Control moveForwardControl;
	Control moveLeftControl;
	Control moveBackControl;
	Control moveRightControl;
	Control zoomInControl;
	Control zoomOutControl;
	Control orbitCCWControl;
	Control orbitCWControl;
	Control adjustCameraControl;
	Control dragCameraControl;
	Control pauseControl;

	// Tool control set
	ControlSet toolControls;
	Control changeToolControl;
	Control useToolControl;

	// Editor control set
	ControlSet editorControls;
	Control toggleEditModeControl;

	// Map of control names
	std::map<std::string, Control*> controlNameMap;

	// Input mapper
	InputMapper* inputMapper;

	// Logic
	float time;
	float timestep;

	// UI
	Font* debugFont;
	Font* menuFont;
	std::vector<Font*> languageSelectionFonts;
	BillboardBatch* uiBatch;
	UIBatcher* uiBatcher;
	UIContainer* uiRootElement;
	UIImage* splashBackgroundImage;
	UIImage* splashImage;
	UIContainer* hudContainer;
	UIImage* toolIndicatorBGImage;
	UIImage* toolIndicatorIconImage;
	Rect* toolIndicatorsBounds;
	UIImage* toolIconBrushImage;
	UIImage* toolIconLensImage;
	UIImage* toolIconForcepsImage;
	UIImage* toolIconSpadeImage;
	UIImage* toolIconCameraImage;
	UIImage* toolIconMicrochipImage;
	UIImage* toolIconTestTubeImage;
	UIContainer* buttonContainer;
	UIImage* playButtonBGImage;
	UIImage* fastForwardButtonBGImage;
	UIImage* pauseButtonBGImage;
	UIImage* playButtonImage;
	UIImage* fastForwardButtonImage;
	UIImage* pauseButtonImage;
	UIContainer* radialMenuContainer;
	UIImage* radialMenuBackgroundImage;
	UIImage* radialMenuImage;
	UIImage* radialMenuSelectorImage;
	UIImage* blackoutImage;
	UIImage* languageSelectBGImage;
	UIImage* cameraFlashImage;
	UIContainer* antTag;
	UIContainer* antLabelContainer;
	UILabel* fpsLabel;
	UILabel* antLabel;
	UIImage* antLabelTL; // Top-left
	UIImage* antLabelTR; // Top-right
	UIImage* antLabelBL; // Bottom-left
	UIImage* antLabelBR; // Bottom-right
	UIImage* antLabelCC; // Center-center
	UIImage* antLabelCT; // Center-top
	UIImage* antLabelCB; // Center-bottom
	UIImage* antLabelCL; // Center-left
	UIImage* antLabelCR; // Center-right
	UIImage* antLabelPinHole;
	UIImage* antPin;

	UIImage* boxSelectionImageBackground;
	UIImage* boxSelectionImageTop;
	UIImage* boxSelectionImageBottom;
	UIImage* boxSelectionImageLeft;
	UIImage* boxSelectionImageRight;
	UIContainer* boxSelectionContainer;
	float boxSelectionBorderWidth;

	UIImage* cameraGridY0Image;
	UIImage* cameraGridY1Image;
	UIImage* cameraGridX0Image;
	UIImage* cameraGridX1Image;
	UIImage* cameraReticleImage;
	UIContainer* cameraGridContainer;
	Vector4 cameraGridColor;
	Vector4 cameraReticleColor;


	// Menu selection
	UIImage* menuSelectorImage;
	Menu* currentMenu;
	Menu* previousMenu;
	MenuItem* currentMenuItem;
	MenuItem* previousMenuItem;
	int menuItemIndex;
	Vector4 standardMenuActiveColor;
	Vector4 standardMenuInactiveColor;
	Vector4 languageMenuActiveColor;
	Vector4 languageMenuInactiveColor;
	Vector4 menuItemActiveColor;
	Vector4 menuItemInactiveColor;


	// Main menu
	Menu* mainMenu;
	MenuItem* mainMenuContinueItem;
	MenuItem* mainMenuNewGameItem;
	MenuItem* mainMenuColoniesItem;
	MenuItem* mainMenuSettingsItem;
	MenuItem* mainMenuQuitItem;

	// Settings menu
	Menu* settingsMenu;
	MenuItem* settingsMenuControlsItem;
	MenuItem* settingsMenuFullscreenItem;
	MenuItem* settingsMenuVSyncItem;
	MenuItem* settingsMenuLanguageItem;
	MenuItem* settingsMenuBackItem;

	// Controls menu
	Menu* controlsMenu;
	MenuItem* controlsMenuMoveForwardItem;
	MenuItem* controlsMenuMoveLeftItem;
	MenuItem* controlsMenuMoveBackItem;
	MenuItem* controlsMenuMoveRightItem;
	MenuItem* controlsMenuChangeToolItem;
	MenuItem* controlsMenuUseToolItem;
	MenuItem* controlsMenuAdjustCameraItem;
	MenuItem* controlsMenuPauseItem;
	MenuItem* controlsMenuToggleFullscreenItem;
	MenuItem* controlsMenuTakeScreenshotItem;
	MenuItem* controlsMenuResetToDefaultItem;
	MenuItem* controlsMenuBackItem;

	// Pause menu
	Menu* pauseMenu;
	MenuItem* pauseMenuResumeItem;
	MenuItem* pauseMenuSettingsItem;
	MenuItem* pauseMenuMainMenuItem;
	MenuItem* pauseMenuQuitItem;

	Menu* languageMenu;
	std::vector<MenuItem*> languageMenuItems;

	// Rendering
	Renderer renderer;
	RenderTarget defaultRenderTarget;
	ClearRenderPass* clearPass;
	ClearRenderPass* clearSilhouettePass;
	SkyRenderPass* skyPass;
	UIRenderPass* uiPass;
	Compositor uiCompositor;
	Compositor defaultCompositor;
	int shadowMapResolution;
	GLuint shadowMapDepthTextureID;
	GLuint shadowMapFramebuffer;
	RenderTarget shadowMapRenderTarget;
	ShadowMapRenderPass* shadowMapPass;
	Compositor shadowMapCompositor;
	Texture2D shadowMapDepthTexture;
	LightingRenderPass* lightingPass;
	SilhouetteRenderPass* silhouettePass;
	FinalRenderPass* finalPass;
	RenderTarget silhouetteRenderTarget;

	// Scene
	Scene* worldScene;
	Scene* uiScene;
	DirectionalLight sunlight;
	Camera camera;
	Camera sunlightCamera;
	Camera uiCamera;

	// Animation
	Animator animator;

	Animation<float> antHillZoomAnimation;
	AnimationClip<float> antHillZoomClip;

	Animation<float> menuFadeAnimation;
	AnimationClip<float> menuFadeInClip;
	AnimationClip<float> menuFadeOutClip;

	Animation<float> splashFadeInAnimation;
	Animation<float> splashFadeOutAnimation;
	AnimationClip<float> splashFadeInClip;
	AnimationClip<float> splashFadeOutClip;
	Animation<float> fadeInAnimation;
	Animation<float> fadeOutAnimation;
	AnimationClip<float> fadeInClip;
	AnimationClip<float> fadeOutClip;
	std::function<void()> fadeInEndCallback;
	std::function<void()> fadeOutEndCallback;
	Animation<float> cameraFlashAnimation;
	AnimationClip<float> cameraFlashClip;
	Animation<float> menuSelectorSlideAnimation;
	AnimationClip<float> menuSelectorSlideClip;
	Animation<Vector4> menuItemSelectAnimation;
	AnimationClip<Vector4> menuItemSelectClip;
	Animation<Vector4> menuItemDeselectAnimation;
	AnimationClip<Vector4> menuItemDeselectClip;



	// Assets
	ResourceManager* resourceManager;
	Texture2D* splashTexture;
	Texture2D* hudSpriteSheetTexture;
	TextureAtlas hudTextureAtlas;
	Material* smokeMaterial;
	Model* lensModel;
	Model* forcepsModel;
	Model* brushModel;
	
	// Game
	CameraRig* cameraRig;
	OrbitCam* orbitCam;
	FreeCam* freeCam;
	Tool* currentTool;
	Lens* lens;
	Forceps* forceps;
	Brush* brush;

	// ECS
	EntityManager* entityManager;
	ComponentManager* componentManager;
	SystemManager* systemManager;
	SoundSystem* soundSystem;
	CollisionSystem* collisionSystem;
	CameraSystem* cameraSystem;
	RenderSystem* renderSystem;
	ToolSystem* toolSystem;
	BehaviorSystem* behaviorSystem;
	SteeringSystem* steeringSystem;
	LocomotionSystem* locomotionSystem;
	ParticleSystem* particleSystem;
	TerrainSystem* terrainSystem;

	bool screenshotQueued;
	bool paused;

	// Settings
	std::string language;
	Vector2 windowedResolution;
	Vector2 fullscreenResolution;
	bool fullscreen;
	bool vsync;
	float fontSizePT;
	std::string controlProfileName;
	bool toggleFullscreenDisabled;

	// Debugging
	Logger* logger;
	std::ofstream logFileStream;
	CommandInterpreter* cli;

private:
	static void saveScreenshot(const std::string& filename, unsigned int width, unsigned int height, unsigned char* pixels);
};

inline const EventDispatcher* Game::getEventDispatcher() const
{
	return &eventDispatcher;
}

inline EventDispatcher* Game::getEventDispatcher()
{
	return &eventDispatcher;
}

inline const Animator* Game::getAnimator() const
{
	return &animator;
}

inline Animator* Game::getAnimator()
{
	return &animator;
}

inline std::size_t Game::getLanguageCount() const
{
	return languageCount;
}

inline std::size_t Game::getCurrentLanguageIndex() const
{
	return currentLanguageIndex;
}

#endif // GAME_HPP

