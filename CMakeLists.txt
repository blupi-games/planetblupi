
cmake_minimum_required (VERSION 3.2)

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
set (CMAKE_CXX_STANDARD 11)

include (${CMAKE_ROOT}/Modules/ExternalProject.cmake)

include_directories (${CMAKE_INSTALL_PREFIX}/include)
link_directories (${CMAKE_INSTALL_PREFIX}/lib)

project (planetblupi)
set (PB_VERSION_MAJOR 1)
set (PB_VERSION_MINOR 11)
set (PB_VERSION_PATCH 0)
set (PB_VERSION_EXTRA "")
set (PB_PRODUCT_NAME "Planet Blupi")
set (PB_PACKAGE_NAME "planetblupi")
set (PB_DESCRIPTION "Planet Blupi - A delerious spell-binding game")

configure_file (
  "${PROJECT_SOURCE_DIR}/src/config.h.in"
  "${PROJECT_BINARY_DIR}/include/config.h"
)

include_directories ("${PROJECT_BINARY_DIR}/include")

file (GLOB_RECURSE sources src/*.cxx src/*.h)
file (GLOB_RECURSE po      resources/po/*.po)

if (APPIMAGE_APPRUN_PROGRAM AND APPIMAGE_ASSISTANT_PROGRAM)
  set (USE_APPIMAGE ON)
endif ()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11 -L${CMAKE_INSTALL_PREFIX}/lib")
endif ()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" AND "${STATIC_BUILD}")
  set (CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
endif ()

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3")
endif ()

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
  set (CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -s")
  set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s")
endif ()

# These copies are necessary with our stuff for AppImage because it's not
# supported by CPack.
if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
  file (COPY resources/data DESTINATION share/planetblupi)
else ()
  file (
    COPY resources/data DESTINATION share/planetblupi
    PATTERN "world2*.blp" EXCLUDE
    PATTERN "world*.dev.blp" EXCLUDE
  )
endif ()
file (COPY resources/image DESTINATION share/planetblupi)
file (COPY resources/movie DESTINATION share/planetblupi)
file (COPY resources/sound DESTINATION share/planetblupi)
file (COPY resources/music DESTINATION share/planetblupi)
file (COPY LICENSE.all DESTINATION share/doc/planetblupi)
file (RENAME "${CMAKE_BINARY_DIR}/share/doc/planetblupi/LICENSE.all"
             "${CMAKE_BINARY_DIR}/share/doc/planetblupi/copyright")

if (UNIX AND NOT APPLE)
  file (COPY resources/icon/hicolor DESTINATION share/icons)
endif ()

# Windows stuff

if (MINGW)
  file (COPY resources/icon/blupi.ico DESTINATION "${CMAKE_BINARY_DIR}")
  configure_file (
    "${CMAKE_CURRENT_SOURCE_DIR}/resources/win32/planetblupi.rc.in"
    "${CMAKE_BINARY_DIR}/planetblupi.rc"
    @ONLY
  )

  list (APPEND sources "${CMAKE_BINARY_DIR}/planetblupi.rc")

  set (CMAKE_RC_COMPILER_INIT windres)
  enable_language (RC)
  set (CMAKE_RC_COMPILE_OBJECT "<CMAKE_RC_COMPILER> <FLAGS> -O coff <DEFINES> -i <SOURCE> -o <OBJECT>")

  # Remove cmd window when executing planetblupi
  set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -mwindows")
endif (MINGW)

# Dependencies

if ("${STATIC_BUILD}")
  set (CMAKE_INCLUDE_PATH ${CMAKE_INSTALL_PREFIX}/include)
  set (CMAKE_LIBRARY_PATH ${CMAKE_INSTALL_PREFIX}/lib)
endif ()

find_package (Intl REQUIRED)
include_directories (${Intl_INCLUDE_DIRS})

find_package (Iconv REQUIRED)
include_directories (${Iconv_INCLUDE_DIRS})

find_package (Argagg REQUIRED)
include_directories (${Argagg_INCLUDE_DIRS})

find_package (SDLKitchensink REQUIRED)
include_directories (${SDLKitchensink_INCLUDE_DIRS})

find_package (PkgConfig REQUIRED)
pkg_search_module (SDL2 REQUIRED sdl2)
pkg_search_module (SDL2_MIXER REQUIRED SDL2_mixer)
pkg_search_module (SDL2_IMAGE REQUIRED SDL2_image)
pkg_search_module (CURL REQUIRED libcurl)

if ("${STATIC_BUILD}")
  pkg_search_module (PNG REQUIRED libpng)
  # Static dependencies for SDL_kitchensink
  pkg_search_module (AVCODEC REQUIRED libavcodec)
  pkg_search_module (AVFORMAT REQUIRED libavformat)
  pkg_search_module (AVUTIL REQUIRED libavutil)
  pkg_search_module (SWSCALE REQUIRED libswscale)
  pkg_search_module (SWRESAMPLE REQUIRED libswresample)

  if (UNIX AND NOT APPLE)
    pkg_search_module (ALSA REQUIRED alsa)
    pkg_search_module (PULSE REQUIRED libpulse)
  endif ()

  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DCURL_STATICLIB")
endif ()

###########################
## Main binary dependencies
###########################

add_executable (planetblupi ${sources})

if ("${STATIC_BUILD}")
  set (planetblupi_DEPS
    ${Intl_LIBRARIES}
    ${Iconv_LIBRARIES}
    ${SDL2_STATIC_LIBRARIES}
    ${SDL2_MIXER_STATIC_LIBRARIES}
    ${SDL2_IMAGE_STATIC_LIBRARIES}
    ${CURL_STATIC_LIBRARIES}
    ${SDLKitchensink_STATIC_LIBRARIES}
    ${PNG_STATIC_LIBRARIES}
    ${AVCODEC_STATIC_LIBRARIES}
    ${AVFORMAT_STATIC_LIBRARIES}
    ${AVUTIL_STATIC_LIBRARIES}
    ${SWSCALE_STATIC_LIBRARIES}
    ${SWRESAMPLE_STATIC_LIBRARIES}
  )

  if (UNIX AND NOT APPLE)
    list (APPEND planetblupi_DEPS
      ${ALSA_STATIC_LIBRARIES}
      ${PULSE_STATIC_LIBRARIES}
    )
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L${CMAKE_INSTALL_PREFIX}/lib/pulseaudio")
  endif ()
else ()
  set (planetblupi_DEPS
    ${SDL2_LIBRARIES}
    ${SDL2_MIXER_LIBRARIES}
    ${SDL2_IMAGE_LIBRARIES}
    ${CURL_LIBRARIES}
    ${SDLKitchensink_LIBRARIES}
    pthread
  )
endif ()

target_link_libraries (planetblupi PUBLIC ${planetblupi_DEPS})

##########
## GetText
##########

find_package (Gettext)

set (_potFile ${CMAKE_CURRENT_SOURCE_DIR}/resources/po/${PROJECT_NAME}.pot)

add_custom_command (OUTPUT ${_potFile}
  COMMAND xgettext --no-location --keyword=translate -o ${_potFile} ${sources}
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
    DEPENDS translations ${file}
  )

  add_custom_target ("po-${_lang}" ALL ${_all}
    DEPENDS ${_out}/planetblupi.mo
  )

  add_dependencies (planetblupi "po-${_lang}")
endforeach (file)

##############
# Installation
##############

install (TARGETS planetblupi
  RUNTIME DESTINATION bin
)

install (
  DIRECTORY resources/data DESTINATION share/planetblupi
  PATTERN "world2*.blp" EXCLUDE
  PATTERN "world*.dev.blp" EXCLUDE
)
install (
  DIRECTORY resources/data DESTINATION share/planetblupi
  CONFIGURATIONS Debug
  PATTERN "world2*.blp"
  PATTERN "world*.dev.blp"
)
install (DIRECTORY resources/image DESTINATION share/planetblupi)
install (DIRECTORY resources/movie DESTINATION share/planetblupi)
install (DIRECTORY resources/sound DESTINATION share/planetblupi)
install (DIRECTORY resources/music DESTINATION share/planetblupi)
install (FILES LICENSE.all DESTINATION share/doc/planetblupi RENAME copyright)

if (UNIX AND NOT APPLE)
  install (DIRECTORY resources/icon/hicolor DESTINATION share/icons)
endif ()

# Copy libwinpthread-1.dll which seems not be linkable statically
if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows" AND MINGW)
  execute_process (COMMAND cygpath.exe --windows /mingw64 OUTPUT_VARIABLE MINGW64_PATH)
  string (REGEX REPLACE "[ \t\n\r]+$" "" MINGW64_PATH "${MINGW64_PATH}")
  string (REGEX REPLACE "[\\]" "\\\\\\\\" MINGW64_PATH "${MINGW64_PATH}")
  install (FILES "${MINGW64_PATH}\\\\bin\\\\libwinpthread-1.dll" DESTINATION bin)
endif ()

#########
## Deploy
#########

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Release" AND "${STATIC_BUILD}")
  # set (CPACK_STRIP_FILES TRUE)
  set (CPACK_PACKAGE_NAME ${PB_PACKAGE_NAME})
  set (CPACK_PACKAGE_VENDOR "blupi.org")
  set (CPACK_PACKAGE_DESCRIPTION_SUMMARY ${PB_DESCRIPTION})
  set (CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
  # set (CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README-user.md")
  set (CPACK_PACKAGE_FILE_NAME ${PB_PACKAGE_NAME})
  set (CPACK_PACKAGE_VERSION "${PB_VERSION_MAJOR}.${PB_VERSION_MINOR}.${PB_VERSION_PATCH}${PB_VERSION_EXTRA}")
  set (CPACK_PACKAGE_INSTALL_DIRECTORY ${PB_PRODUCT_NAME})
  set (CPACK_PACKAGE_EXECUTABLES "planetblupi;Planet Blupi")

  if (USE_APPIMAGE)
    include (LinuxAppImageBuild)
    set (CMAKE_PACKAGED_OUTPUT_PREFIX ${CMAKE_INSTALL_PREFIX})
    APPIMAGE_PACKAGE (planetblupi ${PB_PACKAGE_NAME} ${PB_PRODUCT_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/resources/linux" "${CMAKE_BINARY_DIR}/share" "" "" "blupi")
  elseif (MINGW)
    set (INSTALLER_FILE_NAME "${PB_PACKAGE_NAME}-${PB_VERSION_MAJOR}.${PB_VERSION_MINOR}.${PB_VERSION_PATCH}${PB_VERSION_EXTRA}")
    set (CPACK_PACKAGE_FILE_NAME "${INSTALLER_FILE_NAME}")
    set (CPACK_GENERATOR "NSIS64")
    set (CPACK_PACKAGE_ICON "${CMAKE_CURRENT_SOURCE_DIR}\\\\resources\\\\nsis\\\\installer.bmp")
    set (CPACK_NSIS_COMPRESSOR "/SOLID lzma")
    set (CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\planetblupi.exe")
    set (CPACK_NSIS_URL_INFO_ABOUT "http://www.blupi.org")
    set (CPACK_NSIS_MUI_FINISHPAGE_RUN "planetblupi")
    include (CPack)

    find_program (SIGNTOOL_EXECUTABLE signtool)
    if (SIGN_APP AND SIGNTOOL_EXECUTABLE)
      add_custom_command (TARGET planetblupi
                          POST_BUILD
                          COMMAND ${SIGNTOOL_EXECUTABLE}
                            sign -a -fd sha1 -t http://time.certum.pl -v
                            "${CMAKE_BINARY_DIR}/planetblupi.exe")
    endif ()

    add_custom_command (TARGET planetblupi
                        POST_BUILD
                        COMMAND make package)

    if (SIGN_APP AND SIGNTOOL_EXECUTABLE)
      add_custom_command (TARGET planetblupi
                          POST_BUILD
                          COMMAND ${SIGNTOOL_EXECUTABLE}
                            sign -a -fd sha1 -t http://time.certum.pl
                            "${CMAKE_BINARY_DIR}/${INSTALLER_FILE_NAME}.exe")
    endif ()
  elseif (APPLE)
    set (CPACK_GENERATOR "Bundle")
    set (CPACK_BUNDLE_ICON "${CMAKE_CURRENT_SOURCE_DIR}/resources/darwin/icon.icns")
    set (CPACK_BUNDLE_NAME ${PB_PRODUCT_NAME})
    set (CPACK_BUNDLE_PLIST "${CMAKE_BINARY_DIR}/Info.plist")
    set (CPACK_BUNDLE_STARTUP_COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/resources/darwin/Planet Blupi")
    if (SIGN_APP)
      set (CPACK_BUNDLE_APPLE_CERT_APP "Developer ID Application: Mathieu Schroeter")
    endif ()
    set (CPACK_PACKAGE_FILE_NAME "planetblupi-${PB_VERSION_MAJOR}.${PB_VERSION_MINOR}.${PB_VERSION_PATCH}${PB_VERSION_EXTRA}")

    set (CPACK_DMG_BACKGROUND_IMAGE "${CMAKE_CURRENT_SOURCE_DIR}/resources/darwin/background.tiff")
    set (CPACK_DMG_DS_STORE_SETUP_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/resources/darwin/dmgsetup.scpt")

    set (BUNDLE_VERSION "${PB_VERSION_MAJOR}.${PB_VERSION_MINOR}.${PB_VERSION_PATCH}${PB_VERSION_EXTRA}")
    set (BUNDLE_IDENTIFIER "org.blupi.planet")
    set (BUNDLE_ICON_REF "Planet Blupi")

    configure_file (
      "${CMAKE_CURRENT_SOURCE_DIR}/resources/darwin/Info.plist.in"
      "${CMAKE_BINARY_DIR}/Info.plist"
      @ONLY
    )

    include (CPack)
    add_custom_command (TARGET planetblupi
                        POST_BUILD
                        COMMAND make package)
  endif ()
endif ()
