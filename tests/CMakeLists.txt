cmake_minimum_required(VERSION 3.20)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
add_subdirectory(googletest)

add_executable(jsmn_replica
  tests.cpp 
)

target_link_libraries(jsmn_replica gmock gtest gtest_main)
target_link_libraries(jsmn_replica simpleLibrary)