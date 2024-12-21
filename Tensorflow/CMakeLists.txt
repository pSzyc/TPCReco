cmake_minimum_required(VERSION 3.5)
project(MyTFProject)

set(TENSORFLOW_INCLUDE_DIR /usr/local/include)
set(TENSORFLOW_LIB_DIR /usr/local/lib)
set(SOURCE_DIR ${CMAKE_SOURCE_DIR})

link_directories(${TENSORFLOW_LIB_DIR})

add_executable(my_app main.cpp ${SOURCE_DIR}/tf_functions.cpp)

target_include_directories(my_app PRIVATE ${TENSORFLOW_INCLUDE_DIR} ${SOURCE_DIR})

target_link_libraries(my_app PRIVATE tensorflow tensorflow_framework)