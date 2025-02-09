// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_SCENE_LIGHT_PROBE_HPP
#define ANTKEEPER_SCENE_LIGHT_PROBE_HPP

#include <engine/scene/object.hpp>
#include <engine/gl/texture.hpp>
#include <engine/gl/framebuffer.hpp>
#include <engine/math/matrix.hpp>
#include <memory>
#include <span>
#include <vector>

namespace scene {

/// Captures the luminance and illuminance of a scene.
class light_probe: public object<light_probe>
{
public:
	/// Constructs a light probe and its luminance texture.
	/// @param format Luminance cubemap image format.
	/// @param face_size Luminance cubemap face size.
	light_probe(gl::format format, std::uint32_t face_size);

	/// Constructs a light probe.
	/// @note The light probe's luminance texture must be set before use.
	light_probe();
	
	/// Destructs a light probe.
	~light_probe() override = default;
	
	/// Updates the light probe's illuminance matrices from its illuminance texture.
	/// @warning Reads texture data from the GPU.
	void update_illuminance_matrices();
	
	/// Sets the light probe's luminance texture.
	/// @param texture Luminance cubemap texture.
	/// @note Marks the light probe's luminance as outdated if the luminance texture has changed.
	/// @note Marks the light probe's illuminance as outdated if the luminance texture has changed.
	void set_luminance_texture(std::shared_ptr<gl::texture_cube> texture);
	
	/// Marks the light probe's luminance as either outdated or current.
	/// @param outdated `true` if the light probe's luminance is outdated, `false` otherwise.
	void set_luminance_outdated(bool outdated);
	
	/// Marks the light probe's illuminance as either outdated or current.
	/// @param outdated `true` if the light probe's illuminance is outdated, `false` otherwise.
	void set_illuminance_outdated(bool outdated);
	
	/// Returns the light probe's luminance texture.
	[[nodiscard]] inline const std::shared_ptr<gl::texture_cube>& get_luminance_texture() const noexcept
	{
		return m_luminance_texture;
	}
	
	/// Returns the light probe's luminance framebuffers.
	[[nodiscard]] inline const std::vector<std::shared_ptr<gl::framebuffer>>& get_luminance_framebuffers() const noexcept
	{
		return m_luminance_framebuffers;
	}
	
	/// Returns the light probe's illuminance texture.
	/// @details The illuminance texture is a 12x1 RGBA floating-point LUT which encodes the column vectors of three spherical harmonics illuminance matrices in the layout `R0,R1,R2,R3,G0,G1,G2,G3,B0,B1,B2,B3`. The matrices `R`, `G`, and `B` can be used to recover illuminance of the red, green, and blue color channels, respectively, for a given surface normal, `n`, as follows: `(dot(n, R * n), dot(n, G * n), dot(n, B * n))`, where `n = (x, y, z, 1)`.
	[[nodiscard]] inline const std::shared_ptr<gl::texture_1d>& get_illuminance_texture() const noexcept
	{
		return m_illuminance_texture;
	}
	
	/// Returns the light probe's illuminance framebuffer.
	[[nodiscard]] inline const std::shared_ptr<gl::framebuffer>& get_illuminance_framebuffer() const noexcept
	{
		return m_illuminance_framebuffer;
	}
	
	/// Returns the light probe's illuminance matrices.
	/// @return Red, green, and blue illuminance matrices.
	/// @warning The light probe's illuminance matrices must first be updated.
	/// @see light_probe::update_illuminance_matrices()
	[[nodiscard]] inline std::span<const math::fmat4, 3> get_illuminance_matrices() const noexcept
	{
		return m_illuminance_matrices;
	}
	
	/// Returns `true` if the light probe's luminance is outdated.
	[[nodiscard]] inline bool is_luminance_outdated() const noexcept
	{
		return m_luminance_outdated;
	}
	
	/// Returns `true` if the light probe's illuminance is outdated.
	[[nodiscard]] inline bool is_illuminance_outdated() const noexcept
	{
		return m_illuminance_outdated;
	}
	
	/// Returns the light probe's bounds.
	[[nodiscard]] inline const aabb_type& get_bounds() const noexcept override
	{
		return m_bounds;
	}
	
private:
	void transformed() override;
	aabb_type m_bounds{};
	std::shared_ptr<gl::texture_cube> m_luminance_texture;
	std::vector<std::shared_ptr<gl::framebuffer>> m_luminance_framebuffers;
	std::shared_ptr<gl::texture_1d> m_illuminance_texture;
	std::shared_ptr<gl::framebuffer> m_illuminance_framebuffer;
	math::fmat4 m_illuminance_matrices[3]{};
	bool m_luminance_outdated{};
	bool m_illuminance_outdated{};
};

} // namespace scene

#endif // ANTKEEPER_SCENE_LIGHT_PROBE_HPP
