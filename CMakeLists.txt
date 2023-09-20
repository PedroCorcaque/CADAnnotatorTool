cmake_minimum_required(VERSION 3.0.15)
project(CADAnnotatorTool)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

file(GLOB_RECURSE sources src/*.cpp src/*.hpp src/libs/*.hpp src/libs/*.cpp)

# Set the path to OpenCASCADE libraries
set(OpenCASCADE_INCLUDE_DIRS "/usr/local/include/opencascade/")

# Find OpenCASCADE library
find_package(OpenCASCADE REQUIRED)
if (${OpenCASCADE_FOUND})
    include_directories(${OpenCASCADE_INCLUDE_DIRS})
    include_directories(${OpenCASCADE_LIBRARIES})
endif()

include_directories(src)
include_directories(src/libs)
add_executable(${PROJECT_NAME} ${sources})
target_link_libraries(${PROJECT_NAME} ${OpenCASCADE_LIBRARIES} -lTKV3d)