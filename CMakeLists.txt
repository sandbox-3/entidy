cmake_minimum_required(VERSION 3.12)

project(Entidy
VERSION 0.1.0
LANGUAGES C CXX
)

set(THIS_PROJECT_NS ${PROJECT_NAME})

list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_SOURCE_DIR}/cmake)

message("*")
message("* ${PROJECT_NAME} v${THIS_PROJECT_VERSION} (${CMAKE_BUILD_TYPE})")
message("*")

## CMAKE Instructions ##########################################################

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(PROJECT_DIR ${CMAKE_CURRENT_LIST_DIR})
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

## Project Options #############################################################

option(ENTIDY_BUILD_EXAMPLES "Build the examples" ON)
option(ENTIDY_BUILD_TESTS "Build the tests" OFF)
option(ENTIDY_BUILD_BENCHMARK "Build the benchmarks" ON)
option(ENTIDY_BUILD_STATIC "Build a static library" ON)

## Includes ####################################################################

include(FeatureSummary)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(CMakeDependentOption)
include(CMakeToolsHelpers OPTIONAL)
include(GenerateExportHeader)

## Add Subdirectories ##########################################################

# if(ENTIDY_BUILD_TEST OR ENTIDY_BUILD_BENCHMARK)
#   add_subdirectory(third_party/Catch2) 
# endif()

if(ENTIDY_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()

if(ENTIDY_BUILD_TESTS)
  add_subdirectory(tests)
endif()

if(ENTIDY_BUILD_BENCHMARK)
  add_subdirectory(benchmark)
endif()

## Find Packages ###############################################################

# Workaround for Threads::Threads - Now multiplatform
# ~~v~~~~~~~~~~~~~~~~~~~~~~~~~~~~
set(CMAKE_USE_WIN32_THREADS_INIT "using WIN32 threads" ON)
find_package(Threads REQUIRED)

### OpenMP
# find_package(OpenMP REQUIRED) 
# if(OPENMP_FOUND) 
#   set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}") 
#   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}") 
#   set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
# endif()

### Boost (yes boost is special) 
# set(Boost_USE_STATIC_LIBS ON)
# set(BOOST_MIN_VERSION "1.47.0") 
# find_package(Boost ${BOOST_MIN_VERSION} 
#              COMPONENTS system filesystem date_time regex REQUIRED)

## Target Definitions ##########################################################

if(ENTIDY_BUILD_STATIC)
  set(ENTIDY_BUILD_TYPE STATIC)
  MESSAGE( STATUS "Building a static library." )
else()
  MESSAGE( STATUS "Building a dynamic library." )
  set(ENTIDY_BUILD_TYPE SHARED)
endif()


file(GLOB_RECURSE THIS_PROJECT_H include/*.h)
file(GLOB_RECURSE THIS_PROJECT_C src/*.c)
file(GLOB_RECURSE THIS_PROJECT_CPP src/*.cpp)

add_library(${PROJECT_NAME} ${ENTIDY_BUILD_TYPE} ${THIS_PROJECT_C} ${THIS_PROJECT_CPP})
add_library(${THIS_PROJECT_NS}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(
  ${PROJECT_NAME}
  PUBLIC $<INSTALL_INTERFACE:include>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

target_link_libraries(${PROJECT_NAME} PUBLIC Threads::Threads)

set_target_properties(
  ${PROJECT_NAME} PROPERTIES CXX_STANDARD 17 
                             CXX_STANDARD_REQUIRED YES
                             CXX_EXTENSIONS NO)

install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}-targets
LIBRARY DESTINATION lib
ARCHIVE DESTINATION lib
RUNTIME DESTINATION bin
)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

## Compile Definitions #########################################################

if(CMAKE_SIZEOF_VOID_P EQUAL 4)
  add_compile_definitions(ENTIDY_32_BIT)
else(CMAKE_SIZEOF_VOID_P EQUAL 8)
  add_compile_definitions(ENTIDY_64_BIT)
endif()

## Installation Instructions ###################################################

if(NOT EXISTS "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake.in")
  file(
    WRITE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake.in
    [[
    @PACKAGE_INIT@
    include("${CMAKE_CURRENT_LIST_DIR}/@PROJECT_NAME@-targets.cmake")
  ]])
endif()

configure_package_config_file(
  "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake.in"
  "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
  INSTALL_DESTINATION "${CMAKE_INSTALL_DATADIR}/cmake/${PROJECT_NAME}"
  NO_SET_AND_CHECK_MACRO NO_CHECK_REQUIRED_COMPONENTS_MACRO)

write_basic_package_version_file(
  ${PROJECT_NAME}-config-version.cmake
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion)

install(
  EXPORT ${PROJECT_NAME}-targets
  DESTINATION "${CMAKE_INSTALL_DATADIR}/cmake/${PROJECT_NAME}"
  NAMESPACE ${THIS_PROJECT_NS}::)
install(FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake"
              "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
        DESTINATION ${CMAKE_INSTALL_DATADIR}/cmake/${PROJECT_NAME})

## Print Summary ###############################################################

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  feature_summary(WHAT ENABLED_FEATURES DISABLED_FEATURES PACKAGES_FOUND)
  feature_summary(FILENAME ${CMAKE_CURRENT_BINARY_DIR}/features.log WHAT ALL)
endif()
