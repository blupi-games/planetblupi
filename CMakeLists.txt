
cmake_minimum_required (VERSION 3.2)

include (${CMAKE_ROOT}/Modules/ExternalProject.cmake)

project (planetblupi)
set (PB_VERSION_MAJOR 2)
set (PB_VERSION_MINOR 0)
set (PB_VERSION_PATCH 0)

set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
endif ()

file (GLOB_RECURSE sources src/*.cpp src/*.h)
file (GLOB_RECURSE data    resources/data/*)
file (GLOB_RECURSE image   resources/image/*)
file (GLOB_RECURSE movie   resources/movie/*)
file (GLOB_RECURSE sound   resources/sound/*)
file (GLOB_RECURSE po      resources/translations/*.po)

include_directories (${CMAKE_CURRENT_SOURCE_DIR})

add_executable (planetblupi ${sources} ${data} ${image} ${movie} ${sound})

file (COPY ${data}  DESTINATION data)
file (COPY ${image} DESTINATION image)
file (COPY ${movie} DESTINATION movie)
file (COPY ${sound} DESTINATION sound)

# Dependencies

find_package (PkgConfig)
find_package (Intl REQUIRED)
find_package (SDL2 REQUIRED)

find_package (PkgConfig REQUIRED)
pkg_search_module (SDL2_MIXER REQUIRED SDL2_mixer)
pkg_search_module (SDL2_IMAGE REQUIRED SDL2_image)

##################
## SDL_kitchensink

ExternalProject_Add (SDL_kitchensink_Project
  GIT_REPOSITORY https://github.com/katajakasa/SDL_kitchensink.git
  PREFIX ${CMAKE_CURRENT_BINARY_DIR}
  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
  INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}
)

ExternalProject_Get_Property (SDL_kitchensink_Project install_dir)
include_directories (${install_dir}/include)

if (WIN32)
add_library (SDL_kitchensink STATIC IMPORTED)
set_property (TARGET SDL_kitchensink PROPERTY IMPORTED_LOCATION ${install_dir}/lib/libSDL_kitchensink.dll.a)
else ()
add_library (SDL_kitchensink SHARED IMPORTED)
set_property (TARGET SDL_kitchensink PROPERTY IMPORTED_LOCATION ${install_dir}/lib/libSDL_kitchensink.so)
endif ()

add_dependencies (planetblupi SDL_kitchensink_Project)

## SDL_kitchensink
## ##################

target_link_libraries (planetblupi PUBLIC
  ${Intl_LIBRARIES}
  ${SDL2_LIBRARIES}
  ${SDL2_MIXER_LIBRARIES}
  ${SDL2_IMAGE_LIBRARIES}
  SDL_kitchensink
)

## GetText

find_package (Gettext)

set (_potFile ${CMAKE_CURRENT_SOURCE_DIR}/resources/translations/${PROJECT_NAME}.pot)

add_custom_command (OUTPUT ${_potFile}
  COMMAND xgettext --keyword=translate --keyword=ptranslate:1c,2 -o ${_potFile} ${sources}
  DEPENDS ${_src_list}
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMENT "Extract translatable messages to ${_potFile}"
)

add_custom_target (pot_file ALL ${_all}
  DEPENDS ${_potFile}
)

gettext_create_translations (${_potFile} ALL ${po})
