// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.scene.object;
export import engine.math.vector;
export import engine.math.quaternion;
export import engine.math.transform;
export import engine.geom.primitives.box;
import engine.utility.sized_types;
import <atomic>;

export namespace engine::render
{
	struct context;
}

export namespace engine::scene
{
	/// Abstract base class for scene objects.
	class object_base
	{
	public:
		using vector_type = math::fvec3;
		using quaternion_type = math::fquat;
		using transform_type = math::transform<float>;
		using aabb_type = geom::box<float>;

		/// Destructs a scene object base.
		virtual ~object_base() = default;

		/// Returns the type ID for this scene object type.
		[[nodiscard]] virtual usize get_object_type_id() const noexcept = 0;

		/// Adds render operations to a render context.
		/// @param ctx Render context.
		inline virtual void render(render::context&) const {}

		/// Transforms the object to look at a target.
		/// @param position Object position.
		/// @param target Target position.
		/// @param up Up vector.
		void look_at(const vector_type& position, const vector_type& target, const vector_type& up);

		/// Sets the layer mask of the object.
		/// @param mask 32-bit layer mask in which each set bit represents a layer in which the object is visible.
		inline constexpr void set_layer_mask(u32 mask) noexcept
		{
			m_layer_mask = mask;
		}

		/// Sets the transform of the object.
		/// @param transform Object transform.
		inline void set_transform(const transform_type& transform)
		{
			m_transform = transform;
			transformed();
		}

		/// Sets the translation of the object.
		/// @param translation Object translation.
		inline void set_translation(const vector_type& translation)
		{
			m_transform.translation = translation;
			transformed();
		}

		/// Sets the rotation of the object.
		/// @param rotation Object rotation.
		inline void set_rotation(const quaternion_type& rotation)
		{
			m_transform.rotation = rotation;
			transformed();
		}

		/// Sets the scale of the object.
		/// @param scale Object scale.
		inline void set_scale(const vector_type& scale)
		{
			m_transform.scale = scale;
			transformed();
		}

		/// Sets the scale of the object.
		/// @param scale Object scale.
		inline void set_scale(float scale)
		{
			m_transform.scale = {scale, scale, scale};
			transformed();
		}

		/// Returns the layer mask of the object.
		[[nodiscard]] inline constexpr u32 get_layer_mask() const noexcept
		{
			return m_layer_mask;
		}

		/// Returns the transform of the object.
		[[nodiscard]] inline constexpr const transform_type& get_transform() const noexcept
		{
			return m_transform;
		}

		/// Returns the translation of the object.
		[[nodiscard]] inline constexpr const vector_type& get_translation() const noexcept
		{
			return m_transform.translation;
		}

		/// Returns the rotation of the object.
		[[nodiscard]] inline constexpr const quaternion_type& get_rotation() const noexcept
		{
			return m_transform.rotation;
		}

		/// Returns the scale of the object.
		[[nodiscard]] inline constexpr const vector_type& get_scale() const noexcept
		{
			return m_transform.scale;
		}

		/// Returns the bounds of the object.
		[[nodiscard]] virtual const aabb_type& get_bounds() const noexcept = 0;

	protected:
		static usize next_object_type_id();

		/// Called every time the scene object's tranform is changed.
		inline virtual void transformed() {}

		u32 m_layer_mask{1};
		transform_type m_transform{math::identity<transform_type>};
	};

	/// Abstract base class for lights, cameras, model instances, and other scene objects.
	/// @tparam T This should be the same class that's inheriting from the scene object, in order to give it a valid type-specific ID.
	template <class T>
	class object: public object_base
	{
	public:
		/// Unique type ID for this scene object type.
		static const std::atomic<usize> object_type_id;

		/// Destructs a scene base.
		~object() override = default;

		/// Returns the type ID for this scene object type.
		[[nodiscard]] inline usize get_object_type_id() const noexcept final
		{
			return object_type_id;
		}
	};

	/// @private
	template <typename T>
	const std::atomic<usize> object<T>::object_type_id{object_base::next_object_type_id()};
}
