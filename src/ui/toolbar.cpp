#include "toolbar.hpp"
#include <algorithm>

Toolbar::Toolbar():
	toolbarTopTexture(nullptr),
	toolbarBottomTexture(nullptr),
	toolbarMiddleTexture(nullptr),
	buttonRaisedTexture(nullptr),
	buttonDepressedTexture(nullptr),
	depressedButtonIndex(0)
{
	toolbarContainer.addChild(&toolbarTopImage);
	toolbarContainer.addChild(&toolbarBottomImage);
	toolbarContainer.addChild(&toolbarMiddleImage);
}

void Toolbar::setToolbarTopTexture(Texture* texture)
{
	toolbarTopTexture = texture;
	toolbarTopImage.setTexture(toolbarTopTexture);
}

void Toolbar::setToolbarBottomTexture(Texture* texture)
{
	toolbarBottomTexture = texture;
	toolbarBottomImage.setTexture(toolbarBottomTexture);
}

void Toolbar::setToolbarMiddleTexture(Texture* texture)
{
	toolbarMiddleTexture = texture;
	toolbarMiddleImage.setTexture(toolbarMiddleTexture);
}

void Toolbar::setButtonRaisedTexture(Texture* texture)
{
	buttonRaisedTexture = texture;
}

void Toolbar::setButtonDepressedTexture(Texture* texture)
{
	buttonDepressedTexture = texture;
}

void Toolbar::resize()
{
	int toolbarWidth = toolbarMiddleTexture->getWidth();
	int toolbarHeight = toolbarTopTexture->getHeight() + toolbarBottomTexture->getHeight() + toolbarMiddleTexture->getHeight() * std::max<int>(0, (int)buttons.size() - 1);
	float borderSpacing = 8.0f;
	float buttonOffsetY = ((toolbarTopTexture->getHeight() + toolbarBottomTexture->getHeight()) - buttonRaisedTexture->getHeight()) / 2;
	
	// Resize toolbar
	toolbarContainer.setAnchor(Vector2(0.0f, 0.5f));
	toolbarContainer.setDimensions(Vector2(toolbarWidth, toolbarHeight));
	toolbarContainer.setTranslation(Vector2(borderSpacing, 0.0f));
	
	toolbarTopImage.setAnchor(Vector2(0.0f, 0.0f));
	toolbarTopImage.setDimensions(Vector2(toolbarTopTexture->getWidth(), toolbarTopTexture->getHeight()));
	toolbarTopImage.setTranslation(Vector2(0.0f, 0.0f));
	
	toolbarBottomImage.setAnchor(Vector2(0.0f, 1.0f));
	toolbarBottomImage.setDimensions(Vector2(toolbarBottomTexture->getWidth(), toolbarBottomTexture->getHeight()));
	toolbarBottomImage.setTranslation(Vector2(0.0f, 0.0f));
	
	toolbarMiddleImage.setAnchor(Vector2(0.0f, 0.5f));
	toolbarMiddleImage.setDimensions(Vector2(toolbarMiddleTexture->getWidth(), toolbarMiddleTexture->getHeight() * std::max<int>(0, (int)buttons.size() - 1)));
	toolbarMiddleImage.setTranslation(Vector2(0.0f, 0.0f));
	
	// Resize buttons and icons
	for (std::size_t i = 0; i < buttons.size(); ++i)
	{
		UIImage* button = buttons[i];
		button->setAnchor(Vector2(0.5f, 0.0f));
		button->setDimensions(Vector2(buttonRaisedTexture->getWidth(), buttonRaisedTexture->getHeight()));
		button->setTranslation(Vector2(0.0f, buttonOffsetY + i * toolbarMiddleTexture->getHeight()));
		
		UIImage* icon = icons[i];
		icon->setAnchor(Vector2(0.5f, 0.5f));
		icon->setDimensions(Vector2(icon->getTexture()->getWidth(), icon->getTexture()->getHeight()));
		icon->setTranslation(Vector2(0.0f, 0.0f));
	}
}

void Toolbar::addButton(Texture* iconTexture, std::function<void()> pressCallback, std::function<void()> releaseCallback)
{
	if (depressedButtonIndex == buttons.size())
	{
		++depressedButtonIndex;
	}
	
	// Allocate new button and icon
	UIImage* button = new UIImage();
	button->setTexture(buttonRaisedTexture);
	buttons.push_back(button);
	
	UIImage* icon = new UIImage();
	icon->setTexture(iconTexture);
	icon->setTintColor(Vector4(1.0f, 1.0f, 1.0f, 0.30f));
	icons.push_back(icon);
	
	// Add button to toolbar
	toolbarContainer.addChild(button);
	
	// Add icon to button
	button->addChild(icon);
	
	// Setup callbacks
	std::size_t buttonIndex = buttons.size() - 1;
	//button->setMouseOverCallback(std::bind(&Toolbar::selectMenuItem, this, buttonIndex));
	//button->setMouseMovedCallback(std::bind(&Toolbar::selectMenuItem, this, buttonIndex));
	button->setMousePressedCallback(std::bind(&Toolbar::pressButton, this, buttonIndex));
	
	pressCallbacks.push_back(pressCallback);
	releaseCallbacks.push_back(releaseCallback);
}

void Toolbar::pressButton(std::size_t index)
{
	releaseButton(depressedButtonIndex);
	
	if (index == depressedButtonIndex)
	{
		depressedButtonIndex = buttons.size();
	}
	else
	{
		depressedButtonIndex = index;
		buttons[index]->setTexture(buttonDepressedTexture);
		//icons[index]->setTranslation(Vector2(2.0f, 2.0f));
		icons[index]->setTintColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
		
		pressCallbacks[index]();
	}
}

void Toolbar::releaseButton(std::size_t index)
{
	if (index < buttons.size())
	{
		buttons[index]->setTexture(buttonRaisedTexture);
		//icons[index]->setTranslation(Vector2(0.0f, 0.0f));
		icons[index]->setTintColor(Vector4(1.0f, 1.0f, 1.0f, 0.30f));
		
		releaseCallbacks[index]();
	}
}
