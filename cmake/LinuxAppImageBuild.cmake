
set (APPIMAGE_ASSISTANT_PROGRAM CACHE FILEPATH "AppImageAssistant executable")
set (APPIMAGE_APPRUN_PROGRAM    CACHE FILEPATH "AppImage AppRun executable")

set (APPIMAGE_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/deploy/linux-appimage" CACHE PATH "Where to put the AppDir items")
set (APPIMAGE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/package/linux-appimage" CACHE PATH "AppImage output directory")
set (APPIMAGE_FOLLOW_STANDARD OFF CACHE BOOL "Whether generator should follow the spec")

macro (APPIMAGE_PACKAGE TARGET APPIMAGE_TITLE APPIMAGE_DISPLAYNAME CONFIGDIR DATA LIBRARIES LIBRARY_FILES ICON_REF SIGN_APP)
  string (TOLOWER "${APPIMAGE_TITLE}" APPIMAGE_INTERNALNAME)
  string (MAKE_C_IDENTIFIER "${APPIMAGE_INTERNALNAME}" APPIMAGE_INTERNALNAME)

  # Some prerequisites
  # TITLE here is used as the name of the final AppImage as well as the desktop entry's name
  set (APPIMAGE_TITLE "${APPIMAGE_TITLE}")
  set (APPIMAGE_DISPLAYNAME "${APPIMAGE_DISPLAYNAME}")
  set (APPIMAGE_INTERNALNAME "${APPIMAGE_INTERNALNAME}")
  set (APPIMAGE_LIBRARIES)
  set (APPIMAGE_DATA)
  set (APPIMAGE_CONFIG_DIR "${CONFIGDIR}")
  set (APPIMAGE_DEFAULT_ICON_FILE "${APPIMAGE_CONFIG_DIR}/icon.svg")

  # Icon file to be used for the AppImage, only one in this case, preferrably SVG
  set (APPIMAGE_ICON "${APPIMAGE_DEFAULT_ICON_FILE}")
  # We define a way to reference this icon based on where it is located
  set (APPIMAGE_ICON_REF "${ICON_REF}")

  # This helps the window manager to recognize the program even if it has no embedded or loaded icon
  set (APPIMAGE_EXEC_WM ${TARGET})

  # Sets the launch variable in .desktop entry
  set (APPIMAGE_EXEC ${TARGET})

  # This directory is used for temporary files, might get messy
  set ( APPIMAGE_CACHE_DIR "${APPIMAGE_WORKING_DIRECTORY}/${APPIMAGE_INTERNALNAME}_cache")
  # Where the AppDir is generated
  set (APPIMAGE_INTERMEDIATE_DIR "${APPIMAGE_WORKING_DIRECTORY}/${APPIMAGE_INTERNALNAME}")
  set (APPIMAGE_ICON_TARGET "${APPIMAGE_INTERMEDIATE_DIR}/${APPIMAGE_ICON_REF}")
  set (APPIMAGE_BINARY_DIR "${APPIMAGE_INTERMEDIATE_DIR}/usr/bin")
  set (APPIMAGE_ASSET_DIR "${APPIMAGE_INTERMEDIATE_DIR}/usr/share")
  set (APPIMAGE_LIBRARY_DIR "${APPIMAGE_INTERMEDIATE_DIR}/usr/lib")
  set (APPIMAGE_FINAL_NAME "${APPIMAGE_OUTPUT_DIRECTORY}/${APPIMAGE_TITLE}.AppImage")

  list (APPEND APPIMAGE_LIBRARIES ${LIBRARIES})
  list (APPEND APPIMAGE_DATA ${DATA})

  # Remove the previous AppImage file to avoid confusion when generating a new one
  add_custom_command (
    TARGET ${TARGET}
    PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E remove "${APPIMAGE_FINAL_NAME}"
  )

  # Create some necessary directory structure in AppDir
  add_custom_command (
    TARGET ${TARGET}
    PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${APPIMAGE_OUTPUT_DIRECTORY}"
  )
  add_custom_command (
    TARGET ${TARGET}
    PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${APPIMAGE_BINARY_DIR}"
  )
  add_custom_command (
    TARGET ${TARGET}
    PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${APPIMAGE_CACHE_DIR}"
  )
  add_custom_command (
    TARGET ${TARGET}
    PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${APPIMAGE_LIBRARY_DIR}"
  )
  add_custom_command (
    TARGET ${TARGET}
    PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${APPIMAGE_ASSET_DIR}"
  )

  # Copy and configure some data for the AppDir
  configure_file (
    "${APPIMAGE_ICON}"
    "${APPIMAGE_ICON_TARGET}.svg"
    COPYONLY
  )
  configure_file (
    "${APPIMAGE_ICON}"
    "${APPIMAGE_INTERMEDIATE_DIR}/.DirIcon"
    COPYONLY
  )
  configure_file (
    "${APPIMAGE_CONFIG_DIR}/application.desktop.in"
    "${APPIMAGE_INTERMEDIATE_DIR}/${APPIMAGE_INTERNALNAME}.desktop"
    @ONLY
  )
  configure_file (
    "${APPIMAGE_APPRUN_PROGRAM}"
    "${APPIMAGE_INTERMEDIATE_DIR}/AppRun"
    COPYONLY
  )

  # Copy resources into AppDir
  foreach (RESC ${DATA})
    add_custom_command (
      TARGET ${TARGET}
      PRE_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_directory "${RESC}" "${APPIMAGE_ASSET_DIR}"
    )
  endforeach ()

  # Copy bundled libraries into AppDir
  foreach (LIB ${LIBRARY_FILES})
    add_custom_command (
      TARGET ${TARGET}
      PRE_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy "${LIB}" "${APPIMAGE_LIBRARY_DIR}"
    )
  endforeach ()

  foreach (LIB ${LIBRARIES})
    add_custom_command (
      TARGET ${TARGET}
      PRE_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:${LIB}>" "${APPIMAGE_LIBRARY_DIR}"
    )
  endforeach ()

  # Copy the binary to AppDir
  add_custom_command (
    TARGET ${TARGET}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:${TARGET}>" "${APPIMAGE_BINARY_DIR}"
  )

  if (SIGN_APP)
    set (APPIMGKITARGS -s -n)
  else ()
    set (APPIMGKITARGS -n)
  endif ()

  # Do the actual packaging step with AppImageKit
  add_custom_command (
    TARGET ${TARGET}
    POST_BUILD
    COMMAND "${APPIMAGE_ASSISTANT_PROGRAM}" ${APPIMGKITARGS} "${APPIMAGE_INTERMEDIATE_DIR}" "${APPIMAGE_FINAL_NAME}"
  )

  install (
    FILES "${APPIMAGE_FINAL_NAME}"
    DESTINATION "${CMAKE_PACKAGED_OUTPUT_PREFIX}/linux-appimage"
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
  )
endmacro ()
