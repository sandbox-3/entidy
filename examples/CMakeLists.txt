find_package(Threads REQUIRED)

add_executable(general General/main.cpp)
add_executable(spaceinvaders SpaceInvaders/main.cpp)

target_include_directories(general PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/examples/General>)
target_include_directories(spaceinvaders PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/examples/SpaceInvaders>)

target_link_libraries(general PRIVATE Threads::Threads Entidy::Entidy)
target_link_libraries(spaceinvaders PRIVATE Threads::Threads Entidy::Entidy)

set_target_properties(general PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
set_target_properties(spaceinvaders PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
                              