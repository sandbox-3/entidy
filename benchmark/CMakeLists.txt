find_package(Threads REQUIRED)

add_executable(micro micro.cpp catch2/catch.cpp)
add_executable(macro macro.cpp)

target_include_directories(micro PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/benchmark>)
target_include_directories(macro PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/benchmark>)

target_link_libraries(micro PRIVATE Threads::Threads Entidy::Entidy)
target_link_libraries(macro PRIVATE Threads::Threads Entidy::Entidy)

set_target_properties(micro PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
set_target_properties(macro PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
                              