cmake_minimum_required(VERSION 3.5)

project(lamp_problem LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt5 COMPONENTS Widgets Network LinguistTools REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Network LinguistTools REQUIRED)

add_executable(lamp_problem
    main.cpp
    mainwindow.cpp
    mainwindow.h
    socketlistener.cpp
    socketlistener.h
    types.h
    mainwindow.ui
)

target_link_libraries(lamp_problem PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Network)

if (WIN32)
    target_link_libraries(lamp_problem PRIVATE ws2_32)
endif()

add_custom_command(TARGET lamp_problem POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy
                     ${CMAKE_SOURCE_DIR}/img/lamp.png
                     ${CMAKE_CURRENT_BINARY_DIR}/lamp.png)
