// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "game/menu.hpp"
#include <engine/scene/text.hpp>
#include <engine/config.hpp>
#include <algorithm>
#include <engine/math/vector.hpp>
#include "game/components/animation-component.hpp"
#include "game/strings.hpp"
#include "game/controls.hpp"
#include "game/fonts.hpp"
#include "game/graphics.hpp"

#include "game/states/experiments/test-state.hpp"
#include "game/screen-transition.hpp"

#include <engine/render/passes/sky-pass.hpp>
#include <engine/ui/center-container.hpp>
#include <engine/ui/column-container.hpp>
#include <engine/ui/row-container.hpp>
#include <engine/ui/margin-container.hpp>
#include <engine/ui/label.hpp>
#include <engine/ui/separator.hpp>
#include <engine/ui/range.hpp>
#include <engine/ui/image.hpp>
#include <engine/debug/contract.hpp>
#include <engine/debug/log.hpp>
#include <engine/resources/resource-manager.hpp>

namespace
{
	inline constexpr float menu_fade_in_duration = 0.25f;
	inline constexpr float menu_fade_out_duration = 0.125f;
	inline constexpr float menu_spacing = 16.0f;
	inline constexpr math::fvec4 back_label_anchor = {0.5f, 0.125f, 0.5f, 0.125f};
	inline constexpr math::fvec4 menu_active_label_color = {1.0f, 1.0f, 1.0f, 1.0f};
	inline constexpr math::fvec4 menu_inactive_label_color = {1.0f, 1.0f, 1.0f, 0.5f};
	inline constexpr math::fvec4 pause_menu_bg_color = {0.0f, 0.0f, 0.0f, 0.75f};

	void fade_out_menu(::game& ctx, std::function<void(animation_context&)> end_callback)
	{
		ctx.m_menu_fade_out_sequence->cues().clear();
		if (end_callback)
		{
			ctx.m_menu_fade_out_sequence->cues().emplace(ctx.m_menu_fade_out_sequence->duration(), end_callback);
		}

		auto& player = ctx.entity_registry->get<animation_component>(ctx.m_menu_animation_entity).player;
		player.rewind();
		player.play(ctx.m_menu_fade_out_sequence);

		// Disable menu controls
		ctx.function_queue.emplace(std::bind(::disable_menu_controls, std::ref(ctx)));
		ctx.m_root_menu_container->set_input_handling_enabled(false);
	}

	void fade_in_menu(::game& ctx, std::function<void(animation_context&)> end_callback)
	{
		ctx.m_menu_fade_in_sequence->cues().clear();
		if (end_callback)
		{
			ctx.m_menu_fade_in_sequence->cues().emplace(ctx.m_menu_fade_in_sequence->duration(), end_callback);
		}

		auto& player = ctx.entity_registry->get<animation_component>(ctx.m_menu_animation_entity).player;
		player.rewind();
		player.play(ctx.m_menu_fade_in_sequence);

		// Enable menu controls
		ctx.function_queue.emplace(std::bind(::enable_menu_controls, std::ref(ctx)));
		ctx.m_root_menu_container->set_input_handling_enabled(true);
	}

	void fade_out_pause_menu_bg(::game& ctx)
	{
		auto& player = ctx.entity_registry->get<animation_component>(ctx.m_pause_menu_bg_animation_entity).player;
		player.rewind();
		player.play(ctx.m_pause_menu_bg_fade_out_sequence);
	}

	void fade_in_pause_menu_bg(::game& ctx)
	{
		auto& player = ctx.entity_registry->get<animation_component>(ctx.m_pause_menu_bg_animation_entity).player;
		player.rewind();
		player.play(ctx.m_pause_menu_bg_fade_in_sequence);
	}

	void open_menu(::game& ctx, std::shared_ptr<ui::element> menu_container)
	{
		debug::precondition(menu_container != nullptr);

		// Add menu to UI
		ctx.m_root_menu_container->add_child(menu_container);

		// Fade in menu
		fade_in_menu(ctx, nullptr);

		// Enable menu controls
		ctx.function_queue.emplace(std::bind(::enable_menu_controls, std::ref(ctx)));
	}

	void close_menu(::game& ctx, std::shared_ptr<ui::element> menu_container)
	{
		debug::precondition(menu_container != nullptr);

		// Remove menu from UI
		ctx.m_root_menu_container->remove_child(menu_container);
	}

	void label_mouse_moved(::game*, const ui::element_mouse_moved_event& event)
	{
		auto& label = *static_cast<ui::label*>(event.element);
		label.set_focus(true);
	}

	void label_mouse_entered(::game*, const ui::element_mouse_entered_event& event)
	{
		auto& label = *static_cast<ui::label*>(event.element);
		label.set_focus(true);
	}

	void label_focus_changed(::game* ctx, const ui::element_focus_changed_event& event)
	{
		auto& label = *static_cast<ui::label*>(event.element);

		if (event.has_focus)
		{
			label.set_color(menu_active_label_color);
			if (ctx->m_menu_focused_element.lock() != label.shared_from_this())
			{
				ctx->menu_up_sound->play();
				ctx->m_menu_focused_element = label.shared_from_this();
			}
		}
		else
		{
			label.set_color(menu_inactive_label_color);
		}
	}

	void range_mouse_button_pressed(::game*, const ui::element_mouse_button_pressed_event& event)
	{
		auto& range = *static_cast<ui::range*>(event.element);

		if (range.is_boolean())
		{
			range.set_value(!range.get_value());
		}
		else
		{
			if (event.button == input::mouse_button::right)
			{
				range.decrement();
			}
			else
			{
				range.increment();
			}
		}
	}

	[[nodiscard]] std::string static_label_text_refresher(::game& ctx, const std::string& string_key, const ui::label&)
	{
		return get_string(ctx, string_key);
	}

	[[nodiscard]] std::string boolean_range_text_refresher(::game& ctx, const ui::label& element)
	{
		const auto& range = static_cast<const ui::range&>(element);
		return range.get_value() ? get_string(ctx, "on") : get_string(ctx, "off");
	}

	[[nodiscard]] std::string percentage_range_text_refresher(const ui::label& element)
	{
		const auto& range = static_cast<const ui::range&>(element);
		return std::format("{:.0f}%", range.get_value() * 100.0f);
	}

	[[nodiscard]] std::string get_keyboard_mapping_string(::game& ctx, const input::action_map& action_map, const input::action& control, const ui::label& element)
	{
		const auto& range = static_cast<const ui::range&>(element);
		if (!range.get_value())
		{
			return get_string(ctx, "control_mapping");
		}

		std::string mapping_string;

		if (auto key_mappings = action_map.get_key_mappings(control); !key_mappings.empty())
		{
			const auto& key_mapping = key_mappings.front();

			// Get name of scancode string from scancode
			std::string scancode_string_name = std::format("scancode_{:02x}", std::to_underlying(key_mapping.scancode));

			// Set mapping string to scancode string
			mapping_string = get_string(ctx, scancode_string_name);
		}
		else if (auto mouse_button_mappings = action_map.get_mouse_button_mappings(control); !mouse_button_mappings.empty())
		{
			const auto& mouse_button_mapping = mouse_button_mappings.front();
			switch (mouse_button_mapping.button)
			{
				case input::mouse_button::left:
					mapping_string = get_string(ctx, "mouse_button_left");
					break;

				case input::mouse_button::middle:
					mapping_string = get_string(ctx, "mouse_button_middle");
					break;

				case input::mouse_button::right:
					mapping_string = get_string(ctx, "mouse_button_right");
					break;

				default:
				{
					const auto format_string = get_string(ctx, "mouse_button_n_format");

					auto button = std::to_underlying(mouse_button_mapping.button);

					mapping_string = std::vformat(format_string, std::make_format_args(button));
					break;
				}
			}
		}
		else if (auto mouse_scroll_mappings = action_map.get_mouse_scroll_mappings(control); !mouse_scroll_mappings.empty())
		{
			const auto& mouse_scroll_mapping = mouse_scroll_mappings.front();

			if (mouse_scroll_mapping.axis == input::mouse_scroll_axis::x)
			{
				if (!mouse_scroll_mapping.direction)
				{
					mapping_string = get_string(ctx, "mouse_scroll_left");
				}
				else
				{
					mapping_string = get_string(ctx, "mouse_scroll_right");
				}
			}
			else
			{
				if (!mouse_scroll_mapping.direction)
				{
					mapping_string = get_string(ctx, "mouse_scroll_up");
				}
				else
				{
					mapping_string = get_string(ctx, "mouse_scroll_down");
				}
			}
		}
		else
		{
			mapping_string = get_string(ctx, "control_unmapped");
		}

		return mapping_string;
	}

	[[nodiscard]] std::string get_gamepad_mapping_string(::game& ctx, const input::action_map& action_map, const input::action& control, const ui::label& element)
	{
		const auto& range = static_cast<const ui::range&>(element);
		if (!range.get_value())
		{
			return get_string(ctx, "control_mapping");
		}

		std::string mapping_string;

		if (auto gamepad_axis_mappings = action_map.get_gamepad_axis_mappings(control); !gamepad_axis_mappings.empty())
		{
			const auto& gamepad_axis_mapping = gamepad_axis_mappings.front();

			switch (gamepad_axis_mapping.axis)
			{
				case input::gamepad_axis::left_stick_x:
					if (gamepad_axis_mapping.direction)
					{
						mapping_string = get_string(ctx, "gamepad_left_stick_left");
					}
					else
					{
						mapping_string = get_string(ctx, "gamepad_left_stick_right");
					}
					break;

				case input::gamepad_axis::left_stick_y:
					if (gamepad_axis_mapping.direction)
					{
						mapping_string = get_string(ctx, "gamepad_left_stick_up");
					}
					else
					{
						mapping_string = get_string(ctx, "gamepad_left_stick_down");
					}
					break;

				case input::gamepad_axis::right_stick_x:
					if (gamepad_axis_mapping.direction)
					{
						mapping_string = get_string(ctx, "gamepad_right_stick_left");
					}
					else
					{
						mapping_string = get_string(ctx, "gamepad_right_stick_right");
					}
					break;

				case input::gamepad_axis::right_stick_y:
					if (gamepad_axis_mapping.direction)
					{
						mapping_string = get_string(ctx, "gamepad_right_stick_up");
					}
					else
					{
						mapping_string = get_string(ctx, "gamepad_right_stick_down");
					}
					break;

				case input::gamepad_axis::left_trigger:
					mapping_string = get_string(ctx, "gamepad_left_trigger");
					break;

				case input::gamepad_axis::right_trigger:
					mapping_string = get_string(ctx, "gamepad_right_trigger");
					break;

				default:
				{
					const auto format_string = get_string(ctx, "gamepad_axis_n_format");

					auto axis = std::to_underlying(gamepad_axis_mapping.axis);
					auto sign = (gamepad_axis_mapping.direction) ? '-' : '+';

					mapping_string = std::vformat(format_string, std::make_format_args(axis, sign));
					break;
				}
			}
		}
		else if (auto gamepad_button_mappings = action_map.get_gamepad_button_mappings(control); !gamepad_button_mappings.empty())
		{
			const auto& gamepad_button_mapping = gamepad_button_mappings.front();
			switch (gamepad_button_mapping.button)
			{
				case input::gamepad_button::a:
					mapping_string = get_string(ctx, "gamepad_button_a");
					break;

				case input::gamepad_button::b:
					mapping_string = get_string(ctx, "gamepad_button_b");
					break;

				case input::gamepad_button::x:
					mapping_string = get_string(ctx, "gamepad_button_x");
					break;

				case input::gamepad_button::y:
					mapping_string = get_string(ctx, "gamepad_button_y");
					break;

				case input::gamepad_button::back:
					mapping_string = get_string(ctx, "gamepad_button_back");
					break;

				case input::gamepad_button::guide:
					mapping_string = get_string(ctx, "gamepad_button_guide");
					break;

				case input::gamepad_button::start:
					mapping_string = get_string(ctx, "gamepad_button_start");
					break;

				case input::gamepad_button::left_stick:
					mapping_string = get_string(ctx, "gamepad_button_left_stick");
					break;

				case input::gamepad_button::right_stick:
					mapping_string = get_string(ctx, "gamepad_button_right_stick");
					break;

				case input::gamepad_button::left_shoulder:
					mapping_string = get_string(ctx, "gamepad_button_left_shoulder");
					break;

				case input::gamepad_button::right_shoulder:
					mapping_string = get_string(ctx, "gamepad_button_right_shoulder");
					break;

				case input::gamepad_button::dpad_up:
					mapping_string = get_string(ctx, "gamepad_button_dpad_up");
					break;

				case input::gamepad_button::dpad_down:
					mapping_string = get_string(ctx, "gamepad_button_dpad_down");
					break;

				case input::gamepad_button::dpad_left:
					mapping_string = get_string(ctx, "gamepad_button_dpad_left");
					break;

				case input::gamepad_button::dpad_right:
					mapping_string = get_string(ctx, "gamepad_button_dpad_right");
					break;

				default:
				{
					const auto format_string = get_string(ctx, "gamepad_button_n_format");

					auto button = std::to_underlying(gamepad_button_mapping.button);

					mapping_string = std::vformat(format_string, std::make_format_args(button));
					break;
				}
			}
		}
		else
		{
			mapping_string = get_string(ctx, "control_unmapped");
		}

		return mapping_string;
	}

	bool start_mapping_keyboard_control(::game& ctx, input::action_map& action_map, input::action& action, const std::string& control_name, const ui::range_value_changed_event& event)
	{
		if (event.value)
		{
			return true;
		}

		auto input_mapped_callback = [&ctx, &action_map, &action, element = event.element, control_name](const auto& event)
		{
			// Remove key mappings, mouse button mappings, and mouse scroll mappings mapped to the action
			action_map.remove_mappings(action, input::mapping_type::key);
			action_map.remove_mappings(action, input::mapping_type::mouse_button);
			action_map.remove_mappings(action, input::mapping_type::mouse_scroll);

			//if (event.mapping.scancode != input::scancode::escape && event.mapping.scancode != input::scancode::backspace)
			{
				// Map generated input mapping to the control
				action_map.add_mapping(action, event.mapping);
			}

			element->set_value(1.0f);

			// Queue disabling of input mapper re-enabling of menu controls
			ctx.function_queue.emplace
			(
				[&ctx]()
				{
					ctx.input_mapper.disconnect();
					ctx.m_key_mapped_subscription.reset();
					ctx.m_mouse_button_mapped_subscription.reset();
					ctx.m_mouse_scroll_mapped_subscription.reset();

					::enable_menu_controls(ctx);
					ctx.m_root_menu_container->set_input_handling_enabled(true);
				}
			);

			{
				// Update and save control profile
				::update_control_profile(ctx, *ctx.control_profile);
				ctx.resource_manager->set_write_path(ctx.controls_path);
				ctx.resource_manager->save(*ctx.control_profile, ctx.control_profile_filename);
			}
		};

		ctx.m_key_mapped_subscription = ctx.input_mapper.get_key_mapped_channel().subscribe(input_mapped_callback);
		ctx.m_mouse_button_mapped_subscription = ctx.input_mapper.get_mouse_button_mapped_channel().subscribe(input_mapped_callback);
		ctx.m_mouse_scroll_mapped_subscription = ctx.input_mapper.get_mouse_scroll_mapped_channel().subscribe(input_mapped_callback);

		// Queue disabling of menu controls and enabling of input mapper
		ctx.function_queue.emplace
		(
			[&]()
			{
				::disable_menu_controls(ctx);
				ctx.m_root_menu_container->set_input_handling_enabled(false);
				ctx.input_mapper.connect(ctx.input_manager->get_event_dispatcher());
			}
		);

		return true;
	}

	bool start_mapping_gamepad_control(::game& ctx, input::action_map& action_map, input::action& action, const std::string& control_name, const ui::range_value_changed_event& event)
	{
		if (event.value)
		{
			return true;
		}

		auto input_mapped_callback = [&ctx, &action_map, &action, element = event.element, control_name](const auto& event)
		{
			if (event.mapping.get_mapping_type() != input::mapping_type::key)
			{
				// Remove key mappings, mouse button mappings, and mouse scroll mappings mapped to the action
				action_map.remove_mappings(action, input::mapping_type::gamepad_axis);
				action_map.remove_mappings(action, input::mapping_type::gamepad_button);

				// Map generated input mapping to the control
				action_map.add_mapping(action, event.mapping);
			}

			element->set_value(1.0f);

			// Queue disabling of input mapper re-enabling of menu controls
			ctx.function_queue.emplace
			(
				[&ctx]()
				{
					ctx.input_mapper.disconnect();
					ctx.m_key_mapped_subscription.reset();
					ctx.m_gamepad_axis_mapped_subscription.reset();
					ctx.m_gamepad_button_mapped_subscription.reset();

					::enable_menu_controls(ctx);
					ctx.m_root_menu_container->set_input_handling_enabled(true);
				}
			);

			{
				// Update and save control profile
				::update_control_profile(ctx, *ctx.control_profile);
				ctx.resource_manager->set_write_path(ctx.controls_path);
				ctx.resource_manager->save(*ctx.control_profile, ctx.control_profile_filename);
			}
		};

		ctx.m_key_mapped_subscription = ctx.input_mapper.get_key_mapped_channel().subscribe(input_mapped_callback);
		ctx.m_gamepad_axis_mapped_subscription = ctx.input_mapper.get_gamepad_axis_mapped_channel().subscribe(input_mapped_callback);
		ctx.m_gamepad_button_mapped_subscription = ctx.input_mapper.get_gamepad_button_mapped_channel().subscribe(input_mapped_callback);

		// Queue disabling of menu controls and enabling of input mapper
		ctx.function_queue.emplace
		(
			[&]()
			{
				::disable_menu_controls(ctx);
				ctx.m_root_menu_container->set_input_handling_enabled(false);
				ctx.input_mapper.connect(ctx.input_manager->get_event_dispatcher());
			}
		);

		return true;
	}

	void refresh_submenu_text(ui::element& element)
	{
		element.visit_descendants
		(
			[](auto& descendant)
			{
				if (descendant.get_type() == ui::element_type::label || descendant.get_type() == ui::element_type::range)
				{
					auto& label = static_cast<ui::label&>(descendant);
					label.refresh_text();
				}
			}
		);
	}

	void refresh_menu_text(::game& ctx)
	{
		refresh_submenu_text(*ctx.m_main_menu_container);
		refresh_submenu_text(*ctx.m_pause_menu_container);
		refresh_submenu_text(*ctx.m_options_menu_container);
		refresh_submenu_text(*ctx.m_controls_menu_container);
		refresh_submenu_text(*ctx.m_keyboard_config_menu_container);
		refresh_submenu_text(*ctx.m_gamepad_config_menu_container);
		refresh_submenu_text(*ctx.m_graphics_menu_container);
		refresh_submenu_text(*ctx.m_sound_menu_container);
		refresh_submenu_text(*ctx.m_language_menu_container);
		refresh_submenu_text(*ctx.m_extras_menu_container);
		refresh_submenu_text(*ctx.m_credits_menu_container);
	}

	void update_menu_font(::game& ctx, std::shared_ptr<type::font> old_menu_font, std::shared_ptr<type::font> old_title_font)
	{
		auto update_submenu_font = [&](auto& element)
		{
			element.visit_descendants
			(
				[&](auto& descendant)
			{
				if (descendant.get_type() == ui::element_type::label || descendant.get_type() == ui::element_type::range)
				{
					auto& label = static_cast<ui::label&>(descendant);
					if (label.get_font() == old_menu_font)
					{
						label.set_font(ctx.menu_font);
						label.set_material(ctx.menu_font_material);
					}
					else if (label.get_font() == old_title_font)
					{
						label.set_font(ctx.title_font);
						label.set_material(ctx.title_font_material);
					}
				}
			}
			);
		};

		update_submenu_font(*ctx.m_main_menu_container);
		update_submenu_font(*ctx.m_pause_menu_container);
		update_submenu_font(*ctx.m_options_menu_container);
		update_submenu_font(*ctx.m_controls_menu_container);
		update_submenu_font(*ctx.m_keyboard_config_menu_container);
		update_submenu_font(*ctx.m_gamepad_config_menu_container);
		update_submenu_font(*ctx.m_graphics_menu_container);
		update_submenu_font(*ctx.m_sound_menu_container);
		update_submenu_font(*ctx.m_language_menu_container);
		update_submenu_font(*ctx.m_extras_menu_container);
		update_submenu_font(*ctx.m_credits_menu_container);
	}

	[[nodiscard]] std::shared_ptr<ui::element> build_menu(::game& ctx, const std::vector<std::tuple<std::string, std::function<void(const ui::element_mouse_button_pressed_event)>>>& items, bool has_back, const math::fvec4& anchors)
	{
		// Construct menu container
		auto menu_container = std::make_shared<ui::margin_container>();
		menu_container->set_anchors(0.0f, 0.0f, 1.0f, 1.0f);

		// Construct menu center container
		auto menu_center_container = std::make_shared<ui::center_container>();
		menu_center_container->set_anchors(anchors[0], anchors[1], anchors[2], anchors[3]);
		menu_container->add_child(menu_center_container);

		std::shared_ptr<ui::center_container> menu_bottom_container;
		if (has_back)
		{
			// Construct menu bottom container
			menu_bottom_container = std::make_shared<ui::center_container>();
			menu_bottom_container->set_anchors(back_label_anchor[0], back_label_anchor[1], back_label_anchor[2], back_label_anchor[3]);
			menu_container->add_child(menu_bottom_container);
		}

		// Construct menu column container
		auto menu_column_container = std::make_shared<ui::column_container>();
		menu_column_container->set_spacing(menu_spacing);
		menu_center_container->add_child(menu_column_container);

		// Construct menu labels
		std::vector<std::shared_ptr<ui::label>> menu_labels;
		menu_labels.reserve(items.size());
		for (std::size_t i = 0; i < items.size(); ++i)
		{
			const auto& [text, callback] = items[i];

			auto label = std::make_shared<ui::label>();

			if (!i && callback)
			{
				label->set_focus(true);
				label->set_color(menu_active_label_color);
			}
			else
			{
				label->set_color(menu_inactive_label_color);
			}

			label->set_font(ctx.menu_font);
			label->set_material(ctx.menu_font_material);
			label->set_text_refresher(std::bind_front(&static_label_text_refresher, std::ref(ctx), text));

			if (callback)
			{
				label->set_mouse_moved_callback(std::bind_front(&label_mouse_moved, &ctx));
				label->set_mouse_entered_callback(std::bind_front(&label_mouse_entered, &ctx));
				label->set_mouse_button_pressed_callback(callback);
				label->set_focus_changed_callback(std::bind_front(&label_focus_changed, &ctx));
			}

			if (i < items.size() - 1 || !has_back)
			{
				menu_column_container->add_child(label);
			}
			else
			{
				menu_bottom_container->add_child(label);
			}

			menu_labels.push_back(label);
		}

		for (std::size_t i = 0; i < menu_labels.size(); ++i)
		{
			auto& label = menu_labels[i];
			label->set_focus_down(menu_labels[(i + 1) % menu_labels.size()]);
			label->set_focus_up(menu_labels[(i + menu_labels.size() - 1) % menu_labels.size()]);
		}

		return menu_container;
	}

	[[nodiscard]] std::shared_ptr<ui::element> build_two_column_menu(::game& ctx, std::vector<std::tuple<std::string, std::shared_ptr<ui::label>, std::shared_ptr<ui::range>>>& items)
	{
		// Construct menu container
		auto menu_container = std::make_shared<ui::margin_container>();
		menu_container->set_anchors(0.0f, 0.0f, 1.0f, 1.0f);

		// Construct menu center container
		auto menu_center_container = std::make_shared<ui::center_container>();
		menu_container->add_child(menu_center_container);

		// Construct menu bottom container
		std::shared_ptr<ui::center_container> menu_bottom_container = std::make_shared<ui::center_container>();
		menu_bottom_container->set_anchors(back_label_anchor[0], back_label_anchor[1], back_label_anchor[2], back_label_anchor[3]);
		menu_container->add_child(menu_bottom_container);

		// Construct container for menu left and right columns
		auto menu_row_container = std::make_shared<ui::row_container>();
		menu_row_container->set_spacing(menu_spacing);
		menu_center_container->add_child(menu_row_container);

		// Construct menu left column container
		auto menu_left_column_container = std::make_shared<ui::column_container>();
		menu_left_column_container->set_alignment(ui::alignment::start);
		menu_left_column_container->set_spacing(menu_spacing);
		menu_row_container->add_child(menu_left_column_container);

		// Construct menu right column container
		auto menu_right_column_container = std::make_shared<ui::column_container>();
		menu_right_column_container->set_alignment(ui::alignment::end);
		menu_right_column_container->set_spacing(menu_spacing);
		menu_row_container->add_child(menu_right_column_container);

		// Construct menu labels
		std::vector<std::shared_ptr<ui::label>> menu_labels;
		menu_labels.reserve(items.size());
		for (std::size_t i = 0; i < items.size(); ++i)
		{
			auto& [text, label, range] = items[i];

			label = std::make_shared<ui::label>();
			label->set_color(menu_inactive_label_color);
			label->set_font(ctx.menu_font);
			label->set_material(ctx.menu_font_material);
			label->set_text_refresher(std::bind_front(&static_label_text_refresher, std::ref(ctx), text));

			if (i < items.size() - 1)
			{
				// Construct range
				range = std::make_shared<ui::range>();
				range->set_color(menu_inactive_label_color);
				if (!i)
				{
					range->set_focus(true);
					range->set_color(menu_active_label_color);
				}
				range->set_font(ctx.menu_font);
				range->set_material(ctx.menu_font_material);

				label->set_focus_right(range);
				range->set_focus_left(label);

				// Add label and range to left and right columns, respectively
				menu_left_column_container->add_child(label);
				menu_right_column_container->add_child(range);

				range->set_mouse_moved_callback(std::bind_front(&label_mouse_moved, &ctx));
				range->set_mouse_entered_callback(std::bind_front(&label_mouse_entered, &ctx));
				range->set_mouse_button_pressed_callback(std::bind_front(&range_mouse_button_pressed, &ctx));
				range->set_focus_changed_callback(std::bind_front(&label_focus_changed, &ctx));

				menu_labels.push_back(range);
			}
			else
			{
				label->set_mouse_moved_callback(std::bind_front(&label_mouse_moved, &ctx));
				label->set_mouse_entered_callback(std::bind_front(&label_mouse_entered, &ctx));
				label->set_focus_changed_callback(std::bind_front(&label_focus_changed, &ctx));

				menu_bottom_container->add_child(label);

				menu_labels.push_back(label);
			}
		}

		for (std::size_t i = 0; i < menu_labels.size(); ++i)
		{
			auto& label = menu_labels[i];
			label->set_focus_down(menu_labels[(i + 1) % menu_labels.size()]);
			label->set_focus_up(menu_labels[(i + menu_labels.size() - 1) % menu_labels.size()]);
		}

		return menu_container;
	}

	void build_main_menu(::game& ctx)
	{
		auto start_callback = [&](const auto&)
		{
			// Create change state function
			auto change_state = [&ctx]()
			{
				// Queue change to nuptial state
				ctx.function_queue.emplace
				(
					[&ctx]()
					{
						close_main_menu(ctx);
						ctx.state_machine.emplace(std::make_unique<test_state>(ctx));
					}
				);
			};

			// Fade out menu
			fade_out_menu(ctx, nullptr);

			// Start fade out to white
			fade_out_to(ctx, change_state);
		};

		auto options_callback = [&](const auto&)
		{
			fade_out_menu
			(
				ctx,
				[&ctx](auto&)
				{
					ctx.function_queue.emplace
					(
						[&ctx]()
						{
							close_main_menu(ctx);
							open_menu(ctx, ctx.m_options_menu_container);
						}
					);
				}
			);
		};

		auto extras_callback = [&](const auto&)
		{
			fade_out_menu
			(
				ctx,
				[&ctx](auto&)
				{
					ctx.function_queue.emplace
					(
						[&ctx]()
						{
							close_main_menu(ctx);
							open_menu(ctx, ctx.m_extras_menu_container);
						}
					);
				}
			);
		};

		auto quit_callback = [&](const auto&)
		{
			// Quit immediately
			ctx.function_queue.emplace([&ctx]() {ctx.closed = true; });
		};

		ctx.m_main_menu_container = build_menu
		(
			ctx,
			{
				{"main_menu_start", start_callback},
				{"main_menu_options", options_callback},
				{"main_menu_extras", extras_callback},
				{"main_menu_quit", quit_callback}
			},
			false,
			{0.5f, 1.0f / 3.0f, 0.5f, 1.0f / 3.0f}
		);

		// Construct title container
		auto title_container = std::make_shared<ui::center_container>();
		title_container->set_anchors(0.0f, 2.0f / 3.0f, 1.0f, 2.0f / 3.0f);
		ctx.m_main_menu_container->add_child(title_container);

		// Construct title label
		auto title_label = std::make_shared<ui::label>();
		title_label->set_color(math::fvec4{1.0f, 1.0f, 1.0f, 1.0f});
		title_label->set_font(ctx.title_font);
		title_label->set_material(ctx.title_font_material);
		title_label->set_text_refresher(std::bind_front(&static_label_text_refresher, std::ref(ctx), "title_antkeeper"));
		title_container->add_child(title_label);
	}

	void build_pause_menu(::game& ctx)
	{
		auto resume_callback = [&](const auto&)
		{
			fade_out_menu
			(
				ctx,
				[&ctx](auto&)
				{
					ctx.function_queue.emplace
					(
						[&ctx]()
						{
							close_pause_menu(ctx);
							::enable_game_controls(ctx);
						}
					);
				}
			);
			fade_out_pause_menu_bg(ctx);
		};

		auto options_callback = [&](const auto&)
		{
			fade_out_menu
			(
				ctx,
				[&ctx](auto&)
				{
					ctx.function_queue.emplace
					(
						[&ctx]()
						{
							close_pause_menu(ctx);
							open_menu(ctx, ctx.m_options_menu_container);
						}
					);
				}
			);
		};

		auto main_menu_callback = [&](const auto&)
		{
			fade_out_menu
			(
				ctx,
				[&ctx](auto&)
				{
					ctx.function_queue.emplace
					(
						[&ctx]()
						{
							close_pause_menu(ctx);
						}
					);
				}
			);

			fade_out_to
			(
				ctx,
				[&ctx]()
				{
					ctx.ui_canvas->remove_child(ctx.m_pause_menu_bg);
					ctx.m_ingame = false;
					open_main_menu(ctx, true);
				}
			);
		};

		auto quit_callback = [&](const auto&)
		{
			// Quit immediately
			ctx.function_queue.emplace([&ctx]() {ctx.closed = true;});
		};

		ctx.m_pause_menu_container = build_menu
		(
			ctx,
			{
				{"pause_menu_resume", resume_callback},
				{"pause_menu_options", options_callback},
				{"pause_menu_main_menu", main_menu_callback},
				{"pause_menu_quit", quit_callback}
			},
			false,
			{0.5f, 0.5f, 0.5f, 0.5f}
		);

		auto menu_bg = std::make_shared<ui::image>();
		menu_bg->set_anchors(0.0f, 0.0f, 1.0f, 1.0f);
		menu_bg->set_material(ctx.m_pause_menu_bg_material);
		menu_bg->set_z_offset(-99);
		ctx.m_pause_menu_bg = menu_bg;
	}

	void build_options_menu(::game& ctx)
	{
		auto controls_callback = [&](const auto&)
		{
			fade_out_menu
			(
				ctx,
				[&ctx](auto&)
				{
					ctx.function_queue.emplace
					(
						[&ctx]()
						{
							close_menu(ctx, ctx.m_options_menu_container);
							open_menu(ctx, ctx.m_controls_menu_container);
						}
					);
				}
			);
		};

		auto graphics_callback = [&](const auto&)
		{
			fade_out_menu
			(
				ctx,
				[&ctx](auto&)
				{
					ctx.function_queue.emplace
					(
						[&ctx]()
						{
							close_menu(ctx, ctx.m_options_menu_container);
							open_menu(ctx, ctx.m_graphics_menu_container);
						}
					);
				}
				);
		};

		auto sound_callback = [&](const auto&)
		{
			fade_out_menu
			(
				ctx,
				[&ctx](auto&)
				{
					ctx.function_queue.emplace
					(
						[&ctx]()
						{
							close_menu(ctx, ctx.m_options_menu_container);
							open_menu(ctx, ctx.m_sound_menu_container);
						}
					);
				}
			);
		};

		auto language_callback = [&](const auto&)
		{
			fade_out_menu
			(
				ctx,
				[&ctx](auto&)
				{
					ctx.function_queue.emplace
					(
						[&ctx]()
						{
							close_menu(ctx, ctx.m_options_menu_container);
							open_menu(ctx, ctx.m_language_menu_container);
						}
					);
				}
			);
		};

		auto back_callback = [&](const auto&)
		{
			fade_out_menu
			(
				ctx,
				[&ctx](auto&)
				{
					ctx.function_queue.emplace
					(
						[&ctx]()
						{
							close_menu(ctx, ctx.m_options_menu_container);
							if (ctx.m_ingame)
							{
								open_pause_menu(ctx);
							}
							else
							{
								open_main_menu(ctx, false);
							}
						}
					);
				}
			);
		};

		ctx.m_options_menu_container = build_menu
		(
			ctx,
			{
				{"options_menu_controls", controls_callback},
				{"options_menu_graphics", graphics_callback},
				{"options_menu_sound", sound_callback},
				{"options_menu_language", language_callback},
				{"back", back_callback}
			},
			true,
			{0.5f, 0.5f, 0.5f, 0.5f}
		);
	}

	void build_controls_menu(::game& ctx)
	{
		auto keyboard_callback = [&](const auto&)
		{
			fade_out_menu
			(
				ctx,
				[&ctx](auto&)
				{
					ctx.function_queue.emplace
					(
						[&ctx]()
						{
							close_menu(ctx, ctx.m_controls_menu_container);
							refresh_submenu_text(*ctx.m_keyboard_config_menu_container);
							open_menu(ctx, ctx.m_keyboard_config_menu_container);
						}
					);
				}
			);
		};

		auto gamepad_callback = [&](const auto&)
		{
			fade_out_menu
			(
				ctx,
				[&ctx](auto&)
				{
					ctx.function_queue.emplace
					(
						[&ctx]()
						{
							close_menu(ctx, ctx.m_controls_menu_container);
							refresh_submenu_text(*ctx.m_gamepad_config_menu_container);
							open_menu(ctx, ctx.m_gamepad_config_menu_container);
						}
					);
				}
			);
		};

		auto back_callback = [&](const auto&)
		{
			fade_out_menu
			(
				ctx,
				[&ctx](auto&)
				{
					ctx.function_queue.emplace
					(
						[&ctx]()
						{
							close_menu(ctx, ctx.m_controls_menu_container);
							open_menu(ctx, ctx.m_options_menu_container);
						}
					);
				}
			);
		};

		ctx.m_controls_menu_container = build_menu
		(
			ctx,
			{
				{"controls_menu_keyboard", keyboard_callback},
				{"controls_menu_gamepad", gamepad_callback},
				{"back", back_callback}
			},
			true,
			{0.5f, 0.5f, 0.5f, 0.5f}
		);
	}

	void build_keyboard_config_menu(::game& ctx)
	{
		std::vector<std::tuple<std::string, std::shared_ptr<ui::label>, std::shared_ptr<ui::range>>> items =
		{
			{"control_move_forward", {}, {}},
			{"control_move_back", {}, {}},
			{"control_move_left", {}, {}},
			{"control_move_right", {}, {}},
			{"control_move_up", {}, {}},
			{"control_move_down", {}, {}},
			{"control_pause", {}, {}},
			{"back", {}, {}}
		};

		ctx.m_keyboard_config_menu_container = build_two_column_menu(ctx, items);

		auto setup_keyboard_control_mapper = [&ctx](auto& element, auto& action_map, auto& action, const auto& control_name)
		{
			element.set_min_value(0.0f);
			element.set_max_value(1.0f);
			element.set_step(1.0f);
			element.set_value(1.0f);
			element.set_text_refresher(std::bind_front(&get_keyboard_mapping_string, std::ref(ctx), std::ref(action_map), std::ref(action)));
			element.set_value_changed_callback(std::bind_front(&start_mapping_keyboard_control, std::ref(ctx), std::ref(action_map), std::ref(action), control_name));
		};

		setup_keyboard_control_mapper(*std::get<2>(items[0]), ctx.movement_action_map, ctx.move_forward_action, "control_move_forward");
		setup_keyboard_control_mapper(*std::get<2>(items[1]), ctx.movement_action_map, ctx.move_back_action, "control_move_back");
		setup_keyboard_control_mapper(*std::get<2>(items[2]), ctx.movement_action_map, ctx.move_left_action, "control_move_left");
		setup_keyboard_control_mapper(*std::get<2>(items[3]), ctx.movement_action_map, ctx.move_right_action, "control_move_right");
		setup_keyboard_control_mapper(*std::get<2>(items[4]), ctx.movement_action_map, ctx.move_up_action, "control_move_up");
		setup_keyboard_control_mapper(*std::get<2>(items[5]), ctx.movement_action_map, ctx.move_down_action, "control_move_down");
		setup_keyboard_control_mapper(*std::get<2>(items[6]), ctx.movement_action_map, ctx.pause_action, "control_pause");

		auto& back_label = *std::get<1>(items.back());
		back_label.set_mouse_button_pressed_callback
		(
			[&](const auto&)
			{
				fade_out_menu
				(
					ctx,
					[&ctx](auto&)
					{
						ctx.function_queue.emplace
						(
							[&ctx]()
							{
								close_menu(ctx, ctx.m_keyboard_config_menu_container);
								open_menu(ctx, ctx.m_controls_menu_container);
							}
						);
					}
				);
			}
		);
	}

	void build_gamepad_config_menu(::game& ctx)
	{
		std::vector<std::tuple<std::string, std::shared_ptr<ui::label>, std::shared_ptr<ui::range>>> items =
		{
			{"control_move_forward", {}, {}},
			{"control_move_back", {}, {}},
			{"control_move_left", {}, {}},
			{"control_move_right", {}, {}},
			{"control_move_up", {}, {}},
			{"control_move_down", {}, {}},
			{"control_pause", {}, {}},
			{"back", {}, {}}
		};

		ctx.m_gamepad_config_menu_container = build_two_column_menu(ctx, items);

		auto setup_gamepad_control_mapper = [&ctx](auto& element, auto& action_map, auto& action, const auto& control_name)
		{
			element.set_min_value(0.0f);
			element.set_max_value(1.0f);
			element.set_step(1.0f);
			element.set_value(1.0f);
			element.set_text_refresher(std::bind_front(&get_gamepad_mapping_string, std::ref(ctx), std::ref(action_map), std::ref(action)));
			element.set_value_changed_callback(std::bind_front(&start_mapping_gamepad_control, std::ref(ctx), std::ref(action_map), std::ref(action), control_name));
		};

		setup_gamepad_control_mapper(*std::get<2>(items[0]), ctx.movement_action_map, ctx.move_forward_action, "control_move_forward");
		setup_gamepad_control_mapper(*std::get<2>(items[1]), ctx.movement_action_map, ctx.move_back_action, "control_move_back");
		setup_gamepad_control_mapper(*std::get<2>(items[2]), ctx.movement_action_map, ctx.move_left_action, "control_move_left");
		setup_gamepad_control_mapper(*std::get<2>(items[3]), ctx.movement_action_map, ctx.move_right_action, "control_move_right");
		setup_gamepad_control_mapper(*std::get<2>(items[4]), ctx.movement_action_map, ctx.move_up_action, "control_move_up");
		setup_gamepad_control_mapper(*std::get<2>(items[5]), ctx.movement_action_map, ctx.move_down_action, "control_move_down");
		setup_gamepad_control_mapper(*std::get<2>(items[6]), ctx.movement_action_map, ctx.pause_action, "control_pause");

		auto& back_label = *std::get<1>(items.back());
		back_label.set_mouse_button_pressed_callback
		(
			[&](const auto&)
			{
				fade_out_menu
				(
					ctx,
					[&ctx](auto&)
					{
						ctx.function_queue.emplace
						(
							[&ctx]()
							{
								close_menu(ctx, ctx.m_gamepad_config_menu_container);
								open_menu(ctx, ctx.m_controls_menu_container);
							}
						);
					}
				);
			}
		);
	}

	void build_graphics_menu(::game& ctx)
	{
		std::vector<std::tuple<std::string, std::shared_ptr<ui::label>, std::shared_ptr<ui::range>>> items =
		{
			{"graphics_menu_fullscreen", {}, {}},
			{"graphics_menu_resolution", {}, {}},
			{"graphics_menu_v_sync", {}, {}},
			{"graphics_menu_font_scale", {}, {}},
			{"graphics_menu_dyslexia_font", {}, {}},
			{"back", {}, {}}
		};

		ctx.m_graphics_menu_container = build_two_column_menu(ctx, items);

		auto& fullscreen_range = *std::get<2>(items[0]);
		fullscreen_range.set_min_value(0.0f);
		fullscreen_range.set_max_value(1.0f);
		fullscreen_range.set_step(1.0f);
		fullscreen_range.set_value(ctx.window->is_fullscreen());
		fullscreen_range.set_text_refresher(std::bind_front(&boolean_range_text_refresher, std::ref(ctx)));
		fullscreen_range.set_value_changed_callback
		(
			[&ctx](const auto& event)
			{
				ctx.window->set_fullscreen(static_cast<bool>(event.value));
				return ctx.window->is_fullscreen() == static_cast<bool>(event.value);
			}
		);

		auto& resolution_range = *std::get<2>(items[1]);
		resolution_range.set_min_value(0.25f);
		resolution_range.set_max_value(2.0f);
		resolution_range.set_step(0.25f);
		resolution_range.set_value(ctx.render_scale);
		resolution_range.set_text_refresher(std::bind_front(&percentage_range_text_refresher));
		resolution_range.set_value_changed_callback
		(
			[&ctx](const auto& event)
			{
				ctx.render_scale = event.value;
				::graphics::change_render_resolution(ctx, ctx.render_scale);
				(*ctx.settings)["render_scale"] = ctx.render_scale;
				return true;
			}
		);

		auto& v_sync_range = *std::get<2>(items[2]);
		v_sync_range.set_min_value(0.0f);
		v_sync_range.set_max_value(1.0f);
		v_sync_range.set_step(1.0f);
		v_sync_range.set_value(ctx.window->get_v_sync());
		v_sync_range.set_text_refresher(std::bind_front(&boolean_range_text_refresher, std::ref(ctx)));
		v_sync_range.set_value_changed_callback
		(
			[&ctx](const auto& event)
			{
				ctx.window->set_v_sync(static_cast<bool>(event.value));
				const bool v_sync = ctx.window->get_v_sync();
				(*ctx.settings)["v_sync"] = v_sync;

				return v_sync == static_cast<bool>(event.value);
			}
		);

		auto& font_size_range = *std::get<2>(items[3]);
		font_size_range.set_min_value(0.5f);
		font_size_range.set_max_value(2.0f);
		font_size_range.set_step(0.1f);
		font_size_range.set_value(ctx.font_scale);
		font_size_range.set_text_refresher(std::bind_front(&percentage_range_text_refresher));
		font_size_range.set_value_changed_callback
		(
			[&ctx](const auto& event)
			{
				ctx.font_scale = event.value;
				(*ctx.settings)["font_scale"] = ctx.font_scale;

				auto old_menu_font = ctx.menu_font;
				auto old_title_font = ctx.title_font;

				::load_fonts(ctx);

				update_menu_font(ctx, old_menu_font, old_title_font);
				refresh_menu_text(ctx);

				return true;
			}
		);

		auto& dyslexia_font_range = *std::get<2>(items[4]);
		dyslexia_font_range.set_min_value(0.0f);
		dyslexia_font_range.set_max_value(1.0f);
		dyslexia_font_range.set_step(1.0f);
		dyslexia_font_range.set_value(ctx.dyslexia_font);
		dyslexia_font_range.set_text_refresher(std::bind_front(&boolean_range_text_refresher, std::ref(ctx)));
		dyslexia_font_range.set_value_changed_callback
		(
			[&ctx](const auto& event)
			{
				ctx.dyslexia_font = static_cast<bool>(event.value);
				(*ctx.settings)["dyslexia_font"] = ctx.dyslexia_font;

				auto old_menu_font = ctx.menu_font;
				auto old_title_font = ctx.title_font;

				::load_fonts(ctx);

				update_menu_font(ctx, old_menu_font, old_title_font);

				return true;
			}
		);

		auto& back_label = *std::get<1>(items.back());
		back_label.set_mouse_button_pressed_callback
		(
			[&](const auto&)
			{
				fade_out_menu
				(
					ctx,
					[&ctx](auto&)
					{
						ctx.function_queue.emplace
						(
							[&ctx]()
							{
								close_menu(ctx, ctx.m_graphics_menu_container);
								open_menu(ctx, ctx.m_options_menu_container);
							}
						);
					}
				);
			}
		);
	}

	void build_sound_menu(::game& ctx)
	{
		std::vector<std::tuple<std::string, std::shared_ptr<ui::label>, std::shared_ptr<ui::range>>> items =
		{
			{"sound_menu_master_volume", {}, {}},
			{"sound_menu_ambience_volume", {}, {}},
			{"sound_menu_effects_volume", {}, {}},
			{"back", {}, {}}
		};

		ctx.m_sound_menu_container = build_two_column_menu(ctx, items);

		auto& master_volume_range = *std::get<2>(items[0]);
		master_volume_range.set_value(ctx.master_volume);
		master_volume_range.set_text_refresher(std::bind_front(&percentage_range_text_refresher));
		master_volume_range.set_value_changed_callback
		(
			[&ctx](const auto& event)
			{
				ctx.master_volume = event.value;
				ctx.sound_system->get_listener().set_gain(ctx.master_volume);
				(*ctx.settings)["master_volume"] = ctx.master_volume;
				ctx.menu_down_sound->play();
				return true;
			}
		);

		auto& ambience_volume_range = *std::get<2>(items[1]);
		ambience_volume_range.set_value(ctx.ambience_volume);
		ambience_volume_range.set_text_refresher(std::bind_front(&percentage_range_text_refresher));
		ambience_volume_range.set_value_changed_callback
		(
			[&ctx](const auto& event)
			{
				ctx.ambience_volume = event.value;
				(*ctx.settings)["ambience_volume"] = ctx.ambience_volume;
				return true;
			}
		);

		auto& effects_volume_range = *std::get<2>(items[2]);
		effects_volume_range.set_value(ctx.effects_volume);
		effects_volume_range.set_text_refresher(std::bind_front(&percentage_range_text_refresher));
		effects_volume_range.set_value_changed_callback
		(
			[&ctx](const auto& event)
			{
				ctx.effects_volume = event.value;
				(*ctx.settings)["effects_volume"] = ctx.effects_volume;
				return true;
			}
		);

		auto& back_label = *std::get<1>(items.back());
		back_label.set_mouse_button_pressed_callback
		(
			[&](const auto&)
			{
				fade_out_menu
				(
					ctx,
					[&ctx](auto&)
					{
						ctx.function_queue.emplace
						(
							[&ctx]()
							{
								close_menu(ctx, ctx.m_sound_menu_container);
								open_menu(ctx, ctx.m_options_menu_container);
							}
						);
					}
				);
			}
		);
	}

	void build_language_menu(::game& ctx)
	{
		std::vector<std::tuple<std::string, std::shared_ptr<ui::label>, std::shared_ptr<ui::range>>> items =
		{
			{"language_menu_language", {}, {}},
			{"back", {}, {}}
		};

		ctx.m_language_menu_container = build_two_column_menu(ctx, items);

		auto& language_range = *std::get<2>(items[0]);
		language_range.set_min_value(0.0f);
		language_range.set_max_value(static_cast<float>(ctx.languages->size() - 1));
		language_range.set_step(1.0f);
		language_range.set_value(static_cast<float>(std::distance(ctx.languages->begin(), ctx.languages->find(ctx.language_tag))));
		language_range.set_text_refresher
		(
			[&ctx](const auto&)
			{
				return (*ctx.languages)[ctx.language_tag]["name"].get_ref<const std::string&>();
			}
		);
		language_range.set_value_changed_callback
		(
			[&ctx](const auto& event)
			{
				const auto index = static_cast<std::size_t>(std::round(event.value));
				auto language_it = ctx.languages->begin();
				std::advance(language_it, index);

				ctx.language_tag = language_it.key();

				// Load language strings
				ctx.string_map = ctx.resource_manager->load<json>(std::format("localization/strings.{}.json", ctx.language_tag));

				// Update language tag settings
				(*ctx.settings)["language_tag"] = ctx.language_tag;

				// Log language tag
				debug::log_info("Language tag: {}", ctx.language_tag);

				
				auto old_menu_font = ctx.menu_font;
				auto old_title_font = ctx.title_font;

				::load_fonts(ctx);

				update_menu_font(ctx, old_menu_font, old_title_font);
				refresh_menu_text(ctx);

				return true;
			}
		);

		auto& back_label = *std::get<1>(items.back());
		back_label.set_mouse_button_pressed_callback
		(
			[&](const auto&)
			{
				fade_out_menu
				(
					ctx,
					[&ctx](auto&)
					{
						ctx.function_queue.emplace
						(
							[&ctx]()
							{
								close_menu(ctx, ctx.m_language_menu_container);
								open_menu(ctx, ctx.m_options_menu_container);
							}
						);
					}
				);
			}
		);
	}

	void build_extras_menu(::game& ctx)
	{
		auto credits_callback = [&](const auto&)
		{
			fade_out_menu
			(
				ctx,
				[&ctx](auto&)
				{
					ctx.function_queue.emplace
					(
						[&ctx]()
						{
							close_menu(ctx, ctx.m_extras_menu_container);
							open_menu(ctx, ctx.m_credits_menu_container);
						}
					);
				}
			);
		};

		auto back_callback = [&](const auto&)
		{
			fade_out_menu
			(
				ctx,
				[&ctx](auto&)
				{
					ctx.function_queue.emplace
					(
						[&ctx]()
						{
							close_menu(ctx, ctx.m_extras_menu_container);
							open_main_menu(ctx, false);
						}
					);
				}
			);
		};

		ctx.m_extras_menu_container = build_menu
		(
			ctx,
			{
				{"extras_menu_credits", credits_callback},
				{"back", back_callback}
			},
			true,
			{0.5f, 0.5f, 0.5f, 0.5f}
		);
	}

	void build_credits_menu(::game& ctx)
	{
		auto back_callback = [&](const auto&)
		{
			fade_out_menu
			(
				ctx,
				[&ctx](auto&)
				{
					ctx.function_queue.emplace
					(
						[&ctx]()
						{
							close_menu(ctx, ctx.m_credits_menu_container);
							open_menu(ctx, ctx.m_extras_menu_container);
						}
					);
				}
			);
		};

		ctx.m_credits_menu_container = build_menu
		(
			ctx,
			{
				{"credits", nullptr},
				{"back", back_callback}
			},
			true,
			{0.5f, 0.5f, 0.5f, 0.5f}
		);
	}

	void build_menu_animations(::game& ctx)
	{
		auto set_menu_opacity = [&](auto samples, auto&)
		{
			ctx.m_root_menu_container->set_opacity(samples[0]);
		};

		// Construct fade in animation
		{
			ctx.m_menu_fade_in_sequence = std::make_shared<animation_sequence>();
			auto& opacity_track = ctx.m_menu_fade_in_sequence->tracks()["opacity"];
			auto& opacity_channel = opacity_track.channels().emplace_back();
			opacity_channel.keyframes().emplace(0.0f, 0.0f);
			opacity_channel.keyframes().emplace(menu_fade_in_duration, 1.0f);
			opacity_track.output() = set_menu_opacity;
		}

		// Construct fade out animation
		{
			ctx.m_menu_fade_out_sequence = std::make_shared<animation_sequence>();
			auto& opacity_track = ctx.m_menu_fade_out_sequence->tracks()["opacity"];
			auto& opacity_channel = opacity_track.channels().emplace_back();
			opacity_channel.keyframes().emplace(0.0f, 1.0f);
			opacity_channel.keyframes().emplace(menu_fade_out_duration, 0.0f);
			opacity_track.output() = set_menu_opacity;
		}

		// Construct menu animation entity
		ctx.m_menu_animation_entity = ctx.entity_registry->create();
		ctx.entity_registry->emplace<animation_component>(ctx.m_menu_animation_entity);

		auto set_pause_menu_bg_opacity = [&](auto samples, auto&)
		{
			auto color = pause_menu_bg_color;
			color[3] *= samples[0];
			ctx.m_pause_menu_bg_color->set(color);
		};

		// Construct fade in animation
		{
			ctx.m_pause_menu_bg_fade_in_sequence = std::make_shared<animation_sequence>();
			auto& opacity_track = ctx.m_pause_menu_bg_fade_in_sequence->tracks()["opacity"];
			auto& opacity_channel = opacity_track.channels().emplace_back();
			opacity_channel.keyframes().emplace(0.0f, 0.0f);
			opacity_channel.keyframes().emplace(menu_fade_in_duration, 1.0f);
			opacity_track.output() = set_pause_menu_bg_opacity;
		}

		// Construct fade out animation
		{
			ctx.m_pause_menu_bg_fade_out_sequence = std::make_shared<animation_sequence>();
			auto& opacity_track = ctx.m_pause_menu_bg_fade_out_sequence->tracks()["opacity"];
			auto& opacity_channel = opacity_track.channels().emplace_back();
			opacity_channel.keyframes().emplace(0.0f, 1.0f);
			opacity_channel.keyframes().emplace(menu_fade_out_duration, 0.0f);
			opacity_track.output() = set_pause_menu_bg_opacity;

			ctx.m_pause_menu_bg_fade_out_sequence->cues().emplace
			(
				ctx.m_pause_menu_bg_fade_out_sequence->duration(),
				[&ctx](auto&)
				{
					ctx.ui_canvas->remove_child(ctx.m_pause_menu_bg);
				}
			);
		}

		// Construct pause menu bg animation entity
		ctx.m_pause_menu_bg_animation_entity = ctx.entity_registry->create();
		ctx.entity_registry->emplace<animation_component>(ctx.m_pause_menu_bg_animation_entity);
	}
}

void build_menus(::game& ctx)
{
	ctx.m_root_menu_container = std::make_shared<ui::margin_container>();
	ctx.m_root_menu_container->set_anchors(0.0f, 0.0f, 1.0f, 1.0f);
	ctx.ui_canvas->add_child(ctx.m_root_menu_container);

	build_main_menu(ctx);
	build_pause_menu(ctx);
	build_options_menu(ctx);
	build_controls_menu(ctx);
	build_keyboard_config_menu(ctx);
	build_gamepad_config_menu(ctx);
	build_graphics_menu(ctx);
	build_sound_menu(ctx);
	build_language_menu(ctx);
	build_extras_menu(ctx);
	build_credits_menu(ctx);
	refresh_menu_text(ctx);
	build_menu_animations(ctx);
}

void open_main_menu(::game& ctx, bool fade_in)
{
	debug::precondition(ctx.m_main_menu_container != nullptr);

	// Add camera to scene
	ctx.exterior_scene->add_object(*ctx.exterior_camera);

	// Disable sky pass
	ctx.sky_pass->set_enabled(false);

	ctx.m_root_menu_container->add_child(ctx.m_main_menu_container);

	if (auto focused_element = ctx.m_menu_focused_element.lock())
	{
		focused_element->set_focus(true);
	}

	if (fade_in)
	{
		fade_in_to(ctx, nullptr);
		fade_in_menu(ctx, nullptr);
	}
	else
	{
		fade_in_menu(ctx, nullptr);
	}
}

void close_main_menu(::game& ctx)
{
	close_menu(ctx, ctx.m_main_menu_container);
}

void open_pause_menu(::game& ctx)
{
	open_menu(ctx, ctx.m_pause_menu_container);

	if (!ctx.m_pause_menu_bg->get_parent().lock())
	{
		ctx.ui_canvas->add_child(ctx.m_pause_menu_bg);
		fade_in_pause_menu_bg(ctx);
	}
}

void close_pause_menu(::game& ctx)
{
	close_menu(ctx, ctx.m_pause_menu_container);
}
