// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.gl.shader_template;
export import engine.gl.shader_object;
export import engine.gl.shader_program;
import engine.utility.text_file;
import engine.utility.sized_types;
import <map>;
export import <string>;
export import <memory>;
export import <unordered_map>;
import <unordered_set>;
import <vector>;

export namespace engine::gl
{
	/// Template used to for generating one or more shader variants from a single source.
	/// 
	/// Shader templates support the following preprocessor directives:
	/// 
	/// * `#pragma vertex`: Replaced with `#define __VERTEX__` when generating vertex shader objects.
	/// * `#pragma fragment`: Replaced with `#define __FRAGMENT__` when generating fragment shader objects.
	/// * `#pragma geometry`: Replaced with `#define __GEOMETRY__` when generating geometry shader objects.
	/// * `#pragma define <key>`: Will be replaced with `#define <key> <value>` if its definition is passed to the shader template.
	/// 
	/// @see gl::shader_stage
	/// @see gl::shader_object
	/// @see gl::shader_program
	class shader_template
	{
	public:
		/// Container of definitions used to generate `#pragma define <key> <value>` directives.
		using dictionary_type = std::unordered_map<std::string, std::string>;

		/// Constructs a shader template and sets its source code.
		/// @param source_code Shader template source code.
		explicit shader_template(const text_file& source_code);

		/// @copydoc shader_template(const text_file&)
		explicit shader_template(text_file&& source_code);

		/// Constructs a shader template and sets its source code.
		/// @param source_code Shader template source code.
		/// @param include_files Shader template include files.
		/// @note This constuctor is used to keep the loaded include files cached.
		shader_template(text_file&& source_code, std::vector<std::shared_ptr<text_file>>&& include_files);

		/// Constructs an empty shader template.
		shader_template() noexcept = default;

		/// Replaces the source code of the shader template.
		/// @param source_code Shader template source code.
		void source(const text_file& source_code);

		/// @copydoc source(const text_file&)
		void source(text_file&& source_code);

		/// Configures shader object source code for a given shader stage and template dictionary.
		/// @param stage Shader stage of the shader object to generate. Instances of `#pragma <stage>` in the template source will be replaced with `#define __<STAGE>__`.
		/// @param definitions Container of definitions used to replace `#pragma define <key> <value>` directives.
		/// @return Configured shader object source code.
		[[nodiscard]] std::string configure(gl::shader_stage stage, const dictionary_type& definitions = {}) const;

		/// Configures and compiles a shader object.
		/// @param stage Shader stage of the shader object to generate. Instances of `#pragma <stage>` in the template source will be replaced with `#define __<STAGE>__`.
		/// @param definitions Container of definitions used to replace `#pragma define <key> <value>` directives.
		/// @return Compiled shader object.
		/// @exception std::runtime_error Any exceptions thrown by gl::shader_object.
		[[nodiscard]] std::unique_ptr<gl::shader_object> compile(gl::shader_stage stage, const dictionary_type& definitions = {}) const;

		/// Configures and compiles shader objects, then links them into a shader program. Shader object stages are determined according to the presence of `#pragma <stage>` directives.
		/// @param definitions Container of definitions used to replace `#pragma define <key> <value>` directives.
		/// @return Linked shader program.
		/// @exception std::runtime_error Any exceptions thrown by gl::shader_object or gl::shader_program.
		/// @see has_vertex_directive() const
		/// @see has_fragment_directive() const
		/// @see has_geometry_directive() const
		[[nodiscard]] std::unique_ptr<gl::shader_program> build(const dictionary_type& definitions = {}) const;

		/// Returns `true` if the template source contains one or more `#pragma vertex` directive.
		[[nodiscard]] inline bool has_vertex_directive() const noexcept
		{
			return !m_vertex_directives.empty();
		}

		/// Returns `true` if the template source contains one or more `#pragma fragment` directive.
		[[nodiscard]] inline bool has_fragment_directive() const noexcept
		{
			return !m_fragment_directives.empty();
		}

		/// Returns `true` if the template source contains one or more `#pragma geometry` directive.
		[[nodiscard]] inline bool has_geometry_directive() const noexcept
		{
			return !m_geometry_directives.empty();
		}

		/// Returns `true` if the template source contains one or more instance of `#pragma define <key>`.
		/// @param key Definition key.
		[[nodiscard]] bool has_define_directive(const std::string& key) const;

		/// Returns a hash of the template source code.
		[[nodiscard]] inline constexpr usize hash() const noexcept
		{
			return m_hash;
		}

	private:
		void find_directives();
		void rehash();
		void replace_stage_directives(gl::shader_stage stage) const;
		void replace_define_directives(const dictionary_type& definitions) const;

		mutable text_file m_template_source;
		std::unordered_set<usize> m_vertex_directives;
		std::unordered_set<usize> m_fragment_directives;
		std::unordered_set<usize> m_geometry_directives;
		std::multimap<std::string, usize> m_define_directives;
		usize m_hash{0};
		std::vector<std::shared_ptr<text_file>> m_include_files;
	};
}
