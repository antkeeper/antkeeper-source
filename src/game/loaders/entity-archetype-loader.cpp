/*
 * Copyright (C) 2023  Christopher J. Howard
 *
 * This file is part of Antkeeper source code.
 *
 * Antkeeper source code is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Antkeeper source code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Antkeeper source code.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <engine/resources/resource-loader.hpp>
#include <engine/resources/resource-manager.hpp>
#include <engine/render/model.hpp>
#include <engine/math/angles.hpp>
#include "game/components/atmosphere-component.hpp"
#include "game/components/behavior-component.hpp"
#include "game/components/collision-component.hpp"
#include "game/components/diffuse-reflector-component.hpp"
#include "game/components/terrain-component.hpp"
#include "game/components/transform-component.hpp"
#include "game/components/scene-component.hpp"
#include "game/components/orbit-component.hpp"
#include "game/components/blackbody-component.hpp"
#include "game/components/celestial-body-component.hpp"
#include <engine/entity/archetype.hpp>
#include <engine/physics/orbit/elements.hpp>
#include <engine/utility/json.hpp>
#include <engine/scene/static-mesh.hpp>
#include <stdexcept>

static bool load_component_atmosphere(entity::archetype& archetype, const json& element)
{
	::atmosphere_component component;
	
	if (element.contains("upper_limit"))
		component.upper_limit = element["upper_limit"].get<double>();
	if (element.contains("index_of_refraction"))
		component.index_of_refraction = element["index_of_refraction"].get<double>();
	
	if (element.contains("rayleigh_concentration"))
		component.rayleigh_concentration = element["rayleigh_concentration"].get<double>();
	if (element.contains("rayleigh_scale_height"))
		component.rayleigh_scale_height = element["rayleigh_scale_height"].get<double>();
	
	if (element.contains("mie_concentration"))
		component.mie_concentration = element["mie_concentration"].get<double>();
	if (element.contains("mie_scale_height"))
		component.mie_scale_height = element["mie_scale_height"].get<double>();
	if (element.contains("mie_anisotropy"))
		component.mie_anisotropy = element["mie_anisotropy"].get<double>();
	if (element.contains("mie_albedo"))
		component.mie_albedo = element["mie_albedo"].get<double>();
	
	if (element.contains("ozone_concentration"))
		component.ozone_concentration = element["ozone_concentration"].get<double>();
	if (element.contains("ozone_lower_limit"))
		component.ozone_lower_limit = element["ozone_lower_limit"].get<double>();
	if (element.contains("ozone_upper_limit"))
		component.ozone_upper_limit = element["ozone_upper_limit"].get<double>();
	if (element.contains("ozone_mode"))
		component.ozone_mode = element["ozone_mode"].get<double>();
	
	if (element.contains("airglow_illuminance"))
	{
		const auto& airglow_illuminance = element["airglow_illuminance"];
		component.airglow_illuminance.x() = airglow_illuminance[0].get<double>();
		component.airglow_illuminance.y() = airglow_illuminance[1].get<double>();
		component.airglow_illuminance.z() = airglow_illuminance[2].get<double>();
	}
	
	archetype.stamps.push_back
	(
		[component](entt::handle& handle)
		{
			handle.emplace_or_replace<decltype(component)>(component);
		}
	);

	return true;
}

static bool load_component_blackbody(entity::archetype& archetype, const json& element)
{
	::blackbody_component component;
	component.temperature = 0.0;
	
	if (element.contains("temperature"))
		component.temperature = element["temperature"].get<double>();

	archetype.stamps.push_back
	(
		[component](entt::handle& handle)
		{
			handle.emplace_or_replace<decltype(component)>(component);
		}
	);

	return true;
}

static bool load_component_celestial_body(entity::archetype& archetype, const json& element)
{
	::celestial_body_component component;
	
	if (element.contains("radius"))
		component.radius = element["radius"].get<double>();
	if (element.contains("mass"))
		component.mass = element["mass"].get<double>();
	if (element.contains("pole_ra"))
	{
		component.pole_ra.clear();
		auto& pole_ra_element = element["pole_ra"];
		for (auto it = pole_ra_element.rbegin(); it != pole_ra_element.rend(); ++it) 
			component.pole_ra.push_back(math::radians(it->get<double>()));
	}
	if (element.contains("pole_dec"))
	{
		component.pole_dec.clear();
		auto& pole_dec_element = element["pole_dec"];
		for (auto it = pole_dec_element.rbegin(); it != pole_dec_element.rend(); ++it) 
			component.pole_dec.push_back(math::radians(it->get<double>()));
	}
	if (element.contains("prime_meridian"))
	{
		component.prime_meridian.clear();
		auto& prime_meridian_element = element["prime_meridian"];
		for (auto it = prime_meridian_element.rbegin(); it != prime_meridian_element.rend(); ++it) 
			component.prime_meridian.push_back(math::radians(it->get<double>()));
	}
	if (element.contains("albedo"))
		component.albedo = element["albedo"].get<double>();

	archetype.stamps.push_back
	(
		[component](entt::handle& handle)
		{
			handle.emplace_or_replace<decltype(component)>(component);
		}
	);

	return true;
}

static bool load_component_collision(entity::archetype& archetype, resource_manager& resource_manager, const json& element)
{
	::collision_component component;
	component.mesh = nullptr;
	
	if (element.contains("file"))
	{
		//component.mesh = resource_manager.load<geom::mesh>(element["file"].get<std::string>());
	}
	
	archetype.stamps.push_back
	(
		[component](entt::handle& handle)
		{
			handle.emplace_or_replace<decltype(component)>(component);
		}
	);
	
	return (component.mesh != nullptr);
}

static bool load_component_diffuse_reflector(entity::archetype& archetype, const json& element)
{
	::diffuse_reflector_component component;
	component.albedo = 0.0;
	
	if (element.contains("albedo"))
		component.albedo = element["albedo"].get<double>();

	archetype.stamps.push_back
	(
		[component](entt::handle& handle)
		{
			handle.emplace_or_replace<decltype(component)>(component);
		}
	);

	return true;
}

static bool load_component_model(entity::archetype& archetype, resource_manager& resource_manager, const json& element)
{
	std::shared_ptr<render::model> model;
	if (element.contains("file"))
	{
		model = resource_manager.load<render::model>(element["file"].get<std::string>());
	}
	
	archetype.stamps.push_back
	(
		[model](entt::handle& handle)
		{
			handle.emplace_or_replace<scene_component>
			(
				std::make_unique<scene::static_mesh>(model),
				std::uint8_t{0b00000001}
			);
		}
	);
	
	return true;
}

static bool load_component_orbit(entity::archetype& archetype, const json& element)
{
	::orbit_component component;
	
	component.parent = entt::null;
	component.ephemeris_index = -1;
	component.scale = 1.0;
	component.position = {0, 0, 0};
	
	if (element.contains("ephemeris_index"))
		component.ephemeris_index = element["ephemeris_index"].get<int>();
	if (element.contains("scale"))
		component.scale = element["scale"].get<double>();

	archetype.stamps.push_back
	(
		[component](entt::handle& handle)
		{
			handle.emplace_or_replace<decltype(component)>(component);
		}
	);

	return true;
}

static bool load_component_transform(entity::archetype& archetype, const json& element)
{
	::transform_component component;
	component.local = math::transform<float>::identity;
	component.warp = true;
	
	if (element.contains("translation"))
	{
		auto translation = element["translation"];
		component.local.translation.x() = translation[0].get<float>();
		component.local.translation.y() = translation[1].get<float>();
		component.local.translation.z() = translation[2].get<float>();
	}
	
	if (element.contains("rotation"))
	{
		auto translation = element["rotation"];
		component.local.rotation.w() = translation[0].get<float>();
		component.local.rotation.x() = translation[1].get<float>();
		component.local.rotation.y() = translation[2].get<float>();
		component.local.rotation.z() = translation[3].get<float>();
	}
	
	if (element.contains("scale"))
	{
		auto translation = element["scale"];
		component.local.scale.x() = translation[0].get<float>();
		component.local.scale.y() = translation[1].get<float>();
		component.local.scale.z() = translation[2].get<float>();
	}
	
	component.world = component.local;

	archetype.stamps.push_back
	(
		[component](entt::handle& handle)
		{
			handle.emplace_or_replace<decltype(component)>(component);
		}
	);

	return true;
}

static bool load_component(entity::archetype& archetype, resource_manager& resource_manager, json::const_iterator element)
{
	if (element.key() == "atmosphere")
		return load_component_atmosphere(archetype, element.value());
	// if (element.key() == "behavior")
		// return load_component_behavior(archetype, resource_manager, element.value());
	if (element.key() == "blackbody")
		return load_component_blackbody(archetype, element.value());
	if (element.key() == "celestial_body")
		return load_component_celestial_body(archetype, element.value());
	if (element.key() == "collision")
		return load_component_collision(archetype, resource_manager, element.value());
	if (element.key() == "diffuse_reflector")
		return load_component_diffuse_reflector(archetype, element.value());
	if (element.key() == "model")
		return load_component_model(archetype, resource_manager, element.value());
	if (element.key() == "orbit")
		return load_component_orbit(archetype, element.value());
	if (element.key() == "transform")
		return load_component_transform(archetype, element.value());
	
	//throw std::runtime_error("Unknown component type \"" + element.key() + "\"");
	
	return false;
}

template <>
std::unique_ptr<entity::archetype> resource_loader<entity::archetype>::load(::resource_manager& resource_manager, deserialize_context& ctx)
{
	// Load JSON data
	auto json_data = resource_loader<nlohmann::json>::load(resource_manager, ctx);
	
	// Allocate archetype
	std::unique_ptr<entity::archetype> archetype = std::make_unique<entity::archetype>();
	
	// Load components from table rows
	for (json::const_iterator element = json_data->cbegin(); element != json_data->cend(); ++element)
	{
		if (!load_component(*archetype, resource_manager, element))
		{
			throw std::runtime_error("Failed to load component \"" + element.key() + "\"");
		}
	}
	
	return archetype;
}
