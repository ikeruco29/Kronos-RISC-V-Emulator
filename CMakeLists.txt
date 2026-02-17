cmake_minimum_required(VERSION 3.5)

project(Emulador-RISC-V VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Esto le dice a AUTOUIC dónde empezar a buscar los archivos .ui
set(CMAKE_AUTOUIC_SEARCH_PATHS "${CMAKE_CURRENT_SOURCE_DIR}")

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
        main.cpp
        src/gui/mainwindow.cpp
        include/gui/mainwindow.h
        ui/mainwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Emulador-RISC-V
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        res.qrc
        resources/icons/execute.png resources/icons/stopExe.png
        src/computer.cpp include/computer.h src/core/cpu.cpp include/core/cpu.h src/core/decoder.cpp include/core/decoder.h src/core/endian.cpp include/core/endian.h
        src/memory/memory.cpp include/memory/memory.h
        config.json
        resources/icons/executePaso.png
        include/gui/dialog/statsdialog.h src/gui/dialog/statsdialog.cpp ui/dialog/statsdialog.ui
        include/gui/dialog/instructionsdialog.h src/gui/dialog/instructionsdialog.cpp ui/dialog/instructionsdialog.ui


        include/gui/editor/codeeditor.h src/gui/editor/codeeditor.cpp
        include/gui/editor/highlighter.h src/gui/editor/highlighter.cpp
        include/gui/dialog/executioncampigndialog.h src/gui/dialog/executioncampigndialog.cpp ui/dialog/executioncampigndialog.ui
        include/gui/dialog/compilingprocessdialog.h src/gui/dialog/compilingprocessdialog.cpp ui/dialog/compilingprocessdialog.ui
        include/core/loader.h src/core/loader.cpp





    )

    # Copiar el archivo config.json al directorio de salida del ejecutable
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/config.json DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET Emulador-RISC-V APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(Emulador-RISC-V SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(Emulador-RISC-V
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(Emulador-RISC-V PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# To beautify relative routes
target_include_directories(Emulador-RISC-V PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_BINARY_DIR}"
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.Emulador-RISC-V)
endif()
set_target_properties(Emulador-RISC-V PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS Emulador-RISC-V
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Emulador-RISC-V)
endif()
