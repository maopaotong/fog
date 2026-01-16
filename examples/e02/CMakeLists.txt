# 
cmake_minimum_required(VERSION 3.20)
set(TARGET_NAME examples-e02)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(fmt CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)


#  find_package
file(GLOB_RECURSE  SOURCES "src/*.cpp")

add_library(${TARGET_NAME} ${SOURCES})

#get_target_property(COMPILE_FLAGS ${PROJECT_NAME} COMPILE_OPTIONS)
#message(STATUS "C++ Compile Flags: ${COMPILE_FLAGS}")

#add_compile_definitions(UNICODE _UNICODE)

# framework
target_link_libraries(${TARGET_NAME} PRIVATE framework imgui::imgui GTest::gtest)        
#
target_compile_options(${TARGET_NAME} PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/utf-8>
    $<$<CXX_COMPILER_ID:MSVC>:/bigobj>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-finput-charset=UTF-8 -fexec-charset=UTF-8>
)

target_include_directories(${TARGET_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        src  # 
)


