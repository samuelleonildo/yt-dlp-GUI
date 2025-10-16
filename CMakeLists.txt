cmake_minimum_required(VERSION 3.16)

project(MeuApp LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


set(HEADERS
    src/main_window.hpp
    src/resources/style_loader.hpp
)

set(SOURCES
    src/main_window.cpp
    src/resources/style_loader.cpp
    src/resources/resources.qrc
)


if(WIN32)
    add_executable(yt-dlp-GUI
        src/main_win.cpp
        ${HEADERS}
        ${SOURCES}
    )
else()
    add_executable(yt-dlp-GUI
        src/main_linux.cpp
        ${HEADERS}
        ${SOURCES}
    )
endif()

target_link_libraries(yt-dlp-GUI PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)


if(WIN32)
    target_link_options(yt-dlp-GUI PRIVATE "-mwindows")
endif()
