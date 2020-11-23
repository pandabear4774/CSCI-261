cmake_minimum_required(VERSION 3.12)
project(SFML_Template)

set(CMAKE_CXX_STANDARD 14)

include_directories("/usr/local/include")
link_directories("/usr/local/lib")

add_executable(SFML_Template main.cpp Explosion.cpp Explosion.h Particle.cpp Particle.h)

target_link_libraries(SFML_Template sfml-graphics sfml-window sfml-system)