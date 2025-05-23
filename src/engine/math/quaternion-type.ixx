// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.math.quaternion:type;
import engine.math.constants;
import engine.math.vector;
import engine.math.matrix;
import engine.utility.sized_types;
import <format>;
import <type_traits>;
import <utility>;

export namespace engine::math::inline types
{
	/// @name Quaternion types
	/// @relates quaternion
	/// @{

	/// Quaternion composed of a real scalar part and imaginary vector part.
	/// @tparam T Scalar type.
	template <class T>
	struct quaternion
	{
		/// @name Member types
		/// @{

		/// Scalar type.
		using scalar_type = T;

		/// Imaginary vector part type.
		using vector_type = vec3<T>;

		/// Rotation matrix type.
		using matrix_type = mat3<T>;

		/// @}

		/// @name Data members
		/// @{

		/// Quaternion real part.
		scalar_type r;

		/// Quaternion imaginary part.
		vector_type i;

		/// @}

		/// @name Part access
		/// @{

		/// Returns a reference to the quaternion real part.
		[[nodiscard]] inline constexpr scalar_type& w() noexcept
		{
			return r;
		}

		/// @copydoc w()
		[[nodiscard]] inline constexpr const scalar_type& w() const noexcept
		{
			return r;
		}

		/// Returns a reference to the first element of the quaternion imaginary part.
		[[nodiscard]] inline constexpr scalar_type& x() noexcept
		{
			return i.x();
		}

		/// @copydoc x()
		[[nodiscard]] inline constexpr const scalar_type& x() const noexcept
		{
			return i.x();
		}

		/// Returns a reference to the second element of the quaternion imaginary part.
		[[nodiscard]] inline constexpr scalar_type& y() noexcept
		{
			return i.y();
		}

		/// @copydoc y()
		[[nodiscard]] inline constexpr const scalar_type& y() const noexcept
		{
			return i.y();
		}

		/// Returns a reference to the third element of the quaternion imaginary part.
		[[nodiscard]] inline constexpr scalar_type& z() noexcept
		{
			return i.z();
		}

		/// @copydoc z()
		[[nodiscard]] inline constexpr const scalar_type& z() const noexcept
		{
			return i.z();
		}

		/// @}

		/// @name Conversion
		/// @{

		/// Type-casts the quaternion scalars using `static_cast`.
		/// @tparam U Target scalar type.
		/// @return Type-casted quaternion.
		template <class U>
		[[nodiscard]] inline constexpr explicit operator quaternion<U>() const noexcept
		{
			return {static_cast<U>(r), vec3<U>(i)};
		}

		/// Constructs a matrix representing the rotation described by the quaternion.
		/// @return Rotation matrix.
		[[nodiscard]] inline constexpr matrix_type matrix() const noexcept
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
				{T{1} - (yy + zz) * T { 2 }, (xy + zw) * T { 2 }, (xz - yw) * T { 2 }},
				{(xy - zw) * T { 2 }, T{1} - (xx + zz) * T { 2 }, (yz + xw) * T { 2 }},
				{(xz + yw) * T { 2 }, (yz - xw) * T { 2 }, T{1} - (xx + yy) * T { 2 }}
			}};
		}

		/// @copydoc matrix()
		[[nodiscard]] constexpr explicit operator matrix_type() const noexcept
		{
			return matrix();
		}

		/// @}

		/// @name Operations
		/// @{

		/// Exchanges the parts of this quaternion with the parts of another.
		/// @param other Quaternion with which to exchange parts.
		inline constexpr void swap(quaternion& other) noexcept
		{
			std::swap(r, other.r);
			std::swap(i, other.i);
		};

		/// @}

		/// @name Comparison
		/// @{

		/// Tests two quaternions for equality.
		/// @return `true` if the two quaternions are equivalent, `false` otherwise.
		[[nodiscard]] inline constexpr friend bool operator==(const quaternion&, const quaternion&) noexcept = default;

		/// Compares the parts of two quaternions lexicographically.
		/// @return Lexicographical ordering of the two quaternions.
		[[nodiscard]] inline constexpr friend auto operator<=>(const quaternion&, const quaternion&) noexcept = default;

		/// @}

		/// Returns a quaternion representing a rotation about the x-axis.
		/// @param angle Angle of rotation, in radians.
		/// @return Quaternion representing an x-axis rotation.
		[[nodiscard]] static quaternion rotate_x(scalar_type angle)
		{
			return {cos(angle * T{0.5}), sin(angle * T{0.5}), T{0}, T{0}};
		}

		/// Returns a quaternion representing a rotation about the y-axis.
		/// @param angle Angle of rotation, in radians.
		/// @return Quaternion representing an y-axis rotation.
		[[nodiscard]] static quaternion rotate_y(scalar_type angle)
		{
			return {cos(angle * T{0.5}), T{0}, sin(angle * T{0.5}), T{0}};
		}

		/// Returns a quaternion representing a rotation about the z-axis.
		/// @param angle Angle of rotation, in radians.
		/// @return Quaternion representing an z-axis rotation.
		[[nodiscard]] static quaternion rotate_z(scalar_type angle)
		{
			return {cos(angle * T{0.5}), T{0}, T{0}, sin(angle * T{0.5})};
		}
	};

	/// @copybrief quaternion
	template <class T>
	using quat = quaternion<T>;

	/// Quaternion with single-precision floating-point scalars.
	using fquat = quat<float>;

	/// Quaternion with double-precision floating-point scalars.
	using dquat = quat<double>;

	/// @}

	/// @name Tuple-like interface
	/// @{

	/// Extracts the *i*-th part of a quaternion using a tuple-like interface.
	/// @tparam I Index of a part. Index `0` corresponds to the real part, and `1` to the imaginary part.
	/// @tparam T Scalar type.
	/// @param q Quaternion from which to extract a part.
	/// @return Reference to the *i*-th part of @p q.
	/// @relates quaternion
	template <usize I, class T>
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

	/// @copydoc get(quaternion&)
	/// @relates quaternion
	template <usize I, class T>
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

	/// @copydoc get(quaternion&)
	/// @relates quaternion
	template <usize I, class T>
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

	/// @copydoc get(quaternion&)
	/// @relates quaternion
	template <usize I, class T>
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
}

export namespace std
{
	/// Provides access to the number of parts in a engine::math::quaternion as a compile-time constant expression.
	/// @tparam T Scalar type.
	/// @relates engine::math::quaternion
	template <class T>
	struct tuple_size<engine::math::quaternion<T>>:
		std::integral_constant<engine::usize, 2>
	{};
	
	/// Provides compile-time indexed access to the real part of a engine::math::quaternion using a tuple-like interface.
	/// @tparam T Scalar type.
	/// @relates engine::math::quaternion
	template <class T>
	struct tuple_element<0, engine::math::quaternion<T>>
	{
		/// Quaternion real part type.
		using type = T;
	};
	
	/// Provides compile-time indexed access to the imaginary part of a engine::math::quaternion using a tuple-like interface.
	/// @tparam T Scalar type.
	/// @relates engine::math::quaternion
	template <class T>
	struct tuple_element<1, engine::math::quaternion<T>>
	{
		/// Quaternion imaginary part type.
		using type = engine::math::vec3<T>;
	};
	
	/// Specialization of std::formatter for engine::math::quaternion.
	/// @tparam T Scalar type.
	/// @relates engine::math::quaternion
    template <class T>
    struct formatter<engine::math::quaternion<T>>: formatter<T>
    {
		auto format(const engine::math::quaternion<T>& t, format_context& fc) const
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
