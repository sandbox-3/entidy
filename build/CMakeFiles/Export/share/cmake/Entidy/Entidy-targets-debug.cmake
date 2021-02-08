#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "::Entidy" for configuration "Debug"
set_property(TARGET ::Entidy APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(::Entidy PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/libEntidy.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS ::Entidy )
list(APPEND _IMPORT_CHECK_FILES_FOR_::Entidy "${_IMPORT_PREFIX}/lib/libEntidy.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
