cmake_minimum_required(VERSION 3.5)

project(ClientServeur LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets Network REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Network REQUIRED)

set(TS_FILES ClientServeur_fr_CM.ts)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        serveur.cpp
        serveur.h
        client.cpp
        client.h
        mainwindow.h
        mainwindow.ui
        ${TS_FILES}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(ClientServeur
        ${PROJECT_SOURCES}
    )
    # Lier la bibliothèque Winsock2
    target_link_libraries(ClientServeur PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Network ws2_32)

else()
    if(ANDROID)
        add_library(ClientServeur SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(ClientServeur
            ${PROJECT_SOURCES}
        )
    endif()
    # Lier la bibliothèque Winsock2
    target_link_libraries(ClientServeur PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Network ws2_32)

endif()
