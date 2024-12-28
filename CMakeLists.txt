cmake_minimum_required(VERSION 3.10.0)
project(C-Forms LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)

include(CheckCXXCompilerFlag)
include(CheckCXXSourceCompiles)
include(CheckIncludeFileCXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-color=always")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fdiagnostics-color=always")

add_compile_options(-std=c++17 -Wall)

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(TESTMAIN "Build Test: main" ON)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.2
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

#find_package(Boost)
#if(Boost_FOUND)
#    include_directories(${Boost_INCLUDE_DIRS}) 
#    link_libraries(${Boost_LIBRARIES})
#endif()

include_directories(${CMAKE_SOURCE_DIR}/include)

message(STATUS "Targets:")    
if(TESTMAIN)
    message(STATUS "   test")
    file(GLOB_RECURSE test_source ${CMAKE_SOURCE_DIR}/source/Test/*.cpp ${CMAKE_SOURCE_DIR}/source/Test/*.hpp)
    add_executable(test ${test_source})
    target_link_libraries(test sfml-graphics X11)
endif()
