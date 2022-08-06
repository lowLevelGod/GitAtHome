cmake_minimum_required(VERSION 3.15)

project(gitAtHome)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_CXX_EXTENSIONS OFF)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ~/GitAtHome)

include_directories(headers /usr/include)
set(SOURCES src/main.cpp src/blob.cpp src/commit.cpp src/git.cpp src/index.cpp src/object.cpp src/ref.cpp src/tree.cpp src/utils.cpp)
add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} z)


