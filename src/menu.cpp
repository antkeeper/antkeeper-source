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

#include "menu.hpp"

MenuItem::MenuItem():
	activatedCallback(nullptr)
{
	container = new UIContainer();
	container->setAnchor(Anchor::TOP_LEFT);

	nameLabel = new UILabel();
	nameLabel->setAnchor(Anchor::TOP_LEFT);

	valueLabel = new UILabel();
	valueLabel->setAnchor(Anchor::TOP_RIGHT);

	container->addChild(nameLabel);
	container->addChild(valueLabel);
}

MenuItem::~MenuItem()
{
	delete container;
	delete nameLabel;
	delete valueLabel;
}

void MenuItem::setFont(Font* font)
{
	nameLabel->setFont(font);
	valueLabel->setFont(font);
}

void MenuItem::setName(const std::string& name)
{
	nameLabel->setText(name);
}

void MenuItem::setValue(const std::string& value)
{
	valueLabel->setText(value);
}

void MenuItem::setActivatedCallback(std::function<void()> callback)
{
	activatedCallback = callback;
}

void MenuItem::activate()
{
	if (activatedCallback)
	{
		activatedCallback();
	}
}

Menu::Menu()
{
	container = new UIContainer();
}

Menu::~Menu()
{
	removeItems();
	delete container;
}

MenuItem* Menu::addItem()
{
	MenuItem* item = new MenuItem();
	container->addChild(item->container);
	items.push_back(item);

	return item;
}

void Menu::removeItems()
{
	for (MenuItem* item: items)
	{
		container->removeChild(item->container);
		delete item;
	}
	items.clear();
}

void Menu::setFonts(Font* font)
{
	for (MenuItem* item: items)
	{
		item->setFont(font);
	}
}

void Menu::resize(int w, int h)
{
	container->setDimensions(Vector2(w, h));

	for (std::size_t i = 0; i < items.size(); ++i)
	{
		MenuItem* item = items[i];
		Font* font = item->getNameLabel()->getFont();

		if (font)
		{
			float lineHeight = font->getMetrics().getAscender() - font->getMetrics().getDescender();

			item->container->setTranslation(Vector2(0, 0));
			item->container->setAnchor(Vector2(0, static_cast<float>(i) / static_cast<float>(items.size() - 1)));
			item->container->setDimensions(Vector2(w, lineHeight));
		}
	}
}

