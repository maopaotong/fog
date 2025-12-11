cmake_minimum_required(VERSION 3.16)
project(framework)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找Ogre
find_package(fmt CONFIG REQUIRED)
find_package(OGRE CONFIG REQUIRED COMPONENTS Terrain)
find_package(SDL2 CONFIG REQUIRED)
    
file(GLOB_RECURSE  SOURCES "src/*.cpp"
    "lib/*.cpp"
)

add_library(framework ${SOURCES})

#add_compile_definitions(UNICODE _UNICODE)
# 添加可执行文件
# add_executable(Study_Ogre ${SOURCES})

# 链接Ogre库
target_link_libraries(framework
    PUBLIC
        OgreMain
        OgreBites
        Codec_STBI
        OgreTerrain
        OgrePaging
    PRIVATE
        imgui::imgui   
        fmt::fmt
)
#if(TARGET SDL2::SDL2main)
#    target_link_libraries(Study_Ogre PRIVATE SDL2::SDL2main)
#endif()

#target_link_libraries(Study_Ogre PRIVATE SDL2::SDL2)
# 包含Ogre头文件
#target_include_directories(framework PRIVATE ${OGRE_INCLUDE_DIRS})
#target_include_directories(framework PRIVATE include)
#target_include_directories(framework PRIVATE "lib")
target_compile_options(framework PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/utf-8>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-finput-charset=UTF-8 -fexec-charset=UTF-8>
)

target_include_directories(framework
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        src  # 私有实现目录
)
