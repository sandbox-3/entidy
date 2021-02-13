cmake_minimum_required(VERSION 3.12)

include(FeatureSummary)

project(Entidy CXX)

set(ENTIDY_VERSION_MAJOR 0 CACHE STRING "major version" FORCE)
set(ENTIDY_VERSION_MINOR 1 CACHE STRING "minor version" FORCE)
set(ENTIDY_SANDBOX_VERSION ${ENTIDY_VERSION_MAJOR}.${ENTIDY_VERSION_MINOR} CACHE STRING "version" FORCE)

# ################# #
# Set CMAKE Options
# ################# #

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(PROJECT_DIR ${CMAKE_CURRENT_LIST_DIR})
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

# ####### #
# Options
# ####### #

option(BUILD_TESTS "Build the tests" OFF)

# ######## #
# Includes
# ######## #

include(FeatureSummary)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(CMakeDependentOption)
include(CMakeToolsHelpers OPTIONAL)

# include(GetGitRevisionDescription)
# include(BuildType)

add_subdirectory(third_party/CRoaring EXCLUDE_FROM_ALL)

# ############# #
# Find packages
# ############# #

#if(USE_CUDA)
#  find_package(CUDA               REQUIRED)
#endif()

# 3rd party packages
find_package(Threads              REQUIRED)
find_package(OpenMP               REQUIRED)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

# Boost (yes boost is special)
set(Boost_USE_STATIC_LIBS ON)
set(BOOST_MIN_VERSION "1.47.0")
find_package(Boost ${BOOST_MIN_VERSION}
             COMPONENTS
             system filesystem date_time regex REQUIRED)


include(FindPkgConfig)
if(NOT PKG_CONFIG_FOUND)
  message(FATAL_ERROR "pkg-config not found!" )
endif()



# ########################### #
# setup generated information
# ########################### #

# get_git_head_revision(GIT_REFSPEC GIT_SHA1)
# git_local_changes(GIT_LOCAL_CHANGES)
# git_date(GIT_DATE)
# git_commit_subject(GIT_COMMIT_SUBJECT)
# configure_file("${CMAKE_CURRENT_SOURCE_DIR}/src/Build.cpp.in" "${CMAKE_CURRENT_BINARY_DIR}/src/Build.cpp" @ONLY)

# ####################### #
# Targets
# ####################### #

# to include sub-project using git submodules for example
# add_subdirectory(third_party/entt)


# TARGET examples

file(GLOB_RECURSE ENTIDY_EXAMPLES
  examples/*
)
add_executable(entidy_examples ${ENTIDY_EXAMPLES})

target_include_directories(entidy_examples
  PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party
        ${Boost_INCLUDE_DIRS}
        /usr/local/include
)

target_link_libraries(entidy_examples
PUBLIC
Threads::Threads
roaring
${Boost_LIBRARIES}
${OpenMP_CXX_LIBRARIES}
)

  set_target_properties(entidy_examples PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED YES
  CXX_EXTENSIONS NO
  )


#TARGET benchmark

file(GLOB_RECURSE ENTIDY_BENCHMARK 
  benchmark/*
)
add_executable(entidy_benchmark ${ENTIDY_BENCHMARK})

target_include_directories(entidy_benchmark
  PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party
        ${Boost_INCLUDE_DIRS}
        /usr/local/include
)

target_link_libraries(entidy_benchmark
PUBLIC
Threads::Threads
roaring
${Boost_LIBRARIES}
${OpenMP_CXX_LIBRARIES}
)

  set_target_properties(entidy_benchmark PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED YES
  CXX_EXTENSIONS NO
  )

if( CMAKE_SIZEOF_VOID_P EQUAL 4 )
  add_compile_definitions(ENTIDY_32_BIT)
else( CMAKE_SIZEOF_VOID_P EQUAL 8 )
  add_compile_definitions(ENTIDY_64_BIT)
endif()


#
# Installation Instructions
#

if(NOT EXISTS "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake.in")
  file(
    WRITE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake.in
    [[
    @PACKAGE_INIT@
    include("${CMAKE_CURRENT_LIST_DIR}/@PROJECT_NAME@-targets.cmake")
  ]])
endif()

# configure_package_config_file(
#   "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake.in"
#   "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
#   INSTALL_DESTINATION "${CMAKE_INSTALL_DATADIR}/cmake/${PROJECT_NAME}"
#   NO_SET_AND_CHECK_MACRO NO_CHECK_REQUIRED_COMPONENTS_MACRO)

# write_basic_package_version_file(
#   ${PROJECT_NAME}-config-version.cmake
#   VERSION ${PROJECT_VERSION}
#   COMPATIBILITY SameMajorVersion)

# install(
#   EXPORT ${PROJECT_NAME}-targets
#   DESTINATION "${CMAKE_INSTALL_DATADIR}/cmake/${PROJECT_NAME}"
#   NAMESPACE ${THIS_PROJECT_NS}::)
# install(FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake"
#               "${PROJECT_BINARY_DIR}/${PROJer_loop{}ECT_NAME}-config.cmake"
#         DESTINATION ${CMAKE_INSTALL_DATADIR}/cmake/${PROJECT_NAME})
# install(TARGETS ${PROJECT_NAME})


#################
# Print summary #
#################

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    feature_summary(WHAT ENABLED_FEATURES DISABLED_FEATURES PACKAGES_FOUND)
    feature_summary(FILENAME ${CMAKE_CURRENT_BINARY_DIR}/features.log WHAT ALL)
endif()