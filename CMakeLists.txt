cmake_minimum_required(VERSION 4.0)
project(OOP_new)

set(CMAKE_CXX_STANDARD 20)

add_executable(OOP_new main.cpp
        contact.h
        contact.cpp
        contactmanager.h
        contactmanager.cpp)
