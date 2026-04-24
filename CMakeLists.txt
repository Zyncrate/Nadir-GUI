cmake_minimum_required(VERSION 3.16)
project(MaterialSelector VERSION 0.1 LANGUAGES CXX C)

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
    src/database.cpp
    src/selector.cpp
    src/mathutil.cpp
    src/sqlite3.c
    app.rc
    resources.qrc
)

include_directories(include)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(MaterialSelector
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(MaterialSelector SHARED ${PROJECT_SOURCES})
    else()
        add_executable(MaterialSelector ${PROJECT_SOURCES})
    endif()
endif()

target_link_libraries(MaterialSelector PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

if(WIN32)
    target_link_libraries(MaterialSelector PRIVATE -static-libgcc -static-libstdc++)
endif()

set_target_properties(MaterialSelector PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS MaterialSelector
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(MaterialSelector)
endif()

# Copy database to build folder automatically
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/materials.db
               ${CMAKE_CURRENT_BINARY_DIR}/materials.db COPYONLY)
