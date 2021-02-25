find_package(Threads REQUIRED)

# General
add_executable(general General/main.cpp)
target_include_directories(general 
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/General>
)
target_link_libraries(general 
    PRIVATE Threads::Threads Entidy::Entidy
)
set_target_properties(general 
    PROPERTIES CXX_STANDARD 17 
    CXX_STANDARD_REQUIRED YES 
    CXX_EXTENSIONS NO
)

# SpaceInvaders
if (UNIX)
    file(GLOB_RECURSE SPACE_INVADERS_CPP SpaceInvaders/src/*.cpp)
    add_executable(spaceinvaders ${SPACE_INVADERS_CPP})
    target_include_directories(spaceinvaders 
        PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/SpaceInvaders/include>
    )
    target_link_libraries(spaceinvaders 
        PRIVATE Threads::Threads Entidy::Entidy
    )
    set_target_properties(spaceinvaders 
        PROPERTIES CXX_STANDARD 17 
        CXX_STANDARD_REQUIRED YES 
        CXX_EXTENSIONS NO
    )
else()
    message(WARNING "spaceinvaders is unix-only")
endif()
