cmake_minimum_required(VERSION 3.24)
project(CoChucK)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)
file(GLOB SOURCES src/*.cpp)

find_package(Boost COMPONENTS program_options REQUIRED)

include_directories(/opt/homebrew/Cellar/stk/4.6.2/include/stk/)
link_directories(/opt/homebrew/Cellar/stk/4.6.2/lib)
include_directories(/opt/homebrew/Cellar/rtaudio/5.2.0/include/rtaudio/)
link_directories(/opt/homebrew/Cellar/rtaudio/5.2.0/lib)

add_executable(CoChucK ${SOURCES})

target_link_libraries(CoChucK rtaudio stk Boost::program_options)
