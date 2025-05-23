// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.render.model;
import engine.gl.vertex_array;
import engine.gl.vertex_buffer;
import engine.gl.primitive_topology;
import engine.render.material;
import engine.geom.primitives.box;
import engine.geom.brep.mesh;
import engine.animation.skeleton;
import engine.hash.fnv;
import engine.utility.sized_types;
export import <memory>;
export import <vector>;

export namespace engine::render
{
	/// Part of a model which is associated with exactly one material.
	struct model_group
	{
		hash::fnv32_t id{};
		gl::primitive_topology primitive_topology{gl::primitive_topology::triangle_list};
		u32 first_vertex{};
		u32 vertex_count{};
		u32 material_index{};
	};

	/// A 3D model.
	class model
	{
	public:
		/// AABB type.
		using aabb_type = geom::box<float>;

		/// Constructs a model from a B-rep mesh.
		/// @param mesh Mesh from which to construct the model.
		explicit model(std::shared_ptr<engine::geom::brep::mesh> mesh);

		/// Constructs an empty model.
		model() = default;
	
		/// Sets the byte offset to the first vertex in the vertex buffer.
		/// @param offset Byte offset into the vertex buffer.
		inline void set_vertex_offset(usize offset) noexcept
		{
			m_vertex_offset = offset;
		}
	
		/// Sets the byte stride between consecutive elements within the vertex buffer.
		/// @param stride Byte stride between consecutive elements within the vertex buffer.
		inline void set_vertex_stride(usize stride) noexcept
		{
			m_vertex_stride = stride;
		}
	
		/// Returns the vertex array associated with this model.
		[[nodiscard]] inline std::shared_ptr<gl::vertex_array>& get_vertex_array() noexcept
		{
			return m_vertex_array;
		}

		/// @copydoc get_vertex_array()
		[[nodiscard]] inline const std::shared_ptr<gl::vertex_array>& get_vertex_array() const noexcept
		{
			return m_vertex_array;
		}

		/// Returns the vertex buffer associated with this model.
		[[nodiscard]] inline std::shared_ptr<gl::vertex_buffer>& get_vertex_buffer() noexcept
		{
			return m_vertex_buffer;
		}

		/// @copydoc get_vertex_buffer()
		[[nodiscard]] inline const std::shared_ptr<gl::vertex_buffer>& get_vertex_buffer() const noexcept
		{
			return m_vertex_buffer;
		}

		/// Returns the byte offset to the first vertex in the vertex buffer.
		[[nodiscard]] inline constexpr usize get_vertex_offset() const noexcept
		{
			return m_vertex_offset;
		}
	
		/// Returns the byte stride between consecutive elements within the vertex buffer.
		[[nodiscard]] inline constexpr usize get_vertex_stride() const noexcept
		{
			return m_vertex_stride;
		}
	
		/// Returns the bounds of the model.
		[[nodiscard]] inline aabb_type& get_bounds() noexcept
		{
			return m_bounds;
		}

		/// @copydoc get_bounds()
		[[nodiscard]] inline const aabb_type& get_bounds() const noexcept
		{
			return m_bounds;
		}
	
		/// Returns the model's model groups.
		[[nodiscard]] inline std::vector<model_group>& get_groups() noexcept
		{
			return m_groups;
		}

		/// @copydoc get_groups()
		[[nodiscard]] inline const std::vector<model_group>& get_groups() const noexcept
		{
			return m_groups;
		}

		/// Returns a reference to the shared mesh with which this model is associated.
		[[nodiscard]] inline constexpr auto& mesh() noexcept
		{
			return m_mesh;
		}

		/// @copydoc mesh()
		[[nodiscard]] inline constexpr const auto& mesh() const noexcept
		{
			return m_mesh;
		}

		/// Returns a reference to the shared materials of the model.
		[[nodiscard]] inline constexpr auto& materials() noexcept
		{
			return m_materials;
		}

		/// @copydoc materials()
		[[nodiscard]] inline constexpr const auto& materials() const noexcept
		{
			return m_materials;
		}

		/// Returns a reference to the shared skeleton with which this model is associated.
		[[nodiscard]] inline constexpr auto& skeleton() noexcept
		{
			return m_skeleton;
		}

		/// @copydoc skeleton()
		[[nodiscard]] inline constexpr const auto& skeleton() const noexcept
		{
			return m_skeleton;
		}

		/// Rebuilds the model from its mesh.
		void rebuild();
	
	private:
		std::shared_ptr<engine::geom::brep::mesh> m_mesh;
		std::vector<std::shared_ptr<material>> m_materials;
		std::shared_ptr<animation::skeleton> m_skeleton;
		std::shared_ptr<gl::vertex_array> m_vertex_array;
		std::shared_ptr<gl::vertex_buffer> m_vertex_buffer;
		usize m_vertex_offset{};
		usize m_vertex_stride{};
		aabb_type m_bounds{{0, 0, 0}, {0, 0, 0}};
		std::vector<model_group> m_groups;
	};
}
