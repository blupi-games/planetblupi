# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

#.rst:
# FindSDLKitchensink
# --------
#
# Find the libsdlkitchensink headers and libraries.
#
# This module reports information about the libsdlkitchensink
# installation in several variables.  General variables::
#
#   SDLKitchensink_FOUND - true if the libsdlkitchensink headers and libraries were found
#   SDLKitchensink_INCLUDE_DIRS - the directory containing the libsdlkitchensink headers
#   SDLKitchensink_LIBRARIES - libsdlkitchensink libraries to be linked
#
# The following cache variables may also be set::
#
#   SDLKitchensink_INCLUDE_DIR - the directory containing the libsdlkitchensink headers
#   SDLKitchensink_LIBRARY - the libsdlkitchensink library (if any)


# Based on FindIconv written by Roger Leigh <rleigh@codelibre.net>

# Find include directory
find_path(SDLKitchensink_INCLUDE_DIR
          NAMES "kitchensink/kitchensink.h"
          DOC "libsdlkitchensink include directory")
mark_as_advanced(SDLKitchensink_INCLUDE_DIR)

# Find all SDLKitchensink libraries
find_library(SDLKitchensink_LIBRARY
  NAMES SDL_kitchensink
  DOC "libsdlkitchensink libraries)")
find_library(SDLKitchensink_STATIC_LIBRARY
  NAMES SDL_kitchensink_static
  DOC "libsdlkitchensink static libraries)")
mark_as_advanced(SDLKitchensink_STATIC_LIBRARY)

include(${CMAKE_ROOT}/Modules/FindPackageHandleStandardArgs.cmake)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(SDLKitchensink
                                  FOUND_VAR SDLKitchensink_FOUND
                                  REQUIRED_VARS SDLKitchensink_INCLUDE_DIR
                                  FAIL_MESSAGE "Failed to find libsdlkitchensink")

if(SDLKitchensink_FOUND)
  set(SDLKitchensink_INCLUDE_DIRS "${SDLKitchensink_INCLUDE_DIR}")
  if(SDLKitchensink_LIBRARY)
    set(SDLKitchensink_LIBRARIES "${SDLKitchensink_LIBRARY}")
  else()
    unset(SDLKitchensink_LIBRARIES)
  endif()
  if(SDLKitchensink_STATIC_LIBRARY)
    set(SDLKitchensink_STATIC_LIBRARIES "${SDLKitchensink_STATIC_LIBRARY}")
  else()
    unset(SDLKitchensink_STATIC_LIBRARIES)
  endif()
endif()
