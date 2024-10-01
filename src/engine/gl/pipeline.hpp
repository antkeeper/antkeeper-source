// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GL_PIPELINE_HPP
#define ANTKEEPER_GL_PIPELINE_HPP

#include <engine/gl/pipeline-viewport-state.hpp>
#include <engine/gl/pipeline-rasterization-state.hpp>
#include <engine/gl/pipeline-depth-stencil-state.hpp>
#include <engine/gl/pipeline-input-assembly-state.hpp>
#include <engine/gl/pipeline-vertex-input-state.hpp>
#include <engine/gl/pipeline-color-blend-state.hpp>
#include <engine/gl/vertex-array.hpp>
#include <engine/gl/vertex-buffer.hpp>
#include <engine/gl/clear-value.hpp>
#include <engine/gl/framebuffer.hpp>
#include <engine/gl/shader-program.hpp>
#include <engine/gl/clear-bits.hpp>
#include <engine/gl/stencil-face-bits.hpp>
#include <array>
#include <cstdint>
#include <span>

namespace app { class sdl_window_manager; }

namespace gl {

/// Graphics pipeline interface.
class pipeline
{
public:
	/// Constructs a pipeline.
	pipeline();
	
	/// @name Vertex input state
	/// @{
	
	// Sets the vertex input.
	// @param vertex_bindings Vertex bindings.
	// @param vertex_attributes Vertex attributes.
	// void set_vertex_input(std::span<const vertex_input_binding> vertex_bindings, std::span<const vertex_input_attribute> vertex_attributes);
	
	/// Binds a framebuffer.
	/// @param framebuffer Framebuffer to bind.
	void bind_framebuffer(const gl::framebuffer* framebuffer);

	/// Binds a shader program.
	/// @param shader_program Shader program to bind.
	void bind_shader_program(const gl::shader_program* shader_program);
	
	/// Binds a vertex array.
	/// @param array Vertex array to bind.
	void bind_vertex_array(const vertex_array* array);
	
	/// Binds vertex buffers.
	/// @param first_binding Index of the first vertex input binding.
	/// @param buffers Sequence of buffers to bind.
	/// @param offsets Sequence of byte offsets into each buffer.
	/// @param strides Sequence of byte strides between consecutive elements within each buffer.
	void bind_vertex_buffers(std::uint32_t first_binding, std::span<const vertex_buffer* const> buffers, std::span<const std::size_t> offsets, std::span<const std::size_t> strides);
	
	/// @}
	
	/// @name Input assembly state
	/// @{
	
	/// Sets the primitive topology to use for drawing.
	/// @param topology Primitive topology to use for drawing.
	void set_primitive_topology(primitive_topology topology);
	
	/// Controls whether a special vertex index value is treated as restarting the assembly of primitives. 
	/// @param enabled `true` if a special vertex index value should restart the assembly of primitives, `false` otherwise.
	void set_primitive_restart_enabled(bool enabled);
	
	/// @}
	
	/// @name Viewport state
	/// @{
	
	/// Sets one or more viewports.
	/// @param first_viewport Index of the first viewport to set.
	/// @param viewports Sequence of viewports.
	/// @exception std::out_of_range Viewport index out of range.
	/// @warning Currently only a single viewport is supported.
	void set_viewport(std::uint32_t first_viewport, std::span<const viewport> viewports);
	
	/// Sets one or more scissor regions.
	/// @param first_scissor Index of the first scissor region to set.
	/// @param scissors Sequence of scissor regions.
	/// @exception std::out_of_range Scissor region index out of range.
	/// @warning Currently only a single scissor region is supported.
	void set_scissor(std::uint32_t first_scissor, std::span<const scissor_region> scissors);
	
	/// @}
	
	/// @name Rasterizer state
	/// @{
	
	/// Controls whether primitives are discarded before the rasterization stage.
	/// @param enabled `true` if primitives should be discarded before the rasterization stage, `false` otherwise.
	void set_rasterizer_discard_enabled(bool enabled);
	
	/// Sets the polygon rasterization mode.
	/// @param mode Polygon rasterization mode.
	void set_fill_mode(fill_mode mode);
	
	/// Sets the triangle culling mode.
	/// @param mode Triangle culling mode.
	void set_cull_mode(cull_mode mode);
	
	/// Sets the front-facing triangle orientation.
	/// @param face Front-facing triangle orientation.
	void set_front_face(front_face face);
	
	/// Controls whether to bias fragment depth values.
	/// @param enabled `true` if fragment depth values should be biased, `false` otherwise.
	void set_depth_bias_enabled(bool enabled);
	
	/// Sets depth bias factors.
	/// @param constant_factor Scalar factor controlling the constant depth value added to each fragment.
	/// @param slope_factor Scalar factor applied to a fragment's slope in depth bias calculations.
	void set_depth_bias_factors(float constant_factor, float slope_factor);
	
	/// Controls whether depth clamping is enabled.
	/// @param enabled `true` if depth clamping should be enabled, `false` otherwise.
	void set_depth_clamp_enabled(bool enabled);
	
	/// Enables or disables scissor testing.
	/// @param enabled `true` if scissor testing should be enabled, `false` otherwise.
	void set_scissor_test_enabled(bool enabled);
	
	/// Sets the vertex to be used as the source of data for flat-shaded varyings.
	/// @param mode Provoking vertex mode.
	void set_provoking_vertex_mode(provoking_vertex_mode mode);
	
	/// Sets the the diameter of rasterized points.
	/// @param size Point size.
	void set_point_size(float size);
	
	/// Sets the width of rasterized lines.
	/// @param width Width of rasterized line segments.
	void set_line_width(float width);
	
	/// @}
	
	/// @name Depth/stencil state
	/// @{
	
	/// Controls whether depth testing is enabled.
	/// @param enabled `true` if depth testing should be enabled, `false` otherwise.
	void set_depth_test_enabled(bool enabled);
	
	/// Controls whether depth writes are enabled.
	/// @param enabled `true` if depth writes should be enabled when depth testing is enabled, `false` otherwise.
	/// @note Depth writes are always disabled when depth testing is disabled.
	void set_depth_write_enabled(bool enabled);
	
	/// Sets the depth comparison operator.
	/// @param compare_op Comparison operator to use in the depth comparison step of the depth test.
	void set_depth_compare_op(gl::compare_op compare_op);
	
	/// Controls whether stencil testing is enabled.
	/// @param enabled `true` if stencil testing should be enabled, `false` otherwise.
	void set_stencil_test_enabled(bool enabled);
	
	/// Sets the stencil operations.
	/// @param face_mask Bit mask specifying the set of stencil states for which to update the stencil operation.
	/// @param fail_op Action performed on samples that fail the stencil test.
	/// @param pass_op Action performed on samples that pass both the depth and stencil tests.
	/// @param depth_fail_op Action performed on samples that pass the stencil test and fail the depth test.
	/// @param compare_op Comparison operator used in the stencil test.
	void set_stencil_op(std::uint8_t face_mask, stencil_op fail_op, stencil_op pass_op, stencil_op depth_fail_op, gl::compare_op compare_op);
	
	/// Sets the stencil compare mask.
	/// @param face_mask Bit mask specifying the set of stencil states for which to update the compare mask.
	/// @param compare_mask New value to use as the stencil compare mask.
	void set_stencil_compare_mask(std::uint8_t face_mask, std::uint32_t compare_mask);
	
	/// Sets the stencil reference value.
	/// @param face_mask Bit mask specifying the set of stencil states for which to update the reference value.
	/// @param reference New value to use as the stencil reference value.
	void set_stencil_reference(std::uint8_t face_mask, std::uint32_t reference);
	
	/// Sets the stencil write mask.
	/// @param face_mask Bit mask specifying the set of stencil states for which to update the write mask.
	/// @param write_mask New value to use as the stencil write mask.
	void set_stencil_write_mask(std::uint8_t face_mask, std::uint32_t write_mask);
	
	/// @}
	
	/// @name Color blend state
	/// @{
	
	/// Controls whether whether logical operations are enabled.
	/// @param enabled `true` if logical operations should be enabled, `false` otherwise.
	void set_logic_op_enabled(bool enabled);
	
	/// Selects which logical operation to apply.
	/// @param logic_op Logical operation to apply.
	void set_logic_op(gl::logic_op logic_op);
	
	/// Controls whether blending is enabled for the corresponding color attachment.
	/// @param enabled `true` if color blending should be enabled, `false` otherwise.
	void set_color_blend_enabled(bool enabled);
	
	/// Sets the color blend factors and operations.
	/// @param equation Color blend factors and operations.
	void set_color_blend_equation(const color_blend_equation& equation);
	
	/// Sets the color write mask.
	/// @param mask Bitmask indicating which of the RGBA components are enabled for writing.
	void set_color_write_mask(std::uint8_t mask);
	
	/// Sets the values of the blend constants.
	/// @param blend_constants RGBA components of the blend constant that are used in blending, depending on the blend factor.
	void set_blend_constants(const std::array<float, 4>& blend_constants);
	
	/// @}
	
	/// @name Drawing
	/// @{
	
	/// Draws primitives.
	/// @param vertex_count Number of vertices to draw.
	/// @param instance_count Number of instances to draw.
	/// @param first_vertex Index of the first vertex to draw.
	/// @param first_instance Instance ID of the first instance to draw. (WARNING: not currently supported)
	/// @warning @p first_instance currently not supported.
	void draw(std::uint32_t vertex_count, std::uint32_t instance_count, std::uint32_t first_vertex, std::uint32_t first_instance);
	
	/// Draws primitives with indexed vertices.
	/// @param index_count Number of vertices to draw.
	/// @param instance_count Number of instances to draw.
	/// @param first_index Base index within the index buffer.
	/// @param vertex_offset Value added to the vertex index before indexing into the vertex buffer.
	/// @param first_instance Instance ID of the first instance to draw.
	/// @warning @p vertex_offset currently not supported.
	void draw_indexed(std::uint32_t index_count, std::uint32_t instance_count, std::uint32_t first_index, std::int32_t vertex_offset, std::uint32_t first_instance);
	
	/// @}
	
	/// @name Clear
	/// @{
	
	/// Clears the color, depth, or stencil buffers of current attachments.
	/// @param mask Bit mask indicating which buffers should be cleared.
	/// @param value Color, depth, and stencil values with which to fill the cleared attachments.
	void clear_attachments(std::uint8_t mask, const clear_value& value);
	
	/// @}
	
	/// @name Limitations
	/// @{
	
	/// Returns the dimensions of the default framebuffer.
	[[nodiscard]] inline constexpr const std::array<std::uint32_t, 2>& get_default_framebuffer_dimensions() const noexcept
	{
		return m_default_framebuffer_dimensions;
	}
	
	/// Returns the maximum number of supported viewports.
	[[nodiscard]] inline constexpr std::uint32_t get_max_viewports() const noexcept
	{
		return m_max_viewports;
	}
	
	/// Returns the maximum supported degree of sampler anisotropy.
	[[nodiscard]] inline constexpr float get_max_sampler_anisotropy() const noexcept
	{
		return m_max_sampler_anisotropy;
	}
	
	/// @}
	
private:
	friend class app::sdl_window_manager;
	
	/// Changes the reported dimensions of the default framebuffer.
	void defaut_framebuffer_resized(std::uint32_t width, std::uint32_t height) noexcept;
	
	void fetch_vertex_input_state();
	void fetch_input_assembly_state();
	void fetch_viewport_state();
	void fetch_rasterization_state();
	void fetch_depth_stencil_state();
	void fetch_color_blend_state();
	void fetch_clear_value();
	
	std::uint32_t m_max_viewports{1};
	float m_max_sampler_anisotropy{0.0f};
	std::array<std::uint32_t, 2> m_default_framebuffer_dimensions{0, 0};
	
	pipeline_vertex_input_state m_vertex_input_state;
	pipeline_input_assembly_state m_input_assembly_state;
	pipeline_viewport_state m_viewport_state;
	pipeline_rasterization_state m_rasterization_state;
	pipeline_depth_stencil_state m_depth_stencil_state;
	pipeline_color_blend_state m_color_blend_state;
	clear_value m_clear_value;
	
	unsigned int m_bound_gl_named_framebuffer{};
	unsigned int m_bound_gl_program_id{};
	unsigned int m_bound_gl_named_array{};
};

} // namespace gl

#endif // ANTKEEPER_GL_PIPELINE_HPP
