// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.geom.intersection;
import engine.geom.primitives;
import engine.utility.sized_types;
import engine.math.functions;
export import <optional>;

using namespace engine::math;

export namespace engine::geom
{
	/// Ray-hyperplane intersection test.
	/// @param ray Ray.
	/// @param hyperplane Hyperplane.
	/// @return Distance along the ray to the point of intersection, or `std::nullopt` if no intersection occurred.
	template <class T, usize N>
	[[nodiscard]] constexpr std::optional<T> intersection(const ray<T, N>& ray, const hyperplane<T, N>& hyperplane) noexcept
	{
		const T cos_theta = dot(ray.direction, hyperplane.normal);
		if (cos_theta != T{0})
		{
			const T t = -hyperplane.distance(ray.origin) / cos_theta;
			if (t >= T{0})
			{
				return t;
			}
		}

		return std::nullopt;
	}

	/// @copydoc intersection(const ray<T, N>&, const hyperplane<T, N>&)
	template <class T, usize N>
	[[nodiscard]] inline constexpr std::optional<T> intersection(const hyperplane<T, N>& hyperplane, const ray<T, N>& ray) noexcept
	{
		return intersection<T, N>(ray, hyperplane);
	}

	/// Ray-hyperrectangle intersection test.
	/// @param ray Ray.
	/// @param hyperrectangle Hyperrectangle.
	/// @return Tuple containing the distances along the ray to the first and second points of intersection, or `std::nullopt` if no intersection occurred.
	template <class T, usize N>
	[[nodiscard]] constexpr std::optional<std::tuple<T, T>> intersection(const ray<T, N>& ray, const hyperrectangle<T, N>& hyperrectangle) noexcept
	{
		T t0 = -inf<T>;
		T t1 = inf<T>;

		for (usize i = 0; i < N; ++i)
		{
			if (!ray.direction[i])
			{
				if (ray.origin[i] < hyperrectangle.min[i] || ray.origin[i] > hyperrectangle.max[i])
				{
					return std::nullopt;
				}
			}
			else
			{
				T t_min = (hyperrectangle.min[i] - ray.origin[i]) / ray.direction[i];
				T t_max = (hyperrectangle.max[i] - ray.origin[i]) / ray.direction[i];
				t0 = max(t0, min(t_min, t_max));
				t1 = min(t1, max(t_min, t_max));
			}
		}

		if (t0 > t1 || t1 < T{0})
		{
			return std::nullopt;
		}

		return std::tuple<T, T>{t0, t1};
	}

	/// @copydoc intersection(const ray<T, N>&, const hyperrectangle<T, N>&)
	template <class T, usize N>
	[[nodiscard]] inline constexpr std::optional<std::tuple<T, T>> intersection(const hyperrectangle<T, N>& hyperrectangle, const ray<T, N>& ray) noexcept
	{
		return intersection<T, N>(ray, hyperrectangle);
	}

	/// Ray-hypersphere intersection test.
	/// @param ray Ray.
	/// @param hypersphere Hypersphere.
	/// @return Tuple containing the distances along the ray to the first and second points of intersection, or `std::nullopt` if no intersection occurred.
	/// @see Haines, E., Günther, J., & Akenine-Möller, T. (2019). Precision improvements for ray/sphere intersection. Ray Tracing Gems: High-Quality and Real-Time Rendering with DXR and Other APIs, 87-94.
	template <class T, usize N>
	[[nodiscard]] std::optional<std::tuple<T, T>> intersection(const ray<T, N>& ray, const hypersphere<T, N>& hypersphere) noexcept
	{
		const vector<T, N> d = ray.origin - hypersphere.center;
		const T b = dot(d, ray.direction);
		const vector<T, N> qc = d - ray.direction * b;
		const T h = hypersphere.radius * hypersphere.radius - dot(qc, qc);

		if (h < T{0})
		{
			return std::nullopt;
		}

		const T sqrt_h = sqrt(h);
		return std::tuple<T, T>{-b - sqrt_h, -b + sqrt_h};
	}

	/// Ray-triangle intersection test.
	/// @param ray Ray.
	/// @param a,b,c Triangle points.
	/// @return Tuple containing the distance along the ray to the point of intersection, followed by two barycentric coordinates of the point of intersection, or `std::nullopt` if no intersection occurred.
	template <class T>
	[[nodiscard]] constexpr std::optional<std::tuple<T, T, T>> intersection(const ray<T, 3>& ray, const vec3<T>& a, const vec3<T>& b, const vec3<T>& c) noexcept
	{
		// Find edges
		const vec3<T> edge_ab = b - a;
		const vec3<T> edge_ac = c - a;

		// Calculate determinant
		const vec3<T> pv = cross(ray.direction, edge_ac);
		const T det = dot(edge_ab, pv);
		if (!det)
		{
			return std::nullopt;
		}

		const T inverse_det = T{1} / det;

		// Calculate u
		const vec3<T> tv = ray.origin - a;
		const T u = dot(tv, pv) * inverse_det;
		if (u < T{0} || u > T{1})
		{
			return std::nullopt;
		}

		// Calculate v
		const vec3<T> qv = cross(tv, edge_ab);
		const T v = dot(ray.direction, qv) * inverse_det;
		if (v < T{0} || u + v > T{1})
		{
			return std::nullopt;
		}

		// Calculate t
		const T t = dot(edge_ac, qv) * inverse_det;
		if (t < T{0})
		{
			return std::nullopt;
		}

		return std::tuple<T, T, T>{t, u, v};
	}

	/// @copydoc intersection(const ray<T, 3>&, const vec3<T>&, const vec3<T>&, const vec3<T>&)
	template <class T>
	[[nodiscard]] inline constexpr std::optional<std::tuple<T, T, T>> intersection(const vec3<T>& a, const vec3<T>& b, const vec3<T>& c, const ray<T, 3>& ray) noexcept
	{
		return intersection(ray, a, b, c);
	}

	/// Hyperrectangle-hyperrectangle intersection test.
	/// @param a First hyperrectangle.
	/// @param b Second hyperrectangle.
	/// @return `true` if an intersection occurred, `false` otherwise.
	template <class T, usize N>
	[[nodiscard]] inline constexpr bool intersection(const hyperrectangle<T, N>& a, const hyperrectangle<T, N>& b) noexcept
	{
		return a.intersects(b);
	}

	/// Hyperrectangle-hypersphere intersection test.
	/// @param hyperrectangle Hyperrectangle.
	/// @param hypersphere Hypersphere.
	/// @return `true` if an intersection occurred, `false` otherwise.
	template <class T, usize N>
	[[nodiscard]] constexpr bool intersection(const hyperrectangle<T, N>& hyperrectangle, const hypersphere<T, N>& hypersphere) noexcept
	{
		T sqr_distance{0};
		for (usize i = 0; i < N; ++i)
		{
			if (hypersphere.center[i] < hyperrectangle.min[i])
			{
				const T difference = hyperrectangle.min[i] - hypersphere.center[i];
				sqr_distance += difference * difference;
			}
			else if (hypersphere.center[i] > hyperrectangle.max[i])
			{
				const T difference = hypersphere.center[i] - hyperrectangle.max[i];
				sqr_distance += difference * difference;
			}
		}

		return sqr_distance <= hypersphere.radius * hypersphere.radius;
	}

	/// @copydoc intersection(const hyperrectangle<T, N>&, const hypersphere<T, N>&)
	template <class T, usize N>
	[[nodiscard]] inline constexpr bool intersection(const hypersphere<T, N>& hypersphere, const hyperrectangle<T, N>& hyperrectangle) noexcept
	{
		return intersection<T, N>(hyperrectangle, hypersphere);
	}

	/// Hypersphere-hypersphere intersection test.
	/// @param a First hypersphere.
	/// @param b Second hypersphere.
	/// @return `true` if an intersection occurred, `false` otherwise.
	template <class T, usize N>
	[[nodiscard]] inline constexpr bool intersection(const hypersphere<T, N>& a, const hypersphere<T, N>& b) noexcept
	{
		return a.intersects(b);
	}
}
