
cmake_minimum_required (VERSION 3.2)

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
set (CMAKE_CXX_STANDARD 11)

include (GNUInstallDirs)
include (${CMAKE_ROOT}/Modules/ExternalProject.cmake)
include ("${CMAKE_SOURCE_DIR}/cmake/Ronn2Man.cmake")

include_directories (${CMAKE_INSTALL_PREFIX}/include)
link_directories (${CMAKE_INSTALL_PREFIX}/lib)

project (planetblupi)
set (PB_VERSION_MAJOR 1)
set (PB_VERSION_MINOR 15)
set (PB_VERSION_PATCH 0)
set (PB_VERSION_EXTRA "-rc1")
set (PB_PRODUCT_NAME "Planet Blupi")
set (PB_PACKAGE_NAME "planetblupi")
set (PB_EXEC         "planetblupi")
set (PB_ICON_REF     "blupi")
set (PB_DESCRIPTION "Planet Blupi - A delirious spell-binding game")

option (PB_HTTP_VERSION_CHECK "Run a version check over HTTP (with CURL)" OFF)

configure_file (
  "${PROJECT_SOURCE_DIR}/src/config.h.in"
  "${PROJECT_BINARY_DIR}/include/config.h"
)

include_directories ("${PROJECT_BINARY_DIR}/include")

if (BUILD_JS)
  file (GLOB sources src/*.cxx src/*.h src/json/* src/platform/*_js.*)
else ()
  file (GLOB sources src/*.cxx src/*.h src/json/* src/platform/*_sdl.*)
endif ()

file (GLOB_RECURSE po resources/po/*.po)

if (APPIMAGE_APPRUN_PROGRAM AND APPIMAGE_ASSISTANT_PROGRAM)
  set (USE_APPIMAGE ON)
endif ()

if (NOT USE_APPIMAGE)
  configure_file (
    "${PROJECT_SOURCE_DIR}/resources/linux/application.desktop.in"
    "${CMAKE_CURRENT_BINARY_DIR}/${PB_PACKAGE_NAME}.desktop"
    @ONLY
  )
endif ()

set (planetblupi_OUTPUT planetblupi)

if (NOT DEFINED BUILD_LINUX)
  set (BUILD_LINUX UNIX AND NOT APPLE)
endif ()

if (BUILD_JS)
  set (CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} ${EMSCRIPTEN_FLAGS})
  set (CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} "--preload-file share")
  set (planetblupi_OUTPUT planetblupi.html)
endif ()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11 -L${CMAKE_INSTALL_PREFIX}/lib")
endif ()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" AND "${STATIC_BUILD}")
  set (CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} "-static-libgcc -static-libstdc++")
endif ()

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3 -DDEBUG")
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
file (COPY resources/fonts DESTINATION share/planetblupi)
if (NOT BUILD_JS)
  file (COPY resources/movie DESTINATION share/planetblupi)
endif ()
file (COPY resources/sound DESTINATION share/planetblupi)
file (COPY resources/music DESTINATION share/planetblupi)
file (COPY LICENSE.all DESTINATION share/doc/planetblupi)
file (COPY COPYING DESTINATION share/doc/planetblupi)
file (RENAME "${CMAKE_BINARY_DIR}/share/doc/planetblupi/LICENSE.all"
             "${CMAKE_BINARY_DIR}/share/doc/planetblupi/copyright")

if (BUILD_LINUX)
  file (COPY resources/icon/hicolor DESTINATION share/icons)
  file (
    COPY resources/linux/org.blupi.${PB_PACKAGE_NAME}.appdata.xml
    DESTINATION share/metainfo
  )
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
if (NOT BUILD_JS)
  set (SDL2_DIR       "${CMAKE_INSTALL_PREFIX}/lib/cmake/SDL2")
  set (SDL2_image_DIR "${CMAKE_INSTALL_PREFIX}/lib/cmake/SDL2_image")
  set (SDL2_ttf_DIR   "${CMAKE_INSTALL_PREFIX}/lib/cmake/SDL2_ttf")
  set (SDL2_mixer_DIR "${CMAKE_INSTALL_PREFIX}/lib/cmake/SDL2_mixer")

  if ("${STATIC_BUILD}")
    find_package (SDL2 REQUIRED NO_DEFAULT_PATH)
    include_directories (${SDL2_INCLUDE_DIRS})
    set (planetblupi_DEPS ${planetblupi_DEPS} ${SDL2_STATIC_LIBRARIES})
    find_package (SDL2_image REQUIRED NO_DEFAULT_PATH)
    include_directories (${SDL2_IMAGE_INCLUDE_DIRS})
    set (planetblupi_DEPS ${planetblupi_DEPS} SDL2_image::SDL2_image-static)
    find_package (SDL2_ttf REQUIRED NO_DEFAULT_PATH)
    include_directories (${SDL2_TTF_INCLUDE_DIRS})
    set (planetblupi_DEPS ${planetblupi_DEPS} SDL2_ttf::SDL2_ttf-static)
    find_package (SDL2_mixer REQUIRED NO_DEFAULT_PATH)
    include_directories (${SDL2_MIXER_INCLUDE_DIRS})
    set (planetblupi_DEPS ${planetblupi_DEPS} SDL2_mixer::SDL2_mixer-static)
  else ()
    find_package (SDL2 REQUIRED)
    include_directories (${SDL2_INCLUDE_DIRS})
    find_package (SDL2_image REQUIRED)
    include_directories (${SDL2_IMAGE_INCLUDE_DIRS})
    find_package (SDL2_ttf REQUIRED)
    include_directories (${SDL2_TTF_INCLUDE_DIRS})
    find_package (SDL2_mixer REQUIRED)
    include_directories (${SDL2_MIXER_INCLUDE_DIRS})
  endif ()
endif ()

if (${PB_HTTP_VERSION_CHECK})
  pkg_search_module (CURL REQUIRED libcurl)
  add_definitions (-DUSE_CURL)
endif ()

if ("${STATIC_BUILD}")
  # Static dependencies for SDL_kitchensink
  pkg_search_module (AVCODEC REQUIRED libavcodec)
  pkg_search_module (AVFORMAT REQUIRED libavformat)
  pkg_search_module (AVUTIL REQUIRED libavutil)
  pkg_search_module (SWSCALE REQUIRED libswscale)
  pkg_search_module (SWRESAMPLE REQUIRED libswresample)

  if (NOT BUILD_JS)
    pkg_search_module (PNG REQUIRED libpng)
    set (planetblupi_DEPS ${planetblupi_DEPS} ${PNG_STATIC_LIBRARIES})

    # Static dependencies for SDL_mixer
    pkg_search_module (VORBIS REQUIRED vorbisfile)
    set (planetblupi_DEPS ${planetblupi_DEPS} ${VORBIS_STATIC_LIBRARIES})

    if (BUILD_LINUX)
      pkg_search_module (ALSA REQUIRED alsa)
      pkg_search_module (PULSE REQUIRED libpulse)
    endif ()

    set (planetblupi_DEPS ${planetblupi_DEPS}
      ${Intl_LIBRARIES}
      ${Iconv_LIBRARIES}
    )

    # For the static SDL2::TTF
    set (planetblupi_DEPS ${planetblupi_DEPS}
      -lz
      -lfreetype
      -lharfbuzz
    )
  endif ()

  if (${PB_HTTP_VERSION_CHECK})
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DCURL_STATICLIB")
  endif ()
endif ()

###########################
## Main binary dependencies
###########################

add_executable (${planetblupi_OUTPUT} ${sources})

if ("${STATIC_BUILD}")
  set (planetblupi_DEPS ${planetblupi_DEPS}
    ${SDLKitchensink_STATIC_LIBRARIES}
    ${AVCODEC_STATIC_LIBRARIES}
    ${AVFORMAT_STATIC_LIBRARIES}
    ${AVUTIL_STATIC_LIBRARIES}
    ${SWSCALE_STATIC_LIBRARIES}
    ${SWRESAMPLE_STATIC_LIBRARIES}
  )

  if (${PB_HTTP_VERSION_CHECK})
    list (APPEND planetblupi_DEPS
      ${CURL_STATIC_LIBRARIES}
    )
  endif ()

  if (BUILD_LINUX)
    list (APPEND planetblupi_DEPS
      ${ALSA_STATIC_LIBRARIES}
      ${PULSE_STATIC_LIBRARIES} -lrt
    )
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L${CMAKE_INSTALL_PREFIX}/lib/pulseaudio")
  endif ()

  if (BUILD_WINDOWS)
    # Missing SDL2 static dependencies
    list (APPEND planetblupi_DEPS
      -lwinmm -limm32 -lsetupapi -lversion
    )
  endif ()
else ()
  set (planetblupi_DEPS
    ${SDL2_LIBRARIES}
    ${SDL2_MIXER_LIBRARIES}
    ${SDL2_IMAGE_LIBRARIES}
    ${SDL2_TTF_LIBRARIES}
    ${CURL_LIBRARIES}
    ${SDLKitchensink_LIBRARIES}
    pthread
  )

  if (${PB_HTTP_VERSION_CHECK})
    list (APPEND planetblupi_DEPS
      ${CURL_LIBRARIES}
    )
  endif ()
endif ()

target_link_libraries (${planetblupi_OUTPUT} PUBLIC ${planetblupi_DEPS})

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

  add_dependencies (${planetblupi_OUTPUT} "po-${_lang}")
endforeach (file)

#########
# manpage
#########

add_manpage_target ()
manpage(${PROJECT_NAME} 6)

##############
# Installation
##############

if (NOT BUILD_JS)
  install (TARGETS ${planetblupi_OUTPUT}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
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
  install (DIRECTORY resources/fonts DESTINATION share/planetblupi)
  install (DIRECTORY resources/movie DESTINATION share/planetblupi)
  install (DIRECTORY resources/sound DESTINATION share/planetblupi)
  install (DIRECTORY resources/music DESTINATION share/planetblupi)
  install (FILES LICENSE.all DESTINATION share/doc/planetblupi RENAME copyright)
  install (FILES COPYING DESTINATION share/doc/planetblupi)

  if (BUILD_LINUX)
    install (DIRECTORY resources/icon/hicolor DESTINATION share/icons)
    install (
      FILES resources/linux/org.blupi.${PB_PACKAGE_NAME}.appdata.xml
      DESTINATION share/metainfo
    )
  endif ()

  if (NOT USE_APPIMAGE)
    install (
      FILES ${CMAKE_CURRENT_BINARY_DIR}/${PB_PACKAGE_NAME}.desktop
      DESTINATION share/applications
    )
  endif ()

  # Copy libwinpthread-1.dll which seems not be linkable statically
  if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows" AND MINGW)
    execute_process (COMMAND cygpath.exe --windows $ENV{MINGW_PREFIX} OUTPUT_VARIABLE MINGW_PATH)
    string (REGEX REPLACE "[ \t\n\r]+$" "" MINGW_PATH "${MINGW_PATH}")
    string (REGEX REPLACE "[\\]" "\\\\\\\\" MINGW_PATH "${MINGW_PATH}")
    install (FILES "${MINGW_PATH}\\\\bin\\\\libwinpthread-1.dll" DESTINATION bin)
  endif ()
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
    APPIMAGE_PACKAGE (planetblupi ${PB_PACKAGE_NAME} ${PB_PRODUCT_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/resources/linux" "${CMAKE_BINARY_DIR}/share" "" "" ${PB_ICON_REF} SIGN_APP)
  elseif (MINGW)
    set (INSTALLER_FILE_NAME "${PB_PACKAGE_NAME}-${PB_VERSION_MAJOR}.${PB_VERSION_MINOR}.${PB_VERSION_PATCH}${PB_VERSION_EXTRA}")
    set (CPACK_PACKAGE_FILE_NAME "${INSTALLER_FILE_NAME}")
    set (CPACK_GENERATOR "NSIS64")
    set (CPACK_PACKAGE_ICON "${CMAKE_CURRENT_SOURCE_DIR}\\\\resources\\\\nsis\\\\installer.bmp")
    set (CPACK_NSIS_COMPRESSOR "/SOLID lzma")
    set (CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\planetblupi.exe")
    set (CPACK_NSIS_URL_INFO_ABOUT "https://www.blupi.org")
    set (CPACK_NSIS_MUI_FINISHPAGE_RUN "planetblupi")
    if (SIGN_APP)
      set (CPACK_NSIS_SIGN_UNINSTALLER "sign -a -fd sha256 -t http://time.certum.pl -v")
    endif ()

    include (CPack)

    find_program (SIGNTOOL_EXECUTABLE signtool)
    if (SIGN_APP AND SIGNTOOL_EXECUTABLE)
      add_custom_command (TARGET planetblupi
                          POST_BUILD
                          COMMAND ${SIGNTOOL_EXECUTABLE}
                            sign -a -fd sha256 -t http://time.certum.pl -v
                            "${CMAKE_BINARY_DIR}/planetblupi.exe")
    endif ()

    add_custom_command (TARGET planetblupi
                        POST_BUILD
                        COMMAND make package)

    if (SIGN_APP AND SIGNTOOL_EXECUTABLE)
      add_custom_command (TARGET planetblupi
                          POST_BUILD
                          COMMAND ${SIGNTOOL_EXECUTABLE}
                            sign -a -fd sha256 -t http://time.certum.pl
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
      set (CPACK_BUNDLE_APPLE_CODESIGN_PARAMETER "--deep -f --options runtime --timestamp")
      set (CPACK_BUNDLE_APPLE_CODESIGN_FILES "/Contents/Resources/bin/planetblupi")
      set (CPACK_APPLE_BUNDLE_ID "planetblupi")
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
