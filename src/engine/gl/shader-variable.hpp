// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GL_SHADER_INPUT_HPP
#define ANTKEEPER_GL_SHADER_INPUT_HPP

#include <engine/math/vector.hpp>
#include <engine/math/matrix.hpp>
#include <engine/gl/shader-variable-type.hpp>
#include <cstdint>
#include <span>
#include <memory>

namespace gl {

class texture_1d;
class texture_2d;
class texture_3d;
class texture_cube;

/**
 * Shader program variable.
 */
class shader_variable
{
public:
	/**
	 * Returns the shader variable data type.
	 */
	[[nodiscard]] virtual constexpr shader_variable_type type() const noexcept = 0;
	
	/**
	 * Returns the number of elements in an array variable, or `1` if the variable is not an array.
	 */
	[[nodiscard]] inline std::size_t size() const noexcept
	{
		return m_size;
	}
	
	/**
	 * Updates the value of the variable. If the variable is an array, the value of the first element will be updated.
	 *
	 * @param value Update value.
	 *
	 * @throw std::invalid_argument Shader variable type mismatch.
	 */
	///@{
	virtual void update(bool value) const;
	virtual void update(const math::bvec2& value) const;
	virtual void update(const math::bvec3& value) const;
	virtual void update(const math::bvec4& value) const;
	
	virtual void update(int value) const;
	virtual void update(const math::ivec2& value) const;
	virtual void update(const math::ivec3& value) const;
	virtual void update(const math::ivec4& value) const;
	
	virtual void update(unsigned int value) const;
	virtual void update(const math::uvec2& value) const;
	virtual void update(const math::uvec3& value) const;
	virtual void update(const math::uvec4& value) const;
	
	virtual void update(float value) const;
	virtual void update(const math::fvec2& value) const;
	virtual void update(const math::fvec3& value) const;
	virtual void update(const math::fvec4& value) const;
	
	virtual void update(const math::fmat2& value) const;
	virtual void update(const math::fmat3& value) const;
	virtual void update(const math::fmat4& value) const;
	
	virtual void update(const texture_1d& value) const;
	virtual void update(const texture_2d& value) const;
	virtual void update(const texture_3d& value) const;
	virtual void update(const texture_cube& value) const;
	///@}
	
	/**
	 * Updates the value of a single element in an array variable.
	 *
	 * @param value Update value.
	 * @param index Index of the element to update.
	 *
	 * @throw std::invalid_argument Shader variable type mismatch.
	 */
	/// @{
	virtual void update(bool value, std::size_t index) const;
	virtual void update(const math::bvec2& value, std::size_t index) const;
	virtual void update(const math::bvec3& value, std::size_t index) const;
	virtual void update(const math::bvec4& value, std::size_t index) const;
	
	virtual void update(int value, std::size_t index) const;
	virtual void update(const math::ivec2& value, std::size_t index) const;
	virtual void update(const math::ivec3& value, std::size_t index) const;
	virtual void update(const math::ivec4& value, std::size_t index) const;
	
	virtual void update(unsigned int value, std::size_t index) const;
	virtual void update(const math::uvec2& value, std::size_t index) const;
	virtual void update(const math::uvec3& value, std::size_t index) const;
	virtual void update(const math::uvec4& value, std::size_t index) const;
	
	virtual void update(float value, std::size_t index) const;
	virtual void update(const math::fvec2& value, std::size_t index) const;
	virtual void update(const math::fvec3& value, std::size_t index) const;
	virtual void update(const math::fvec4& value, std::size_t index) const;
	
	virtual void update(const math::fmat2& value, std::size_t index) const;
	virtual void update(const math::fmat3& value, std::size_t index) const;
	virtual void update(const math::fmat4& value, std::size_t index) const;
	
	virtual void update(const texture_1d& value, std::size_t index) const;
	virtual void update(const texture_2d& value, std::size_t index) const;
	virtual void update(const texture_3d& value, std::size_t index) const;
	virtual void update(const texture_cube& value, std::size_t index) const;
	///@}
	
	/**
	 * Updates the values of one or more elements in an array variable.
	 *
	 * @param values Contiguous sequence of update values.
	 * @param index Index of the first element to update.
	 *
	 * @throw std::invalid_argument Shader variable type mismatch.
	 */
	///@{
	virtual void update(std::span<const bool> values, std::size_t index = 0) const;
	virtual void update(std::span<const math::bvec2> values, std::size_t index = 0) const;
	virtual void update(std::span<const math::bvec3> values, std::size_t index = 0) const;
	virtual void update(std::span<const math::bvec4> values, std::size_t index = 0) const;
	
	virtual void update(std::span<const int> values, std::size_t index = 0) const;
	virtual void update(std::span<const math::ivec2> values, std::size_t index = 0) const;
	virtual void update(std::span<const math::ivec3> values, std::size_t index = 0) const;
	virtual void update(std::span<const math::ivec4> values, std::size_t index = 0) const;
	
	virtual void update(std::span<const unsigned int> values, std::size_t index = 0) const;
	virtual void update(std::span<const math::uvec2> values, std::size_t index = 0) const;
	virtual void update(std::span<const math::uvec3> values, std::size_t index = 0) const;
	virtual void update(std::span<const math::uvec4> values, std::size_t index = 0) const;
	
	virtual void update(std::span<const float> values, std::size_t index = 0) const;
	virtual void update(std::span<const math::fvec2> values, std::size_t index = 0) const;
	virtual void update(std::span<const math::fvec3> values, std::size_t index = 0) const;
	virtual void update(std::span<const math::fvec4> values, std::size_t index = 0) const;
	
	virtual void update(std::span<const math::fmat2> values, std::size_t index = 0) const;
	virtual void update(std::span<const math::fmat3> values, std::size_t index = 0) const;
	virtual void update(std::span<const math::fmat4> values, std::size_t index = 0) const;
	
	virtual void update(std::span<const texture_1d* const> values, std::size_t index = 0) const;
	virtual void update(std::span<const texture_2d* const> values, std::size_t index = 0) const;
	virtual void update(std::span<const texture_3d* const> values, std::size_t index = 0) const;
	virtual void update(std::span<const texture_cube* const> values, std::size_t index = 0) const;
	
	virtual void update(std::span<const std::shared_ptr<texture_1d>> values, std::size_t index = 0) const;
	virtual void update(std::span<const std::shared_ptr<texture_2d>> values, std::size_t index = 0) const;
	virtual void update(std::span<const std::shared_ptr<texture_3d>> values, std::size_t index = 0) const;
	virtual void update(std::span<const std::shared_ptr<texture_cube>> values, std::size_t index = 0) const;
	///@}
	
protected:
	/**
	 * Constructs a shader variable.
	 *
	 * @param size Number of elements in the array, or `1` if the variable is not an array.
	 */
	explicit shader_variable(std::size_t size) noexcept;
	
private:
	const std::size_t m_size{0};
};

} // namespace gl

#endif // ANTKEEPER_GL_SHADER_VARIABLE_HPP
