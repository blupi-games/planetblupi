
cmake_minimum_required (VERSION 3.2)

include (${CMAKE_ROOT}/Modules/ExternalProject.cmake)

project (planetblupi)
set (PB_VERSION_MAJOR 2)
set (PB_VERSION_MINOR 0)
set (PB_VERSION_PATCH 0)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

file (GLOB_RECURSE sources src/*.cpp src/*.h)
file (GLOB_RECURSE data    resources/data/*)
file (GLOB_RECURSE image   resources/image/*)
file (GLOB_RECURSE movie   resources/movie/*)
file (GLOB_RECURSE sound   resources/sound/*)

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

##################
## SDL_kitchensink
ExternalProject_Add (SDL_kitchensink
  GIT_REPOSITORY https://github.com/katajakasa/SDL_kitchensink.git
  PREFIX ${CMAKE_CURRENT_BINARY_DIR}/SDL_kitchensink
  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
  INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}
)

ExternalProject_Get_Property (SDL_kitchensink install_dir)
include_directories (${install_dir}/include)

add_dependencies (planetblupi SDL_kitchensink)
## SDL_kitchensink
## ##################

target_link_libraries (planetblupi PUBLIC
  ${Intl_LIBRARIES}
  ${SDL2_LIBRARIES}
  ${SDL2_MIXER_LIBRARIES}
  ${install_dir}/lib/libSDL_kitchensink.dll.a
)
