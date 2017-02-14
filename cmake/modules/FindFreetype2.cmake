# - Try to find FREETYPE2
# This module defines
#  FREETYPE2_FOUND        - System has FREETYPE2
#  FREETYPE2_INCLUDE_DIRS - The FREETYPE2 include directories
#  FREETYPE2_LIBRARIES    - Link to these to use FREETYPE2

# Copyright (C) 2011-2014  Christopher J. Howard
#
# This file is part of Open Graphics Framework (OGF).
#
# OGF is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# OGF is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with OGF.  If not, see <http://www.gnu.org/licenses/>.

find_path(FREETYPE2_INCLUDE_DIR ft2build.h PATH_SUFFIXES include/freetype2 include)
find_library(FREETYPE2_LIBRARY NAMES freetype)

set(FREETYPE2_LIBRARIES ${FREETYPE2_LIBRARY})
set(FREETYPE2_INCLUDE_DIRS ${FREETYPE2_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
	FREETYPE2 DEFAULT_MSG
	FREETYPE2_LIBRARY FREETYPE2_INCLUDE_DIR)

mark_as_advanced(FREETYPE2_INCLUDE_DIR FREETYPE2_LIBRARY)
