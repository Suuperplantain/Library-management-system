cmake_minimum_required(VERSION 3.16)

project(LibraryManagementSystem LANGUAGES CXX)

add_executable(library_management_system
    src/main.cpp
    src/Library.cpp
    src/Authentication.cpp
    src/Books.cpp
    src/Borrowing.cpp
    src/Menus.cpp
)

target_include_directories(library_management_system PRIVATE include)
target_compile_features(library_management_system PRIVATE cxx_std_17)

if(MSVC)
    target_compile_options(library_management_system PRIVATE /W4)
else()
    target_compile_options(library_management_system PRIVATE -Wall -Wextra -Wpedantic)
endif()
