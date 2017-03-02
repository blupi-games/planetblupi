
cmake_minimum_required (VERSION 3.2)

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

include (${CMAKE_ROOT}/Modules/ExternalProject.cmake)

include_directories (${CMAKE_INSTALL_PREFIX}/include)
link_directories (${CMAKE_INSTALL_PREFIX}/lib)

project (planetblupi)
set (PB_VERSION_MAJOR 2)
set (PB_VERSION_MINOR 0)
set (PB_VERSION_PATCH 0)

configure_file (
  "${PROJECT_SOURCE_DIR}/src/config.h.in"
  "${PROJECT_BINARY_DIR}/include/config.h"
)

include_directories ("${PROJECT_BINARY_DIR}/include")

file (GLOB_RECURSE sources src/*.cxx src/*.h)
file (GLOB_RECURSE po      resources/po/*.po)

if (MINGW)
  file (GLOB_RECURSE rc resources/win32/*.rc)
  list (APPEND sources ${rc})
endif (MINGW)

if (APPIMAGE_APPRUN_PROGRAM AND APPIMAGE_ASSISTANT_PROGRAM)
  set (USE_APPIMAGE ON)
endif ()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11 -L${CMAKE_INSTALL_PREFIX}/lib")
endif ()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  set (CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
endif ()

if (USE_APPIMAGE AND "${CMAKE_BUILD_TYPE}" STREQUAL "Release")
  set (CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -s")
  set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s")
endif ()

add_executable (planetblupi ${sources})

file (COPY resources/data  DESTINATION share/planetblupi)
file (COPY resources/image DESTINATION share/planetblupi)
file (COPY resources/movie DESTINATION share/planetblupi)
file (COPY resources/sound DESTINATION share/planetblupi)
file (COPY resources/music DESTINATION share/planetblupi)
file (COPY resources/icon/planetblupi.png DESTINATION share/planetblupi)

# Windows stuff

if (MINGW)
  set (CMAKE_RC_COMPILER_INIT windres)
  enable_language (RC)
  set (CMAKE_RC_COMPILE_OBJECT "<CMAKE_RC_COMPILER> <FLAGS> -O coff <DEFINES> -i <SOURCE> -o <OBJECT>")
endif (MINGW)

# Dependencies

find_package (Intl REQUIRED)

find_package (PkgConfig REQUIRED)
pkg_search_module (SDL2 REQUIRED sdl2)
pkg_search_module (SDL2_MIXER REQUIRED SDL2_mixer)
pkg_search_module (SDL2_IMAGE REQUIRED SDL2_image)
# Static dependencies for SDL_kitchensink
pkg_search_module (PNG REQUIRED libpng)
pkg_search_module (AVCODEC REQUIRED libavcodec)
pkg_search_module (AVFORMAT REQUIRED libavformat)
pkg_search_module (AVUTIL REQUIRED libavutil)
pkg_search_module (SWSCALE REQUIRED libswscale)
pkg_search_module (SWRESAMPLE REQUIRED libswresample)

##################
## SDL_kitchensink
##################

add_library (SDL_kitchensink STATIC IMPORTED)
set_property (TARGET SDL_kitchensink PROPERTY IMPORTED_LOCATION ${CMAKE_INSTALL_PREFIX}/lib/libSDL_kitchensink_static.a)

###########################
## Main binary dependencies
###########################

target_link_libraries (planetblupi PUBLIC
  ${Intl_LIBRARIES}
  ${SDL2_STATIC_LIBRARIES}
  ${SDL2_MIXER_STATIC_LIBRARIES}
  ${SDL2_IMAGE_STATIC_LIBRARIES}
  SDL_kitchensink
  ${PNG_STATIC_LIBRARIES}
  ${AVCODEC_STATIC_LIBRARIES}
  ${AVFORMAT_STATIC_LIBRARIES}
  ${AVUTIL_STATIC_LIBRARIES}
  ${SWSCALE_STATIC_LIBRARIES}
  ${SWRESAMPLE_STATIC_LIBRARIES}
)

##########
## GetText
##########

find_package (Gettext)

set (_potFile ${CMAKE_CURRENT_SOURCE_DIR}/resources/po/${PROJECT_NAME}.pot)

add_custom_command (OUTPUT ${_potFile}
  COMMAND xgettext --keyword=translate -o ${_potFile} ${sources}
  DEPENDS planetblupi ${sources}
  COMMENT "Extract translatable messages to ${_potFile}"
)

add_custom_target (pot_file ALL ${_all}
  DEPENDS ${_potFile}
)

gettext_create_translations (${_potFile} ALL ${po})

## Put mo files to appropriate directory
foreach (file ${_gmoFiles})
  get_filename_component (_lang ${file} NAME_WE)
  set (_out "share/locale/${_lang}/LC_MESSAGES")

  add_custom_command (OUTPUT ${_out}/planetblupi.mo
    COMMAND ${CMAKE_COMMAND} -E copy ${file} ${_out}/planetblupi.mo
    DEPENDS ${file}
  )

  add_custom_target ("po-${_lang}" ALL ${_all}
    DEPENDS ${_out}/planetblupi.mo
  )
endforeach (file)

# Installation

install (TARGETS planetblupi
  RUNTIME DESTINATION bin
)

install (DIRECTORY resources/data DESTINATION share/planetblupi)
install (DIRECTORY resources/image DESTINATION share/planetblupi)
install (DIRECTORY resources/movie DESTINATION share/planetblupi)
install (DIRECTORY resources/sound DESTINATION share/planetblupi)
install (DIRECTORY resources/music DESTINATION share/planetblupi)
install (FILES resources/icon/planetblupi.png DESTINATION share/planetblupi)

#########
## Deploy
#########

if (USE_APPIMAGE AND "${CMAKE_BUILD_TYPE}" STREQUAL "Release")
  include (LinuxAppImageBuild)
  set (CMAKE_PACKAGED_OUTPUT_PREFIX ${CMAKE_INSTALL_PREFIX})
  APPIMAGE_PACKAGE (planetblupi "planetblupi" "${CMAKE_CURRENT_SOURCE_DIR}/resources/linux" "share" "" "")
endif ()
