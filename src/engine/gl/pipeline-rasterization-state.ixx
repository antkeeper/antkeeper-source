// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.gl.pipeline_rasterization_state;
export import engine.gl.front_face;
export import engine.gl.cull_mode;
export import engine.gl.fill_mode;
export import engine.gl.provoking_vertex_mode;

export namespace engine::gl
{
	/// Pipeline rasterization state.
	struct pipeline_rasterization_state
	{
		/// `true` if rasterizer discard should be enabled, `false` otherwise.
		bool rasterizer_discard_enabled{false};

		/// Polygon rasterization mode.
		gl::fill_mode fill_mode{gl::fill_mode::fill};

		/// Triangle culling mode.
		gl::cull_mode cull_mode{gl::cull_mode::back};

		/// Polygon front-facing orientation.
		gl::front_face front_face{gl::front_face::counter_clockwise};

		/// `true` if depth bias should be enabled, `false` otherwise.
		bool depth_bias_enabled{false};

		/// Depth bias constant factor.
		float depth_bias_constant_factor{0.0f};

		/// Depth bias slope factor.
		float depth_bias_slope_factor{0.0f};

		/// `true` if depth clamp should be enabled, `false` otherwise.
		bool depth_clamp_enabled{false};

		/// `true` if scissor testing should be enabled, `false` otherwise.
		bool scissor_test_enabled{false};

		/// Vertex to be used as the source of data for flat-shaded varyings.
		gl::provoking_vertex_mode provoking_vertex_mode{gl::provoking_vertex_mode::last};

		/// Diameter of rasterized points.
		float point_size{1.0f};

		/// Width of rasterized line segments.
		float line_width{1.0f};
	};
}
