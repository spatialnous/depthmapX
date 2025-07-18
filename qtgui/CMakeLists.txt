# SPDX-FileCopyrightText: 2017 Christian Sailer
# SPDX-FileCopyrightText: 2017 Petros Koutsolampros
#
# SPDX-License-Identifier: GPL-3.0-or-later

if (NOT project_NAME)
    set(target_NAME qtgui)
else()
    set(target_NAME ${project_NAME})
endif()

# set this to allow for dminterface to interact with sala
set(dminterface ${target_NAME})

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)
# Create code from a list of Qt designer ui files
# set(CMAKE_AUTOUIC ON)
# set(CMAKE_AUTORCC ON)

set(CMAKE_COMPILE_WARNING_AS_ERROR OFF)

if (NOT FORCE_GLOBAL_COMPILE_WARNINGS)
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR
        "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        set(COMPILE_WARNINGS -Wall -Wextra -Wpedantic)
    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
        set(COMPILE_WARNINGS /W4 /EHsc)
    endif()
endif()

# Find the QtWidgets library
find_package(Qt6 COMPONENTS Core Widgets Gui OpenGLWidgets OpenGL REQUIRED)

add_compile_definitions(_DEPTHMAP)

set(target_SRCS
    graphdoc.cpp
    indexWidget.cpp
    mainwindow.cpp
    mdichild.cpp
    renderthread.cpp
    treeWindow.cpp
    mainwindowfactory.cpp
    settingsimpl.cpp
    mainwindowmoduleregistry.hpp
    compatibilitydefines.hpp
    mainwindow.hpp
    settings.hpp
    graphdoc.hpp
    mainwindowfactory.hpp
    settingsimpl.hpp
    indexWidget.hpp
    mdichild.hpp
    treeWindow.hpp
    main.cpp
    coreapplication.cpp
    coreapplication.hpp
    mainwindowhelpers.cpp
    mainwindowmoduleregistry.cpp
    imainwindowmodulefactory.hpp
    interfaceversion.hpp
)

qt_wrap_ui(UI_HDRS
    UI/TopoMetDlg.ui
    UI/SegmentAnalysisDlg.ui
    UI/RenameObjectDlg.ui
    UI/PushDialog.ui
    UI/PromptReplace.ui
    UI/OptionsDlg.ui
    UI/NewLayerDlg.ui
    UI/MakeOptionsDlg.ui
    UI/MakeLayerDlg.ui
    UI/LicenceDialog.ui
    UI/LayerChooserDlg.ui
    UI/IsovistPathDlg.ui
    UI/InsertColumnDlg.ui
    UI/GridDialog.ui
    UI/FindLocDlg.ui
    UI/FilePropertiesDlg.ui
    UI/FewestLineOptionsDlg.ui
    UI/EditConnectionsDlg.ui
    UI/ConvertShapesDlg.ui
    UI/ColumnPropertiesDlg.ui
    UI/ColourScaleDlg.ui
    UI/AxialAnalysisOptionsDlg.ui
    UI/AttributeSummary.ui
    UI/AttributeChooserDlg.ui
    UI/AgentAnalysisDlg.ui
    UI/AboutDlg.ui
    UI/licenseagreement.ui)

qt_add_resources(DM_RSRC resource.qrc dialogs/settings/settingsdialog.qrc)

if (WIN32)
        add_executable(${target_NAME} WIN32 ${target_SRCS} ${UI_HDRS} ${DM_RSRC} icons.rc)
        if(MINGW)
            # https://stackoverflow.com/a/18138926
            # MinGW compilation does not bring in the required DLLs to make the executable self-contained
            # The missing DLLs are:
            # - libstdc++-6.dll
            # - libwinpthread-1.dll
            # - libgcc_s_seh-1.dll
            # Additionally, windeployqt does not drag them into the build directory, so it might be
            # necessary to manually find and copy them after compilation
            set(CMAKE_CXX_STANDARD_LIBRARIES "-static-libgcc -static-libstdc++ -lwsock32 -lws2_32 ${CMAKE_CXX_STANDARD_LIBRARIES}")
            set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive")
        endif(MINGW)
endif(WIN32)
if(UNIX AND NOT APPLE)
        add_executable(${target_NAME} ${target_SRCS} ${UI_HDRS} ${DM_RSRC}
            consts.hpp)
        if (Qt5_POSITION_INDEPENDENT_CODE)
            SET(CMAKE_POSITION_INDEPENDENT_CODE ON)
        endif()

        # remove all warnings from this target
        set_target_properties(${target_NAME} PROPERTIES COMPILE_FLAGS "-w")
endif()
if(APPLE)
        set(MACOSX_BUNDLE_ICON_FILE depthmap.icns)
        set(depthmap_ICON ${CMAKE_CURRENT_SOURCE_DIR}/icons/depthmap.icns)
        set_source_files_properties(${target_ICON} PROPERTIES
               MACOSX_PACKAGE_LOCATION "Resources")
        set(graph_ICON ${CMAKE_CURRENT_SOURCE_DIR}/icons/graph.icns)
        set_source_files_properties(${graph_ICON} PROPERTIES
               MACOSX_PACKAGE_LOCATION "Resources")
        add_definitions(-DGL_SILENCE_DEPRECATION)
        add_executable(${target_NAME} MACOSX_BUNDLE ${depthmap_ICON} ${graph_ICON}
            ${target_SRCS} ${UI_HDRS} ${DM_RSRC})
        set_target_properties(${target_NAME} PROPERTIES
            MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/resources/Info.plist)
endif(APPLE)

target_compile_options(${target_NAME} PRIVATE ${COMPILE_WARNINGS})

find_package(OpenGL REQUIRED)

set(modules_gui "" CACHE INTERNAL "modules_gui" FORCE)
set(MODULES_GUI TRUE)
set(MODULES_CLI FALSE)
set(MODULES_CLI_TEST FALSE)
set(MODULES_CORE FALSE)
set(MODULES_CORE_TEST FALSE)
add_subdirectory(../modules modules)

set(LIBRARIES
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::OpenGLWidgets
    Qt6::OpenGL
    OpenGL::GL
    OpenGL::GLU
)
find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    set(${LIBRARIES} ${LIBRARIES} OpenMP::OpenMP_CXX)
elseif()
    message("OpenMP not found, building without")
endif()

target_link_libraries(${target_NAME} salalib ${LIBRARIES} ${modules_gui} ${modules_core})

if ((MSVC) AND (MSVC_VERSION GREATER_EQUAL 1914))
    # new option required from MSVC, but not yet implemented in CMake
    # see: https://gitlab.kitware.com/cmake/cmake/-/issues/18837
    target_compile_options(${target_NAME} PUBLIC "/Zc:__cplusplus" "-permissive-")
    target_compile_options(${target_NAME} PRIVATE ${COMPILE_WARNINGS})
endif()

add_subdirectory(dialogs)
add_subdirectory(views)
add_subdirectory(dminterface)

