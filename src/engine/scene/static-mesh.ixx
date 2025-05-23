// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.scene.static_mesh;
export import engine.scene.object;
export import engine.render.model;
export import engine.render.material;
import engine.render.operation;
import engine.utility.sized_types;
import <vector>;

export namespace engine::scene
{
	/// Static mesh object.
	class static_mesh: public object<static_mesh>
	{
	public:
		/// Constructs a static mesh from a model.
		/// @param model Model from which the static mesh will be constructed.
		explicit static_mesh(std::shared_ptr<render::model> model);

		/// Constructs a static mesh.
		static_mesh() = default;

		/// Destructs a static mesh.
		~static_mesh() override = default;

		/// Sets the model with which this model instance is associated.
		/// @param model Shared pointer to the model.
		void set_model(std::shared_ptr<render::model> model);

		/// Overwrites the material of a model group for this model instance.
		/// @param index Index of a model group.
		/// @param material Pointer to the material which should overwrite the model group's material. A value of `nullptr` indicates the material will not be overwritten.
		void set_material(usize index, std::shared_ptr<render::material> material);

		/// Resets all overwritten materials.
		void reset_materials();

		/// Returns the bounds of the static mesh.
		[[nodiscard]] inline const aabb_type& get_bounds() const noexcept override
		{
			return m_bounds;
		}

		/// Returns the model of the model instance.
		[[nodiscard]] inline std::shared_ptr<render::model> get_model() const noexcept
		{
			return m_model;
		}

		void render(render::context& ctx) const override;

	private:
		void update_bounds();
		void transformed() override;

		std::shared_ptr<render::model> m_model;
		mutable std::vector<render::operation> m_operations;
		aabb_type m_bounds{{0, 0, 0}, {0, 0, 0}};
	};
}
