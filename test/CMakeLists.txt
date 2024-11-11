cmake_minimum_required(VERSION 3.10)
project(tensor_example LANGUAGES CXX)

# Find cppflow

find_package(cppflow REQUIRED)

# Add the executable
add_executable(tensor_example main.cpp)

# Link cppflow library to your project
target_link_libraries(tensor_example cppflow::cppflow)
