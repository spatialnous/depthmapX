# SPDX-FileCopyrightText: 2020 Petros Koutsolampros
#
# SPDX-License-Identifier: GPL-3.0-or-later

set(module segmentpathsgui)
set(segmentpathsgui_SRCS
    segmentpathsmainwindow.hpp
    segmentpathsmainwindow.cpp)
set(modules_gui "${modules_gui}" ${module} CACHE INTERNAL "modules_gui" FORCE)

find_package(Qt6 COMPONENTS Core Widgets Gui OpenGLWidgets OpenGL REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${Qt6Core_INCLUDE_DIRS})
include_directories(${Qt6Widgets_INCLUDE_DIRS})
include_directories(${Qt6Gui_INCLUDE_DIRS})
include_directories(${Qt6OpenGL_INCLUDE_DIRS})
include_directories(${Qt6OpenGLWidgets_INCLUDE_DIRS})

set(CMAKE_AUTOMOC OFF)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS "../../../qtgui/UI")

add_definitions(${Qt6Core_DEFINITIONS})
add_definitions(${Qt6Widgets_DEFINITIONS})
add_definitions(${Qt6Gui_DEFINITIONS})
add_definitions(${Qt6OpenGL_DEFINITIONS})
add_definitions(${Qt6OpenGLWidgets_DEFINITIONS})

add_compile_definitions(SEGMENTPATHS_GUI_LIBRARY)

add_library(${module} OBJECT ${segmentpathsgui_SRCS}
    ../../../qtgui/imainwindowmodule.hpp)

if ((MSVC) AND (MSVC_VERSION GREATER_EQUAL 1914))
    # new option required from MSVC, but not yet implemented in CMake
    # see: https://gitlab.kitware.com/cmake/cmake/-/issues/18837
    target_compile_options(${module} PUBLIC "/Zc:__cplusplus" "-permissive-")
endif()
