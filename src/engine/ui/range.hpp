// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_UI_RANGE_HPP
#define ANTKEEPER_UI_RANGE_HPP

#include <engine/ui/label.hpp>

namespace ui {

class range;

/// Event generated when the value of a range element is changed.
struct range_value_changed_event
{
	/// Pointer to the range element which had its value changed.
	range* element{nullptr};

	/// New value of the range element.
	float value{0.0f};
};

/// Range UI element.
class range: public label
{
public:
	/// Destructs a range.
	~range() override = default;

	[[nodiscard]] inline constexpr element_type get_type() const noexcept override
	{
		return element_type::range;
	}

	/// Sets the value of the range.
	/// @param value Value of the range.
	void set_value(float value);

	/// Sets the minimum value of the range.
	/// @param value Minimum value of the range.
	void set_min_value(float value);

	/// Sets the maximum value of the range.
	/// @param value Maximum value of the range.
	void set_max_value(float value);

	/// Sets the increment and decrement size of the range.
	/// @param step Increment and decrement size.
	void set_step(float step);

	/// Increments the value of the range.
	void increment();

	/// Decrements the value of the range.
	void decrement();

	/// Returns the value of the range.
	[[nodiscard]] inline constexpr float get_value() const noexcept
	{
		return m_value;
	}

	/// Returns the minimum value of the range.
	[[nodiscard]] inline constexpr float get_min_value() const noexcept
	{
		return m_min_value;
	}

	/// Returns the maximum value of the range.
	[[nodiscard]] inline constexpr float get_max_value() const noexcept
	{
		return m_max_value;
	}

	/// Returns the increment and decrement size of the range.
	[[nodiscard]] inline constexpr float get_step() const noexcept
	{
		return m_step;
	}

	/// Returns `true` if the range is boolean, `false` otherwise.
	[[nodiscard]] bool is_boolean() const noexcept;

	/// Sets the callback called each time the range's value changes.
	/// @param callback Value changed callback. Returns `true` if the value change is accepted, `false` otherwise.
	void set_value_changed_callback(std::function<bool(const range_value_changed_event&)> callback);

private:
	float m_value{0.0f};
	float m_min_value{0.0f};
	float m_max_value{1.0f};
	float m_step{0.1f};

	std::function<bool(const range_value_changed_event&)> m_value_changed_callback;
};

} // namespace ui

#endif // ANTKEEPER_UI_RANGE_HPP
