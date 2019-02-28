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

#include "entity/entity-id.hpp"
#include <map>
#include <string>
#include <vector>

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
typedef std::vector<std::vector<std::string>> CSVTable;
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
class ToolSystem;
class BehaviorSystem;
class SteeringSystem;
class LocomotionSystem;

class TestEvent: public Event<TestEvent>
{
	public:
		inline EventBase* clone() const
		{
			TestEvent* event = new TestEvent();
			event->id = id;
			return event;
		}

		int id;
};


class Game:
	public Application,
	public EventHandler<TestEvent>
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
	 * Gets a string in the specified language.
	 *
	 * @param languageIndex Index of a language.
	 * @param name Name of the string.
	 * @return String in the specified language.
	 */
	std::string getString(std::size_t languageIndex, const std::string& name) const;

	/**
	 * Changes the current language.
	 *
	 * @param languageIndex Index of the language to use.
	 */
	void changeLanguage(std::size_t languageIndex);

	/// Returns the number of available languages.
	std::size_t getLanguageCount() const;

	/// Returns the index of the current language.
	std::size_t getCurrentLanguage() const;

	void toggleFullscreen();
	void setUpdateRate(double frequency);

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

	void selectTool(int toolIndex);
	void pushNotification(const std::string& text);
	void popNotification();

private:
	virtual void setup();
	virtual void input();
	virtual void update(float t, float dt);
	virtual void render();
	virtual void exit();
	virtual void handleEvent(const WindowResizedEvent& event);
	virtual void handleEvent(const KeyPressedEvent& event);
	virtual void handleEvent(const TestEvent& event);
	void resizeUI(int w, int h);
	void restringUI();
	void resizeRenderTargets();

	void setTimeOfDay(float time);
	void toggleWireframe();
	void screenshot();
	void queueScreenshot();


public:
	EntityID createInstanceOf(const std::string& templateName);
	void destroyInstance(EntityID entity);
	void setTranslation(EntityID entity, const Vector3& translation);
	void setRotation(EntityID entity, const Quaternion& rotation);
	void setScale(EntityID entity, const Vector3& scale);

	void boxSelect(float x, float y, float w, float h);

public:
	// States
	GameState* currentState;
	SplashState* splashState;
	SandboxState* sandboxState;

	// Paths
	std::string dataPath;
	std::string configPath;

	// Localization
	CSVTable* stringTable;
	std::map<std::string, std::size_t> stringMap;
	std::size_t languageCount;
	std::size_t currentLanguage;

	// Window management
	Window* window;
	bool fullscreen;
	std::string title;
	int w, h;
	float dpi;
	float fontSizePT;
	float fontSizePX;

	// Input
	Mouse* mouse;
	Keyboard* keyboard;
	ControlProfile controlProfile;
	Control fullscreenControl;
	Control closeControl;
	Control openRadialMenuControl;
	Control moveForwardControl;
	Control moveBackControl;
	Control moveLeftControl;
	Control moveRightControl;
	Control rotateCCWControl;
	Control rotateCWControl;
	Control zoomInControl;
	Control zoomOutControl;
	Control adjustCameraControl;
	Control dragCameraControl;
	Control toggleNestViewControl;
	Control toggleWireframeControl;
	Control screenshotControl;
	Control toggleEditModeControl;

	// Logic
	float time;
	float timestep;

	// UI
	Typeface* labelTypeface;
	Font* labelFont;
	Typeface* debugTypeface;
	Font* debugFont;
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
	UIImage* cameraFlashImage;
	UIImage* notificationBoxImage;
	int notificationCount;
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
	UIContainer* cameraGridContainer;
	Vector4 cameraGridColor;

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
	Scene* scene;
	SceneLayer* defaultLayer;
	SceneLayer* uiLayer;
	DirectionalLight sunlight;
	Camera camera;
	Camera sunlightCamera;
	Camera uiCamera;

	// Animation
	Animator animator;
	Animation<float> fadeInAnimation;
	Animation<float> fadeOutAnimation;
	AnimationClip<float> fadeInClip;
	AnimationClip<float> fadeOutClip;
	std::function<void()> fadeInEndCallback;
	std::function<void()> fadeOutEndCallback;
	Animation<Vector2> showNotificationAnimation;
	Animation<float> hideNotificationAnimation;
	AnimationClip<Vector2> showNotificationClip;
	AnimationClip<float> hideNotificationClip;
	Animation<float> cameraFlashAnimation;
	AnimationClip<float> cameraFlashClip;


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
	ParticleSystem* particleSystem;

	// ECS
	EntityManager* entityManager;
	ComponentManager* componentManager;
	SystemManager* systemManager;
	SoundSystem* soundSystem;
	CollisionSystem* collisionSystem;
	RenderSystem* renderSystem;
	ToolSystem* toolSystem;
	BehaviorSystem* behaviorSystem;
	SteeringSystem* steeringSystem;
	LocomotionSystem* locomotionSystem;

	EntityID lollipop;

	bool wireframe;
	bool screenshotQueued;

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

inline std::size_t Game::getCurrentLanguage() const
{
	return currentLanguage;
}

#endif // GAME_HPP

