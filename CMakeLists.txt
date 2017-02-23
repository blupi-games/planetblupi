
cmake_minimum_required (VERSION 3.2)

include (${CMAKE_ROOT}/Modules/ExternalProject.cmake)

include_directories (${CMAKE_INSTALL_PREFIX}/include)
link_directories (${CMAKE_INSTALL_PREFIX}/lib)

project (planetblupi)
set (PB_VERSION_MAJOR 2)
set (PB_VERSION_MINOR 0)
set (PB_VERSION_PATCH 0)

# set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
# set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
# set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11 -L${CMAKE_INSTALL_PREFIX}/lib")
endif ()

file (GLOB_RECURSE sources src/*.cpp src/*.h)
file (GLOB_RECURSE po      resources/po/*.po)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  set (CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
endif ()

add_executable (planetblupi ${sources})

file (COPY resources/data  DESTINATION share/planetblupi)
file (COPY resources/image DESTINATION share/planetblupi)
file (COPY resources/movie DESTINATION share/planetblupi)
file (COPY resources/sound DESTINATION share/planetblupi)
file (COPY resources/music DESTINATION share/planetblupi)

# Dependencies

find_package (Intl REQUIRED)

find_package (PkgConfig REQUIRED)
pkg_search_module (SDL2 REQUIRED sdl2)
pkg_search_module (SDL2_MIXER REQUIRED SDL2_mixer)
pkg_search_module (SDL2_IMAGE REQUIRED SDL2_image)
# Static dependencies for SDL_kitchensink
# pkg_search_module (ASS REQUIRED libass)
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
#   ${ASS_STATIC_LIBRARIES}
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
  DEPENDS ${sources}
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
