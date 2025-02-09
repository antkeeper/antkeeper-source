// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_ASTRONOMY_SYSTEM_HPP
#define ANTKEEPER_GAME_ASTRONOMY_SYSTEM_HPP

#include "game/systems/fixed-update-system.hpp"
#include <engine/entity/id.hpp>
#include <engine/scene/directional-light.hpp>
#include <engine/math/vector.hpp>
#include <engine/math/se3.hpp>
#include <engine/render/passes/sky-pass.hpp>
#include "game/components/observer-component.hpp"
#include "game/components/atmosphere-component.hpp"
#include "game/components/celestial-body-component.hpp"
#include "game/components/orbit-component.hpp"
#include <engine/geom/primitives/ray.hpp>

/// Calculates apparent properties of celestial bodies as seen by an observer.
class astronomy_system:
	public fixed_update_system
{
public:
	explicit astronomy_system(entity::registry& registry);
	~astronomy_system() override;
	void fixed_update(entity::registry& registry, float t, float dt) override;
	
	/// Sets the current time.
	/// @param t Time since epoch, in days.
	void set_time(double t);
	
	/// Sets the observer entity.
	/// @param eid Entity ID of the observer.
	void set_observer(entity::id eid);
	
	/// Sets the number of samples to take when integrating atmospheric transmittance.
	/// @param samples Number of integration samples.
	void set_transmittance_samples(std::size_t samples);
	
	void set_sun_light(scene::directional_light* light);
	void set_moon_light(scene::directional_light* light);
	void set_starlight_illuminance(const math::dvec3& illuminance);
	void set_sky_pass(::render::sky_pass* pass);
	
	[[nodiscard]] inline double get_time() const noexcept
	{
		return m_time_days;
	}
	
private:
	void on_observer_modified(entity::registry& registry, entity::id entity_id);
	void on_observer_destroyed(entity::registry& registry, entity::id entity_id);
	void on_celestial_body_modified(entity::registry& registry, entity::id entity_id);
	void on_celestial_body_destroyed(entity::registry& registry, entity::id entity_id);
	void on_orbit_modified(entity::registry& registry, entity::id entity_id);
	void on_orbit_destroyed(entity::registry& registry, entity::id entity_id);
	void on_atmosphere_modified(entity::registry& registry, entity::id entity_id);
	void on_atmosphere_destroyed(entity::registry& registry, entity::id entity_id);
	
	/// Called each time the observer is modified.
	void observer_modified();
	
	/// Called each time the celestial body of the reference body is modified.
	void reference_body_modified();
	
	/// Called each time the orbit of the reference body is modified.
	void reference_orbit_modified();
	
	/// Called each time the atmosphere of the reference body is modified.
	void reference_atmosphere_modified();

	/// Updates the BCBF to EUS transformation.
	void update_bcbf_to_eus(const ::observer_component& observer, const ::celestial_body_component& body);
	
	/// Updates the ICRF to EUS transformation.
	void update_icrf_to_eus(const ::celestial_body_component& body, const ::orbit_component& orbit);
	
	/// Integrates a transmittance factor due to atmospheric extinction along a ray.
	/// @param ray Ray to cast, in the EUS frame.
	/// @param samples Number of samples to integrate.
	/// @return Spectral transmittance factor.
	[[nodiscard]] math::dvec3 integrate_transmittance(const ::observer_component& observer, const ::celestial_body_component& body, const ::atmosphere_component& atmosphere, geom::ray<double, 3> ray) const;
	
	entity::registry& m_registry;

	/// Time since epoch, in days.
	double m_time_days{};
	
	/// Time since epoch, in centuries.
	double m_time_centuries{};
	
	/// Number of transmittance integration samples.
	std::size_t m_transmittance_samples{};
	
	/// Entity ID of the observer.
	entity::id m_observer_eid{entt::null};
	
	/// Entity ID of the reference body.
	entity::id m_reference_body_eid{entt::null};
	
	/// ENU to EUS transformation.
	math::se3<double> m_enu_to_eus{};
	
	/// BCBF to EUS transformation.
	math::se3<double> m_bcbf_to_eus{};
	
	/// ICRF to EUS tranformation.
	math::se3<double> m_icrf_to_eus{};
	
	scene::directional_light* m_sun_light{};
	scene::directional_light* m_moon_light{};
	::render::sky_pass* m_sky_pass{};
	math::dvec3 m_starlight_illuminance{};
};

#endif // ANTKEEPER_GAME_ASTRONOMY_SYSTEM_HPP
