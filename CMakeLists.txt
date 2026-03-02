cmake_minimum_required(VERSION 3.16)

project(yt-dlp-GUI LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


set(HEADERS
    src/main_window.hpp
    src/resources/style_loader.hpp
)

set(SOURCES
    src/main.cpp
    src/main_window.cpp
    src/resources/style_loader.cpp
    src/resources/resources.qrc
)

add_executable(yt-dlp-GUI
    ${HEADERS}
    ${SOURCES}
)


target_link_libraries(yt-dlp-GUI PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)


# Windows GUI subsystem + icon
if(WIN32)
    target_sources(yt-dlp-GUI PRIVATE app.rc)
    target_link_options(yt-dlp-GUI PRIVATE "-mwindows")
endif()
