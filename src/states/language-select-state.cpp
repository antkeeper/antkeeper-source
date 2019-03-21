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

#include "game.hpp"
#include "debug/logger.hpp"
#include "ui/ui.hpp"
#include "resources/resource-manager.hpp"
#include "menu.hpp"

void Game::enterLanguageSelectState()
{
	// Set fonts for language menu items
	for (std::size_t i = 0; i < languageCount; ++i)
	{
		MenuItem* item = languageMenuItems[i];
		item->setFont(languageSelectionFonts[i]);
	}

	// Re-string and resie UI
	restringUI();
	resizeUI(w, h);

	// Set menu font color
	menuItemActiveColor = languageMenuActiveColor;
	menuItemInactiveColor = languageMenuInactiveColor;
	menuSelectorImage->setTintColor(menuItemActiveColor);

	// Add background image
	uiRootElement->addChild(languageSelectBGImage);

	// Hide menu item selector
	menuSelectorImage->setVisible(false);

	// Open language menu
	openMenu(languageMenu, 0);

	// Disable non-system controls
	disableNonSystemControls();

	// Disable UI callbacks
	uiRootElement->setCallbacksEnabled(false);

	// Begin fade-in
	fadeIn(0.5f, {0, 0, 0},
		std::bind
		(
			[this]()
			{
				// Enable menu controls and UI callbacks
				menuControls.setCallbacksEnabled(true);
				uiRootElement->setCallbacksEnabled(true);
			}
		));
}

void Game::exitLanguageSelectState()
{
	// Remove background image
	uiRootElement->removeChild(languageSelectBGImage);

	// Close language select menu
	closeCurrentMenu();

	// Show menu item selector
	menuSelectorImage->setVisible(false);

	// Set standard menu item colors
	menuItemActiveColor = standardMenuActiveColor;
	menuItemInactiveColor = standardMenuInactiveColor;
	menuSelectorImage->setTintColor(menuItemActiveColor);
}

