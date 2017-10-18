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

#include "menu.hpp"
#include "ui.hpp"

#include <algorithm>

MenuItem::MenuItem(Menu* parent, std::size_t index):
	parent(parent),
	index(index),
	selectedCallback(nullptr),
	deselectedCallback(nullptr),
	activatedCallback(nullptr),
	valueChangedCallback(nullptr),
	valueIndex(0),
	nameLabel(nullptr),
	valueLabel(nullptr),
	rowContainer(nullptr)
	
{
	nameLabel = new UILabel();
	nameLabel->setAnchor(Vector2(0.0f, 0.0f));
	
	valueLabel = new UILabel();
	valueLabel->setAnchor(Vector2(1.0f, 0.0f));
	
	rowContainer = new UIContainer();
	rowContainer->addChild(nameLabel);
	rowContainer->addChild(valueLabel);
	
	rowContainer->setMouseOverCallback(std::bind(&Menu::select, parent, index));
	rowContainer->setMouseMovedCallback(std::bind(&Menu::select, parent, index));
	rowContainer->setMousePressedCallback(std::bind(&Menu::activate, parent));
}

MenuItem::~MenuItem()
{
	delete nameLabel;
	delete valueLabel;
	delete rowContainer;
}

void MenuItem::select()
{
	if (selectedCallback != nullptr)
	{
		selectedCallback();
	}
}

void MenuItem::deselect()
{
	if (deselectedCallback != nullptr)
	{
		deselectedCallback();
	}
}

void MenuItem::activate()
{
	if (activatedCallback != nullptr)
	{
		activatedCallback();
	}
}

void MenuItem::setSelectedCallback(std::function<void()> callback)
{
	this->selectedCallback = callback = callback;
}

void MenuItem::setDeselectedCallback(std::function<void()> callback)
{
	this->deselectedCallback = callback;
}

void MenuItem::setActivatedCallback(std::function<void()> callback)
{
	this->activatedCallback = callback;
}

void MenuItem::setValueChangedCallback(std::function<void(std::size_t)> callback)
{
	this->valueChangedCallback = callback;
}

void MenuItem::setName(const std::string& text)
{
	nameLabel->setText(text);
	parent->resize();
}

void MenuItem::addValue(const std::string& text)
{
	values.push_back(text);
	valueLabel->setText(values[valueIndex]);
	parent->resize();
}

void MenuItem::removeValues()
{
	values.clear();
	valueIndex = 0;
	valueLabel->setText(std::string());
	parent->resize();
}

void MenuItem::setValueIndex(std::size_t index)
{
	if (valueIndex != index)
	{
		valueIndex = index;
		valueLabel->setText(values[index]);
		
		if (valueChangedCallback != nullptr)
		{
			valueChangedCallback(index);
		}
	}
}

bool MenuItem::isSelected() const
{
	return (parent->getSelectedItem() == this);
}

Menu::Menu():
	selectedItem(nullptr),
	enteredCallback(nullptr),
	exitedCallback(nullptr),
	font(nullptr),
	lineSpacing(1.0f),
	columnMargin(0.0f)
{
	container = new UIContainer();
	resize();
}

Menu::~Menu()
{
	removeItems();
	delete container;
}

void Menu::enter()
{
	if (enteredCallback != nullptr)
	{
		enteredCallback();
	}
}

void Menu::exit()
{
	if (exitedCallback != nullptr)
	{
		exitedCallback();
	}
}

MenuItem* Menu::addItem()
{
	// Allocate item and add to items
	MenuItem* item = new MenuItem(this, items.size());
	items.push_back(item);
	
	// Set item fonts
	item->nameLabel->setFont(font);
	item->valueLabel->setFont(font);
	
	// Set item colors
	item->nameLabel->setTintColor(Vector4(1.0f, 1.0f, 1.0f, 0.35f));
	item->valueLabel->setTintColor(Vector4(1.0f, 1.0f, 1.0f, 0.35f));
	
	// Add item labels to UI container
	container->addChild(item->rowContainer);
	
	// Resize UI container
	resize();
	
	return item;
}

void Menu::removeItems()
{
	for (MenuItem* item: items)
	{
		// Remove labels from UI container
		container->removeChild(item->rowContainer);
		
		delete item;
	}
	items.clear();
	
	resize();
}

void Menu::setEnteredCallback(std::function<void()> callback)
{
	this->enteredCallback = callback;
}

void Menu::setExitedCallback(std::function<void()> callback)
{
	this->exitedCallback = callback;
}

void Menu::setFont(Font* font)
{
	this->font = font;
	for (MenuItem* item: items)
	{
		item->nameLabel->setFont(font);
		item->valueLabel->setFont(font);
	}
	
	resize();
}

void Menu::setLineSpacing(float spacing)
{
	lineSpacing = spacing;
	resize();
}

void Menu::setColumnMargin(float margin)
{
	columnMargin = margin;
	resize();
}

void Menu::deselect()
{
	if (selectedItem != nullptr)
	{
		selectedItem->deselect();
		
		selectedItem->nameLabel->setTintColor(Vector4(1.0f, 1.0f, 1.0f, 0.35f));
		selectedItem->valueLabel->setTintColor(Vector4(1.0f, 1.0f, 1.0f, 0.35f));
		
		selectedItem = nullptr;
	}
}

void Menu::select(std::size_t index)
{
	deselect();
	
	MenuItem* item = items[index];
	item->select();
	
	selectedItem = item;
	selectedItem->nameLabel->setTintColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	selectedItem->valueLabel->setTintColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
}

void Menu::activate()
{
	if (selectedItem != nullptr)
	{
		selectedItem->activate();
	}
}

void Menu::resize()
{
	if (!font)
	{
		container->setDimensions(Vector2(0.0f));
	}
	else
	{
		// Determine maximum width of menu
		float menuWidth = 0.0f;
		
		// For each menu item
		for (std::size_t i = 0; i < items.size(); ++i)
		{
			const MenuItem* item = items[i];
			
			// Calculate width of item name label
			float nameLabelWidth = item->nameLabel->getDimensions().x;
			
			// For each item value
			for (std::size_t j = 0; j < item->getValueCount(); ++j)
			{
				// Calculate width of item value label
				float valueLabelWidth = font->getWidth(item->getValue(j).c_str());
				
				menuWidth = std::max<float>(menuWidth, nameLabelWidth + columnMargin + valueLabelWidth);
			}
			menuWidth = std::max<float>(menuWidth, nameLabelWidth);
		}
		
		Vector2 dimensions(menuWidth, 0.0f);
		for (std::size_t i = 0; i < items.size(); ++i)
		{
			const MenuItem* item = items[i];
			
			float translationY = static_cast<float>(static_cast<int>(font->getMetrics().getHeight() * lineSpacing * static_cast<float>(i)));
			
			item->rowContainer->setDimensions(Vector2(menuWidth, font->getMetrics().getHeight()));
			item->rowContainer->setTranslation(Vector2(0.0f, translationY));
			
			if (!i)
			{
				dimensions.y += font->getMetrics().getHeight();
			}
			else
			{
				dimensions.y += font->getMetrics().getHeight() * lineSpacing;
			}
		}
		
		container->setDimensions(dimensions);
	}
}