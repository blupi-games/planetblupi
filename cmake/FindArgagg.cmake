# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

#.rst:
# FindArgagg
# --------
#
# Find the libargagg headers and libraries.
#
# This module reports information about the libargagg
# installation in several variables.  General variables::
#
#   Argagg_FOUND - true if the libargagg headers and libraries were found
#   Argagg_INCLUDE_DIRS - the directory containing the libargagg headers
#
# The following cache variables may also be set::
#
#   Argagg_INCLUDE_DIR - the directory containing the libargagg headers


# Based on FindIconv written by Roger Leigh <rleigh@codelibre.net>

# Find include directory
find_path(Argagg_INCLUDE_DIR
          NAMES "argagg/argagg.hpp"
          DOC "libargagg include directory")
mark_as_advanced(Argagg_INCLUDE_DIR)

include(${CMAKE_ROOT}/Modules/FindPackageHandleStandardArgs.cmake)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Argagg
                                  FOUND_VAR Argagg_FOUND
                                  REQUIRED_VARS Argagg_INCLUDE_DIR
                                  FAIL_MESSAGE "Failed to find libargagg")

if(Argagg_FOUND)
  set(Argagg_INCLUDE_DIRS "${Argagg_INCLUDE_DIR}")
endif()
