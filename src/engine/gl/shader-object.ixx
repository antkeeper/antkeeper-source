// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.gl.shader_object;
export import engine.gl.shader_stage;
export import <string>;
export import <string_view>;

export namespace engine::gl
{
	class shader_program;

	/// Shader object which can be compiled and linked to a shader program.
	/// @see gl::shader_program
	/// @see gl::shader_stage
	class shader_object
	{
	public:
		/// Creates an empty shader object for the specified shader stage.
		/// @param stage Shader stage in which this shader object will be used.
		/// @exception std::runtime_error An error occurred while creating an OpenGL shader object.
		explicit shader_object(shader_stage stage);

		/// Destroys a shader object.
		~shader_object();

		/// Replaces the source code of the shader object.
		/// @param source_code String containing shader object source code.
		/// @exception std::runtime_error Shader object handle is not a value generated by OpenGL.
		/// @exception std::runtime_error Shader object handle is not a shader object.
		void source(std::string_view source_code);

		/// Compiles the shader object.
		/// @return `true` if the shader object was compiled successfully, `false` otherwise. If compilation fails, check the info log via shader_object::get_info_log() for more information.
		/// @exception std::runtime_error Shader object handle is not a value generated by OpenGL.
		/// @exception std::runtime_error Shader object handle is not a shader object.
		/// @see shader_object::get_info_log()
		bool compile();

		/// Returns the shader stage of this shader object.
		[[nodiscard]] inline shader_stage stage() const noexcept
		{
			return m_stage;
		}

		/// Returns the shader object info log, which is updated when the shader object is compiled.
		[[nodiscard]] inline const std::string& info() const noexcept
		{
			return m_info_log;
		}

		/// Returns `true` if the shader object has been successfully compiled, `false` otherwise.
		[[nodiscard]] inline bool compiled() const noexcept
		{
			return m_compiled;
		}

		shader_object(const shader_object&) = delete;
		shader_object(shader_object&&) = delete;
		shader_object& operator=(const shader_object&) = delete;
		shader_object& operator=(shader_object&&) = delete;

	private:
		friend class shader_program;

		unsigned int m_gl_shader_id{0};
		shader_stage m_stage{0};
		bool m_compiled{false};
		std::string m_info_log;
	};
}
