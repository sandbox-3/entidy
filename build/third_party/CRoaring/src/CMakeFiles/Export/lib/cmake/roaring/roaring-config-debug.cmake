#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "roaring::roaring" for configuration "Debug"
set_property(TARGET roaring::roaring APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(roaring::roaring PROPERTIES
  IMPORTED_LINK_INTERFACE_LIBRARIES_DEBUG "roaring::roaring-headers;roaring::roaring-headers-cpp"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/libroaring.so.0.2.66"
  IMPORTED_SONAME_DEBUG "libroaring.so.0"
  )

list(APPEND _IMPORT_CHECK_TARGETS roaring::roaring )
list(APPEND _IMPORT_CHECK_FILES_FOR_roaring::roaring "${_IMPORT_PREFIX}/lib/libroaring.so.0.2.66" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
