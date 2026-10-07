cmake_minimum_required(VERSION 3.16)
project(ReservasQuadras LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# Regras de negócio (as mesmas usadas pela CLI)
add_library(reservas_core STATIC
    services/cliente.cpp
    services/quadra.cpp
    services/reserva.cpp
)
target_include_directories(reservas_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Versão CLI original
add_executable(reservas_cli main.cpp)
target_link_libraries(reservas_cli PRIVATE reservas_core)

# Interface gráfica Qt Widgets
add_executable(reservas_gui
    gui/main_gui.cpp
    gui/MainWindow.cpp
    gui/MainWindow.hpp
    gui/resources.qrc
)
target_link_libraries(reservas_gui PRIVATE reservas_core Qt${QT_VERSION_MAJOR}::Widgets)
