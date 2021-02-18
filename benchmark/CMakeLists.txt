find_package(Threads REQUIRED)

add_executable(entidy_benchmark main.cpp)

target_link_libraries(entidy_benchmark PRIVATE Threads::Threads Entidy::Entidy)

set_target_properties(
  entidy_benchmark PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES
                              CXX_EXTENSIONS NO)
