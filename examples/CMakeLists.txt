find_package(Threads REQUIRED)

add_executable(entidy_examples main.cpp)

target_link_libraries(entidy_examples PUBLIC Entidy::Entidy Threads::Threads)

set_target_properties(
  entidy_examples PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES
                             CXX_EXTENSIONS NO)
