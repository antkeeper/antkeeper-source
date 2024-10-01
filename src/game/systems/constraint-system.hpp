// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_CONSTRAINT_SYSTEM_HPP
#define ANTKEEPER_GAME_CONSTRAINT_SYSTEM_HPP

#include "game/systems/updatable-system.hpp"
#include "game/components/transform-component.hpp"
#include "game/constraints/child-of-constraint.hpp"
#include "game/constraints/copy-rotation-constraint.hpp"
#include "game/constraints/copy-scale-constraint.hpp"
#include "game/constraints/copy-transform-constraint.hpp"
#include "game/constraints/copy-translation-constraint.hpp"
#include "game/constraints/ease-to-constraint.hpp"
#include "game/constraints/pivot-constraint.hpp"
#include "game/constraints/spring-rotation-constraint.hpp"
#include "game/constraints/spring-to-constraint.hpp"
#include "game/constraints/spring-translation-constraint.hpp"
#include "game/constraints/three-dof-constraint.hpp"
#include "game/constraints/track-to-constraint.hpp"
#include <engine/entity/id.hpp>


/// Applies constraint stacks to transform components.
class constraint_system:
	public updatable_system
{
public:
	explicit constraint_system(entity::registry& registry);
	~constraint_system() override;
	
	virtual void update(float t, float dt);
	
	/// Manually evaluates an entity's constraints.
	/// @param entity_id ID of a constrained entity.
	void evaluate(entity::id entity_id);
	
private:
	void on_constraint_stack_update(entity::registry& registry, entity::id constraint_stack_eid);
	
	void handle_constraint(transform_component& transform, entity::id constraint_eid, float dt);
	void handle_child_of_constraint(transform_component& transform, const child_of_constraint& constraint);
	void handle_copy_rotation_constraint(transform_component& transform, const copy_rotation_constraint& constraint);
	void handle_copy_scale_constraint(transform_component& transform, const copy_scale_constraint& constraint);
	void handle_copy_transform_constraint(transform_component& transform, const copy_transform_constraint& constraint);
	void handle_copy_translation_constraint(transform_component& transform, const copy_translation_constraint& constraint);
	void handle_ease_to_constraint(transform_component& transform, ease_to_constraint& constraint, float dt);
	void handle_pivot_constraint(transform_component& transform, const pivot_constraint& constraint);
	void handle_spring_rotation_constraint(transform_component& transform, spring_rotation_constraint& constraint, float dt);
	void handle_spring_to_constraint(transform_component& transform, spring_to_constraint& constraint, float dt);
	void handle_spring_translation_constraint(transform_component& transform, spring_translation_constraint& constraint, float dt);
	void handle_three_dof_constraint(transform_component& transform, const three_dof_constraint& constraint);
	void handle_track_to_constraint(transform_component& transform, const track_to_constraint& constraint);
};


#endif // ANTKEEPER_GAME_CONSTRAINT_SYSTEM_HPP
