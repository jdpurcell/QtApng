cmake_minimum_required(VERSION 3.20)
cmake_policy(SET CMP0077 NEW)

project(
  QtApng
  VERSION 0.1.0
  LANGUAGES CXX)

option(APNG_TESTS "Enable tests" OFF)

find_package(Qt6 REQUIRED COMPONENTS Core Gui)

add_subdirectory(src)

if(APNG_TESTS)
  find_package(Qt6 REQUIRED COMPONENTS Test)
  add_subdirectory(tests)
endif()
