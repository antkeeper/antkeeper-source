// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.geom.brep.loop_container;
export import engine.geom.brep.element_container;
export import engine.geom.brep.loop;

export namespace engine::geom::brep
{
	/// B-rep loop container.
	class loop_container: public element_container<loop>
	{
	private:
		friend class mesh;
		friend class face_container;

		/// Destructs a B-rep loop container.
		~loop_container() override = default;

		/// Appends a new loop to the end of the container.
		/// @return Pointer to the new loop.
		loop* emplace_back() override;
	};
}
