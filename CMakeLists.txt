cmake_minimum_required(VERSION 3.16)

project(ShowerToys VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
)

file(GLOB QHOTKEY_SOURCES
    "lib/QHotkey/qhotkey.h"
    "lib/QHotkey/qhotkey.cpp"
    "lib/QHotkey/qhotkey_p.h"
)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    file(GLOB QHOTKEY_SOURCES ${QHOTKEY_SOURCES}
        "lib/QHotkey/qhotkey_win.h"
        "lib/QHotkey/qhotkey_win.cpp"
    )
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin") # Darwin is the name for macOS
    file(GLOB QHOTKEY_SOURCES ${QHOTKEY_SOURCES}
        "lib/QHotkey/qhotkey_mac.h"
        "lib/QHotkey/qhotkey_mac.mm" # Note: .mm, not .cpp
    )
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    file(GLOB QHOTKEY_SOURCES ${QHOTKEY_SOURCES}
        "lib/QHotkey/qhotkey_x11.h"
        "lib/QHotkey/qhotkey_x11.cpp"
    )
endif()

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    find_package(Qt6 REQUIRED COMPONENTS Core)

    qt_add_executable(ShowerToys
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        src/modules/MacroManager/macromanager.h src/modules/MacroManager/macromanager.cpp
        src/core/AppController/appcontroller.h src/core/AppController/appcontroller.cpp
        resources.qrc
        src/core/Actions/action.h
        src/core/Actions/openappaction.h src/core/Actions/openappaction.cpp
        src/modules/MacroManager/macro.h src/modules/MacroManager/macro.cpp
        src/modules/moduleinfo.h
        src/modules/modulemanager.h src/modules/modulemanager.cpp
        src/modules/MacroManager/macromanagerwidget.h src/modules/MacroManager/macromanagerwidget.cpp src/modules/MacroManager/macromanagerwidget.ui
        src/modules/MacroManager/addactiondialog.h src/modules/MacroManager/addactiondialog.cpp src/modules/MacroManager/addactiondialog.ui
        src/core/Actions/openurlaction.h src/core/Actions/openurlaction.cpp
        src/core/Actions/typekeystrokeaction.h src/core/Actions/typekeystrokeaction.cpp
        src/core/HotkeyManager/hotkeymanager.h src/core/HotkeyManager/hotkeymanager.cpp
        ${QHOTKEY_SOURCES}

    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET ShowerToys APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(ShowerToys SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(ShowerToys
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_include_directories(ShowerToys PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/QHotkey"
)

target_link_libraries(ShowerToys PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt6::Core
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.ShowerToys)
endif()
set_target_properties(ShowerToys PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
include_directories("src/modules")
include_directories("src/core")
install(TARGETS ShowerToys
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(ShowerToys)
endif()
