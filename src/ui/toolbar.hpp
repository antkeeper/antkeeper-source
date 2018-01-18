#ifndef TOOLBAR_HPP
#define TOOLBAR_HPP

#include "ui.hpp"
#include <functional>

#include <emergent/emergent.hpp>

using namespace Emergent;

class Toolbar
{
public:
	Toolbar();
	
	void setToolbarTopTexture(Texture2D* texture);
	void setToolbarBottomTexture(Texture2D* texture);
	void setToolbarMiddleTexture(Texture2D* texture);
	void setButtonRaisedTexture(Texture2D* texture);
	void setButtonDepressedTexture(Texture2D* texture);
	
	void resize();
	
	void addButton(Texture2D* iconTexture, std::function<void()> pressCallback, std::function<void()> releaseCallback);
	
	void pressButton(std::size_t index);
	void releaseButton(std::size_t index);
	
	const UIContainer* getContainer() const;
	UIContainer* getContainer();
	
private:
	Texture2D* toolbarTopTexture;
	Texture2D* toolbarBottomTexture;
	Texture2D* toolbarMiddleTexture;
	Texture2D* buttonRaisedTexture;
	Texture2D* buttonDepressedTexture;
	
	UIContainer toolbarContainer;
	UIImage toolbarTopImage;
	UIImage toolbarBottomImage;
	UIImage toolbarMiddleImage;
	std::vector<UIImage*> buttons;
	std::vector<UIImage*> icons;
	std::vector<std::function<void()>> pressCallbacks;
	std::vector<std::function<void()>> releaseCallbacks;

	std::size_t depressedButtonIndex;
};

inline const UIContainer* Toolbar::getContainer() const
{
	return &toolbarContainer;
}

inline UIContainer* Toolbar::getContainer()
{
	return &toolbarContainer;
}

#endif // TOOLBAR_HPP
