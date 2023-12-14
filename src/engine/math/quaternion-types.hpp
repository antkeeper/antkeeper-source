// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_MATH_QUATERNION_TYPES_HPP
#define ANTKEEPER_MATH_QUATERNION_TYPES_HPP

#include <engine/math/constants.hpp>
#include <engine/math/vector.hpp>
#include <engine/math/matrix.hpp>
#include <format>
#include <type_traits>

// export module math.quaternion:type;
// import math.constants;
// import math.vector;
// import math.matrix;
// import <format>;
// import <type_traits>;

namespace math {
namespace types {

/**
 * Quaternion composed of a real scalar part and imaginary vector part.
 *
 * @tparam T Scalar type.
 */
template <class T>
struct quaternion
{
	/// @name Member types
	/// @{
	
	/** Scalar type. */
	using scalar_type = T;
	
	/** Imaginary vector part type. */
	using vector_type = vec3<T>;
	
	/** Rotation matrix type. */
	using matrix_type = mat3<T>;
	
	/// @}
	
	/// @name Data members
	/// @{
	
	/** Quaternion real part. */
	scalar_type r;
	
	/** Quaternion imaginary part. */
	vector_type i;
	
	/// @}
	
	/// @name Element access
	/// @{
	
	/// @{
	/** Returns a reference to the quaternion real part. */
	[[nodiscard]] inline constexpr scalar_type& w() noexcept
	{
		return r;
	}
	[[nodiscard]] inline constexpr const scalar_type& w() const noexcept
	{
		return r;
	}
	/// @}
	
	/// @{
	/** Returns a reference to the first element of the quaternion imaginary part. */
	[[nodiscard]] inline constexpr scalar_type& x() noexcept
	{
		return i.x();
	}
	[[nodiscard]] inline constexpr const scalar_type& x() const noexcept
	{
		return i.x();
	}
	/// @}
	
	/// @{
	/** Returns a reference to the second element of the quaternion imaginary part. */
	[[nodiscard]] inline constexpr scalar_type& y() noexcept
	{
		return i.y();
	}
	[[nodiscard]] inline constexpr const scalar_type& y() const noexcept
	{
		return i.y();
	}
	/// @}
	
	/// @{
	/** Returns a reference to the third element of the quaternion imaginary part. */
	[[nodiscard]] inline constexpr scalar_type& z() noexcept
	{
		return i.z();
	}
	[[nodiscard]] inline constexpr const scalar_type& z() const noexcept
	{
		return i.z();
	}
	/// @}
	
	/// @}
	
	/**
	 * Returns a quaternion representing a rotation about the x-axis.
	 *
	 * @param angle Angle of rotation, in radians.
	 *
	 * @return Quaternion representing an x-axis rotation.
	 */
	[[nodiscard]] static quaternion rotate_x(scalar_type angle)
	{
		return {cos(angle * T{0.5}), sin(angle * T{0.5}), T{0}, T{0}};
	}
	
	/**
	 * Returns a quaternion representing a rotation about the y-axis.
	 *
	 * @param angle Angle of rotation, in radians.
	 *
	 * @return Quaternion representing an y-axis rotation.
	 */
	[[nodiscard]] static quaternion rotate_y(scalar_type angle)
	{
		return {cos(angle * T{0.5}), T{0}, sin(angle * T{0.5}), T{0}};
	}
	
	/**
	 * Returns a quaternion representing a rotation about the z-axis.
	 *
	 * @param angle Angle of rotation, in radians.
	 * @return Quaternion representing an z-axis rotation.
	 */
	[[nodiscard]] static quaternion rotate_z(scalar_type angle)
	{
		return {cos(angle * T{0.5}), T{0}, T{0}, sin(angle * T{0.5})};
	}
	
	/**
	 * Type-casts the quaternion scalars using `static_cast`.
	 *
	 * @tparam U Target scalar type.
	 *
	 * @return Type-casted quaternion.
	 */
	template <class U>
	[[nodiscard]] inline constexpr explicit operator quaternion<U>() const noexcept
	{
		return {static_cast<U>(r), vec3<U>(i)};
	}
	
	/// @{
	/**
	 * Constructs a matrix representing the rotation described by the quaternion.
	 *
	 * @return Rotation matrix.
	 */
	[[nodiscard]] constexpr explicit operator matrix_type() const noexcept
	{
		const T xx = x() * x();
		const T xy = x() * y();
		const T xz = x() * z();
		const T xw = x() * w();
		const T yy = y() * y();
		const T yz = y() * z();
		const T yw = y() * w();
		const T zz = z() * z();
		const T zw = z() * w();
		
		return
		{{
			{T{1} - (yy + zz) * T{2}, (xy + zw) * T{2}, (xz - yw) * T{2}},
			{(xy - zw) * T{2}, T{1} - (xx + zz) * T{2}, (yz + xw) * T{2}},
			{(xz + yw) * T{2}, (yz - xw) * T{2}, T{1} - (xx + yy) * T{2}}
		}};
	}
	
	[[nodiscard]] inline constexpr matrix_type matrix() const noexcept
	{
		return matrix_type(*this);
	}
	/// @}
	
	/**
	 * Casts the quaternion to a 4-element vector, with the real part as the first element and the imaginary part as the following three elements.
	 *
	 * @return Vector containing the real and imaginary parts of the quaternion.
	 */
	[[nodiscard]] inline constexpr explicit operator vec4<T>() const noexcept
	{
		return {r, i.x(), i.y(), i.z()};
	}
};

/// @name Tuple-like interface
/// @{

/// @{
/**
 * Extracts the *i*-th part of a quaternion using a tuple-like interface.
 *
 * @tparam I Index of a part. Index `0` corresponds to the real part, and `1` to the imaginary part.
 * @tparam T Scalar type.
 *
 * @param q Quaternion from which to extract a part.
 *
 * @return Reference to the *i*-th part of @p q.
 *
 * @relates quaternion
 */
template <std::size_t I, class T>
[[nodiscard]] inline constexpr auto& get(quaternion<T>& q) noexcept
{
	static_assert(I < 2);
	if constexpr (!I)
	{
		return q.r;
	}
	else
	{
		return q.i;
	}
}

/// @relates quaternion
template <std::size_t I, class T>
[[nodiscard]] inline constexpr auto&& get(quaternion<T>&& q) noexcept
{
	static_assert(I < 2);
	if constexpr (!I)
	{
		return std::move(q.r);
	}
	else
	{
		return std::move(q.i);
	}
}

/// @relates quaternion
template <std::size_t I, class T>
[[nodiscard]] inline constexpr const auto& get(const quaternion<T>& q) noexcept
{
	static_assert(I < 2);
	if constexpr (!I)
	{
		return q.r;
	}
	else
	{
		return q.i;
	}
}

/// @relates quaternion
template <std::size_t I, class T>
[[nodiscard]] inline constexpr const auto&& get(const quaternion<T>&& q) noexcept
{
	static_assert(I < 2);
	if constexpr (!I)
	{
		return std::move(q.r);
	}
	else
	{
		return std::move(q.i);
	}
}
/// @}

/// @}

/// @name Quaternion types
/// @relates quaternion
/// @{

/// @copydoc quaternion
template <class T>
using quat = quaternion<T>;

/// Quaternion with single-precision floating-point scalars.
using fquat = quat<float>;

/// Quaternion with double-precision floating-point scalars.
using dquat = quat<double>;

/// @}

} // namespace types

// Bring math::types into math namespace
using namespace types;

} // namespace math

namespace std
{
	/**
	 * Provides access to the number of parts in a math::types::quaternion as a compile-time constant expression.
	 *
	 * @tparam T Scalar type.
	 *
	 * @relates math::types::quaternion
	 */
	template <class T>
	struct tuple_size<math::quaternion<T>>:
		std::integral_constant<std::size_t, 2>
	{};
	
	/**
	 * Provides compile-time indexed access to the real part of a math::types::quaternion using a tuple-like interface.
	 *
	 * @tparam T Scalar type.
	 *
	 * @relates math::types::quaternion
	 */
	template <class T>
	struct tuple_element<0, math::quaternion<T>>
	{
		/** Quaternion real part type. */
		using type = T;
	};
	
	/**
	 * Provides compile-time indexed access to the imaginary part of a math::types::quaternion using a tuple-like interface.
	 *
	 * @tparam T Scalar type.
	 *
	 * @relates math::types::quaternion
	 */
	template <class T>
	struct tuple_element<1, math::quaternion<T>>
	{
		/** Quaternion imaginary part type. */
		using type = math::vec3<T>;
	};
	
	/**
	 * Specialization of std::formatter for math::types::quaternion.
	 *
	 * @tparam T Scalar type.
	 *
	 * @relates math::types::quaternion
	 */
    template <class T>
    struct formatter<math::quaternion<T>>: formatter<T>
    {
		auto format(const math::quaternion<T>& t, format_context& fc) const
		{
			auto&& out = fc.out();
			format_to(out, "{{");
			
			formatter<T>::format(t.w(), fc);
			format_to(out, ", {{");
			formatter<T>::format(t.x(), fc);
			format_to(out, ", ");
			formatter<T>::format(t.y(), fc);
			format_to(out, ", ");
			formatter<T>::format(t.z(), fc);
			
			return format_to(out, "}}}}");
		}
    };
}

#endif // ANTKEEPER_MATH_QUATERNION_TYPES_HPP
