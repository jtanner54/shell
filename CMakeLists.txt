cmake_minimum_required(VERSION 3.15)
project(shell-starter-cpp VERSION 1.0 LANGUAGES CXX)

# Strictly enforce C++23 and disable non-standard compiler extensions
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Explicitly list source files. NEVER use GLOB.
set(SOURCE_FILES
    src/main.cpp
    src/lexer.cpp
    src/executor.cpp
    src/builtins.cpp
    src/utils.cpp
)

add_executable(shell ${SOURCE_FILES})

# Properly expose the include directory
target_include_directories(shell PRIVATE include)

# Link your external library
target_link_libraries(shell PRIVATE readline)

# Mandatory strict compiler warnings
target_compile_options(shell PRIVATE 
    -Wall 
    -Wextra 
    -Werror 
    -Wpedantic 
    -Wconversion
    -Wshadow
)