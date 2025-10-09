// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/math/vector.hpp>
#include <engine/math/polynomial.hpp>
#include <engine/utility/sized-types.hpp>
#include <vector>

namespace engine::physics::orbit
{
	/// Describes the trajectory of an orbit with Chebyshev polynomials.
	/// @tparam t Real type.
	template <class T>
	struct trajectory
	{
		/// Start time of the trajectory.
		T t0;

		/// End time of the trajectory.
		T t1;

		/// Time step duration.
		T dt;

		/// Chebyshev polynomial degree.
		usize n;

		/// Chebyshev polynomial coefficients.
		std::vector<T> a;

		/// Calculates the Cartesian position of a trajectory at a given time.
		/// @param t Time, on `[t0, t1)`.
		/// @return Trajectory position at time @p t.
		[[nodiscard]] math::vec3<T> position(T t) const
		{
			t -= t0;
			usize i = static_cast<usize>(t / dt);

			const T* ax = &a[i * n * 3];
			const T* ay = ax + n;
			const T* az = ay + n;

			t = (t / dt - i) * T{2} - T{1};

			math::vec3<T> r;
			r.x() = math::chebyshev(ax, ay, t);
			r.y() = math::chebyshev(ay, az, t);
			r.z() = math::chebyshev(az, az + n, t);

			return r;
		}
	};
}
