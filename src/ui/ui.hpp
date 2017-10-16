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

#ifndef UI_HPP
#define UI_HPP

#include "../input.hpp"
#include "../materials.hpp"
#include <vector>
#include <functional>

#include <emergent/emergent.hpp>
using namespace Emergent;

namespace Anchor
{
	static const Vector2& TOP_LEFT = Vector2(0.0f, 0.0f);
	static const Vector2& TOP_RIGHT = Vector2(1.0f, 0.0f);
	static const Vector2& BOTTOM_LEFT = Vector2(0.0f, 1.0f);
	static const Vector2& BOTTOM_RIGHT = Vector2(1.0f, 1.0f);
	static const Vector2& CENTER = Vector2(0.5f, 0.5f);
}

class UIElement: public MouseMotionObserver, public MouseButtonObserver
{
public:
	enum class Type
	{
		CONTAINER,
		LABEL,
		IMAGE
	};
	
	UIElement();
	virtual ~UIElement();
	
	/// Sets the anchor vector. Possible values are 0, 0.5, and 1, corresponding to the left, center, and right, respectively, and the top, center, and bottom.
	void setAnchor(const Vector2& anchor);
	
	/// Sets the layer offset, relative to its parent layer + 1
	void setLayerOffset(int offset);
	
	/// Sets the local origin of the element
	void setOrigin(const Vector2& origin);
	
	/// Sets the translation of the element, relative to its position in its parent element
	void setTranslation(const Vector2& translation);
	
	/// Sets the rotation of the element
	void setRotation(float angle);
	
	/// Sets the dimensions of the element
	void setDimensions(const Vector2& dimensions);
	
	/// Sets the tint color of the element
	void setTintColor(const Vector4& color);
	
	/// Sets the visibility of the element
	void setVisible(bool visible);
	
	/// Enables or disables callbacks
	void setActive(bool active);
	
	/// Returns the type of this element
	virtual UIElement::Type getElementType() const = 0;
	
	/// Returns the material of this element
	const UIMaterial* getMaterial() const;
	
	/// @copydoc UIElement::getMaterial() const
	UIMaterial* getMaterial();
	
	/// Returns the parent of this element
	const UIElement* getParent() const;
	
	/// @copydoc UIElement::getParent() const
	UIElement* getParent();
	
	/// Returns the number of child elements
	std::size_t getChildCount() const;
	
	/// Returns the child element at the specified index
	const UIElement* getChild(std::size_t index) const;
	
	/// @copydoc UIElement::getChild() const
	UIElement* getChild(std::size_t index);
	
	/// Returns the anchor vector
	const Vector2& getAnchor() const;
	
	/// Returns the layer offset
	int getLayerOffset() const;
	
	/// Returns the layer of this element
	int getLayer() const;
	
	/// Returns the origin of this element
	const Vector2& getOrigin() const;
	
	/// Returns the translation of this element, relative to its parent
	const Vector2& getTranslation() const;
	
	/// Returns the rotation of this element
	float getRotation() const;
	
	/// Returns the dimensions of this element
	const Vector2& getDimensions() const;
	
	/// Returns the world-space position of this element. Only accurate after update() is called
	const Vector2& getPosition() const;
	
	/// Returns the world-space bounds of this element.
	const Rect& getBounds() const;
	
	/// Returns the tint color of this element
	const Vector4& getTintColor() const;
	
	/// Returns the final color of this element, relative to its ancestor's tint colors
	const Vector4& getColor() const;
	
	/// Returns the visibility of this element
	bool isVisible() const;
	
	/// Returns `true` if the element is active (callbacks enabled)
	bool isActive() const;
	
	/// Calculates the world-space position and bounds of this element and its children
	virtual void update();
	
	/// Adds a child to this element
	void addChild(UIElement* element);
	
	/// Removes a child from this element
	void removeChild(UIElement* element);
	
	void setMouseOverCallback(std::function<void()> callback);
	void setMouseOutCallback(std::function<void()> callback);
	void setMouseMovedCallback(std::function<void(int, int)> callback);
	void setMousePressedCallback(std::function<void(int, int, int)> callback);
	void setMouseReleasedCallback(std::function<void(int, int, int)> callback);
	
	void mouseMoved(int x, int y);
	void mouseButtonPressed(int button, int x, int y);
	void mouseButtonReleased(int button, int x, int y);
	
protected:
	UIMaterial material;
	
private:
	UIElement* parent;
	std::vector<UIElement*> children;
	Vector2 anchor;
	int layerOffset;
	int layer;
	Vector2 origin;
	Vector2 translation;
	float rotation;
	Vector2 dimensions;
	Vector2 position;
	Rect bounds;
	Vector4 tintColor;
	Vector4 color;
	bool visible;
	bool active;
	bool mouseOver;
	std::function<void()> mouseOverCallback;
	std::function<void()> mouseOutCallback;
	std::function<void(int, int)> mouseMovedCallback;
	std::function<void(int, int, int)> mousePressedCallback;
	std::function<void(int, int, int)> mouseReleasedCallback;
};

inline void UIElement::setAnchor(const Vector2& anchor)
{
	this->anchor = anchor;
}

inline void UIElement::setLayerOffset(int offset)
{
	this->layerOffset = offset;
}

inline void UIElement::setOrigin(const Vector2& origin)
{
	this->origin = origin;
}

inline void UIElement::setTranslation(const Vector2& translation)
{
	this->translation = translation;
}

inline void UIElement::setRotation(float angle)
{
	this->rotation = angle;
}

inline void UIElement::setDimensions(const Vector2& dimensions)
{
	this->dimensions = dimensions;
}

inline void UIElement::setTintColor(const Vector4& color)
{
	this->tintColor = color;
}

inline void UIElement::setVisible(bool visible)
{
	this->visible = visible;
}

inline void UIElement::setActive(bool active)
{
	this->active = active;
}

inline const UIElement* UIElement::getParent() const
{
	return parent;
}

inline const UIMaterial* UIElement::getMaterial() const
{
	return &material;
}

inline UIMaterial* UIElement::getMaterial()
{
	return &material;
}

inline UIElement* UIElement::getParent()
{
	return parent;
}

inline std::size_t UIElement::getChildCount() const
{
	return children.size();
}

inline const UIElement* UIElement::getChild(std::size_t index) const
{
	return children[index];
}

inline UIElement* UIElement::getChild(std::size_t index)
{
	return children[index];
}

inline const Vector2& UIElement::getAnchor() const
{
	return anchor;
}

inline int UIElement::getLayerOffset() const
{
	return layerOffset;
}

inline int UIElement::getLayer() const
{
	return layer;
}

inline const Vector2& UIElement::getOrigin() const
{
	return origin;
}

inline const Vector2& UIElement::getTranslation() const
{
	return translation;
}

inline float UIElement::getRotation() const
{
	return rotation;
}

inline const Vector2& UIElement::getDimensions() const
{
	return dimensions;
}

inline const Vector2& UIElement::getPosition() const
{
	return position;
}

inline const Rect& UIElement::getBounds() const
{
	return bounds;
}

inline const Vector4& UIElement::getTintColor() const
{
	return tintColor;
}

inline const Vector4& UIElement::getColor() const
{
	return color;
}

inline bool UIElement::isVisible() const
{
	return visible;
}

inline bool UIElement::isActive() const
{
	return active;
}

class UIContainer: public UIElement
{
public:
	virtual UIElement::Type getElementType() const;
};

inline UIElement::Type UIContainer::getElementType() const
{
	return UIElement::Type::CONTAINER;
}

class UILabel: public UIElement
{
public:
	UILabel();
	virtual ~UILabel();
	
	virtual UIElement::Type getElementType() const;
	
	void setFont(Font* font);
	void setText(const std::string& text);
	
	const Font* getFont() const;
	Font* getFont();
	const std::string& getText() const;
		
private:
	void calculateDimensions();

	Font* font;
	std::string text;
};

inline UIElement::Type UILabel::getElementType() const
{
	return UIElement::Type::LABEL;
}

inline const Font* UILabel::getFont() const
{
	return font;
}

inline Font* UILabel::getFont()
{
	return font;
}

inline const std::string& UILabel::getText() const
{
	return text;
}

class UIImage: public UIElement
{
public:
	UIImage();
	virtual ~UIImage();
	
	virtual UIElement::Type getElementType() const;
	virtual const Texture* getTexture() const;
	
	void setTexture(Texture* texture);
	void setTextureBounds(const Rect& bounds);
	
	const Rect& getTextureBounds() const;

private:
	Rect textureBounds;
};

inline UIElement::Type UIImage::getElementType() const
{
	return UIElement::Type::IMAGE;
}

inline const Texture* UIImage::getTexture() const
{
	return material.texture;
}

inline void UIImage::setTexture(Texture* texture)
{
	material.texture = texture;
}

inline void UIImage::setTextureBounds(const Rect& bounds)
{
	this->textureBounds = bounds;
}

inline const Rect& UIImage::getTextureBounds() const
{
	return textureBounds;
}

// Creates a scene from a root UI element (follows visitor pattern?)
class UIBatcher
{
public:
	void batch(BillboardBatch* result, const UIElement* ui);
	
private:
	void queueElements(std::list<const UIElement*>* elements, const UIElement* element) const;
	BillboardBatch::Range* getRange(BillboardBatch* result, const UIElement* element) const;
	void batchElement(BillboardBatch* result, const UIElement* element);
	void batchLabel(BillboardBatch* result, const UILabel* label);
	void batchImage(BillboardBatch* result, const UIImage* image);
};

#endif // UI_HPP
