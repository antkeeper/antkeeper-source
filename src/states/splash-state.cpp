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

#include "splash-state.hpp"
#include "title-state.hpp"
#include "experiment-state.hpp"
#include "../application.hpp"
#include "../camera-controller.hpp"
#include "../debug.hpp"
#include "../model-loader.hpp"
#include <iostream>
#include <cstdio>
#include <SDL.h>

const float blankDuration = 0.0f;
const float fadeInDuration = 0.5f;
const float hangDuration = 1.0f;
const float fadeOutDuration = 0.5f;

SplashState::SplashState(Application* application):
	ApplicationState(application)
{}

SplashState::~SplashState()
{}

void SplashState::enter()
{
	std::cout << "Entering SplashState..." << std::endl;
	
	application->scene.addLayer();
	application->uiScene.addLayer();
	
	// Debug
	application->lineBatcher = new LineBatcher(4096);
	BillboardBatch* lineBatch = application->lineBatcher->getBatch();
	lineBatch->setAlignment(&application->camera, BillboardAlignmentMode::CYLINDRICAL);
	lineBatch->setAlignmentVector(Vector3(1, 0, 0));
	application->scene.getLayer(0)->addObject(lineBatch);
	
	// Load menu font
	application->menuFont = new Font(512, 512);
	FontLoader* fontLoader = new FontLoader();
	if (!fontLoader->load("data/fonts/Varela-Regular.ttf", application->fontSizePX, application->menuFont))
	{
		std::cerr << "Failed to load font" << std::endl;
	}
	delete fontLoader;
	
	// Load splash & title textures
	application->textureLoader->setGamma(1.0f);
	application->textureLoader->setCubemap(false);
	application->textureLoader->setMipmapChain(false);
	application->textureLoader->setMaxAnisotropy(1.0f);
	application->splashTexture = application->textureLoader->load("data/textures/splash.png");
	application->titleTexture = application->textureLoader->load("data/textures/title.png");
	
	// Get UI strings
	std::string pressAnyKeyString;
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
	application->strings.get("press-any-key", &pressAnyKeyString);
	application->strings.get("back", &backString);
	application->strings.get("challenge", &challengeString);
	application->strings.get("experiment", &experimentString);
	application->strings.get("settings", &settingsString);
	application->strings.get("quit", &quitString);
	application->strings.get("load", &loadString);
	application->strings.get("new", &newString);
	application->strings.get("video", &videoString);
	application->strings.get("audio", &audioString);
	application->strings.get("controls", &controlsString);
	application->strings.get("game", &gameString);
	application->strings.get("resume", &resumeString);
	application->strings.get("return-to-main-menu", &returnToMainMenuString);
	application->strings.get("quit-to-desktop", &quitToDesktopString);

	
	// Colors
	application->selectedColor = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
	application->deselectedColor = Vector4(0.0f, 0.0f, 0.0f, 0.35f);
	
	// Build UI
	application->uiRootElement = new UIContainer();
	application->uiRootElement->setDimensions(Vector2(application->width, application->height));
	application->mouse->addMouseMotionObserver(application->uiRootElement);
	application->mouse->addMouseButtonObserver(application->uiRootElement);
	
	application->blackoutImage = new UIImage();
	application->blackoutImage->setDimensions(Vector2(application->width, application->height));
	application->blackoutImage->setLayerOffset(99);
	application->blackoutImage->setTintColor(Vector4(0.0f, 0.0f, 0.0f, 1.0f));
	application->blackoutImage->setVisible(false);
	application->uiRootElement->addChild(application->blackoutImage);
	
	application->splashImage = new UIImage();
	application->splashImage->setAnchor(Anchor::CENTER);
	application->splashImage->setDimensions(Vector2(application->splashTexture->getWidth(), application->splashTexture->getHeight()));
	application->splashImage->setTexture(application->splashTexture);
	application->splashImage->setVisible(false);
	application->uiRootElement->addChild(application->splashImage);
	
	application->titleImage = new UIImage();
	application->titleImage->setAnchor(Vector2(0.5f, 0.0f));
	application->titleImage->setDimensions(Vector2(application->titleTexture->getWidth(), application->titleTexture->getHeight()));
	application->titleImage->setTranslation(Vector2(0.0f, (int)(application->height * (1.0f / 3.0f) - application->titleTexture->getHeight())));
	application->titleImage->setTexture(application->titleTexture);
	application->titleImage->setVisible(false);
	application->uiRootElement->addChild(application->titleImage);
	
	/*
	application->copyrightImage = new UIImage();
	application->copyrightImage->setAnchor(Vector2(0.5f, 1.0f));
	application->copyrightImage->setDimensions(Vector2(copyrightTextureWidth, copyrightTextureHeight));
	application->copyrightImage->setTranslation(Vector2(-.5f, (int)(-application->height * (1.0f / 10.0f) - copyrightTextureHeight * 0.5f)));
	application->copyrightImage->setTexture(nullptr);
	application->copyrightImage->setVisible(false);
	application->uiRootElement->addChild(application->copyrightImage);
	*/
	
	application->anyKeyLabel = new UILabel();
	application->anyKeyLabel->setAnchor(Vector2(0.5f, 1.0f));
	application->anyKeyLabel->setFont(application->menuFont);
	application->anyKeyLabel->setTranslation(Vector2(0.0f, (int)(-application->height * (1.0f / 3.0f)/* - application->menuFont->getMetrics().getHeight() * 0.5f*/)));
	application->anyKeyLabel->setText(pressAnyKeyString);
	application->anyKeyLabel->setVisible(false);
	application->uiRootElement->addChild(application->anyKeyLabel);
	
	
	application->menuSelectorLabel = new UILabel();
	application->menuSelectorLabel->setAnchor(Anchor::TOP_LEFT);
	application->menuSelectorLabel->setFont(application->menuFont);
	application->menuSelectorLabel->setText("<");
	application->menuSelectorLabel->setTintColor(application->selectedColor);
	
	/*
	application->menuSelectorLabel = new UIImage();
	application->menuSelectorLabel->setAnchor(Anchor::TOP_LEFT);
	application->menuSelectorLabel->setDimensions(Vector2(selectorTextureWidth, selectorTextureHeight));
	application->menuSelectorLabel->setTextureID(selectorTextureID);
	*/
	application->menuSelectorLabel->setVisible(false);
	application->uiRootElement->addChild(application->menuSelectorLabel);
	
	application->mainMenuContainer = new UIContainer();
	application->mainMenuContainer->setDimensions(Vector2(application->width, application->menuFont->getMetrics().getHeight() * 4));
	application->mainMenuContainer->setAnchor(Vector2(0.0f, 0.5f));
	application->mainMenuContainer->setVisible(false);
	application->mainMenuContainer->setActive(false);
	application->uiRootElement->addChild(application->mainMenuContainer);
	application->challengeLabel = new UILabel();
	application->challengeLabel->setFont(application->menuFont);
	application->challengeLabel->setText(challengeString);
	application->challengeLabel->setTintColor(application->deselectedColor);
	application->experimentLabel = new UILabel();
	application->experimentLabel->setFont(application->menuFont);
	application->experimentLabel->setText(experimentString);
	application->experimentLabel->setTranslation(Vector2(0.0f, application->menuFont->getMetrics().getHeight()));
	application->experimentLabel->setTintColor(application->deselectedColor);
	application->settingsLabel = new UILabel();
	application->settingsLabel->setFont(application->menuFont);
	application->settingsLabel->setText(settingsString);
	application->settingsLabel->setTranslation(Vector2(0.0f, application->menuFont->getMetrics().getHeight() * 2));
	application->settingsLabel->setTintColor(application->deselectedColor);
	application->quitLabel = new UILabel();
	application->quitLabel->setFont(application->menuFont);
	application->quitLabel->setText(quitString);
	application->quitLabel->setTintColor(application->deselectedColor);
	application->quitLabel->setTranslation(Vector2(0.0f, application->menuFont->getMetrics().getHeight() * 3));
	application->mainMenuContainer->addChild(application->challengeLabel);
	application->mainMenuContainer->addChild(application->experimentLabel);
	application->mainMenuContainer->addChild(application->settingsLabel);
	application->mainMenuContainer->addChild(application->quitLabel);
	
	application->challengeMenuContainer = new UIContainer();
	application->challengeMenuContainer->setDimensions(Vector2(application->width, application->menuFont->getMetrics().getHeight() * 4));
	application->challengeMenuContainer->setAnchor(Vector2(0.0f, 0.5f));
	application->challengeMenuContainer->setVisible(false);
	application->challengeMenuContainer->setActive(false);
	application->uiRootElement->addChild(application->challengeMenuContainer);
	
	application->experimentMenuContainer = new UIContainer();
	application->experimentMenuContainer->setDimensions(Vector2(application->width, application->menuFont->getMetrics().getHeight() * 3));
	application->experimentMenuContainer->setAnchor(Vector2(0.0f, 0.5f));
	application->experimentMenuContainer->setVisible(false);
	application->experimentMenuContainer->setActive(false);
	application->uiRootElement->addChild(application->experimentMenuContainer);
	application->loadLabel = new UILabel();
	application->loadLabel->setFont(application->menuFont);
	application->loadLabel->setText(loadString);
	application->loadLabel->setTintColor(application->deselectedColor);
	application->loadLabel->setTranslation(Vector2(0.0f, application->menuFont->getMetrics().getHeight() * 0));
	application->experimentMenuContainer->addChild(application->loadLabel);
	application->newLabel = new UILabel();
	application->newLabel->setFont(application->menuFont);
	application->newLabel->setText(newString);
	application->newLabel->setTintColor(application->deselectedColor);
	application->newLabel->setTranslation(Vector2(0.0f, application->menuFont->getMetrics().getHeight() * 1));
	application->experimentMenuContainer->addChild(application->newLabel);
	application->experimentBackLabel = new UILabel();
	application->experimentBackLabel->setFont(application->menuFont);
	application->experimentBackLabel->setText(backString);
	application->experimentBackLabel->setTintColor(application->deselectedColor);
	application->experimentBackLabel->setTranslation(Vector2(0.0f, application->menuFont->getMetrics().getHeight() * 2));
	application->experimentMenuContainer->addChild(application->experimentBackLabel);
	
	application->settingsMenuContainer = new UIContainer();
	application->settingsMenuContainer->setDimensions(Vector2(application->width, application->menuFont->getMetrics().getHeight() * 5));
	application->settingsMenuContainer->setAnchor(Vector2(0.0f, 0.5f));
	application->settingsMenuContainer->setVisible(false);
	application->settingsMenuContainer->setActive(false);
	application->uiRootElement->addChild(application->settingsMenuContainer);
	application->videoLabel = new UILabel();
	application->videoLabel->setFont(application->menuFont);
	application->videoLabel->setText(videoString);
	application->videoLabel->setTintColor(application->deselectedColor);
	application->videoLabel->setTranslation(Vector2(0.0f, application->menuFont->getMetrics().getHeight() * 0));
	application->settingsMenuContainer->addChild(application->videoLabel);
	application->audioLabel = new UILabel();
	application->audioLabel->setFont(application->menuFont);
	application->audioLabel->setText(audioString);
	application->audioLabel->setTintColor(application->deselectedColor);
	application->audioLabel->setTranslation(Vector2(0.0f, application->menuFont->getMetrics().getHeight() * 1));
	application->settingsMenuContainer->addChild(application->audioLabel);
	application->controlsLabel = new UILabel();
	application->controlsLabel->setFont(application->menuFont);
	application->controlsLabel->setText(controlsString);
	application->controlsLabel->setTintColor(application->deselectedColor);
	application->controlsLabel->setTranslation(Vector2(0.0f, application->menuFont->getMetrics().getHeight() * 2));
	application->settingsMenuContainer->addChild(application->controlsLabel);
	application->gameLabel = new UILabel();
	application->gameLabel->setFont(application->menuFont);
	application->gameLabel->setText(gameString);
	application->gameLabel->setTintColor(application->deselectedColor);
	application->gameLabel->setTranslation(Vector2(0.0f, application->menuFont->getMetrics().getHeight() * 3));
	application->settingsMenuContainer->addChild(application->gameLabel);
	application->settingsBackLabel = new UILabel();
	application->settingsBackLabel->setFont(application->menuFont);
	application->settingsBackLabel->setText(backString);
	application->settingsBackLabel->setTintColor(application->deselectedColor);
	application->settingsBackLabel->setTranslation(Vector2(0.0f, application->menuFont->getMetrics().getHeight() * 4));
	application->settingsMenuContainer->addChild(application->settingsBackLabel);
	
	application->pauseMenuContainer = new UIContainer();
	application->pauseMenuContainer->setDimensions(Vector2(application->width, application->menuFont->getMetrics().getHeight() * 6));
	application->pauseMenuContainer->setAnchor(Anchor::CENTER);
	application->pauseMenuContainer->setVisible(false);
	application->pauseMenuContainer->setActive(false);
	application->uiRootElement->addChild(application->pauseMenuContainer);
	application->pausedResumeLabel = new UILabel();
	application->pausedResumeLabel->setFont(application->menuFont);
	application->pausedResumeLabel->setText(resumeString);
	application->pausedResumeLabel->setTintColor(application->deselectedColor);
	application->pausedResumeLabel->setTranslation(Vector2(0.0f, application->menuFont->getMetrics().getHeight() * 0));
	application->pauseMenuContainer->addChild(application->pausedResumeLabel);
	application->returnToMainMenuLabel = new UILabel();
	application->returnToMainMenuLabel->setFont(application->menuFont);
	application->returnToMainMenuLabel->setText(returnToMainMenuString);
	application->returnToMainMenuLabel->setTintColor(application->deselectedColor);
	application->returnToMainMenuLabel->setTranslation(Vector2(0.0f, application->menuFont->getMetrics().getHeight() * 1));
	application->pauseMenuContainer->addChild(application->returnToMainMenuLabel);
	application->quitToDesktopLabel = new UILabel();
	application->quitToDesktopLabel->setFont(application->menuFont);
	application->quitToDesktopLabel->setText(quitToDesktopString);
	application->quitToDesktopLabel->setTintColor(application->deselectedColor);
	application->quitToDesktopLabel->setTranslation(Vector2(0.0f, application->menuFont->getMetrics().getHeight() * 2));
	application->pauseMenuContainer->addChild(application->quitToDesktopLabel);
	
	/*
	UIContainer* pauseMenuContainer;
	UILabel* pausedResumeLabel;
	UILabel* pausedSaveLabel;
	UILabel* pausedNewLabel;
	UILabel* pausedSettingsLabel;
	UILabel* returnToMainMenuLabel;
	UILabel* quitToDesktopLabel;*/
	
	// Setup UI batch
	application->uiBatch = new BillboardBatch();
	application->uiBatch->resize(256);
	application->uiBatcher = new UIBatcher();
	
	// Setup UI render pass and compositor
	application->uiPass.setRenderTarget(&application->defaultRenderTarget);
	application->uiCompositor.addPass(&application->uiPass);
	application->uiCompositor.load(nullptr);
	
	// Setup UI camera
	application->uiCamera.lookAt(glm::vec3(0), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));
	application->uiCamera.setCompositor(&application->uiCompositor);
	application->uiCamera.setCompositeIndex(0);
	
	// Setup UI scene
	application->uiScene.getLayer(0)->addObject(application->uiBatch);
	application->uiScene.getLayer(0)->addObject(&application->uiCamera);
	
	// Setup tweening
	application->tweener = new Tweener();
	application->fadeInTween = new Tween<Vector4>(EaseFunction::IN_CUBIC, 0.0f, 1.5f, Vector4(0.0f, 0.0f, 0.0f, 1.0f), Vector4(0.0f, 0.0f, 0.0f, -1.0f));
	application->fadeInTween->setUpdateCallback(std::bind(UIElement::setTintColor, application->blackoutImage, std::placeholders::_1));
	application->tweener->addTween(application->fadeInTween);
	application->fadeOutTween = new Tween<Vector4>(EaseFunction::OUT_CUBIC, 0.0f, 1.5f, Vector4(0.0f, 0.0f, 0.0f, 0.0f), Vector4(0.0f, 0.0f, 0.0f, 1.0f));
	application->fadeOutTween->setUpdateCallback(std::bind(UIElement::setTintColor, application->blackoutImage, std::placeholders::_1));
	application->tweener->addTween(application->fadeOutTween);
	
	application->splashFadeInTween = new Tween<Vector4>(EaseFunction::IN_CUBIC, 0.0f, 0.5f, Vector4(1.0f, 1.0f, 1.0f, 0.0f), Vector4(0.0f, 0.0f, 0.0f, 1.0f));
	application->splashFadeInTween->setUpdateCallback(std::bind(UIElement::setTintColor, application->splashImage, std::placeholders::_1));
	application->tweener->addTween(application->splashFadeInTween);
	application->splashFadeOutTween = new Tween<Vector4>(EaseFunction::OUT_CUBIC, 0.0f, 0.5f, Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector4(0.0f, 0.0f, 0.0f, -1.0f));
	application->splashFadeOutTween->setUpdateCallback(std::bind(UIElement::setTintColor, application->splashImage, std::placeholders::_1));
	application->tweener->addTween(application->splashFadeOutTween);	
	
	application->titleFadeInTween = new Tween<Vector4>(EaseFunction::IN_CUBIC, 0.0f, 2.0f, Vector4(1.0f, 1.0f, 1.0f, 0.0f), Vector4(0.0f, 0.0f, 0.0f, 1.0f));
	application->titleFadeInTween->setUpdateCallback(std::bind(UIElement::setTintColor, application->titleImage, std::placeholders::_1));
	application->tweener->addTween(application->titleFadeInTween);
	application->titleFadeOutTween = new Tween<Vector4>(EaseFunction::OUT_CUBIC, 0.0f, 0.25f, Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector4(0.0f, 0.0f, 0.0f, -1.0f));
	application->titleFadeOutTween->setUpdateCallback(std::bind(UIElement::setTintColor, application->titleImage, std::placeholders::_1));
	application->tweener->addTween(application->titleFadeOutTween);
	
	application->copyrightFadeInTween = new Tween<Vector4>(EaseFunction::IN_CUBIC, 0.0f, 1.0f, Vector4(1.0f, 1.0f, 1.0f, 0.0f), Vector4(0.0f, 0.0f, 0.0f, 1.0f));
	application->copyrightFadeInTween->setUpdateCallback(std::bind(UIElement::setTintColor, application->copyrightImage, std::placeholders::_1));
	application->tweener->addTween(application->copyrightFadeInTween);
	application->copyrightFadeOutTween = new Tween<Vector4>(EaseFunction::OUT_CUBIC, 0.0f, 0.25f, Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector4(0.0f, 0.0f, 0.0f, -1.0f));
	application->copyrightFadeOutTween->setUpdateCallback(std::bind(UIElement::setTintColor, application->copyrightImage, std::placeholders::_1));
	application->tweener->addTween(application->copyrightFadeOutTween);
	
	application->anyKeyFadeInTween = new Tween<Vector4>(EaseFunction::LINEAR, 0.0f, 1.5f, Vector4(0.0f, 0.0f, 0.0f, 0.0f), Vector4(0.0f, 0.0f, 0.0f, 1.0f));
	application->anyKeyFadeInTween->setUpdateCallback(std::bind(UIElement::setTintColor, application->anyKeyLabel, std::placeholders::_1));
	application->tweener->addTween(application->anyKeyFadeInTween);
	application->anyKeyFadeOutTween = new Tween<Vector4>(EaseFunction::LINEAR, 0.0f, 1.5f, Vector4(0.0f, 0.0f, 0.0f, 1.0f), Vector4(0.0f, 0.0f, 0.0f, -1.0f));
	application->anyKeyFadeOutTween->setUpdateCallback(std::bind(UIElement::setTintColor, application->anyKeyLabel, std::placeholders::_1));
	application->tweener->addTween(application->anyKeyFadeOutTween);
	
	
	float menuFadeInDuration = 0.15f;
	Vector4 menuFadeInStartColor = Vector4(1.0f, 1.0f, 1.0f, 0.0f);
	Vector4 menuFadeInDeltaColor = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
	float menuFadeOutDuration = 0.15f;
	Vector4 menuFadeOutStartColor = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	Vector4 menuFadeOutDeltaColor = Vector4(0.0f, 0.0f, 0.0f, -1.0f);
	float menuSlideInDuration = 0.35f;
	Vector2 menuSlideInStartTranslation = Vector2(-64.0f, 0.0f);
	Vector2 menuSlideInDeltaTranslation = Vector2(128.0f, 0.0f);
	
	application->menuFadeInTween = new Tween<Vector4>(EaseFunction::OUT_QUINT, 0.0f, menuFadeInDuration, menuFadeInStartColor, menuFadeInDeltaColor);
	application->tweener->addTween(application->menuFadeInTween);
	application->menuFadeOutTween = new Tween<Vector4>(EaseFunction::OUT_QUINT, 0.0f, menuFadeOutDuration, menuFadeOutStartColor, menuFadeOutDeltaColor);
	application->tweener->addTween(application->menuFadeOutTween);
	application->menuSlideInTween = new Tween<Vector2>(EaseFunction::OUT_QUINT, 0.0f, menuSlideInDuration, menuSlideInStartTranslation, menuSlideInDeltaTranslation);
	application->tweener->addTween(application->menuSlideInTween);
	
	// Link tweens
	application->anyKeyFadeInTween->setEndCallback(std::bind(TweenBase::start, application->anyKeyFadeOutTween));
	application->anyKeyFadeOutTween->setEndCallback(std::bind(TweenBase::start, application->anyKeyFadeInTween));
	
	// Menus
	application->selectedMenuItemIndex = 0;
	application->mainMenu = new Menu();
	MenuItem* challengeItem = application->mainMenu->addItem();
	challengeItem->setSelectedCallback(std::bind(UIElement::setTintColor, application->challengeLabel, application->selectedColor));
	challengeItem->setDeselectedCallback(std::bind(UIElement::setTintColor, application->challengeLabel, application->deselectedColor));
	challengeItem->setActivatedCallback(std::bind(std::printf, "0\n"));
	application->challengeLabel->setMouseOverCallback(std::bind(Application::selectMenuItem, application, challengeItem->getIndex()));
	application->challengeLabel->setMouseMovedCallback(std::bind(Application::selectMenuItem, application, challengeItem->getIndex()));
	application->challengeLabel->setMousePressedCallback(std::bind(Application::activateMenuItem, application, challengeItem->getIndex()));
	MenuItem* experimentItem = application->mainMenu->addItem();
	experimentItem->setSelectedCallback(std::bind(UIElement::setTintColor, application->experimentLabel, application->selectedColor));
	experimentItem->setDeselectedCallback(std::bind(UIElement::setTintColor, application->experimentLabel, application->deselectedColor));
	experimentItem->setActivatedCallback(std::bind(Application::enterMenu, application, 2));
	application->experimentLabel->setMouseOverCallback(std::bind(Application::selectMenuItem, application, experimentItem->getIndex()));
	application->experimentLabel->setMouseMovedCallback(std::bind(Application::selectMenuItem, application, experimentItem->getIndex()));
	application->experimentLabel->setMousePressedCallback(std::bind(Application::activateMenuItem, application, experimentItem->getIndex()));
	MenuItem* settingsItem = application->mainMenu->addItem();
	settingsItem->setSelectedCallback(std::bind(UIElement::setTintColor, application->settingsLabel, application->selectedColor));
	settingsItem->setDeselectedCallback(std::bind(UIElement::setTintColor, application->settingsLabel, application->deselectedColor));
	settingsItem->setActivatedCallback(std::bind(Application::enterMenu, application, 3));
	application->settingsLabel->setMouseOverCallback(std::bind(Application::selectMenuItem, application, settingsItem->getIndex()));
	application->settingsLabel->setMouseMovedCallback(std::bind(Application::selectMenuItem, application, settingsItem->getIndex()));
	application->settingsLabel->setMousePressedCallback(std::bind(Application::activateMenuItem, application, settingsItem->getIndex()));
	MenuItem* quitItem = application->mainMenu->addItem();
	quitItem->setSelectedCallback(std::bind(UIElement::setTintColor, application->quitLabel, application->selectedColor));
	quitItem->setDeselectedCallback(std::bind(UIElement::setTintColor, application->quitLabel, application->deselectedColor));
	quitItem->setActivatedCallback(std::bind(Application::close, application, EXIT_SUCCESS));
	application->quitLabel->setMouseOverCallback(std::bind(Application::selectMenuItem, application, quitItem->getIndex()));
	application->quitLabel->setMouseMovedCallback(std::bind(Application::selectMenuItem, application, quitItem->getIndex()));
	application->quitLabel->setMousePressedCallback(std::bind(Application::activateMenuItem, application, quitItem->getIndex()));
	
	application->experimentMenu = new Menu();
	MenuItem* loadItem = application->experimentMenu->addItem();
	loadItem->setSelectedCallback(std::bind(UIElement::setTintColor, application->loadLabel, application->selectedColor));
	loadItem->setDeselectedCallback(std::bind(UIElement::setTintColor, application->loadLabel, application->deselectedColor));
	loadItem->setActivatedCallback(std::bind(std::printf, "0\n"));
	application->loadLabel->setMouseOverCallback(std::bind(Application::selectMenuItem, application, loadItem->getIndex()));
	application->loadLabel->setMouseMovedCallback(std::bind(Application::selectMenuItem, application, loadItem->getIndex()));
	application->loadLabel->setMousePressedCallback(std::bind(Application::activateMenuItem, application, loadItem->getIndex()));
	MenuItem* newItem = application->experimentMenu->addItem();
	newItem->setSelectedCallback(std::bind(UIElement::setTintColor, application->newLabel, application->selectedColor));
	newItem->setDeselectedCallback(std::bind(UIElement::setTintColor, application->newLabel, application->deselectedColor));
	newItem->setActivatedCallback(std::bind(Application::changeState, application, application->experimentState));
	application->newLabel->setMouseOverCallback(std::bind(Application::selectMenuItem, application, newItem->getIndex()));
	application->newLabel->setMouseMovedCallback(std::bind(Application::selectMenuItem, application, newItem->getIndex()));
	application->newLabel->setMousePressedCallback(std::bind(Application::activateMenuItem, application, newItem->getIndex()));
	MenuItem* experimentBackItem = application->experimentMenu->addItem();
	experimentBackItem->setSelectedCallback(std::bind(UIElement::setTintColor, application->experimentBackLabel, application->selectedColor));
	experimentBackItem->setDeselectedCallback(std::bind(UIElement::setTintColor, application->experimentBackLabel, application->deselectedColor));
	experimentBackItem->setActivatedCallback(std::bind(Application::enterMenu, application, 0));
	application->experimentBackLabel->setMouseOverCallback(std::bind(Application::selectMenuItem, application, experimentBackItem->getIndex()));
	application->experimentBackLabel->setMouseMovedCallback(std::bind(Application::selectMenuItem, application, experimentBackItem->getIndex()));
	application->experimentBackLabel->setMousePressedCallback(std::bind(Application::activateMenuItem, application, experimentBackItem->getIndex()));
	
	application->settingsMenu = new Menu();
	MenuItem* videoItem = application->settingsMenu->addItem();
	videoItem->setSelectedCallback(std::bind(UIElement::setTintColor, application->videoLabel, application->selectedColor));
	videoItem->setDeselectedCallback(std::bind(UIElement::setTintColor, application->videoLabel, application->deselectedColor));
	videoItem->setActivatedCallback(std::bind(std::printf, "0\n"));
	application->videoLabel->setMouseOverCallback(std::bind(Application::selectMenuItem, application, videoItem->getIndex()));
	application->videoLabel->setMouseMovedCallback(std::bind(Application::selectMenuItem, application, videoItem->getIndex()));
	application->videoLabel->setMousePressedCallback(std::bind(Application::activateMenuItem, application, videoItem->getIndex()));
	MenuItem* audioItem = application->settingsMenu->addItem();
	audioItem->setSelectedCallback(std::bind(UIElement::setTintColor, application->audioLabel, application->selectedColor));
	audioItem->setDeselectedCallback(std::bind(UIElement::setTintColor, application->audioLabel, application->deselectedColor));
	audioItem->setActivatedCallback(std::bind(std::printf, "1\n"));
	application->audioLabel->setMouseOverCallback(std::bind(Application::selectMenuItem, application, audioItem->getIndex()));
	application->audioLabel->setMouseMovedCallback(std::bind(Application::selectMenuItem, application, audioItem->getIndex()));
	application->audioLabel->setMousePressedCallback(std::bind(Application::activateMenuItem, application, audioItem->getIndex()));
	MenuItem* controlsItem = application->settingsMenu->addItem();
	controlsItem->setSelectedCallback(std::bind(UIElement::setTintColor, application->controlsLabel, application->selectedColor));
	controlsItem->setDeselectedCallback(std::bind(UIElement::setTintColor, application->controlsLabel, application->deselectedColor));
	controlsItem->setActivatedCallback(std::bind(std::printf, "2\n"));
	application->controlsLabel->setMouseOverCallback(std::bind(Application::selectMenuItem, application, controlsItem->getIndex()));
	application->controlsLabel->setMouseMovedCallback(std::bind(Application::selectMenuItem, application, controlsItem->getIndex()));
	application->controlsLabel->setMousePressedCallback(std::bind(Application::activateMenuItem, application, controlsItem->getIndex()));
	MenuItem* gameItem = application->settingsMenu->addItem();
	gameItem->setSelectedCallback(std::bind(UIElement::setTintColor, application->gameLabel, application->selectedColor));
	gameItem->setDeselectedCallback(std::bind(UIElement::setTintColor, application->gameLabel, application->deselectedColor));
	gameItem->setActivatedCallback(std::bind(std::printf, "3\n"));
	application->gameLabel->setMouseOverCallback(std::bind(Application::selectMenuItem, application, gameItem->getIndex()));
	application->gameLabel->setMouseMovedCallback(std::bind(Application::selectMenuItem, application, gameItem->getIndex()));
	application->gameLabel->setMousePressedCallback(std::bind(Application::activateMenuItem, application, gameItem->getIndex()));
	MenuItem* settingsBackItem = application->settingsMenu->addItem();
	settingsBackItem->setSelectedCallback(std::bind(UIElement::setTintColor, application->settingsBackLabel, application->selectedColor));
	settingsBackItem->setDeselectedCallback(std::bind(UIElement::setTintColor, application->settingsBackLabel, application->deselectedColor));
	settingsBackItem->setActivatedCallback(std::bind(Application::enterMenu, application, 0));
	application->settingsBackLabel->setMouseOverCallback(std::bind(Application::selectMenuItem, application, settingsBackItem->getIndex()));
	application->settingsBackLabel->setMouseMovedCallback(std::bind(Application::selectMenuItem, application, settingsBackItem->getIndex()));
	application->settingsBackLabel->setMousePressedCallback(std::bind(Application::activateMenuItem, application, settingsBackItem->getIndex()));
	
	
	application->menuCount = 4;
	application->menus = new Menu*[application->menuCount];
	application->menus[0] = application->mainMenu;
	application->menus[1] = application->challengeMenu;
	application->menus[2] = application->experimentMenu;
	application->menus[3] = application->settingsMenu;
	
	application->menuContainers = new UIContainer*[application->menuCount];
	application->menuContainers[0] = application->mainMenuContainer;
	application->menuContainers[1] = application->challengeMenuContainer;
	application->menuContainers[2] = application->experimentMenuContainer;
	application->menuContainers[3] = application->settingsMenuContainer;
	
	application->currentMenu = application->mainMenu;
	application->currentMenuIndex = 0;
	application->selectedMenuItemIndex = 0;
	application->selectMenuItem(application->selectedMenuItemIndex);
	
	// Models
	application->antModel = application->modelLoader->load("data/models/debug-worker.mdl");
	
	// Model instances
	application->antModelInstance = new ModelInstance();
	
	// Allocate game variables
	application->surfaceCam = new SurfaceCameraController();
	application->tunnelCam = new TunnelCameraController();
	
	// Load biosphere
	application->biosphere.load("data/biomes/");
	
	// Load campaign
	application->campaign.load("data/levels/");
	
	// Setup screen fade-in transition
	fadeIn = false;
	fadeOut = false;
	
	// Check for splash screen skip setting
	skip = false;
	application->settings.get("skip_splash", &skip);
	
	// Add window observer and set layout
	application->inputManager->addWindowObserver(this);
	windowResized(application->width, application->height);
	
	// Start timer
	stateTime = 0.0f;
	application->frameTimer.reset();
	application->frameTimer.start();
}

void SplashState::execute()
{
	// Calculate delta time (in seconds)
	float dt = static_cast<float>(application->frameTimer.microseconds().count()) / 1000000.0f;
	application->frameTimer.reset();
	
	// Add dt to state time
	stateTime += dt;
	
	// Listen for splash screen skip
	InputEvent event;
	application->inputManager->listen(&event);
	if (skip || event.type != InputEvent::Type::NONE)
	{
		application->menuControlProfile->update();
		application->inputManager->update();
		
		// Check if application was closed
		if (application->escape.isTriggered())
		{
			application->close(EXIT_SUCCESS);
			return;
		}
		// Check if fullscreen was toggled
		else if (application->toggleFullscreen.isTriggered() && !application->toggleFullscreen.wasTriggered())
		{
			application->changeFullscreen();
		}
		else
		{
			// Clear screen
			glClear(GL_COLOR_BUFFER_BIT);
			SDL_GL_SwapWindow(application->window);
			
			// Change to title state
			application->changeState(application->titleState);
			return;
		}
	}
	
	// Start fade-in
	if (!fadeIn && stateTime >= blankDuration)
	{
		// Begin fade-in
		fadeIn = true;
		application->splashImage->setVisible(true);
		application->splashFadeInTween->start();
	}
	
	// Begin fade-out
	if (!fadeOut && stateTime >= blankDuration + application->splashFadeInTween->getDuration() + hangDuration)
	{
		fadeOut = true;
		application->splashFadeOutTween->start();
	}
	
	// Next state
	if (fadeOut && application->splashFadeOutTween->isStopped())
	{
		application->splashImage->setVisible(false);
		application->changeState(application->titleState);
		return;
	}
	
	// Update input
	application->inputManager->update();
	
	// Update menu controls
	application->menuControlProfile->update();
	
	// Check if application was closed
	if (application->inputManager->wasClosed() || application->escape.isTriggered())
	{
		application->close(EXIT_SUCCESS);
		return;
	}
	
	// Perform tweening
	application->tweener->update(dt);
	
	// Update UI
	application->uiRootElement->update();
	
	// Clear to black
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	
	// Form billboard batch for UI then render UI scene
	application->uiBatcher->batch(application->uiBatch, application->uiRootElement);
	application->renderer.render(application->uiScene);
	
	// Swap buffers
	SDL_GL_SwapWindow(application->window);
}

void SplashState::exit()
{
	std::cout << "Exiting SplashState..." << std::endl;
	
	// Hide splash screen
	application->splashImage->setVisible(false);
	
	application->inputManager->removeWindowObserver(this);
}

void SplashState::windowClosed()
{
	application->close(EXIT_SUCCESS);
}

void SplashState::windowResized(int width, int height)
{
	// Update application dimensions
	application->width = width;
	application->height = height;
	if (application->fullscreen)
	{
		application->fullscreenWidth = width;
		application->fullscreenHeight = height;
	}
	else
	{
		application->windowedWidth = width;
		application->windowedHeight = height;
	}
	
	// Setup default render target
	application->defaultRenderTarget.width = application->width;
	application->defaultRenderTarget.height = application->height;
	
	// Resize UI
	application->resizeUI();
}