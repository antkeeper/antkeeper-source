#ifndef PIE_MENU_HPP
#define PIE_MENU_HPP

#include "ui.hpp"
#include "tween.hpp"
#include <functional>
#include <vector>

#include <emergent/emergent.hpp>
using namespace Emergent;

class PieMenu
{
public:
	PieMenu(Tweener* tweener);
		
	void resize();
	
	void addOption(Texture* backgroundTexture, Texture* iconTexture, std::function<void()> selectedCallback, std::function<void()> deselectedCallback);
	
	void select(std::size_t index);
	void deselect(std::size_t index);
	
	const UIContainer* getContainer() const;
	UIContainer* getContainer();	
	void mouseMoved(int x, int y);
	void mouseButtonPressed(int button, int x, int y);
	void mouseButtonReleased(int button, int x, int y);
	
	void setScale(float scale);
	
private:
	void highlight(std::size_t index);
	void unhighlight(std::size_t index);
	
	Tweener* tweener;
	Tween<float>* scaleUpTween;
	Tween<float>* scaleDownTween;
	float scale;
	
	UIContainer fullscreenContainer;
	UIContainer croppedContainer;
	UIContainer scalingContainer;
	std::vector<UIImage*> options;
	std::vector<UIImage*> icons;
	std::vector<std::function<void()>> selectedCallbacks;
	std::vector<std::function<void()>> deselectedCallbacks;
	std::size_t selectionIndex;
	
	bool dragging;
	Vector2 dragStart;
	std::size_t highlightedIndex;
};

inline const UIContainer* PieMenu::getContainer() const
{
	return &fullscreenContainer;
}

inline UIContainer* PieMenu::getContainer()
{
	return &fullscreenContainer;
}

#endif // PIE_MENU_HPP
