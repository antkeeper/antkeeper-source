// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.animation.ik_rig;
export import engine.animation.ik_constraint;
export import engine.animation.ik_solver;
import engine.scene.skeletal_mesh;
import engine.utility.sized_types;
export import <memory>;
import <vector>;

export namespace engine::animation
{
	/// Inverse kinematics rig.
	class ik_rig
	{
	public:
		/// Constructs an IK rig.
		/// @param skeletal_mesh Skeletal mesh with which to associate the IK rig.
		explicit ik_rig(scene::skeletal_mesh& skeletal_mesh);

		/// Returns the skeleton with which the IK rig is associated.
		[[nodiscard]] inline scene::skeletal_mesh& get_skeletal_mesh() noexcept
		{
			return *m_skeletal_mesh;
		}

		/// @copydoc get_skeletal_mesh() noexcept
		[[nodiscard]] inline const scene::skeletal_mesh& get_skeletal_mesh() const noexcept
		{
			return *m_skeletal_mesh;
		}

		/// @name Constraints
		/// @{

		/// Sets the IK constraint of a bone.
		/// @param index Index of a bone.
		/// @param constraint IK constraint of the bone.
		void set_constraint(usize index, std::shared_ptr<ik_constraint> constraint);

		/// Removes all constraints from the IK rig.
		void clear_constraints();

		/// Returns the IK constraint of a bone.
		/// @param index Index of a bone.
		/// @return Pointer to the IK constraint of the bone, or `nullptr` if the bone is unconstrained.
		[[nodiscard]] inline ik_constraint* get_constraint(usize index)
		{
			return m_constraints[index].get();
		}

		/// @copydoc get_constraint(usize)
		[[nodiscard]] inline const ik_constraint* get_constraint(usize index) const
		{
			return m_constraints[index].get();
		}

		/// @}

		/// @name Solvers
		/// @{

		/// Solves each solver in the IK rig.
		void solve();

		/// Adds a solver to the IK rig.
		/// @param solver IK solver to add.
		void add_solver(std::shared_ptr<ik_solver> solver);

		/// Removes all solvers from the IK rig.
		void remove_solvers();

		/// @}

	private:
		scene::skeletal_mesh* m_skeletal_mesh{nullptr};
		std::vector<std::shared_ptr<ik_constraint>> m_constraints;
		std::vector<std::shared_ptr<ik_solver>> m_solvers;
	};
}
