cmake_minimum_required(VERSION 3.16)
project(EpicTracer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Path tracing is unusable without optimisation, so Release is the default
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release)
endif()

find_package(OpenMP)

# Everything except main(), shared by the program and the tests
add_library(epic STATIC src/Camera.cpp src/Image.cpp)
target_include_directories(epic PUBLIC src)
target_include_directories(epic SYSTEM PUBLIC third_party/stb)
if(OpenMP_CXX_FOUND)
  target_link_libraries(epic PUBLIC OpenMP::OpenMP_CXX)
endif()

add_executable(epictracer src/main.cpp)
target_link_libraries(epictracer PRIVATE epic)

include(CTest)
if(BUILD_TESTING)
  add_executable(epic_tests tests/test_main.cpp tests/test_vec3.cpp tests/test_sphere.cpp
                 tests/test_material.cpp tests/test_camera.cpp tests/test_image.cpp)
  target_include_directories(epic_tests SYSTEM PRIVATE third_party/doctest)
  target_link_libraries(epic_tests PRIVATE epic)
  add_test(NAME unit COMMAND epic_tests)
  # A tiny end-to-end render, run from the repository root so the sky image is found
  add_test(NAME smoke_render
           COMMAND epictracer --width 64 --height 36 --spp 4 --out ${CMAKE_BINARY_DIR}/smoke.ppm
           WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endif()
