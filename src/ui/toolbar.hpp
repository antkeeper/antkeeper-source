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
	
	void setToolbarTopTexture(Texture* texture);
	void setToolbarBottomTexture(Texture* texture);
	void setToolbarMiddleTexture(Texture* texture);
	void setButtonRaisedTexture(Texture* texture);
	void setButtonDepressedTexture(Texture* texture);
	
	void resize();
	
	void addButton(Texture* iconTexture, std::function<void()> pressCallback, std::function<void()> releaseCallback);
	
	void pressButton(std::size_t index);
	void releaseButton(std::size_t index);
	
	const UIContainer* getContainer() const;
	UIContainer* getContainer();
	
private:
	Texture* toolbarTopTexture;
	Texture* toolbarBottomTexture;
	Texture* toolbarMiddleTexture;
	Texture* buttonRaisedTexture;
	Texture* buttonDepressedTexture;
	
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
