#include "pie-menu.hpp"
#include <cmath>

PieMenu::PieMenu(Tweener* tweener):
	tweener(tweener),
	scaleUpTween(nullptr),
	scaleDownTween(nullptr),
	scale(1.0f),
	selectionIndex(0),
	dragging(false),
	dragStart(0.0f)
{	
	// Setup fullscreen container
	fullscreenContainer.addChild(&croppedContainer);
	fullscreenContainer.setMouseMovedCallback(std::bind(&PieMenu::mouseMoved, this, std::placeholders::_1, std::placeholders::_2));
	fullscreenContainer.setMousePressedCallback(std::bind(&PieMenu::mouseButtonPressed, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	fullscreenContainer.setMouseReleasedCallback(std::bind(&PieMenu::mouseButtonReleased, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	
	// Setup cropped container
	croppedContainer.addChild(&scalingContainer);
	
	// Setup scaling container
	scalingContainer.setAnchor(Vector2(0.5f));
	
	// Create tweens
	scaleUpTween = new Tween<float>(EaseFunction::OUT_SINE, 0.0f, 0.1f, 0.0f, 1.0f);
	scaleUpTween->setUpdateCallback(std::bind(&PieMenu::setScale, this, std::placeholders::_1));
	scaleDownTween = new Tween<float>(EaseFunction::IN_SINE, 0.0f, 0.1f, 1.0f, -1.0f);
	scaleDownTween->setUpdateCallback(std::bind(&PieMenu::setScale, this, std::placeholders::_1));
	
	// Add tweens
	tweener->addTween(scaleUpTween);
	tweener->addTween(scaleDownTween);
}

void PieMenu::resize()
{
	float iconDistance = 0.0f;
	
	if (fullscreenContainer.getParent() == nullptr)
	{
		return;
	}
	
	// Resize fullscreen container
	fullscreenContainer.setDimensions(fullscreenContainer.getParent()->getDimensions());
	
	// Resize cropped container
	croppedContainer.setDimensions(Vector2(options[0]->getTexture()->getWidth(), options[0]->getTexture()->getHeight()));
	
	// Place options
	for (std::size_t i = 0; i < options.size(); ++i)
	{
		float angle = glm::radians(360.0f) / static_cast<float>(i + 1) / static_cast<float>(options.size());
		
		options[i]->setAnchor(Vector2(0.5f, 0.5f));
		options[i]->setTranslation(Vector2(0.0f, 0.0f));
		
		icons[i]->setAnchor(Vector2(0.5f, 0.5f));
		icons[i]->setTranslation(Vector2(0.0f, 0.0f));
	}
}

void PieMenu::setScale(float scale)
{
	for (std::size_t i = 0; i < options.size(); ++i)
	{
		options[i]->setDimensions(Vector2(options[i]->getTexture()->getWidth(), options[i]->getTexture()->getHeight()) * scale);
		icons[i]->setDimensions(Vector2(icons[i]->getTexture()->getWidth(), icons[i]->getTexture()->getHeight()) * scale);
	}
}

void PieMenu::addOption(Texture2D* backgroundTexture, Texture2D* iconTexture, std::function<void()> selectedCallback, std::function<void()> deselectedCallback)
{
	// Allocate new option
	UIImage* option = new UIImage();
	option->setTexture(backgroundTexture);
	option->setDimensions(Vector2(backgroundTexture->getWidth(), backgroundTexture->getHeight()));
	option->setTintColor(Vector4(1.0f, 1.0f, 1.0f, 0.50f));
	options.push_back(option);
	
	UIImage* icon = new UIImage();
	icon->setTexture(iconTexture);
	icon->setDimensions(Vector2(iconTexture->getWidth(), iconTexture->getHeight()));
	icon->setTintColor(Vector4(1.0f, 1.0f, 1.0f, 0.50f));
	icons.push_back(icon);
	
	// Add icon to option
	scalingContainer.addChild(icon);
	
	// Add option to menu
	scalingContainer.addChild(option);
	
	// Setup callbacks
	selectedCallbacks.push_back(selectedCallback);
	deselectedCallbacks.push_back(deselectedCallback);
}

void PieMenu::select(std::size_t index)
{
	if (index != selectionIndex && selectionIndex < options.size())
	{
		deselect(selectionIndex);
	}
	
	selectionIndex = index;	
	selectedCallbacks[index]();
}

void PieMenu::deselect(std::size_t index)
{
	deselectedCallbacks[index]();
}

void PieMenu::highlight(std::size_t index)
{
	options[index]->setTintColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	icons[index]->setTintColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
}

void PieMenu::unhighlight(std::size_t index)
{
	options[index]->setTintColor(Vector4(1.0f, 1.0f, 1.0f, 0.50f));
	icons[index]->setTintColor(Vector4(1.0f, 1.0f, 1.0f, 0.50f));
}

void PieMenu::mouseMoved(int x, int y)
{
	if (dragging)
	{
		Vector2 direction = Vector2(x, y) - dragStart;
		if (direction.x != 0.0f || direction.y != 0.0f)
		{
			direction = glm::normalize(direction);
			
			// Calculate arc length
			float arcLength = glm::radians(360.0f) / static_cast<float>(options.size());
			
			// Calculate angle between cursor and pie menu
			float angle = std::atan2(direction.y, direction.x) + glm::radians(90.0f) + arcLength * 0.5f;
			while (angle < 0.0f) angle += glm::radians(360.0f);
			
			// Determine option index from angle
			std::size_t index = static_cast<std::size_t>(angle / arcLength);
			
			if (index != highlightedIndex)
			{
				if (highlightedIndex < options.size())
				{
					unhighlight(highlightedIndex);
				}
				
				highlight(index);
				highlightedIndex = index;
			}
		}
	}
}

void PieMenu::mouseButtonPressed(int button, int x, int y)
{
	if (button == 3)
	{
		// Start dragging
		dragging = true;
		dragStart.x = x;
		dragStart.y = y;
		
		// Set pie menu position
		Vector2 halfDimensions = croppedContainer.getDimensions() * 0.5f;
		croppedContainer.setTranslation(Vector2(x - halfDimensions.x, y - halfDimensions.y));
		
		// Clear highlights
		for (std::size_t i = 0; i < options.size(); ++i)
		{
			unhighlight(i);
		}
		
		// Reset highlighted index
		highlightedIndex = options.size();
		
		// Show pie menu
		fullscreenContainer.setVisible(true);
		
		// Start scale-up tween
		scaleDownTween->stop();
		scaleUpTween->start();
	}
}

void PieMenu::mouseButtonReleased(int button, int x, int y)
{
	if (button == 3)
	{
		// Stop dragging
		dragging = false;
		
		// Select highlighted index
		if (highlightedIndex != selectionIndex && highlightedIndex < options.size())
		{
			select(highlightedIndex);
		}
		
		// Clear highlights
		for (std::size_t i = 0; i < options.size(); ++i)
		{
			unhighlight(i);
		}
		
		// Reset highlighted index
		highlightedIndex = options.size();
		
		// Start scale-down tween
		scaleUpTween->stop();
		scaleDownTween->start();
		//fullscreenContainer.setVisible(false);
	}
}
