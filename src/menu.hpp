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

#ifndef MENU_HPP
#define MENU_HPP

#include <emergent/emergent.hpp>
using namespace Emergent;

#include "ui/ui.hpp"
#include <functional>

class Menu;

class MenuItem
{
public:
	MenuItem();
	~MenuItem();

	void setFont(Font* font);

	void setName(const std::string& name);

	void setValue(const std::string& value);

	void setActivatedCallback(std::function<void()> callback);

	void activate();

	UIContainer* getContainer() const;
	UILabel* getNameLabel() const;
	UILabel* getValueLabel() const;

private:
	friend class Menu;

	UIContainer* container;
	UILabel* nameLabel;
	UILabel* valueLabel;
	std::function<void()> activatedCallback;
};

inline UIContainer* MenuItem::getContainer() const
{
	return container;
}

inline UILabel* MenuItem::getNameLabel() const
{
	return nameLabel;
}

inline UILabel* MenuItem::getValueLabel() const
{
	return valueLabel;
}

class Menu
{
public:
	/// Creates a menu.
	Menu();

	/// Destroys a menu.
	~Menu();

	/**
	 * Adds a new item to the menu.
	 */
	MenuItem* addItem();

	/**
	 * Removes an item from the menu.
	 */
	void removeItem(MenuItem* item);

	/**
	 * Removes all items from the menu.
	 */
	void removeItems();

	/**
	 * Sets the font for all menu items in the menu.
	 */
	void setFonts(Font* font);

	const std::vector<MenuItem*>* getItems() const;

	UIContainer* getContainer() const;

	void resize(int w, int h);

private:

	UIContainer* container;
	std::vector<MenuItem*> items;
};

inline const std::vector<MenuItem*>* Menu::getItems() const
{
	return &items;
}

inline UIContainer* Menu::getContainer() const
{
	return container;
}

#endif // MENU_HPP

