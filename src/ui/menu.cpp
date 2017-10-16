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
	activatedCallback(nullptr)
{
	label = new UILabel();
	
	label->setMouseOverCallback(std::bind(&Menu::select, parent, index));
	label->setMouseMovedCallback(std::bind(&Menu::select, parent, index));
	label->setMousePressedCallback(std::bind(&Menu::activate, parent));
}

MenuItem::~MenuItem()
{
	delete label;
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

void MenuItem::setLabel(const std::string& text)
{
	label->setText(text);
	parent->resize();
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
	lineSpacing(1.0f)
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
	
	// Set item label font
	item->label->setFont(font);
	item->label->setTintColor(Vector4(1.0f, 1.0f, 1.0f, 0.35f));
	
	// Add item label to UI container
	container->addChild(item->label);
	
	// Resize UI container
	resize();
	
	return item;
}

void Menu::removeItems()
{
	for (MenuItem* item: items)
	{
		// Remove label from UI container
		container->removeChild(item->label);
		
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
		item->label->setFont(font);
	}
	
	resize();
}

void Menu::setLineSpacing(float spacing)
{
	lineSpacing = spacing;
	resize();
}

void Menu::deselect()
{
	if (selectedItem != nullptr)
	{
		selectedItem->deselect();
		
		selectedItem->label->setTintColor(Vector4(1.0f, 1.0f, 1.0f, 0.35f));
		
		selectedItem = nullptr;
	}
}

void Menu::select(std::size_t index)
{
	deselect();
	
	MenuItem* item = items[index];
	item->select();
	
	selectedItem = item;
	selectedItem->label->setTintColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
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
		Vector2 dimensions(0.0f);
		for (std::size_t i = 0; i < items.size(); ++i)
		{
			const MenuItem* item = items[i];
			
			item->label->setTranslation(Vector2(0.0f, static_cast<int>(font->getMetrics().getHeight() * lineSpacing * static_cast<float>(i))));
			
			dimensions.x = std::max<float>(dimensions.x, item->label->getDimensions().x);
			
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