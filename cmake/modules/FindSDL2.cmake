# - Try to find SDL2
# This module defines
#  SDL2_FOUND        - System has SDL2
#  SDL2_INCLUDE_DIRS - The SDL2 include directories
#  SDL2_LIBRARIES    - Link to these to use SDL2

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

set(SDL2_SEARCH_PATHS
	/)

find_path(SDL2_INCLUDE_DIR SDL.h
	NAMES SDL2
	PATH_SUFFIXES include/SDL2 include
	PATHS ${SDL2_SEARCH_PATHS})

find_library(SDL2MAIN_LIBRARY
	NAMES SDL2main
	PATH_SUFFIXES lib64 lib
	PATHS ${SDL2_SEARCH_PATHS})
	
find_library(SDL2_LIBRARY
	NAMES SDL2
	PATH_SUFFIXES lib64 lib
	PATHS ${SDL2_SEARCH_PATHS})

set(SDL2_LIBRARIES ${SDL2MAIN_LIBRARY} ${SDL2_LIBRARY})
set(SDL2_INCLUDE_DIRS ${SDL2_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
	SDL2 DEFAULT_MSG
	SDL2_LIBRARY SDL2MAIN_LIBRARY SDL2_INCLUDE_DIR)

mark_as_advanced(SDL2_INCLUDE_DIR SDL2_LIBRARY SDL2MAIN_LIBRARY)
