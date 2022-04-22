cmake_minimum_required(VERSION 3.5)

project(Tracker VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Sql Network)

set(PROJECT_SOURCES
        main.cpp

        interface/addtransactionform.cpp
        databaseManager/dbconnection.cpp
        databaseManager/dbgetter.cpp
        databaseManager/dbsetter.cpp
        interface/edittransactiondialog.cpp
        databaseManager/global.cpp
        interface/startscreen.cpp
        interface/trackerapp.cpp

        interface/addtransactionform.h
        databaseManager/dbconnection.h
        databaseManager/dbgetter.h
        databaseManager/dbsetter.h
        interface/edittransactiondialog.h
        databaseManager/global.h
        templates/operation.h
        interface/startscreen.h
        interface/trackerapp.h
        templates/transaction.h

        interface/addtransactionform.ui
        interface/edittransactiondialog.ui
        interface/startscreen.ui
        interface/trackerapp.ui

        images.qrc
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Tracker
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET Tracker APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(Tracker SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(Tracker
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(Tracker PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(Tracker PRIVATE Qt${QT_VERSION_MAJOR}::Core)
target_link_libraries(Tracker PRIVATE Qt${QT_VERSION_MAJOR}::Gui)
target_link_libraries(Tracker PRIVATE Qt${QT_VERSION_MAJOR}::Sql)
target_link_libraries(Tracker PRIVATE Qt${QT_VERSION_MAJOR}::Network)

set_target_properties(Tracker PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Tracker)
endif()
