cmake_minimum_required(VERSION 3.19 FATAL_ERROR)
project(parser VERSION 1.0.0 LANGUAGES CXX)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS true)
set(CMAKE_CXX_STANDARD_REQUIRED on)

find_package(GTest REQUIRED)

add_executable(${PROJECT_NAME}-test
    main.cpp
)

target_link_libraries(${PROJECT_NAME}-test PRIVATE
    GTest::gtest
    GTest::gtest_main
)
