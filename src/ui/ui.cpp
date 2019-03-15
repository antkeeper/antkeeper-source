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

#include "ui.hpp"

UIMaterial::UIMaterial()
{
	texture = addVariable<const Texture2D*>("texture");
	textureOffset = addVariable<Vector2>("offset");
	textureScale = addVariable<Vector2>("scale");
	
	texture->setValue(nullptr);
	textureOffset->setValue(Vector2(0.0f));
	textureScale->setValue(Vector2(1.0f));
}

UIElement::UIElement():
	parent(nullptr),
	anchor(Anchor::TOP_LEFT),
	layerOffset(0),
	layer(0),
	bounds(Vector2(0.0f), Vector2(0.0f)),
	tintColor(1.0f),
	color(tintColor),
	visible(true),
	callbacksEnabled(true),
	mouseOver(false),
	mouseOverCallback(nullptr),
	mouseOutCallback(nullptr),
	mouseMovedCallback(nullptr),
	mousePressedCallback(nullptr),
	mouseReleasedCallback(nullptr),
	origin(0.0f),
	translation(0.0f),
	rotation(0.0f),
	dimensions(0.0f),
	position(0.0f),
	originTween(&origin, lerp<Vector2>),
	translationTween(&translation, lerp<Vector2>),
	rotationTween(&rotation, lerp<float>),
	dimensionsTween(&dimensions, lerp<Vector2>),
	positionTween(&position, lerp<Vector2>),
	tintColorTween(&tintColor, lerp<Vector4>),
	colorTween(&color, lerp<Vector4>)
{}

UIElement::~UIElement()
{}

void UIElement::update()
{
	resetTweens();

	// Calculate position
	if (parent != nullptr)
	{
		// Calculate world-space position
		Vector2 anchorPoint = parent->position + parent->dimensions * anchor - dimensions * anchor;
		position = anchorPoint + origin + translation;
		
		// Calculate layer
		layer = parent->layer + 1 + layerOffset;
		
		// Calculate color
		color = parent->color * tintColor;
	}
	else
	{
		position = origin + translation;
		layer = layerOffset;
		color = tintColor;
	}
	
	// Calculate bounds
	bounds.setMin(position);
	bounds.setMax(position + dimensions);
	
	// Update children
	for (UIElement* child: children)
	{
		child->update();
	}
}

void UIElement::addChild(UIElement* element)
{
	children.push_back(element);
	element->parent = this;
}

void UIElement::removeChild(UIElement* element)
{
	for (auto it = children.begin(); it != children.end(); ++it)
	{
		if (*it == element)
		{
			children.erase(it);
			return;
		}
	}
}

void UIElement::setMouseOverCallback(std::function<void()> callback)
{
	mouseOverCallback = callback;
}

void UIElement::setMouseOutCallback(std::function<void()> callback)
{
	mouseOutCallback = callback;
}

void UIElement::setMouseMovedCallback(std::function<void(int, int)> callback)
{
	mouseMovedCallback = callback;
}

void UIElement::setMousePressedCallback(std::function<void(int, int, int)> callback)
{
	mousePressedCallback = callback;
}

void UIElement::setMouseReleasedCallback(std::function<void(int, int, int)> callback)
{
	mouseReleasedCallback = callback;
}

void UIElement::handleEvent(const MouseMovedEvent& event)
{
	if (!callbacksEnabled)
	{
		return;
	}
	
	if (bounds.contains(Vector2(event.x, event.y)))
	{
		if (!mouseOver)
		{
			mouseOver = true;
			if (mouseOverCallback)
			{
				mouseOverCallback();
			}
		}
		
		if (mouseMovedCallback)
		{
			mouseMovedCallback(event.x, event.y);
		}
	}
	else if (mouseOver)
	{
		mouseOver = false;
		if (mouseOutCallback)
		{
			mouseOutCallback();
		}
	}
	
	for (UIElement* child: children)
	{
		child->handleEvent(event);
	}
}

void UIElement::handleEvent(const MouseButtonPressedEvent& event)
{
	if (!callbacksEnabled)
	{
		return;
	}
	
	if (bounds.contains(Vector2(event.x, event.y)))
	{
		if (mousePressedCallback)
		{
			mousePressedCallback(event.button, event.x, event.y);
		}
		
		for (UIElement* child: children)
		{
			child->handleEvent(event);
		}
	}
}

void UIElement::handleEvent(const MouseButtonReleasedEvent& event)
{
	if (!callbacksEnabled)
	{
		return;
	}
	
	if (bounds.contains(Vector2(event.x, event.y)))
	{
		if (mouseReleasedCallback)
		{
			mouseReleasedCallback(event.button, event.x , event.y);
		}
		
		for (UIElement* child: children)
		{
			child->handleEvent(event);
		}
	}
}

void UIElement::interpolate(float dt)
{
	originTween.interpolate(dt);
	translationTween.interpolate(dt);
	rotationTween.interpolate(dt);
	dimensionsTween.interpolate(dt);
	positionTween.interpolate(dt);
	tintColorTween.interpolate(dt);
	colorTween.interpolate(dt);

	for (UIElement* child: children)
	{
		child->interpolate(dt);
	}
}

void UIElement::resetTweens()
{
	originTween.reset();
	translationTween.reset();
	rotationTween.reset();
	dimensionsTween.reset();
	positionTween.reset();
	tintColorTween.reset();
	colorTween.reset();

	for (UIElement* child: children)
	{
		child->resetTweens();
	}
}

UILabel::UILabel():
	font(nullptr)
{}

UILabel::~UILabel()
{}

void UILabel::setFont(Font* font)
{
	this->font = font;
	material.texture->setValue(font->getTexture());
	calculateDimensions();
}

void UILabel::setText(const std::string& text)
{
	this->text = text;
	calculateDimensions();
}

void UILabel::calculateDimensions()
{
	if (font != nullptr && !text.empty())
	{
		float width = font->getWidth(text);
		float height = font->getMetrics().getHeight();
		setDimensions(Vector2(width, height));
	}
	else
	{
		setDimensions(Vector2(0.0f));
	}
}

UIImage::UIImage():
	textureBounds(Vector2(0.0f), Vector2(1.0f))
{}

UIImage::~UIImage()
{}

void UIBatcher::batch(BillboardBatch* result, const UIElement* ui)
{
	// Create list of visible UI elements
	std::list<const UIElement*> elements;
	queueElements(&elements, ui);
	
	// Sort UI elements according to layer and texture
	elements.sort([](const UIElement* a, const UIElement* b)
	{
		if (a->getLayer() < b->getLayer())
		{
			return true;
		}
		else if (b->getLayer() < a->getLayer())
		{
			return false;
		}
		
		return (a->getMaterial()->texture->getValue() < b->getMaterial()->texture->getValue());
	});
	
	// Clear previous ranges
	
	result->removeRanges();
	
	// Batch UI elements
	for (const UIElement* element: elements)
	{
		batchElement(result, element);
	}
	
	// Update batch
	result->batch();
}

void UIBatcher::queueElements(std::list<const UIElement*>* elements, const UIElement* element) const
{
	if (element->isVisible())
	{
		elements->push_back(element);
		
		for (std::size_t i = 0; i < element->getChildCount(); ++i)
		{
			queueElements(elements, element->getChild(i));
		}
	}
}

BillboardBatch::Range* UIBatcher::getRange(BillboardBatch* result, const UIElement* element) const
{
	BillboardBatch::Range* range = nullptr;
	
	if (!result->getRangeCount())
	{
		// Create initial range
		range = result->addRange();
		range->material = (Material*)element->getMaterial();
		range->start = 0;
		range->length = 0;
	}
	else
	{
		range = result->getRange(result->getRangeCount() - 1);
		
		const UIMaterial* material = static_cast<UIMaterial*>(range->material);
		
		if (material->texture->getValue() != element->getMaterial()->texture->getValue())
		{
			// Create new range for the element
			range = result->addRange();
			BillboardBatch::Range* precedingRange = result->getRange(result->getRangeCount() - 2);
			range->material = (Material*)element->getMaterial();
			range->start = precedingRange->start + precedingRange->length;
			range->length = 0;
		}
	}
	
	return range;
}

void UIBatcher::batchElement(BillboardBatch* result, const UIElement* element)
{	
	switch (element->getElementType())
	{
		case UIElement::Type::LABEL:
			batchLabel(result, static_cast<const UILabel*>(element));
			break;
		
		case UIElement::Type::IMAGE:
			batchImage(result, static_cast<const UIImage*>(element));
			break;
		
		case UIElement::Type::CONTAINER:
			break;
		
		default:
			break;
	}
}

void UIBatcher::batchLabel(BillboardBatch* result, const UILabel* label)
{
	if (label->getFont() != nullptr && !label->getText().empty())
	{
		// Get range
		BillboardBatch::Range* range = getRange(result, label);
		
		// Pixel-perfect
		//Vector3 origin = Vector3((int)(label->getPosition().x + 0.5f), (int)(label->getPosition().y + 0.5f), label->getLayer() * 0.01f);
		Vector3 origin = Vector3((int)(label->getPositionTween()->getSubstate().x), (int)(label->getPositionTween()->getSubstate().y), label->getLayer() * 0.01f);
		
		// Print billboards
		const Font* font = label->getFont();
		std::size_t index = range->start + range->length;
		std::size_t count = 0;
		font->puts(result, origin, label->getText(), label->getColorTween()->getSubstate(), index, &count);

		for (std::size_t i = index; i < index + count; ++i)
		{
			result->getBillboard(i)->resetTweens();
		}
		
		// Increment range length
		range->length += count;
	}
}

void UIBatcher::batchImage(BillboardBatch* result, const UIImage* image)
{
	// Get range
	BillboardBatch::Range* range = getRange(result, image);
	
	// Pixel-perfect
	//Vector2 position = Vector2((int)(image->getPosition().x + 0.5f), (int)(image->getPosition().y + 0.5f));
	//Vector2 dimensions = Vector2((int)(image->getDimensions().x + 0.5f), (int)(image->getDimensions().y + 0.5f));
	//Vector3 translation = Vector3(position.x + dimensions.x * 0.5f, position.y + dimensions.y * 0.5f, image->getLayer() * 0.01f);

	Vector2 position = Vector2((int)(image->getPositionTween()->getSubstate().x), (int)(image->getPositionTween()->getSubstate().y));
	Vector2 dimensions = Vector2((int)(image->getDimensionsTween()->getSubstate().x), (int)(image->getDimensionsTween()->getSubstate().y));
	Vector3 translation = Vector3(position.x + dimensions.x * 0.5f, position.y + dimensions.y * 0.5f, image->getLayer() * 0.01f);
	
	// Create billboard
	std::size_t index = range->start + range->length;
	Billboard* billboard = result->getBillboard(index);
	billboard->setDimensions(dimensions);
	billboard->setTranslation(translation);
	
	if (image->getRotationTween()->getSubstate() != 0.0f)
	{
		billboard->setRotation(glm::angleAxis(image->getRotationTween()->getSubstate(), Vector3(0, 0, -1.0f)));
	}
	else
	{
		billboard->setRotation(Quaternion(1, 0, 0, 0));
	}
	
	billboard->setTextureCoordinates(image->getTextureBounds().getMin(), image->getTextureBounds().getMax());
	billboard->setTintColor(image->getColorTween()->getSubstate());
	billboard->resetTweens();
	
	// Increment range length
	++(range->length);
}
