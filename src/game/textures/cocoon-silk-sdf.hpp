// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_COCOON_SILK_SDF_HPP
#define ANTKEEPER_COCOON_SILK_SDF_HPP

#include <filesystem>

/// Generates the cocoon silk signed distance field texture.
void generate_silk_sdf_image(std::filesystem::path path);

#endif // ANTKEEPER_COCOON_SILK_SDF_HPP
