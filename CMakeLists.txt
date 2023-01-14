cmake_minimum_required(VERSION 3.24)
project(handmade_hero_SDL)

set(CMAKE_CXX_STANDARD 17)

find_package(SDL2 REQUIRED)

add_executable(handmade_hero_SDL main.cpp)
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})