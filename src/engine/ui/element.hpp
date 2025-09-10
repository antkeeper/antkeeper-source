// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/scene/collection.hpp>
#include <engine/input/mouse.hpp>
#include <engine/ui/element-events.hpp>
#include <engine/math/vector.hpp>
#include <engine/geom/primitives/rectangle.hpp>
#include <functional>
#include <memory>
#include <vector>

namespace engine::ui
{
	enum class element_type
	{
		/// Unknown element type.
		unknown,

		/// Canvas element.
		canvas,

		/// Column container element.
		column_container,

		/// Row container element.
		row_container,

		/// Center container element.
		center_container,

		/// Margin container element.
		margin_container,

		/// Label element.
		label,

		/// Range element.
		range,

		/// Image element.
		image,

		/// Separator element.
		separator
	};

	/// Abstract base class for UI elements.
	class element: public std::enable_shared_from_this<element>
	{
	public:
		/// Vector type.
		using vector_type = math::fvec2;

		/// Rectangle type.
		using rectangle_type = geom::rectangle<float>;

		/// Destructs an element.
		virtual ~element() = 0;

		/// Returns the type of the element.
		[[nodiscard]] virtual element_type get_type() const noexcept = 0;

		/// @name Hierarchy
		/// @{

		/// Adds a child element to this element.
		/// @param child Child element to add.
		void add_child(std::shared_ptr<element> child);

		/// Removes a child element from this element.
		/// @param child Child element to remove.
		void remove_child(std::shared_ptr<element> child);

		/// Removes all children from this element.
		void remove_children();

		/// Returns a weak pointer to the parent element, if any.
		[[nodiscard]] inline constexpr const auto& get_parent() const noexcept
		{
			return m_parent;
		}

		/// Returns `true` if the element has a parent, `false` otherwise.
		[[nodiscard]] inline auto has_parent() const noexcept
		{
			return !m_parent.expired();
		}

		/// Returns a reference to the child element container.
		[[nodiscard]] inline constexpr const auto& get_children() const noexcept
		{
			return m_children;
		}

		/// Returns `true` if the element has one or more children, `false` otherwise.
		[[nodiscard]] inline constexpr auto has_children() const noexcept
		{
			return !m_children.empty();
		}

		/// Returns the root element.
		[[nodiscard]] std::shared_ptr<element> get_root();

		/// Returns the root element.
		[[nodiscard]] std::shared_ptr<const element> get_root() const;

		/// Visits each ancestor of this element.
		/// @param visitor Visitor function.
		void visit_ancestors(const std::function<void(element&)>& visitor);

		/// Visits each descendant of this element.
		/// @param visitor Visitor function.
		void visit_descendants(const std::function<void(element&)>& visitor);

		/// Finds the first ancestor of this element which returns `true` for the visitor function.
		/// @param visitor Visitor function.
		/// @return Shared pointer to the found ancestor element, or `nullptr` if no matching ancestor was found.
		[[nodiscard]] std::shared_ptr<element> find_ancestor(const std::function<bool(element&)>& visitor);

		/// Finds the first descendant of this element which returns `true` for the visitor function.
		/// @param visitor Visitor function.
		/// @return Shared pointer to the found descendant element, or `nullptr` if no matching descendant was found.
		[[nodiscard]] std::shared_ptr<element> find_descendant(const std::function<bool(element&)>& visitor);

		/// Returns the element which has focus.
		[[nodiscard]] std::shared_ptr<element> find_focus();

		/// @}

		/// @name Anchors
		/// @{

		/// Sets the distances from the edges of the parent container to which the element margins are relative.
		/// @param left Normalized distance from the left edge of the parent element.
		/// @param bottom Normalized distance from the bottom edge of the parent element.
		/// @param right Normalized distance from the right edge of the parent element.
		/// @param top Normalized distance from the top edge of the parent element.
		void set_anchors(float left, float bottom, float right, float top);

		/// Sets the distance from the left edge of the parent element, to which the left margin is relative.
		/// @param left Normalized distance from the left edge of the parent element.
		void set_anchor_left(float left);

		/// Sets the distance from the bottom edge of the parent element, to which the bottom margin is relative.
		/// @param bottom Normalized distance from the bottom edge of the parent element.
		void set_anchor_bottom(float bottom);

		/// Sets the distance from the right edge of the parent element, to which the right margin is relative.
		/// @param right Normalized distance from the right edge of the parent element.
		void set_anchor_right(float right);

		/// Sets the distance from the top edge of the parent element, to which the top margin is relative.
		/// @param top Normalized distance from the top edge of the parent element.
		void set_anchor_top(float top);

		/// Returns the normalized distance from the left edge of the parent element, to which the left margin is relative.
		[[nodiscard]] inline constexpr auto get_anchor_left() const noexcept
		{
			return m_anchor_min.x();
		}

		/// Returns the normalized distance from the bottom edge of the parent element, to which the bottom margin is relative.
		[[nodiscard]] inline constexpr auto get_anchor_bottom() const noexcept
		{
			return m_anchor_min.y();
		}

		/// Returns the normalized distance from the right edge of the parent element, to which the right margin is relative.
		[[nodiscard]] inline constexpr auto get_anchor_right() const noexcept
		{
			return m_anchor_max.x();
		}

		/// Returns the normalized distance from the top edge of the parent element, to which the top margin is relative.
		[[nodiscard]] inline constexpr auto get_anchor_top() const noexcept
		{
			return m_anchor_max.y();
		}

		/// @}

		/// @name Margins
		/// @{

		/// Sets the distances to the edges of the element, relative to the element anchors.
		/// @param left Distance to the left edge of the element, relative to the left anchor.
		/// @param bottom Distance to the bottom edge of the element, relative to the bottom anchor.
		/// @param right Distance to the right edge of the element, relative to the right anchor.
		/// @param top Distance to the top edge of the element, relative to the top anchor.
		void set_margins(float left, float bottom, float right, float top);

		/// Sets the distance to the left edge of the element.
		/// @param left Distance to the left edge of the element, relative to the left anchor.
		void set_margin_left(float left);

		/// Sets the distance to the bottom edge of the element.
		/// @param bottom Distance to the bottom edge of the element, relative to the bottom anchor.
		void set_margin_bottom(float bottom);

		/// Sets the distance from the right edge of the element.
		/// @param right Distance to the right edge of the element, relative to the right anchor.
		void set_margin_right(float right);

		/// Sets the distance from the top edge of the element.
		/// @param top Distance to the top edge of the element, relative to the top anchor.
		void set_margin_top(float top);

		/// Returns the distance to the left edge of the element, relative to the left anchor.
		[[nodiscard]] inline constexpr auto get_margin_left() const noexcept
		{
			return m_margin_min.x();
		}

		/// Returns the distance to the bottom edge of the element, relative to the bottom anchor.
		[[nodiscard]] inline constexpr auto get_margin_bottom() const noexcept
		{
			return m_margin_min.y();
		}

		/// Returns the distance to the right edge of the element, relative to the right anchor.
		[[nodiscard]] inline constexpr auto get_margin_right() const noexcept
		{
			return m_margin_max.x();
		}

		/// Returns the distance to the top edge of the element, relative to the top anchor.
		[[nodiscard]] inline constexpr auto get_margin_top() const noexcept
		{
			return m_margin_max.y();
		}

		/// @}

		/// @name Bounds
		/// @{

		/// Sets the minimum size of the element.
		void set_min_size(const vector_type& size);

		/// Returns the minimum size of the element.
		[[nodiscard]] inline constexpr const auto& get_min_size() const noexcept
		{
			return m_min_size;
		}

		/// Returns the bounds of the element.
		[[nodiscard]] inline constexpr const auto& get_bounds() const noexcept
		{
			return m_bounds;
		}

		/// @}

		/// @name Depth
		/// @{

		/// Sets the depth offset of the element.
		/// @param offset Depth offset.
		void set_z_offset(int offset);

		/// Returns the depth offset of the element.
		[[nodiscard]] inline constexpr auto get_z_offset() const noexcept
		{
			return m_z_offset;
		}

		/// Returns the depth of the element.
		[[nodiscard]] inline constexpr auto get_depth() const noexcept
		{
			return m_z_offset;
		}

		/// @} 

		/// @name Focus
		/// @{

		/// Sets the focus state of the element.
		/// @param focus `true` if the element should have focus, `false` otherwise. If `true`, the focus state of all other elements in the hierarchy will be set to `false`.
		void set_focus(bool focus);

		/// Sets the element to focus when focus is shifted back.
		/// @param back Element to focus when focus is shifted back.
		void set_focus_back(std::weak_ptr<element> back);

		/// Sets the element to focus when focus is shifted forward.
		/// @param forward Element to focus when focus is shifted forward.
		void set_focus_forward(std::weak_ptr<element> forward);

		/// Sets the element to focus when focus is shifted up.
		/// @param up Element to focus when focus is shifted up.
		void set_focus_up(std::weak_ptr<element> up);

		/// Sets the element to focus when focus is shifted down.
		/// @param down Element to focus when focus is shifted down.
		void set_focus_down(std::weak_ptr<element> down);

		/// Sets the element to focus when focus is shifted left.
		/// @param left Element to focus when focus is shifted left.
		void set_focus_left(std::weak_ptr<element> left);

		/// Sets the element to focus when focus is shifted right.
		/// @param right Element to focus when focus is shifted right.
		void set_focus_right(std::weak_ptr<element> right);

		/// Returns `true` if the element has focus, `false` otherwise.
		[[nodiscard]] inline constexpr auto has_focus() const noexcept
		{
			return m_focus;
		}

		/// Returns the element to focus when focus is shifted back.
		[[nodiscard]] inline constexpr const auto& get_focus_back() const noexcept
		{
			return m_focus_back;
		}

		/// Returns the element to focus when focus is shifted forward.
		[[nodiscard]] inline constexpr const auto& get_focus_forward() const noexcept
		{
			return m_focus_forward;
		}

		/// Returns the element to focus when focus is shifted left.
		[[nodiscard]] inline constexpr const auto& get_focus_left() const noexcept
		{
			return m_focus_left;
		}

		/// Returns the element to focus when focus is shifted right.
		[[nodiscard]] inline constexpr const auto& get_focus_right() const noexcept
		{
			return m_focus_right;
		}

		/// Returns the element to focus when focus is shifted up.
		[[nodiscard]] inline constexpr const auto& get_focus_up() const noexcept
		{
			return m_focus_up;
		}

		/// Returns the element to focus when focus is shifted down.
		[[nodiscard]] inline constexpr const auto& get_focus_down() const noexcept
		{
			return m_focus_down;
		}

		/// @}

		/// @name Color
		/// @{

		void set_opacity(float opacity);

		/// Returns the opacity factor of the individual element.
		[[nodiscard]] inline constexpr float get_opacity() const noexcept
		{
			return m_opacity;
		}

		/// Returns the combined opacity factor of the element and its ancestors.
		[[nodiscard]] inline float get_effective_opacity() const noexcept
		{
			return m_effective_opacity;
		}

		/// @}

		/// @name Callbacks
		/// @{

		/// Enables or disables input handling for the element.
		void set_input_handling_enabled(bool enabled);

		/// Sets the callback called each time the element's focus state changes.
		/// @param callback Focus changed callback.
		void set_focus_changed_callback(std::function<void(const element_focus_changed_event&)> callback);

		/// Sets the callback called each time a mouse moves within the bounds of the element.
		/// @param callback Mouse moved callback.
		void set_mouse_moved_callback(std::function<void(const element_mouse_moved_event&)> callback);

		/// Sets the callback called each time a mouse enters the bounds of the element.
		/// @param callback Mouse entered callback.
		void set_mouse_entered_callback(std::function<void(const element_mouse_entered_event&)> callback);

		/// Sets the callback called each time a mouse exits the bounds of the element.
		/// @param callback Mouse exited callback.
		void set_mouse_exited_callback(std::function<void(const element_mouse_exited_event&)> callback);

		/// Sets the callback called each time a mouse button is pressed on the element.
		/// @param callback Mouse button pressed callback.
		void set_mouse_button_pressed_callback(std::function<void(const element_mouse_button_pressed_event&)> callback);

		/// Sets the callback called each time a mouse button is released on the element.
		/// @param callback Mouse button released callback.
		void set_mouse_button_released_callback(std::function<void(const element_mouse_button_released_event&)> callback);

		/// Simulates a mouse button press.
		/// @param button Mouse button to simulate.
		void press(input::mouse_button button);

		/// Simulates a mouse button release.
		/// @param button Mouse button to simulate.
		void release(input::mouse_button button);

		/// Simulates a mouse button press, followed by a mouse button release.
		/// @param button Mouse button to simulate.
		void click(input::mouse_button button);

		/// Returns `true` if input handling is enabled, `false` otherwise.
		[[nodiscard]] inline constexpr bool is_input_handling_enabled() const noexcept
		{
			return m_handle_input;
		}

		/// @}

		virtual void add_to_scene(scene::collection& scene);
		virtual void remove_from_scene(scene::collection& scene);

		virtual bool handle_mouse_moved(const input::mouse_moved_event& event);
		virtual bool handle_mouse_button_pressed(const input::mouse_button_pressed_event& event);
		virtual bool handle_mouse_button_released(const input::mouse_button_released_event& event);

	protected:
		virtual void bounds_recalculated();

	private:
		void set_parent(std::weak_ptr<element> parent);
		void recalculate_bounds();
		void recalculate_effective_opacity();
		virtual void descendant_added(element& descendant);
		virtual void descendant_removed(element& descendant);
		virtual void child_min_size_changed(element& descendant);
		virtual void effective_opacity_changed();
		virtual void depth_changed();

		std::weak_ptr<element> m_parent;
		std::vector<std::shared_ptr<element>> m_children;

		vector_type m_anchor_min{0.5f, 0.5f};
		vector_type m_anchor_max{0.5f, 0.5f};
		vector_type m_margin_min{};
		vector_type m_margin_max{};
		vector_type m_min_size{};
		rectangle_type m_bounds{};

		int m_z_offset{0};

		bool m_focus{false};
		std::weak_ptr<element> m_focus_back;
		std::weak_ptr<element> m_focus_forward;
		std::weak_ptr<element> m_focus_left;
		std::weak_ptr<element> m_focus_right;
		std::weak_ptr<element> m_focus_up;
		std::weak_ptr<element> m_focus_down;

		bool m_hover{false};

		float m_opacity{1.0f};
		float m_effective_opacity{1.0f};
		bool m_handle_input{true};

		std::function<void(const element_focus_changed_event&)> m_focus_changed_callback;
		std::function<void(const element_mouse_moved_event&)> m_mouse_moved_callback;
		std::function<void(const element_mouse_entered_event&)> m_mouse_entered_callback;
		std::function<void(const element_mouse_exited_event&)> m_mouse_exited_callback;
		std::function<void(const element_mouse_button_pressed_event&)> m_mouse_button_pressed_callback;
		std::function<void(const element_mouse_button_released_event&)> m_mouse_button_released_callback;
	};
}
