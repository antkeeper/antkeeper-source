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

#ifndef MENU_HPP
#define MENU_HPP

#include <emergent/emergent.hpp>
using namespace Emergent;

#include <functional>
#include <string>
#include <vector>

class Menu;
class UIContainer;
class UILabel;

class MenuItem
{
public:
	void setSelectedCallback(std::function<void()> callback);
	void setDeselectedCallback(std::function<void()> callback);
	void setActivatedCallback(std::function<void()> callback);
	void setValueChangedCallback(std::function<void(std::size_t)> callback);
	
	void setName(const std::u32string& text);
	std::size_t addValue();
	void removeValues();
	void setValueName(std::size_t index, const std::u32string& text);
	void setValueIndex(std::size_t index);
	
	std::size_t getValueCount() const;
	const std::u32string& getValue(std::size_t index) const;
	std::size_t getValueIndex() const;
	
	
	std::size_t getItemIndex() const;
	bool isSelected() const;

private:
	friend class Menu;
	
	MenuItem(Menu* parent, std::size_t index);
	~MenuItem();
	void select();
	void deselect();
	void activate();

	Menu* parent;
	std::size_t index;
	std::function<void()> selectedCallback;
	std::function<void()> deselectedCallback;
	std::function<void()> activatedCallback;
	std::function<void(std::size_t)> valueChangedCallback;
	std::vector<std::u32string> values;
	std::size_t valueIndex;
	UILabel* nameLabel;
	UILabel* valueLabel;
	UIContainer* rowContainer;
};

inline std::size_t MenuItem::getItemIndex() const
{
	return index;
}

inline std::size_t MenuItem::getValueCount() const
{
	return values.size();
}

inline const std::u32string& MenuItem::getValue(std::size_t index) const
{
	return values[index];
}

inline std::size_t MenuItem::getValueIndex() const
{
	return valueIndex;
}

class Menu
{
public:
	Menu();
	~Menu();
	
	void enter();
	void exit();
	
	MenuItem* addItem();
	void removeItems();
	
	void setEnteredCallback(std::function<void()> callback);
	void setExitedCallback(std::function<void()> callback);
	void setFont(Font* font);
	void setLineSpacing(float spacing);
	void setColumnMargin(float margin);
	
	std::size_t getItemCount();
	const MenuItem* getItem(std::size_t index) const;
	MenuItem* getItem(std::size_t index);
	const MenuItem* getSelectedItem() const;
	MenuItem* getSelectedItem();
	
	const UIContainer* getUIContainer() const;
	UIContainer* getUIContainer();
	
	/**
	 * Deselects the currently selected item (if any)
	 */
	void deselect();
	
	/**
	 * Selects the item at the specified index
	 */
	void select(std::size_t index);
	
	/**
	 * Activates the selected item (if any)
	 */
	void activate();
	
	/**
	 * Recalculates the dimensions of the UI container according the dimensions of the menu item labels and the line spacing.
	 */
	void resize();
	
private:
	friend class MenuItem;
	
	std::vector<MenuItem*> items;
	MenuItem* selectedItem;
	std::function<void()> enteredCallback;
	std::function<void()> exitedCallback;
	Font* font;
	float lineSpacing;
	float columnMargin;
	UIContainer* container;
};

inline std::size_t Menu::getItemCount()
{
	return items.size();
}

inline const MenuItem* Menu::getItem(std::size_t index) const
{
	return items[index];
}

inline MenuItem* Menu::getItem(std::size_t index)
{
	return items[index];
}

inline const MenuItem* Menu::getSelectedItem() const
{
	return selectedItem;
}

inline MenuItem* Menu::getSelectedItem()
{
	return selectedItem;
}

inline const UIContainer* Menu::getUIContainer() const
{
	return container;
}

inline UIContainer* Menu::getUIContainer()
{
	return container;
}

#endif // MENU_HPP