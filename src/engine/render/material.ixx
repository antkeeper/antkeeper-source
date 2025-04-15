// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.render.material;
import engine.gl.shader_template;
export import engine.render.material_variable;
export import engine.hash.fnv;
import engine.utility.sized_types;
export import <unordered_map>;

export namespace engine::render
{
	namespace material_flag
	{
		enum: u32
		{
			x_ray = 0x10,
			outline = 0x20,
			vegetation = 0x40,
			refractive = 0x80,
			decal = 0x100,
			decal_surface = 0x200,
			wireframe = 0x80000000
		};
	}

	/// Material blend modes.
	enum class material_blend_mode
	{
		/// Material is fully opaque.
		opaque,

		/// Material has binary masked opacity.
		masked,

		/// Material is translucent.
		translucent
	};

	/// Material shadow casting modes.
	enum class material_shadow_mode
	{
		/// Material does not cast shadows.
		none,

		/// Material casts fully opaque shadows.
		opaque
	};

	/// A material is associated with exactly one shader program and contains a set of material properties which can be uploaded to that shader program via shader inputs.
	class material
	{
	public:
		/// Constructs a material.
		material() = default;
	
		/// Constructs a copy of another material.
		/// @param other Material to copy.
		material(const material& other);
	
		/// Destroys a material.
		~material() = default;
	
		/// Makes this material a copy of aother material.
		/// @param other Material to copy.
		/// @return Reference to this material.
		material& operator=(const material& other);
	
		/// @name Settings
		/// @{
	
		/// Enables or disables back-face culling of the material surface.
		/// @param two_sided `true` to disable back-face culling, or `false` to enable it.
		void set_two_sided(bool two_sided) noexcept;
	
		/// Sets the material blend mode.
		/// @param mode Blend mode.
		void set_blend_mode(material_blend_mode mode) noexcept;
	
		/// Sets the material shadow mode.
		/// @param mode Shadow mode.
		void set_shadow_mode(material_shadow_mode mode) noexcept;
	
		/// Sets the material flags.
		/// @param flags Material flags.
		void set_flags(u32 flags) noexcept;
	
		/// Returns `true` if the material surface is two-sided, and `false` otherwise.
		[[nodiscard]] inline bool is_two_sided() const noexcept
		{
			return m_two_sided;
		}
	
		/// Returns the material blend mode.
		[[nodiscard]] inline material_blend_mode get_blend_mode() const noexcept
		{
			return m_blend_mode;
		}
	
		/// Returns the material shadow mode.
		[[nodiscard]] inline material_shadow_mode get_shadow_mode() const noexcept
		{
			return m_shadow_mode;
		}
	
		/// Returns the material flags.
		[[nodiscard]] inline u32 get_flags() const noexcept
		{
			return m_flags;
		}
	
		/// @}
	
		/// @name Shading
		/// @{
	
		/// Sets the material's shader template.
		/// @param shader_template Shader template with which to associate the material.
		void set_shader_template(std::shared_ptr<gl::shader_template> shader_template);
	
		/// Returns the shader template with which this material is associated.
		[[nodiscard]] inline const std::shared_ptr<gl::shader_template>& get_shader_template() const noexcept
		{
			return m_shader_template;
		}
	
		/// Sets the value of a material variable with the given name.
		/// @param key 32-bit FNV-1a hash value of the variable name.
		/// @param value Shared pointer to the material variable value.
		void set_variable(hash::fnv32_t key, std::shared_ptr<material_variable_base> value);
	
		/// Returns a shared pointer to the material variable with the given name, or `nullptr` if not found.
		/// @param key 32-bit FNV-1a hash value of the variable name.
		/// @return Shared pointer to the material variable with the given name, or `nullptr` if not found.
		[[nodiscard]] std::shared_ptr<material_variable_base> get_variable(hash::fnv32_t key) const;
	
		/// Returns all material variables.
		/// @return Map of 32-bit FNV-1a hash values of variable names to variables.
		[[nodiscard]] inline const std::unordered_map<hash::fnv32_t, std::shared_ptr<material_variable_base>>& get_variables() const noexcept
		{
			return m_variable_map;
		}
	
		/// @}
	
		/// Returns a hash of the material state.
		/// 
		/// The followings functions may change the material hash:
		/// 
		/// * material::set_shader_template
		/// * material::set_flags
		/// * material::set_blend_mode
		/// * material::set_two_sided
		/// * material::set_shadow_mode
		/// 
		[[nodiscard]] inline usize hash() const noexcept
		{
			return m_hash;
		}
	
	private:
		/// Recalculates the material state hash.
		void rehash() noexcept;
	
		bool m_two_sided{false};
		material_blend_mode m_blend_mode{material_blend_mode::opaque};
		material_shadow_mode m_shadow_mode{material_shadow_mode::opaque};
		u32 m_flags{0};
		std::shared_ptr<gl::shader_template> m_shader_template;
		std::unordered_map<hash::fnv32_t, std::shared_ptr<material_variable_base>> m_variable_map;
		usize m_hash{0};
	};
}
