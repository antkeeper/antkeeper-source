/*
 * Copyright (C) 2020  Christopher J. Howard
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

#ifndef ANTKEEPER_MATERIAL_FLAGS_HPP
#define ANTKEEPER_MATERIAL_FLAGS_HPP

#define MATERIAL_FLAG_OPAQUE 0x00
#define MATERIAL_FLAG_TRANSLUCENT 0x01
#define MATERIAL_FLAG_FRONT_FACES 0x00
#define MATERIAL_FLAG_BACK_FACES 0x02
#define MATERIAL_FLAG_FRONT_AND_BACK_FACES 0x04
#define MATERIAL_FLAG_SHADOW_CASTER 0x00
#define MATERIAL_FLAG_NOT_SHADOW_CASTER 0x08
#define MATERIAL_FLAG_X_RAY 0x10
#define MATERIAL_FLAG_OUTLINE 0x20
#define MATERIAL_FLAG_VEGETATION 0x40
#define MATERIAL_FLAG_WIREFRAME 0x80000000

#endif // ANTKEEPER_MATERIAL_FLAGS_HPP

