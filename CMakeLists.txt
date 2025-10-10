cmake_minimum_required(VERSION 3.16)

project(MeuApp LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


if(WIN32)
    add_executable(yt-dlp-GUI
        src/main_gui.cpp
        src/main_window.cpp
        src/main_window.hpp
    )
else()
    add_executable(yt-dlp-GUI
        src/main.cpp
        src/main_window.cpp
        src/main_window.hpp
    )
endif()


target_link_libraries(yt-dlp-GUI PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)


# for Windows/MinGW
if(WIN32)
    target_link_options(yt-dlp-GUI PRIVATE "-mwindows")
endif()