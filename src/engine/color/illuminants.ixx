// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.color.illuminants;
export import engine.math.vector;

export namespace engine::color
{
	/// @name CIE standard illuminants
	/// @{

	/// @name CIE 1931 2 Degree Standard Observer illuminants
	/// @{

	template <class T>
	inline constexpr math::vec2<T> deg2_a    = {T{0.44757}, T{0.40745}};
	template <class T>
	inline constexpr math::vec2<T> deg2_b    = {T{0.34842}, T{0.35161}};
	template <class T>
	inline constexpr math::vec2<T> deg2_c    = {T{0.31006}, T{0.31616}};
	template <class T>
	inline constexpr math::vec2<T> deg2_d50  = {T{0.34567}, T{0.35850}};
	template <class T>
	inline constexpr math::vec2<T> deg2_d55  = {T{0.33242}, T{0.34743}};
	template <class T>
	inline constexpr math::vec2<T> deg2_d65  = {T{0.31270}, T{0.32900}};
	template <class T>
	inline constexpr math::vec2<T> deg2_d75  = {T{0.29902}, T{0.31485}};
	template <class T>
	inline constexpr math::vec2<T> deg2_d93  = {T{0.28315}, T{0.29711}};
	template <class T>
	inline constexpr math::vec2<T> deg2_e    = {T{1} / T{3}, T{1} / T{3}};
	template <class T>
	inline constexpr math::vec2<T> deg2_f1   = {T{0.31310}, T{0.33727}};
	template <class T>
	inline constexpr math::vec2<T> deg2_f2   = {T{0.37208}, T{0.37529}};
	template <class T>
	inline constexpr math::vec2<T> deg2_f3   = {T{0.40910}, T{0.39430}};
	template <class T>
	inline constexpr math::vec2<T> deg2_f4   = {T{0.44018}, T{0.40329}};
	template <class T>
	inline constexpr math::vec2<T> deg2_f5   = {T{0.31379}, T{0.34531}};
	template <class T>
	inline constexpr math::vec2<T> deg2_f6   = {T{0.37790}, T{0.38835}};
	template <class T>
	inline constexpr math::vec2<T> deg2_f7   = {T{0.31292}, T{0.32933}};
	template <class T>
	inline constexpr math::vec2<T> deg2_f8   = {T{0.34588}, T{0.35875}};
	template <class T>
	inline constexpr math::vec2<T> deg2_f9   = {T{0.37417}, T{0.37281}};
	template <class T>
	inline constexpr math::vec2<T> deg2_f10  = {T{0.34609}, T{0.35986}};
	template <class T>
	inline constexpr math::vec2<T> deg2_f11  = {T{0.38052}, T{0.37713}};
	template <class T>
	inline constexpr math::vec2<T> deg2_f12  = {T{0.43695}, T{0.40441}};
	template <class T>
	inline constexpr math::vec2<T> deg2_led_b1   = {T{0.4560}, T{0.4078}};
	template <class T>
	inline constexpr math::vec2<T> deg2_led_b2   = {T{0.4357}, T{0.4012}};
	template <class T>
	inline constexpr math::vec2<T> deg2_led_b3   = {T{0.3756}, T{0.3723}};
	template <class T>
	inline constexpr math::vec2<T> deg2_led_b4   = {T{0.3422}, T{0.3502}};
	template <class T>
	inline constexpr math::vec2<T> deg2_led_b5   = {T{0.3118}, T{0.3236}};
	template <class T>
	inline constexpr math::vec2<T> deg2_led_bh1  = {T{0.4474}, T{0.4066}};
	template <class T>
	inline constexpr math::vec2<T> deg2_led_rgb1 = {T{0.4557}, T{0.4211}};
	template <class T>
	inline constexpr math::vec2<T> deg2_led_v1   = {T{0.4560}, T{0.4548}};
	template <class T>
	inline constexpr math::vec2<T> deg2_led_v2   = {T{0.3781}, T{0.3775}};

	/// @}

	/// @name CIE 1964 10 Degree Standard Observer illuminants
	/// @{

	template <class T>
	inline constexpr math::vec2<T> deg10_a   = {T{0.45117}, T{0.40594}};
	template <class T>
	inline constexpr math::vec2<T> deg10_b   = {T{0.34980}, T{0.35270}};
	template <class T>
	inline constexpr math::vec2<T> deg10_c   = {T{0.31039}, T{0.31905}};
	template <class T>
	inline constexpr math::vec2<T> deg10_d50 = {T{0.34773}, T{0.35952}};
	template <class T>
	inline constexpr math::vec2<T> deg10_d55 = {T{0.33411}, T{0.34877}};
	template <class T>
	inline constexpr math::vec2<T> deg10_d65 = {T{0.31382}, T{0.33100}};
	template <class T>
	inline constexpr math::vec2<T> deg10_d75 = {T{0.29968}, T{0.31740}};
	template <class T>
	inline constexpr math::vec2<T> deg10_d93 = {T{0.28327}, T{0.30043}};
	template <class T>
	inline constexpr math::vec2<T> deg10_e   = {T{1} / T{3}, T{1} / T{3}};
	template <class T>
	inline constexpr math::vec2<T> deg10_f1  = {T{0.31811}, T{0.33559}};
	template <class T>
	inline constexpr math::vec2<T> deg10_f2  = {T{0.37925}, T{0.36733}};
	template <class T>
	inline constexpr math::vec2<T> deg10_f3  = {T{0.41761}, T{0.38324}};
	template <class T>
	inline constexpr math::vec2<T> deg10_f4  = {T{0.44920}, T{0.39074}};
	template <class T>
	inline constexpr math::vec2<T> deg10_f5  = {T{0.31975}, T{0.34246}};
	template <class T>
	inline constexpr math::vec2<T> deg10_f6  = {T{0.38660}, T{0.37847}};
	template <class T>
	inline constexpr math::vec2<T> deg10_f7  = {T{0.31569}, T{0.32960}};
	template <class T>
	inline constexpr math::vec2<T> deg10_f8  = {T{0.34902}, T{0.35939}};
	template <class T>
	inline constexpr math::vec2<T> deg10_f9  = {T{0.37829}, T{0.37045}};
	template <class T>
	inline constexpr math::vec2<T> deg10_f10 = {T{0.35090}, T{0.35444}};
	template <class T>
	inline constexpr math::vec2<T> deg10_f11 = {T{0.38541}, T{0.37123}};
	template <class T>
	inline constexpr math::vec2<T> deg10_f12 = {T{0.44256}, T{0.39717}};

	/// @}

	/// @}
}
