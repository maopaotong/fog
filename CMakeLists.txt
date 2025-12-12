# game-01/CMakeLists.txt
cmake_minimum_required(VERSION 3.16)
option(BUILD_FRAMEWORK "Build framework" ON)
option(BUILD_DEMO "Build framework" OFF)
option(BUILD_TOOLS "Build framework" OFF)
set(PROJECT_NAME, "Game01")
project(Game01 LANGUAGES CXX)

# 设置全局标准（可选）
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
if(MSVC)
    add_compile_options(/utf-8)
endif()



if(BUILD_FRAMEWORK)
    add_subdirectory(framework)
endif()
if(BUILD_DEMO)
    add_subdirectory(demo)
endif()

message(STATUS "C++ Compile Flags: ${COMPILE_FLAGS}")


