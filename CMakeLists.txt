# nix workaround
if(CMAKE_EXPORT_COMPILE_COMMANDS)
  set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES
      ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
endif()

cmake_minimum_required(VERSION 3.19)

project(hyprland-c VERSION 0.1)
find_package(json-c REQUIRED)

add_executable(${PROJECT_NAME} hyprland.c)
target_link_libraries(${PROJECT_NAME} PRIVATE json-c::json-c)
