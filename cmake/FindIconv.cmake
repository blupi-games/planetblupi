# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

#.rst:
# FindIconv
# --------
#
# Find the libiconv headers and libraries.
#
# This module reports information about the libiconv
# installation in several variables.  General variables::
#
#   Iconv_FOUND - true if the libiconv headers and libraries were found
#   Iconv_INCLUDE_DIRS - the directory containing the libiconv headers
#   Iconv_LIBRARIES - libiconv libraries to be linked
#
# The following cache variables may also be set::
#
#   Iconv_INCLUDE_DIR - the directory containing the libiconv headers
#   Iconv_LIBRARY - the libiconv library (if any)


# Written by Roger Leigh <rleigh@codelibre.net>

# Find include directory
find_path(Iconv_INCLUDE_DIR
          NAMES "iconv.h"
          DOC "libiconv include directory")
mark_as_advanced(Iconv_INCLUDE_DIR)

# Find all Iconv libraries
find_library(Iconv_LIBRARY "iconv"
  DOC "libiconv libraries)")
mark_as_advanced(Iconv_LIBRARY)

include(${CMAKE_ROOT}/Modules/FindPackageHandleStandardArgs.cmake)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Iconv
                                  FOUND_VAR Iconv_FOUND
                                  REQUIRED_VARS Iconv_INCLUDE_DIR
                                  FAIL_MESSAGE "Failed to find libiconv")

if(Iconv_FOUND)
  set(Iconv_INCLUDE_DIRS "${Iconv_INCLUDE_DIR}")
  if(Iconv_LIBRARY)
    set(Iconv_LIBRARIES "${Iconv_LIBRARY}")
  else()
    unset(Iconv_LIBRARIES)
  endif()
endif()
