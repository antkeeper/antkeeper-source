// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/gl/pipeline.hpp>
#include <engine/gl/clear-bits.hpp>
#include <engine/gl/stencil-face-bits.hpp>
#include <engine/gl/color-component-bits.hpp>
#include <engine/debug/log.hpp>
#include <glad/gl.h>
#include <algorithm>
#include <stdexcept>
#include <bit>
#include <stacktrace>

namespace {

	static constexpr GLenum stencil_face_lut[] =
	{
		GL_NONE,          // 0
		GL_FRONT,         // stencil_face_front_bit
		GL_BACK,          // stencil_face_back_bit
		GL_FRONT_AND_BACK // stencil_face_front_and_back
	};
	
	static constexpr GLenum stencil_op_lut[] =
	{
		GL_KEEP,      // stencil_op::keep
		GL_ZERO,      // stencil_op::zero
		GL_REPLACE,   // stencil_op::replace
		GL_INCR,      // stencil_op::increment_and_clamp
		GL_DECR,      // stencil_op::decrement_and_clamp
		GL_INVERT,    // stencil_op::invert
		GL_INCR_WRAP, // stencil_op::increment_and_wrap
		GL_DECR_WRAP  // stencil_op::decrement_and_wrap
	};
	
	static constexpr GLenum compare_op_lut[] =
	{
		GL_NEVER,    // compare_op::never
		GL_LESS,     // compare_op::less
		GL_EQUAL,    // compare_op::equal
		GL_LEQUAL,   // compare_op::less_or_equal
		GL_GREATER,  // compare_op::greater
		GL_NOTEQUAL, // compare_op::not_equal
		GL_GEQUAL,   // compare_op::greater_or_equal
		GL_ALWAYS    // compare_op::always
	};
	
	static constexpr GLenum provoking_vertex_mode_lut[] =
	{
		GL_FIRST_VERTEX_CONVENTION, // provoking_vertex_mode::first
		GL_LAST_VERTEX_CONVENTION   // provoking_vertex_mode::last
	};
	
	static constexpr GLenum primitive_topology_lut[] =
	{
		GL_POINTS,                   // primitive_topology::point_list
		GL_LINES,                    // primitive_topology::line_list
		GL_LINE_STRIP,               // primitive_topology::line_strip,
		GL_TRIANGLES,                // primitive_topology::triangle_list
		GL_TRIANGLE_STRIP,           // primitive_topology::triangle_strip
		GL_TRIANGLE_FAN,             // primitive_topology::triangle_fan
		GL_LINES_ADJACENCY,          // primitive_topology::line_list_with_adjacency
		GL_LINE_STRIP_ADJACENCY,     // primitive_topology::line_strip_with_adjacency
		GL_TRIANGLES_ADJACENCY,      // primitive_topology::triangle_list_with_adjacency
		GL_TRIANGLE_STRIP_ADJACENCY, // primitive_topology::triangle_strip_with_adjacency
		GL_PATCHES	                 // primitive_topology::patch_list
	};
	
	static constexpr GLenum logic_op_lut[] =
	{
		GL_CLEAR        , // logic_op::bitwise_clear
		GL_AND          , // logic_op::bitwise_and
		GL_AND_REVERSE  , // logic_op::bitwise_and_reverse
		GL_COPY         , // logic_op::bitwise_copy
		GL_AND_INVERTED , // logic_op::bitwise_and_inverted
		GL_NOOP         , // logic_op::bitwise_no_op
		GL_XOR          , // logic_op::bitwise_xor
		GL_OR           , // logic_op::bitwise_or
		GL_NOR          , // logic_op::bitwise_nor
		GL_EQUIV        , // logic_op::bitwise_equivalent
		GL_INVERT       , // logic_op::bitwise_invert
		GL_OR_REVERSE   , // logic_op::bitwise_or_reverse
		GL_COPY_INVERTED, // logic_op::bitwise_copy_inverted
		GL_OR_INVERTED  , // logic_op::bitwise_or_inverted
		GL_NAND         , // logic_op::bitwise_nand
		GL_SET            // logic_op::bitwise_set
	};

	static constexpr GLenum blend_factor_lut[] =
	{
		GL_ZERO                    , // blend_factor::zero
		GL_ONE                     , // blend_factor::one
		GL_SRC_COLOR               , // blend_factor::src_color
		GL_ONE_MINUS_SRC_COLOR     , // blend_factor::one_minus_src_color
		GL_DST_COLOR               , // blend_factor::dst_color
		GL_ONE_MINUS_DST_COLOR     , // blend_factor::one_minus_dst_color
		GL_SRC_ALPHA               , // blend_factor::src_alpha
		GL_ONE_MINUS_SRC_ALPHA     , // blend_factor::one_minus_src_alpha
		GL_DST_ALPHA               , // blend_factor::dst_alpha
		GL_ONE_MINUS_DST_ALPHA     , // blend_factor::one_minus_dst_alpha
		GL_CONSTANT_COLOR          , // blend_factor::constant_color
		GL_ONE_MINUS_CONSTANT_COLOR, // blend_factor::one_minus_constant_color
		GL_CONSTANT_ALPHA          , // blend_factor::constant_alpha
		GL_ONE_MINUS_CONSTANT_ALPHA, // blend_factor::one_minus_constant_alpha
		GL_SRC_ALPHA_SATURATE      , // blend_factor::src_alpha_saturate
		GL_SRC1_COLOR              , // blend_factor::src1_color
		GL_ONE_MINUS_SRC1_COLOR    , // blend_factor::one_minus_src1_color
		GL_SRC1_ALPHA              , // blend_factor::src1_alpha
		GL_ONE_MINUS_SRC1_ALPHA      // blend_factor::one_minus_src1_alpha
	};
	
	static constexpr GLenum blend_op_lut[] =
	{
		GL_FUNC_ADD             , // blend_op::add
		GL_FUNC_SUBTRACT        , // blend_op::subtract
		GL_FUNC_REVERSE_SUBTRACT, // blend_op::reverse_subtract
		GL_MIN                  , // blend_op::min
		GL_MAX                    // blend_op::max
	};
	
	void gl_debug_message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, [[maybe_unused]] GLsizei length, const GLchar* message, [[maybe_unused]] const void* user_param)
	{
		const auto src_str = [source]() -> const char*
		{
			switch (source)
			{
				case GL_DEBUG_SOURCE_API:
					return "API";
				case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
					return "window system";
				case GL_DEBUG_SOURCE_SHADER_COMPILER:
					return "shader compiler";
				case GL_DEBUG_SOURCE_THIRD_PARTY:
					return "third party";
				case GL_DEBUG_SOURCE_APPLICATION:
					return "application";
				case GL_DEBUG_SOURCE_OTHER:
				default:
					return "other";
			}
		}();
		
		const auto type_str = [type]() -> const char*
		{
			switch (type)
			{
				case GL_DEBUG_TYPE_ERROR:
					return "error";
				case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
					return "deprecated behavior";
				case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
					return "undefined behavior";
				case GL_DEBUG_TYPE_PORTABILITY:
					return "portability";
				case GL_DEBUG_TYPE_PERFORMANCE:
					return "performance";
				case GL_DEBUG_TYPE_MARKER:
					return "marker";
				case GL_DEBUG_TYPE_OTHER:
				default:
					return "message";
			}
		}();
		
		const auto severity_str = [severity]() -> const char*
		{
			switch (severity)
			{
				case GL_DEBUG_SEVERITY_LOW:
					return "low severity";
				case GL_DEBUG_SEVERITY_MEDIUM:
					return "medium severity";
				case GL_DEBUG_SEVERITY_HIGH:
					return "high severity";
				case GL_DEBUG_SEVERITY_NOTIFICATION:
				default:
					return "notification";
			}
		}();
		
		switch (type)
		{
			case GL_DEBUG_TYPE_ERROR:
			{
				std::string formatted_message;
				std::format_to(std::back_inserter(formatted_message), "OpenGL {} {} ({}) {}: {}", src_str, type_str, severity_str, id, message);
				debug::log_fatal("{}\n{}", formatted_message, std::to_string(std::stacktrace::current()));
				throw std::runtime_error(formatted_message);
			}
			
			case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
			case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
				debug::log_error("OpenGL {} {} ({}) {}: {}", src_str, type_str, severity_str, id, message);
				break;
			
			case GL_DEBUG_TYPE_PORTABILITY:
			case GL_DEBUG_TYPE_PERFORMANCE:
				debug::log_warning("OpenGL {} {} ({}) {}: {}", src_str, type_str, severity_str, id, message);
				break;
			
			case GL_DEBUG_TYPE_MARKER:
			case GL_DEBUG_TYPE_OTHER:
			default:
				debug::log_debug("OpenGL {} {} ({}) {}: {}", src_str, type_str, severity_str, id, message);
				break;
		}
	}
}

namespace gl {

pipeline::pipeline()
{
	#if defined(DEBUG)
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(gl_debug_message_callback, nullptr);
	#endif
	
	// Fetch limitations
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &m_max_sampler_anisotropy);
	
	// Fetch dimensions of default framebuffer
	GLint gl_scissor_box[4] = {0, 0, 0, 0};
	glGetIntegerv(GL_SCISSOR_BOX, gl_scissor_box);
	m_default_framebuffer_dimensions[0] = static_cast<std::uint32_t>(gl_scissor_box[2]);
	m_default_framebuffer_dimensions[1] = static_cast<std::uint32_t>(gl_scissor_box[3]);
	
	// Enable seamless cubemap filtering
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	
	// Set clip control to lower left, 0 to 1
	glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE);
	
	// Disable multisampling
	glDisable(GL_MULTISAMPLE);
	
	// Set byte-alignment for packing and unpacking pixel rows
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	
	// Fetch pipeline state
	fetch_vertex_input_state();
	fetch_input_assembly_state();
	fetch_viewport_state();
	fetch_rasterization_state();
	fetch_depth_stencil_state();
	fetch_color_blend_state();
	fetch_clear_value();
}

// void pipeline::set_vertex_input(std::span<const vertex_input_binding> vertex_bindings, std::span<const vertex_input_attribute> vertex_attributes)
// {
	// m_vertex_input_state.vertex_bindings.assign(vertex_bindings.begin(), vertex_bindings.end());
	// m_vertex_input_state.vertex_attributes.assign(vertex_attributes.begin(), vertex_attributes.end());
// }

void pipeline::bind_framebuffer(const gl::framebuffer* framebuffer)
{
	if (framebuffer)
	{
		if (framebuffer->m_gl_named_framebuffer != m_bound_gl_named_framebuffer)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->m_gl_named_framebuffer);
			m_bound_gl_named_framebuffer = framebuffer->m_gl_named_framebuffer;
		}
	}
	else if (m_bound_gl_named_framebuffer)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		m_bound_gl_named_framebuffer = 0;
	}
}

void pipeline::bind_shader_program(const gl::shader_program* shader_program)
{
	if (shader_program)
	{
		if (shader_program->m_gl_program_id != m_bound_gl_program_id)
		{
			glUseProgram(shader_program->m_gl_program_id);
			m_bound_gl_program_id = shader_program->m_gl_program_id;
		}
	}
	else if (m_bound_gl_program_id)
	{
		glUseProgram(0);
		m_bound_gl_program_id = 0;
	}
}

void pipeline::bind_vertex_array(const vertex_array* array)
{
	m_bound_gl_named_array = array ? array->m_gl_named_array : 0;
	glBindVertexArray(m_bound_gl_named_array);
	
	/// @BUG
	/// 
	/// The following code was occasionally causing this exception:
	///
	/// > OpenGL API error (high severity) 1282: GL_INVALID_OPERATION error generated. Array object is not active.
	///
	/// Why?
	
	
	// if (array)
	// {
		// if (array->m_gl_named_array != m_bound_gl_named_array)
		// {
			// glBindVertexArray(array->m_gl_named_array);
			// m_bound_gl_named_array = array->m_gl_named_array;
		// }
	// }
	// else if (m_bound_gl_named_array)
	// {
		// glBindVertexArray(0);
		// m_bound_gl_named_array = 0;
	// }
}

void pipeline::bind_vertex_buffers(std::uint32_t first_binding, std::span<const vertex_buffer* const> buffers, std::span<const std::size_t> offsets, std::span<const std::size_t> strides)
{
	if (!m_bound_gl_named_array)
	{
		throw std::runtime_error("Failed to bind vertex buffer: no vertex array bound.");
	}
	
	if (offsets.size() < buffers.size())
	{
		throw std::out_of_range("Vertex binding offset out of range.");
	}
	
	if (strides.size() < buffers.size())
	{
		throw std::out_of_range("Vertex binding stride out of range.");
	}
	
	for (std::size_t i = 0; i < buffers.size(); ++i)
	{
		glVertexArrayVertexBuffer
		(
			m_bound_gl_named_array,
			static_cast<GLuint>(first_binding + i),
			buffers[i]->m_gl_named_buffer,
			static_cast<GLintptr>(offsets[i]),
			static_cast<GLsizei>(strides[i])
		);
	}
}

void pipeline::set_primitive_topology(primitive_topology topology)
{
	if (m_input_assembly_state.topology != topology)
	{
		m_input_assembly_state.topology = topology;
	}
}

void pipeline::set_primitive_restart_enabled(bool enabled)
{
	if (m_input_assembly_state.primitive_restart_enabled != enabled)
	{
		if (enabled)
		{
			glEnable(GL_PRIMITIVE_RESTART_FIXED_INDEX);
		}
		else
		{
			glDisable(GL_PRIMITIVE_RESTART_FIXED_INDEX);
		}
		
		m_input_assembly_state.primitive_restart_enabled = enabled;
	}
}

void pipeline::set_viewport(std::uint32_t first_viewport, std::span<const gl::viewport> viewports)
{
	// Bounds check
	if (first_viewport + viewports.size() > m_max_viewports)
	{
		throw std::out_of_range("Viewport index out of range.");
	}
	
	// Ignore empty commands
	if (!viewports.size())
	{
		return;
	}
	
	const auto& active_viewport = m_viewport_state.viewports.front();
	const auto& viewport = viewports.front();
	
	// Update viewport position and dimensions
	if (active_viewport.width != viewport.width ||
		active_viewport.height != viewport.height ||
		active_viewport.x != viewport.x ||
		active_viewport.y != viewport.y)
	{
		glViewport
		(
			static_cast<GLint>(viewport.x),
			static_cast<GLint>(viewport.y),
			std::max(0, static_cast<GLsizei>(viewport.width)),
			std::max(0, static_cast<GLsizei>(viewport.height))
		);
	}
	
	// Update viewport depth range
	if (active_viewport.min_depth != viewport.min_depth ||
		active_viewport.max_depth != viewport.max_depth)
	{
		glDepthRange(viewport.min_depth, viewport.max_depth);
	}
	
	// Update viewport state
	std::copy(viewports.begin(), viewports.end(), m_viewport_state.viewports.begin() + first_viewport);
}

void pipeline::set_scissor(std::uint32_t first_scissor, std::span<const gl::scissor_region> scissors)
{
	// Bounds check
	if (first_scissor + scissors.size() > m_max_viewports)
	{
		throw std::out_of_range("Scissor region index out of range.");
	}
	
	// Ignore empty commands
	if (scissors.empty())
	{
		return;
	}
	
	const auto& active_scissor = m_viewport_state.scissors.front();
	const auto& scissor = scissors.front();
	
	// Update scissor region
	if (active_scissor.width != scissor.width ||
		active_scissor.height != scissor.height ||
		active_scissor.x != scissor.x ||
		active_scissor.y != scissor.y)
	{
		glScissor
		(
			static_cast<GLint>(scissor.x),
			static_cast<GLint>(scissor.y),
			std::max(0, static_cast<GLsizei>(scissor.width)),
			std::max(0, static_cast<GLsizei>(scissor.height))
		);
	}
	
	// Update viewport state
	std::copy(scissors.begin(), scissors.end(), m_viewport_state.scissors.begin() + first_scissor);
}

void pipeline::set_rasterizer_discard_enabled(bool enabled)
{
	if (m_rasterization_state.rasterizer_discard_enabled != enabled)
	{
		if (enabled)
		{
			glEnable(GL_RASTERIZER_DISCARD);
		}
		else
		{
			glDisable(GL_RASTERIZER_DISCARD);
		}
		
		m_rasterization_state.rasterizer_discard_enabled = enabled;
	}
}

void pipeline::set_fill_mode(fill_mode mode)
{
	if (m_rasterization_state.fill_mode != mode)
	{
		switch (mode)
		{
			case fill_mode::fill:
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				break;
			
			case fill_mode::line:
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				break;
			
			case fill_mode::point:
				glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
				break;
			
			default:
				break;
		}
		
		m_rasterization_state.fill_mode = mode;
	}
}

void pipeline::set_cull_mode(cull_mode mode)
{
	if (m_rasterization_state.cull_mode != mode)
	{
		if (mode == cull_mode::none)
		{
			glDisable(GL_CULL_FACE);
		}
		else
		{
			if (m_rasterization_state.cull_mode == cull_mode::none)
			{
				glEnable(GL_CULL_FACE);
			}
			
			switch (mode)
			{
				case cull_mode::back:
					glCullFace(GL_BACK);
					break;
				
				case cull_mode::front:
					glCullFace(GL_FRONT);
					break;
				
				case cull_mode::front_and_back:
					glCullFace(GL_FRONT_AND_BACK);
					break;
				
				case cull_mode::none:
					[[fallthrough]];
				default:
					break;
			}
		}
		
		m_rasterization_state.cull_mode = mode;
	}
}

void pipeline::set_front_face(front_face face)
{
	if (m_rasterization_state.front_face != face)
	{
		glFrontFace(face == front_face::counter_clockwise ? GL_CCW : GL_CW);
		
		m_rasterization_state.front_face = face;
	}
}

void pipeline::set_depth_bias_enabled(bool enabled)
{
	if (m_rasterization_state.depth_bias_enabled != enabled)
	{
		if (enabled)
		{
			glEnable(GL_POLYGON_OFFSET_FILL);
			glEnable(GL_POLYGON_OFFSET_LINE);
			glEnable(GL_POLYGON_OFFSET_POINT);
		}
		else
		{
			glDisable(GL_POLYGON_OFFSET_FILL);
			glDisable(GL_POLYGON_OFFSET_LINE);
			glDisable(GL_POLYGON_OFFSET_POINT);
		}
		
		m_rasterization_state.depth_bias_enabled = enabled;
	}
}

void pipeline::set_depth_bias_factors(float constant_factor, float slope_factor)
{
	// Update depth bias factors
	if (m_rasterization_state.depth_bias_constant_factor != constant_factor ||
		m_rasterization_state.depth_bias_slope_factor != slope_factor)
	{
		glPolygonOffset(slope_factor, constant_factor);
		
		m_rasterization_state.depth_bias_constant_factor = constant_factor;
		m_rasterization_state.depth_bias_slope_factor = slope_factor;
	}
}

void pipeline::set_depth_clamp_enabled(bool enabled)
{
	if (m_rasterization_state.depth_clamp_enabled != enabled)
	{
		if (enabled)
		{
			glEnable(GL_DEPTH_CLAMP);
		}
		else
		{
			glDisable(GL_DEPTH_CLAMP);
		}
		
		m_rasterization_state.depth_clamp_enabled = enabled;
	}
}

void pipeline::set_scissor_test_enabled(bool enabled)
{
	if (m_rasterization_state.scissor_test_enabled != enabled)
	{
		if (enabled)
		{
			glEnable(GL_SCISSOR_TEST);
		}
		else
		{
			glDisable(GL_SCISSOR_TEST);
		}
		
		m_rasterization_state.scissor_test_enabled = enabled;
	}
}

void pipeline::set_provoking_vertex_mode(provoking_vertex_mode mode)
{
	if (m_rasterization_state.provoking_vertex_mode != mode)
	{
		const auto gl_provoking_vertex_mode = provoking_vertex_mode_lut[std::to_underlying(mode)];
		glProvokingVertex(gl_provoking_vertex_mode);
		m_rasterization_state.provoking_vertex_mode = mode;
	}
}

void pipeline::set_point_size(float size)
{
	if (m_rasterization_state.point_size != size)
	{
		glPointSize(size);
		
		m_rasterization_state.point_size = size;
	}
}

void pipeline::set_line_width(float width)
{
	if (m_rasterization_state.line_width != width)
	{
		glLineWidth(width);
		
		m_rasterization_state.line_width = width;
	}
}

void pipeline::set_depth_test_enabled(bool enabled)
{
	if (m_depth_stencil_state.depth_test_enabled != enabled)
	{
		m_depth_stencil_state.depth_test_enabled = enabled;
		
		if (enabled)
		{
			glEnable(GL_DEPTH_TEST);
		}
		else
		{
			glDisable(GL_DEPTH_TEST);
		}
	}
}

void pipeline::set_depth_write_enabled(bool enabled)
{
	if (m_depth_stencil_state.depth_write_enabled != enabled)
	{
		m_depth_stencil_state.depth_write_enabled = enabled;
		glDepthMask(enabled);
	}
}

void pipeline::set_depth_compare_op(gl::compare_op compare_op)
{
	if (m_depth_stencil_state.depth_compare_op != compare_op)
	{
		m_depth_stencil_state.depth_compare_op = compare_op;
		const auto gl_compare_op = compare_op_lut[std::to_underlying(compare_op)];
		glDepthFunc(gl_compare_op);
	}
}

void pipeline::set_stencil_test_enabled(bool enabled)
{
	if (m_depth_stencil_state.stencil_test_enabled != enabled)
	{
		m_depth_stencil_state.stencil_test_enabled = enabled;
		
		if (enabled)
		{
			glEnable(GL_STENCIL_TEST);
		}
		else
		{
			glDisable(GL_STENCIL_TEST);
		}
	}
}

void pipeline::set_stencil_op(std::uint8_t face_mask, stencil_op fail_op, stencil_op pass_op, stencil_op depth_fail_op, gl::compare_op compare_op)
{
	bool stencil_op_updated = false;
	bool compare_op_updated = false;
	
	if (face_mask & stencil_face_front_bit)
	{
		if (m_depth_stencil_state.stencil_front.fail_op != fail_op ||
			m_depth_stencil_state.stencil_front.pass_op != pass_op ||
			m_depth_stencil_state.stencil_front.depth_fail_op != depth_fail_op)
		{
			m_depth_stencil_state.stencil_front.fail_op = fail_op;
			m_depth_stencil_state.stencil_front.pass_op = pass_op;
			m_depth_stencil_state.stencil_front.depth_fail_op = depth_fail_op;
			stencil_op_updated = true;
		}
		
		if (m_depth_stencil_state.stencil_front.compare_op != compare_op)
		{
			m_depth_stencil_state.stencil_front.compare_op = compare_op;
			compare_op_updated = true;
		}
	}
	
	if (face_mask & stencil_face_back_bit)
	{
		if (m_depth_stencil_state.stencil_back.fail_op != fail_op ||
			m_depth_stencil_state.stencil_back.pass_op != pass_op ||
			m_depth_stencil_state.stencil_back.depth_fail_op != depth_fail_op)
		{
			m_depth_stencil_state.stencil_back.fail_op = fail_op;
			m_depth_stencil_state.stencil_back.pass_op = pass_op;
			m_depth_stencil_state.stencil_back.depth_fail_op = depth_fail_op;
			stencil_op_updated = true;
		}
		
		if (m_depth_stencil_state.stencil_back.compare_op != compare_op)
		{
			m_depth_stencil_state.stencil_back.compare_op = compare_op;
			compare_op_updated = true;
		}
	}
	
	if (stencil_op_updated || compare_op_updated)
	{
		const auto gl_face = stencil_face_lut[face_mask];
		
		if (stencil_op_updated)
		{
			const auto gl_fail_op = stencil_op_lut[std::to_underlying(fail_op)];
			const auto gl_pass_op = stencil_op_lut[std::to_underlying(pass_op)];
			const auto gl_depth_fail_op = stencil_op_lut[std::to_underlying(depth_fail_op)];
			glStencilOpSeparate(gl_face, gl_fail_op, gl_depth_fail_op, gl_pass_op);
		}
		
		if (compare_op_updated)
		{
			const auto gl_compare_op = compare_op_lut[std::to_underlying(compare_op)];
			
			if (face_mask == stencil_face_front_and_back)
			{
				if (m_depth_stencil_state.stencil_front.reference == m_depth_stencil_state.stencil_back.reference &&
					m_depth_stencil_state.stencil_front.compare_mask == m_depth_stencil_state.stencil_back.compare_mask)
				{
					glStencilFuncSeparate(gl_face, gl_compare_op, std::bit_cast<GLint>(m_depth_stencil_state.stencil_front.reference), std::bit_cast<GLuint>(m_depth_stencil_state.stencil_front.compare_mask));
				}
				else
				{
					glStencilFuncSeparate(GL_FRONT, gl_compare_op, std::bit_cast<GLint>(m_depth_stencil_state.stencil_front.reference), std::bit_cast<GLuint>(m_depth_stencil_state.stencil_front.compare_mask));
					glStencilFuncSeparate(GL_BACK, gl_compare_op, std::bit_cast<GLint>(m_depth_stencil_state.stencil_back.reference), std::bit_cast<GLuint>(m_depth_stencil_state.stencil_back.compare_mask));
				}
			}
			else if (face_mask == stencil_face_front_bit)
			{
				glStencilFuncSeparate(gl_face, gl_compare_op, std::bit_cast<GLint>(m_depth_stencil_state.stencil_front.reference), std::bit_cast<GLuint>(m_depth_stencil_state.stencil_front.compare_mask));
			}
			else
			{
				glStencilFuncSeparate(gl_face, gl_compare_op, std::bit_cast<GLint>(m_depth_stencil_state.stencil_back.reference), std::bit_cast<GLuint>(m_depth_stencil_state.stencil_back.compare_mask));
			}
		}
	}
}

void pipeline::set_stencil_compare_mask(std::uint8_t face_mask, std::uint32_t compare_mask)
{
	bool compare_mask_updated = false;
	
	if (face_mask & stencil_face_front_bit)
	{
		if (m_depth_stencil_state.stencil_front.compare_mask != compare_mask)
		{
			m_depth_stencil_state.stencil_front.compare_mask = compare_mask;
			compare_mask_updated = true;
		}
	}
	
	if (face_mask & stencil_face_back_bit)
	{
		if (m_depth_stencil_state.stencil_back.compare_mask != compare_mask)
		{
			m_depth_stencil_state.stencil_back.compare_mask = compare_mask;
			compare_mask_updated = true;
		}
	}
	
	if (compare_mask_updated)
	{
		const auto gl_face = stencil_face_lut[face_mask];
		
		if (face_mask == stencil_face_front_and_back)
		{
			if (m_depth_stencil_state.stencil_front.reference == m_depth_stencil_state.stencil_back.reference &&
				m_depth_stencil_state.stencil_front.compare_op == m_depth_stencil_state.stencil_back.compare_op)
			{
				const auto gl_compare_op = compare_op_lut[std::to_underlying(m_depth_stencil_state.stencil_front.compare_op)];
				glStencilFuncSeparate(gl_face, gl_compare_op, std::bit_cast<GLint>(m_depth_stencil_state.stencil_front.reference), static_cast<GLuint>(compare_mask));
			}
			else
			{
				const auto gl_compare_op_front = compare_op_lut[std::to_underlying(m_depth_stencil_state.stencil_front.compare_op)];
				const auto gl_compare_op_back = compare_op_lut[std::to_underlying(m_depth_stencil_state.stencil_back.compare_op)];
				glStencilFuncSeparate(GL_FRONT, gl_compare_op_front, std::bit_cast<GLint>(m_depth_stencil_state.stencil_front.reference), std::bit_cast<GLuint>(compare_mask));
				glStencilFuncSeparate(GL_BACK, gl_compare_op_back, std::bit_cast<GLint>(m_depth_stencil_state.stencil_back.reference), std::bit_cast<GLuint>(compare_mask));
			}
		}
		else if (face_mask == stencil_face_front_bit)
		{
			const auto gl_compare_op = compare_op_lut[std::to_underlying(m_depth_stencil_state.stencil_front.compare_op)];
			glStencilFuncSeparate(gl_face, gl_compare_op, std::bit_cast<GLint>(m_depth_stencil_state.stencil_front.reference), std::bit_cast<GLuint>(compare_mask));
		}
		else
		{
			const auto gl_compare_op = compare_op_lut[std::to_underlying(m_depth_stencil_state.stencil_back.compare_op)];
			glStencilFuncSeparate(gl_face, gl_compare_op, std::bit_cast<GLint>(m_depth_stencil_state.stencil_back.reference), std::bit_cast<GLuint>(compare_mask));
		}
	}
}

void pipeline::set_stencil_reference(std::uint8_t face_mask, std::uint32_t reference)
{
	bool reference_updated = false;
	
	if (face_mask & stencil_face_front_bit)
	{
		if (m_depth_stencil_state.stencil_front.reference != reference)
		{
			m_depth_stencil_state.stencil_front.reference = reference;
			reference_updated = true;
		}
	}
	
	if (face_mask & stencil_face_back_bit)
	{
		if (m_depth_stencil_state.stencil_back.reference != reference)
		{
			m_depth_stencil_state.stencil_back.reference = reference;
			reference_updated = true;
		}
	}
	
	if (reference_updated)
	{
		const auto gl_face = stencil_face_lut[face_mask];
		
		if (face_mask == stencil_face_front_and_back)
		{
			if (m_depth_stencil_state.stencil_front.compare_mask == m_depth_stencil_state.stencil_back.compare_mask &&
				m_depth_stencil_state.stencil_front.compare_op == m_depth_stencil_state.stencil_back.compare_op)
			{
				const auto gl_compare_op = compare_op_lut[std::to_underlying(m_depth_stencil_state.stencil_front.compare_op)];
				glStencilFuncSeparate(gl_face, gl_compare_op, std::bit_cast<GLint>(reference), std::bit_cast<GLuint>(m_depth_stencil_state.stencil_front.compare_mask));
			}
			else
			{
				const auto gl_compare_op_front = compare_op_lut[std::to_underlying(m_depth_stencil_state.stencil_front.compare_op)];
				const auto gl_compare_op_back = compare_op_lut[std::to_underlying(m_depth_stencil_state.stencil_back.compare_op)];
				glStencilFuncSeparate(GL_FRONT, gl_compare_op_front, std::bit_cast<GLint>(reference), std::bit_cast<GLuint>(m_depth_stencil_state.stencil_front.compare_mask));
				glStencilFuncSeparate(GL_BACK, gl_compare_op_back, std::bit_cast<GLint>(reference), std::bit_cast<GLuint>(m_depth_stencil_state.stencil_back.compare_mask));
			}
		}
		else if (face_mask == stencil_face_front_bit)
		{
			const auto gl_compare_op = compare_op_lut[std::to_underlying(m_depth_stencil_state.stencil_front.compare_op)];
			glStencilFuncSeparate(gl_face, gl_compare_op, std::bit_cast<GLint>(reference), std::bit_cast<GLuint>(m_depth_stencil_state.stencil_front.compare_mask));
		}
		else
		{
			const auto gl_compare_op = compare_op_lut[std::to_underlying(m_depth_stencil_state.stencil_back.compare_op)];
			glStencilFuncSeparate(gl_face, gl_compare_op, std::bit_cast<GLint>(reference), std::bit_cast<GLuint>(m_depth_stencil_state.stencil_back.compare_mask));
		}
	}
}

void pipeline::set_stencil_write_mask(std::uint8_t face_mask, std::uint32_t write_mask)
{
	bool write_mask_updated = false;
	
	if (face_mask & stencil_face_front_bit)
	{
		if (m_depth_stencil_state.stencil_front.write_mask != write_mask)
		{
			m_depth_stencil_state.stencil_front.write_mask = write_mask;
			write_mask_updated = true;
		}
	}
	
	if (face_mask & stencil_face_back_bit)
	{
		if (m_depth_stencil_state.stencil_back.write_mask != write_mask)
		{
			m_depth_stencil_state.stencil_back.write_mask = write_mask;
			write_mask_updated = true;
		}
	}
	
	if (write_mask_updated)
	{
		const auto gl_face = stencil_face_lut[face_mask];
		glStencilMaskSeparate(gl_face, std::bit_cast<GLuint>(write_mask));
	}
}

void pipeline::set_logic_op_enabled(bool enabled)
{
	if (m_color_blend_state.logic_op_enabled != enabled)
	{
		m_color_blend_state.logic_op_enabled = enabled;
		
		if (enabled)
		{
			glEnable(GL_COLOR_LOGIC_OP);
		}
		else
		{
			glDisable(GL_COLOR_LOGIC_OP);
		}
	}
}

void pipeline::set_logic_op(gl::logic_op logic_op)
{
	if (m_color_blend_state.logic_op != logic_op)
	{
		m_color_blend_state.logic_op = logic_op;
		
		const auto gl_logic_op = logic_op_lut[std::to_underlying(logic_op)];
		glLogicOp(gl_logic_op);
	}
}

void pipeline::set_color_blend_enabled(bool enabled)
{
	if (m_color_blend_state.blend_enabled != enabled)
	{
		m_color_blend_state.blend_enabled = enabled;
		
		if (enabled)
		{
			glEnable(GL_BLEND);
		}
		else
		{
			glDisable(GL_BLEND);
		}
	}
}

void pipeline::set_color_blend_equation(const color_blend_equation& equation)
{
	if (m_color_blend_state.color_blend_equation.src_color_blend_factor != equation.src_color_blend_factor ||
		m_color_blend_state.color_blend_equation.dst_color_blend_factor != equation.dst_color_blend_factor ||
		m_color_blend_state.color_blend_equation.src_alpha_blend_factor != equation.src_alpha_blend_factor ||
		m_color_blend_state.color_blend_equation.dst_alpha_blend_factor != equation.dst_alpha_blend_factor)
	{
		m_color_blend_state.color_blend_equation.src_color_blend_factor = equation.src_color_blend_factor;
		m_color_blend_state.color_blend_equation.dst_color_blend_factor = equation.dst_color_blend_factor;
		m_color_blend_state.color_blend_equation.src_alpha_blend_factor = equation.src_alpha_blend_factor;
		m_color_blend_state.color_blend_equation.dst_alpha_blend_factor = equation.dst_alpha_blend_factor;
		
		const auto gl_src_rgb = blend_factor_lut[std::to_underlying(equation.src_color_blend_factor)];
		const auto gl_dst_rgb = blend_factor_lut[std::to_underlying(equation.dst_color_blend_factor)];
		const auto gl_src_alpha = blend_factor_lut[std::to_underlying(equation.src_alpha_blend_factor)];
		const auto gl_dst_alpha = blend_factor_lut[std::to_underlying(equation.dst_alpha_blend_factor)];
		
		glBlendFuncSeparate(gl_src_rgb, gl_dst_rgb, gl_src_alpha, gl_dst_alpha);
	}
	
	if (m_color_blend_state.color_blend_equation.color_blend_op != equation.color_blend_op ||
		m_color_blend_state.color_blend_equation.alpha_blend_op != equation.alpha_blend_op)
	{
		m_color_blend_state.color_blend_equation.color_blend_op = equation.color_blend_op;
		m_color_blend_state.color_blend_equation.alpha_blend_op = equation.alpha_blend_op;
		
		const auto gl_mode_rgb = blend_op_lut[std::to_underlying(equation.color_blend_op)];
		const auto gl_mode_alpha = blend_op_lut[std::to_underlying(equation.alpha_blend_op)];
		
		glBlendEquationSeparate(gl_mode_rgb, gl_mode_alpha);
	}
}

void pipeline::set_color_write_mask(std::uint8_t mask)
{
	if (m_color_blend_state.color_write_mask != mask)
	{
		m_color_blend_state.color_write_mask = mask;
		
		glColorMask
		(
			mask & color_component_r_bit,
			mask & color_component_g_bit,
			mask & color_component_b_bit,
			mask & color_component_a_bit
		);
	}
}

void pipeline::set_blend_constants(const std::array<float, 4>& blend_constants)
{
	if (m_color_blend_state.blend_constants != blend_constants)
	{
		m_color_blend_state.blend_constants = blend_constants;
		glBlendColor(blend_constants[0], blend_constants[1], blend_constants[2], blend_constants[3]);
	}
}

void pipeline::draw(std::uint32_t vertex_count, std::uint32_t instance_count, std::uint32_t first_vertex, std::uint32_t first_instance)
{
	glDrawArraysInstancedBaseInstance
	(
		primitive_topology_lut[std::to_underlying(m_input_assembly_state.topology)],
		static_cast<GLint>(first_vertex),
		static_cast<GLsizei>(vertex_count),
		static_cast<GLsizei>(instance_count),
		static_cast<GLuint>(first_instance)
	);
}

void pipeline::draw_indexed(std::uint32_t index_count, std::uint32_t instance_count, std::uint32_t first_index, [[maybe_unused]] std::int32_t vertex_offset, std::uint32_t first_instance)
{
	glDrawElementsInstancedBaseInstance
	(
		primitive_topology_lut[std::to_underlying(m_input_assembly_state.topology)],
		static_cast<GLsizei>(index_count),
		GL_UNSIGNED_INT,// GL_UNSIGNED_SHORT, GL_UNSIGNED_BYTE
		reinterpret_cast<const GLvoid*>(first_index * sizeof(std::uint32_t)),
		static_cast<GLsizei>(instance_count),
		static_cast<GLuint>(first_instance)
	);
}

void pipeline::clear_attachments(std::uint8_t mask, const clear_value& value)
{
	GLbitfield gl_clear_mask = 0;
	
	if (mask & color_clear_bit)
	{
		// Add color attachment to OpenGL clear mask
		gl_clear_mask |= GL_COLOR_BUFFER_BIT;
		
		if (m_clear_value.color != value.color)
		{
			// Update color clear value
			glClearColor(value.color[0], value.color[1], value.color[2], value.color[3]);
			m_clear_value.color = value.color;
		}
	}
	
	if (mask & depth_clear_bit)
	{
		// Add depth attachment to OpenGL clear mask
		gl_clear_mask |= GL_DEPTH_BUFFER_BIT;
		
		if (m_clear_value.depth != value.depth)
		{
			// Update depth clear value
			glClearDepth(value.depth);
			m_clear_value.depth = value.depth;
		}
	}
	
	if (mask & stencil_clear_bit)
	{
		// Add stencil attachment to OpenGL clear mask
		gl_clear_mask |= GL_STENCIL_BUFFER_BIT;
		
		if (m_clear_value.stencil != value.stencil)
		{
			// Update stencil clear value
			glClearStencil(static_cast<GLint>(value.stencil));
			m_clear_value.stencil = value.stencil;
		}
	}
	
	// Clear attachments
	glClear(gl_clear_mask);
}

void pipeline::defaut_framebuffer_resized(std::uint32_t width, std::uint32_t height) noexcept
{
	m_default_framebuffer_dimensions = {width, height};
}

void pipeline::fetch_vertex_input_state()
{
}

void pipeline::fetch_input_assembly_state()
{
	m_input_assembly_state.primitive_restart_enabled = glIsEnabled(GL_PRIMITIVE_RESTART);
}

void pipeline::fetch_viewport_state()
{
	// Query viewport position and dimensions
	GLint gl_viewport[4];
	glGetIntegerv(GL_VIEWPORT, gl_viewport);
	
	// Query viewport depth range
	GLfloat gl_depth_range[2];
	glGetFloatv(GL_DEPTH_RANGE, gl_depth_range);
	
	// Query scissor box
	GLint gl_scissor_box[4] = {0, 0, 0, 0};
	glGetIntegerv(GL_SCISSOR_BOX, gl_scissor_box);
	
	// Match viewport state
	m_viewport_state.viewports =
	{{
		static_cast<float>(gl_viewport[0]),
		static_cast<float>(gl_viewport[1]),
		static_cast<float>(gl_viewport[2]),
		static_cast<float>(gl_viewport[3]),
		gl_depth_range[0],
		gl_depth_range[1]
	}};
	m_viewport_state.scissors =
	{{
		static_cast<std::int32_t>(gl_scissor_box[0]),
		static_cast<std::int32_t>(gl_scissor_box[1]),
		static_cast<std::uint32_t>(std::max(0, gl_scissor_box[2])),
		static_cast<std::uint32_t>(std::max(0, gl_scissor_box[3]))
	}};
}

void pipeline::fetch_rasterization_state()
{
	// Query rasterizer discard
	bool gl_rasterizer_discard_enabled = glIsEnabled(GL_RASTERIZER_DISCARD);
	
	// Query fill mode
	GLint gl_fill_mode;
	glGetIntegerv(GL_POLYGON_MODE, &gl_fill_mode);
	
	// Query cull mode
	bool gl_cull_enabled = glIsEnabled(GL_CULL_FACE);
	GLint gl_cull_mode;
	glGetIntegerv(GL_CULL_FACE_MODE, &gl_cull_mode);
	
	// Query front face
	GLint gl_front_face;
	glGetIntegerv(GL_FRONT_FACE, &gl_front_face);
	
	// Query depth bias
	bool gl_depth_bias_enabled = glIsEnabled(GL_POLYGON_OFFSET_FILL) &&
		glIsEnabled(GL_POLYGON_OFFSET_LINE) &&
		glIsEnabled(GL_POLYGON_OFFSET_POINT);
	float gl_depth_bias_constant_factor;
	float gl_depth_bias_slope_factor;
	glGetFloatv(GL_POLYGON_OFFSET_UNITS, &gl_depth_bias_constant_factor);
	glGetFloatv(GL_POLYGON_OFFSET_FACTOR, &gl_depth_bias_slope_factor);
	
	// Query depth clamp
	bool gl_depth_clamp_enabled = glIsEnabled(GL_DEPTH_CLAMP);
	
	// Query scissor test
	bool gl_scissor_test_enabled = glIsEnabled(GL_SCISSOR_TEST);
	
	// Query provoking vertex
	GLint gl_provoking_vertex;
	glGetIntegerv(GL_PROVOKING_VERTEX, &gl_provoking_vertex);
	
	// Query point size
	float gl_point_size;
	glGetFloatv(GL_POINT_SIZE, &gl_point_size);
	
	// Query line width
	float gl_line_width;
	glGetFloatv(GL_LINE_WIDTH, &gl_line_width);
	
	// Match rasterizer state
	m_rasterization_state.rasterizer_discard_enabled = gl_rasterizer_discard_enabled;
	m_rasterization_state.fill_mode = (gl_fill_mode == GL_POINT ? fill_mode::point : gl_fill_mode == GL_LINE ? fill_mode::line : fill_mode::fill);
	if (gl_cull_enabled)
	{
		m_rasterization_state.cull_mode = (gl_cull_mode == GL_FRONT_AND_BACK ? cull_mode::front_and_back : gl_fill_mode == GL_FRONT ? cull_mode::front : cull_mode::back);
	}
	else
	{
		m_rasterization_state.cull_mode = cull_mode::none;
	}
	m_rasterization_state.front_face = (gl_front_face == GL_CW ? front_face::clockwise : front_face::counter_clockwise);
	m_rasterization_state.depth_bias_enabled = gl_depth_bias_enabled;
	m_rasterization_state.depth_bias_constant_factor = gl_depth_bias_constant_factor;
	m_rasterization_state.depth_bias_slope_factor = gl_depth_bias_slope_factor;
	m_rasterization_state.depth_clamp_enabled = gl_depth_clamp_enabled;
	m_rasterization_state.scissor_test_enabled = gl_scissor_test_enabled;
	m_rasterization_state.provoking_vertex_mode = (gl_provoking_vertex == GL_FIRST_VERTEX_CONVENTION ? provoking_vertex_mode::first : provoking_vertex_mode::last);
	m_rasterization_state.point_size = gl_point_size;
	m_rasterization_state.line_width = gl_line_width;
}

void pipeline::fetch_depth_stencil_state()
{
	auto inv_compare_op_lut = [](GLint func) -> gl::compare_op
	{
		switch (func)
		{
			case GL_NEVER:
				return compare_op::never;
			case GL_LESS:
				return compare_op::less;
			case GL_EQUAL:
				return compare_op::equal;
			case GL_LEQUAL:
				return compare_op::less_or_equal;
			case GL_GREATER:
				return compare_op::greater;
			case GL_NOTEQUAL:
				return compare_op::not_equal;
			case GL_GEQUAL:
				return compare_op::greater_or_equal;
			case GL_ALWAYS:
			default:
				return compare_op::always;
		}
	};
	
	auto inv_stencil_op_lut = [](GLint op) -> gl::stencil_op
	{
		switch (op)
		{
			case GL_KEEP:
				return stencil_op::keep;
			case GL_ZERO:
				return stencil_op::zero;
			case GL_REPLACE:
				return stencil_op::replace;
			case GL_INCR:
				return stencil_op::increment_and_clamp;
			case GL_DECR:
				return stencil_op::decrement_and_clamp;
			case GL_INVERT:
				return stencil_op::invert;
			case GL_INCR_WRAP:
				return stencil_op::increment_and_wrap;
			case GL_DECR_WRAP:
			default:
				return stencil_op::decrement_and_wrap;
		}
	};
	
	m_depth_stencil_state.depth_test_enabled = glIsEnabled(GL_DEPTH_TEST);
	
	GLboolean gl_depth_write_enabled;
	glGetBooleanv(GL_DEPTH_WRITEMASK, &gl_depth_write_enabled);
	m_depth_stencil_state.depth_write_enabled = gl_depth_write_enabled;
	
	GLint gl_depth_compare_op;
	glGetIntegerv(GL_DEPTH_FUNC, &gl_depth_compare_op);
	m_depth_stencil_state.depth_compare_op = inv_compare_op_lut(gl_depth_compare_op);
	
	m_depth_stencil_state.stencil_test_enabled = glIsEnabled(GL_STENCIL_TEST);
	
	// Stencil front
	{
		GLint gl_stencil_front_fail;
		GLint gl_stencil_front_pass_depth_pass;
		GLint gl_stencil_front_pass_depth_fail;
		GLint gl_stencil_front_func;
		GLint gl_stencil_front_value_mask;
		GLint gl_stencil_front_write_mask;
		GLint gl_stencil_front_ref;
		
		glGetIntegerv(GL_STENCIL_FAIL, &gl_stencil_front_fail);
		glGetIntegerv(GL_STENCIL_PASS_DEPTH_PASS, &gl_stencil_front_pass_depth_pass);
		glGetIntegerv(GL_STENCIL_PASS_DEPTH_FAIL, &gl_stencil_front_pass_depth_fail);
		glGetIntegerv(GL_STENCIL_FUNC, &gl_stencil_front_func);
		glGetIntegerv(GL_STENCIL_VALUE_MASK, &gl_stencil_front_value_mask);
		glGetIntegerv(GL_STENCIL_WRITEMASK, &gl_stencil_front_write_mask);
		glGetIntegerv(GL_STENCIL_REF, &gl_stencil_front_ref);
		
		m_depth_stencil_state.stencil_front.fail_op = inv_stencil_op_lut(gl_stencil_front_fail);
		m_depth_stencil_state.stencil_front.pass_op = inv_stencil_op_lut(gl_stencil_front_pass_depth_pass);
		m_depth_stencil_state.stencil_front.depth_fail_op = inv_stencil_op_lut(gl_stencil_front_pass_depth_fail);
		m_depth_stencil_state.stencil_front.compare_op = inv_compare_op_lut(gl_stencil_front_func);
		m_depth_stencil_state.stencil_front.compare_mask = std::bit_cast<std::uint32_t>(gl_stencil_front_value_mask);
		m_depth_stencil_state.stencil_front.write_mask = std::bit_cast<std::uint32_t>(gl_stencil_front_write_mask);
		m_depth_stencil_state.stencil_front.reference = std::bit_cast<std::uint32_t>(gl_stencil_front_ref);
	}
	
	// Stencil back
	{
		GLint gl_stencil_back_fail;
		GLint gl_stencil_back_pass_depth_pass;
		GLint gl_stencil_back_pass_depth_fail;
		GLint gl_stencil_back_func;
		GLint gl_stencil_back_value_mask;
		GLint gl_stencil_back_write_mask;
		GLint gl_stencil_back_ref;
		
		glGetIntegerv(GL_STENCIL_BACK_FAIL, &gl_stencil_back_fail);
		glGetIntegerv(GL_STENCIL_BACK_PASS_DEPTH_PASS, &gl_stencil_back_pass_depth_pass);
		glGetIntegerv(GL_STENCIL_BACK_PASS_DEPTH_FAIL, &gl_stencil_back_pass_depth_fail);
		glGetIntegerv(GL_STENCIL_BACK_FUNC, &gl_stencil_back_func);
		glGetIntegerv(GL_STENCIL_BACK_VALUE_MASK, &gl_stencil_back_value_mask);
		glGetIntegerv(GL_STENCIL_BACK_WRITEMASK, &gl_stencil_back_write_mask);
		glGetIntegerv(GL_STENCIL_BACK_REF, &gl_stencil_back_ref);
		
		m_depth_stencil_state.stencil_back.fail_op = inv_stencil_op_lut(gl_stencil_back_fail);
		m_depth_stencil_state.stencil_back.pass_op = inv_stencil_op_lut(gl_stencil_back_pass_depth_pass);
		m_depth_stencil_state.stencil_back.depth_fail_op = inv_stencil_op_lut(gl_stencil_back_pass_depth_fail);
		m_depth_stencil_state.stencil_back.compare_op = inv_compare_op_lut(gl_stencil_back_func);
		m_depth_stencil_state.stencil_back.compare_mask = std::bit_cast<std::uint32_t>(gl_stencil_back_value_mask);
		m_depth_stencil_state.stencil_back.write_mask = std::bit_cast<std::uint32_t>(gl_stencil_back_write_mask);
		m_depth_stencil_state.stencil_back.reference = std::bit_cast<std::uint32_t>(gl_stencil_back_ref);
	}
}

void pipeline::fetch_color_blend_state()
{
	auto inv_logic_op_lut = [](GLint op) -> gl::logic_op
	{
		switch (op)
		{
			case GL_CLEAR:
				return gl::logic_op::bitwise_clear;
			case GL_AND:
				return gl::logic_op::bitwise_and;
			case GL_AND_REVERSE:
				return gl::logic_op::bitwise_and_reverse;
			case GL_COPY:
				return gl::logic_op::bitwise_copy;
			case GL_AND_INVERTED:
				return gl::logic_op::bitwise_and_inverted;
			case GL_NOOP:
				return gl::logic_op::bitwise_no_op;
			case GL_XOR:
				return gl::logic_op::bitwise_xor;
			case GL_OR:
				return gl::logic_op::bitwise_or;
			case GL_NOR:
				return gl::logic_op::bitwise_nor;
			case GL_EQUIV:
				return gl::logic_op::bitwise_equivalent;
			case GL_INVERT:
				return gl::logic_op::bitwise_invert;
			case GL_OR_REVERSE:
				return gl::logic_op::bitwise_or_reverse;
			case GL_COPY_INVERTED:
				return gl::logic_op::bitwise_copy_inverted;
			case GL_OR_INVERTED:
				return gl::logic_op::bitwise_or_inverted;
			case GL_NAND:
				return gl::logic_op::bitwise_nand;
			case GL_SET:
			default:
				return gl::logic_op::bitwise_set;
		}
	};
	
	auto inv_blend_factor_lut = [](GLint func) -> gl::blend_factor
	{
		switch (func)
		{
			case GL_ZERO:
				return blend_factor::zero;
			case GL_ONE:
				return blend_factor::one;
			case GL_SRC_COLOR:
				return blend_factor::src_color;
			case GL_ONE_MINUS_SRC_COLOR:
				return blend_factor::one_minus_src_color;
			case GL_DST_COLOR:
				return blend_factor::dst_color;
			case GL_ONE_MINUS_DST_COLOR:
				return blend_factor::one_minus_dst_color;
			case GL_SRC_ALPHA:
				return blend_factor::src_alpha;
			case GL_ONE_MINUS_SRC_ALPHA:
				return blend_factor::one_minus_src_alpha;
			case GL_DST_ALPHA:
				return blend_factor::dst_alpha;
			case GL_ONE_MINUS_DST_ALPHA:
				return blend_factor::one_minus_dst_alpha;
			case GL_CONSTANT_COLOR:
				return blend_factor::constant_color;
			case GL_ONE_MINUS_CONSTANT_COLOR:
				return blend_factor::one_minus_constant_color;
			case GL_CONSTANT_ALPHA:
				return blend_factor::constant_alpha;
			case GL_ONE_MINUS_CONSTANT_ALPHA:
				return blend_factor::one_minus_constant_alpha;
			case GL_SRC_ALPHA_SATURATE:
				return blend_factor::src_alpha_saturate;
			case GL_SRC1_COLOR:
				return blend_factor::src1_color;
			case GL_ONE_MINUS_SRC1_COLOR:
				return blend_factor::one_minus_src1_color;
			case GL_SRC1_ALPHA:
				return blend_factor::src1_alpha;
			case GL_ONE_MINUS_SRC1_ALPHA:
			default:
				return blend_factor::one_minus_src1_alpha;
		}
	};
	
	auto inv_blend_op_lut = [](GLint mode) -> gl::blend_op
	{
		switch (mode)
		{
			case GL_FUNC_ADD:
				return blend_op::add;
			case GL_FUNC_SUBTRACT:
				return blend_op::subtract;
			case GL_FUNC_REVERSE_SUBTRACT:
				return blend_op::reverse_subtract;
			case GL_MIN:
				return blend_op::min;
			case GL_MAX:
				default:
				return blend_op::max;
		}
	};
	
	m_color_blend_state.logic_op_enabled = glIsEnabled(GL_COLOR_LOGIC_OP);
	
	GLint gl_logic_op;
	glGetIntegerv(GL_LOGIC_OP_MODE, &gl_logic_op);
	
	m_color_blend_state.logic_op = inv_logic_op_lut(gl_logic_op);
	
	m_color_blend_state.blend_enabled = glIsEnabled(GL_BLEND);
	
	GLint gl_blend_src_rgb;
	GLint gl_blend_dst_rgb;
	GLint gl_blend_equation_rgb;
	GLint gl_blend_src_alpha;
	GLint gl_blend_dst_alpha;
	GLint gl_blend_equation_alpha;
	glGetIntegerv(GL_BLEND_SRC_RGB, &gl_blend_src_rgb);
	glGetIntegerv(GL_BLEND_DST_RGB, &gl_blend_dst_rgb);
	glGetIntegerv(GL_BLEND_EQUATION_RGB, &gl_blend_equation_rgb);
	glGetIntegerv(GL_BLEND_SRC_ALPHA, &gl_blend_src_alpha);
	glGetIntegerv(GL_BLEND_DST_ALPHA, &gl_blend_dst_alpha);
	glGetIntegerv(GL_BLEND_EQUATION_ALPHA, &gl_blend_equation_alpha);
	
	m_color_blend_state.color_blend_equation.src_color_blend_factor = inv_blend_factor_lut(gl_blend_src_rgb);
	m_color_blend_state.color_blend_equation.dst_color_blend_factor = inv_blend_factor_lut(gl_blend_dst_rgb);
	m_color_blend_state.color_blend_equation.color_blend_op = inv_blend_op_lut(gl_blend_equation_rgb);
	m_color_blend_state.color_blend_equation.src_alpha_blend_factor = inv_blend_factor_lut(gl_blend_src_alpha);
	m_color_blend_state.color_blend_equation.dst_alpha_blend_factor = inv_blend_factor_lut(gl_blend_dst_alpha);
	m_color_blend_state.color_blend_equation.alpha_blend_op = inv_blend_op_lut(gl_blend_equation_alpha);
	
	GLboolean gl_color_writemask[4];
	glGetBooleanv(GL_COLOR_WRITEMASK, gl_color_writemask);
	
	m_color_blend_state.color_write_mask =
		static_cast<std::uint8_t>(gl_color_writemask[0]) |
		(static_cast<std::uint8_t>(gl_color_writemask[1]) << 1) |
		(static_cast<std::uint8_t>(gl_color_writemask[2]) << 2) |
		(static_cast<std::uint8_t>(gl_color_writemask[3]) << 3);
	
	glGetFloatv(GL_BLEND_COLOR, m_color_blend_state.blend_constants.data());
}

void pipeline::fetch_clear_value()
{
	// Query clear values
	GLfloat gl_color_clear[4];
	GLfloat gl_depth_clear;
	GLint gl_stencil_clear;
	glGetFloatv(GL_COLOR_CLEAR_VALUE, gl_color_clear);
	glGetFloatv(GL_DEPTH_CLEAR_VALUE, &gl_depth_clear);
	glGetIntegerv(GL_STENCIL_CLEAR_VALUE, &gl_stencil_clear);
	
	// Match clear state
	m_clear_value.color = {gl_color_clear[0], gl_color_clear[1], gl_color_clear[2], gl_color_clear[3]};
	m_clear_value.depth = gl_depth_clear;
	m_clear_value.stencil = static_cast<std::uint32_t>(gl_stencil_clear);
}

} // namespace gl
