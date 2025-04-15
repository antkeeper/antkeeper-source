// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.gl.shader_variable;
import engine.math.matrix;
import engine.math.vector;
import engine.utility.sized_types;
export import <span>;
export import <memory>;

export namespace engine::gl
{
	class texture_1d;
	class texture_2d;
	class texture_3d;
	class texture_cube;

	/// Shader variable data types.
	enum class shader_variable_type
	{
		bvec1,
		bvec2,
		bvec3,
		bvec4,
		ivec1,
		ivec2,
		ivec3,
		ivec4,
		uvec1,
		uvec2,
		uvec3,
		uvec4,
		fvec1,
		fvec2,
		fvec3,
		fvec4,
		fmat2,
		fmat3,
		fmat4,
		texture_1d,
		texture_2d,
		texture_3d,
		texture_cube
	};

	/// Shader program variable.
	class shader_variable
	{
	public:
		/// Destructs a shader variable.
		virtual ~shader_variable() = default;

		/// Returns the shader variable data type.
		[[nodiscard]] virtual constexpr shader_variable_type type() const noexcept = 0;

		/// Returns the number of elements in an array variable, or `1` if the variable is not an array.
		[[nodiscard]] inline usize size() const noexcept
		{
			return m_size;
		}

		/// Updates the value of the variable. If the variable is an array, the value of the first element will be updated.
		/// @param value Update value.
		/// @throw std::invalid_argument Shader variable type mismatch.
		virtual void update(bool value) const;

		/// @copydoc update(bool) const
		virtual void update(const math::bvec2& value) const;

		/// @copydoc update(bool) const
		virtual void update(const math::bvec3& value) const;

		/// @copydoc update(bool) const
		virtual void update(const math::bvec4& value) const;

		///	@copydoc update(bool) const
		virtual void update(int value) const;

		///	@copydoc update(bool) const
		virtual void update(const math::ivec2& value) const;

		///	@copydoc update(bool) const
		virtual void update(const math::ivec3& value) const;

		///	@copydoc update(bool) const
		virtual void update(const math::ivec4& value) const;

		///	@copydoc update(bool) const
		virtual void update(unsigned int value) const;

		///	@copydoc update(bool) const
		virtual void update(const math::uvec2& value) const;

		///	@copydoc update(bool) const
		virtual void update(const math::uvec3& value) const;

		///	@copydoc update(bool) const
		virtual void update(const math::uvec4& value) const;

		///	@copydoc update(bool) const
		virtual void update(float value) const;

		///	@copydoc update(bool) const
		virtual void update(const math::fvec2& value) const;

		///	@copydoc update(bool) const
		virtual void update(const math::fvec3& value) const;

		///	@copydoc update(bool) const
		virtual void update(const math::fvec4& value) const;

		///	@copydoc update(bool) const
		virtual void update(const math::fmat2& value) const;

		///	@copydoc update(bool) const
		virtual void update(const math::fmat3& value) const;

		///	@copydoc update(bool) const
		virtual void update(const math::fmat4& value) const;

		///	@copydoc update(bool) const
		virtual void update(const texture_1d& value) const;

		///	@copydoc update(bool) const
		virtual void update(const texture_2d& value) const;

		///	@copydoc update(bool) const
		virtual void update(const texture_3d& value) const;

		///	@copydoc update(bool) const
		virtual void update(const texture_cube& value) const;

		/// Updates the value of a single element in an array variable.
		/// @param value Update value.
		/// @param index Index of the element to update.
		/// @throw std::invalid_argument Shader variable type mismatch.
		virtual void update(bool value, usize index) const;

		/// @copydoc update(bool, usize) const
		virtual void update(const math::bvec2& value, usize index) const;

		/// @copydoc update(bool, usize) const
		virtual void update(const math::bvec3& value, usize index) const;

		/// @copydoc update(bool, usize) const
		virtual void update(const math::bvec4& value, usize index) const;

		/// @copydoc update(bool, usize) const
		virtual void update(int value, usize index) const;

		/// @copydoc update(bool, usize) const
		virtual void update(const math::ivec2& value, usize index) const;

		/// @copydoc update(bool, usize) const
		virtual void update(const math::ivec3& value, usize index) const;

		/// @copydoc update(bool, usize) const
		virtual void update(const math::ivec4& value, usize index) const;

		/// @copydoc update(bool, usize) const
		virtual void update(unsigned int value, usize index) const;

		/// @copydoc update(bool, usize) const
		virtual void update(const math::uvec2& value, usize index) const;

		/// @copydoc update(bool, usize) const
		virtual void update(const math::uvec3& value, usize index) const;

		/// @copydoc update(bool, usize) const
		virtual void update(const math::uvec4& value, usize index) const;

		/// @copydoc update(bool, usize) const
		virtual void update(float value, usize index) const;

		/// @copydoc update(bool, usize) const
		virtual void update(const math::fvec2& value, usize index) const;

		/// @copydoc update(bool, usize) const
		virtual void update(const math::fvec3& value, usize index) const;

		/// @copydoc update(bool, usize) const
		virtual void update(const math::fvec4& value, usize index) const;

		/// @copydoc update(bool, usize) const
		virtual void update(const math::fmat2& value, usize index) const;

		/// @copydoc update(bool, usize) const
		virtual void update(const math::fmat3& value, usize index) const;

		/// @copydoc update(bool, usize) const
		virtual void update(const math::fmat4& value, usize index) const;

		/// @copydoc update(bool, usize) const
		virtual void update(const texture_1d& value, usize index) const;

		/// @copydoc update(bool, usize) const
		virtual void update(const texture_2d& value, usize index) const;

		/// @copydoc update(bool, usize) const
		virtual void update(const texture_3d& value, usize index) const;

		/// @copydoc update(bool, usize) const
		virtual void update(const texture_cube& value, usize index) const;

		/// Updates the values of one or more elements in an array variable.
		/// @param values Contiguous sequence of update values.
		/// @param index Index of the first element to update.
		/// @throw std::invalid_argument Shader variable type mismatch.
		virtual void update(std::span<const bool> values, usize index = 0) const;

		/// @copydoc update(std::span<const bool>, usize) const
		virtual void update(std::span<const math::bvec2> values, usize index = 0) const;

		/// @copydoc update(std::span<const bool>, usize) const
		virtual void update(std::span<const math::bvec3> values, usize index = 0) const;

		/// @copydoc update(std::span<const bool>, usize) const
		virtual void update(std::span<const math::bvec4> values, usize index = 0) const;

		/// @copydoc update(std::span<const bool>, usize) const
		virtual void update(std::span<const int> values, usize index = 0) const;

		/// @copydoc update(std::span<const bool>, usize) const
		virtual void update(std::span<const math::ivec2> values, usize index = 0) const;

		/// @copydoc update(std::span<const bool>, usize) const
		virtual void update(std::span<const math::ivec3> values, usize index = 0) const;

		/// @copydoc update(std::span<const bool>, usize) const
		virtual void update(std::span<const math::ivec4> values, usize index = 0) const;

		/// @copydoc update(std::span<const bool>, usize) const
		virtual void update(std::span<const unsigned int> values, usize index = 0) const;

		/// @copydoc update(std::span<const bool>, usize) const
		virtual void update(std::span<const math::uvec2> values, usize index = 0) const;

		/// @copydoc update(std::span<const bool>, usize) const
		virtual void update(std::span<const math::uvec3> values, usize index = 0) const;

		/// @copydoc update(std::span<const bool>, usize) const
		virtual void update(std::span<const math::uvec4> values, usize index = 0) const;

		/// @copydoc update(std::span<const bool>, usize) const
		virtual void update(std::span<const float> values, usize index = 0) const;

		/// @copydoc update(std::span<const bool>, usize) const
		virtual void update(std::span<const math::fvec2> values, usize index = 0) const;

		/// @copydoc update(std::span<const bool>, usize) const
		virtual void update(std::span<const math::fvec3> values, usize index = 0) const;

		/// @copydoc update(std::span<const bool>, usize) const
		virtual void update(std::span<const math::fvec4> values, usize index = 0) const;

		/// @copydoc update(std::span<const bool>, usize) const
		virtual void update(std::span<const math::fmat2> values, usize index = 0) const;

		/// @copydoc update(std::span<const bool>, usize) const
		virtual void update(std::span<const math::fmat3> values, usize index = 0) const;

		/// @copydoc update(std::span<const bool>, usize) const
		virtual void update(std::span<const math::fmat4> values, usize index = 0) const;

		/// @copydoc update(std::span<const bool>, usize) const
		virtual void update(std::span<const texture_1d* const> values, usize index = 0) const;

		/// @copydoc update(std::span<const bool>, usize) const
		virtual void update(std::span<const texture_2d* const> values, usize index = 0) const;

		/// @copydoc update(std::span<const bool>, usize) const
		virtual void update(std::span<const texture_3d* const> values, usize index = 0) const;

		/// @copydoc update(std::span<const bool>, usize) const
		virtual void update(std::span<const texture_cube* const> values, usize index = 0) const;

		/// @copydoc update(std::span<const bool>, usize) const
		virtual void update(std::span<const std::shared_ptr<texture_1d>> values, usize index = 0) const;

		/// @copydoc update(std::span<const bool>, usize) const
		virtual void update(std::span<const std::shared_ptr<texture_2d>> values, usize index = 0) const;

		/// @copydoc update(std::span<const bool>, usize) const
		virtual void update(std::span<const std::shared_ptr<texture_3d>> values, usize index = 0) const;

		/// @copydoc update(std::span<const bool>, usize) const
		virtual void update(std::span<const std::shared_ptr<texture_cube>> values, usize index = 0) const;

	protected:
		/// Constructs a shader variable.
		/// @param size Number of elements in the array, or `1` if the variable is not an array.
		explicit shader_variable(usize size) noexcept;

	private:
		const usize m_size{0};
	};
}
