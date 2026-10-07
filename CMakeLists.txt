cmake_minimum_required(VERSION 3.16)
project(GerenciadorReservas LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Núcleo (models + services): sem nenhuma dependência de interface.
add_library(nucleo STATIC
    services/cliente.cpp
    services/quadra.cpp
    services/reserva.cpp)

# Versão CLI (main.cpp original).
add_executable(reservas-cli main.cpp)
target_link_libraries(reservas-cli PRIVATE nucleo)

# Versão gráfica (Qt Widgets). Só é compilada se o Qt6 for encontrado.
find_package(Qt6 COMPONENTS Widgets)
if(Qt6_FOUND)
    set(CMAKE_AUTOMOC ON)
    add_executable(reservas-gui
        interface/qt/main_qt.cpp
        interface/qt/Sistema.cpp
        interface/qt/MapaQuadras.cpp
        interface/qt/Dialogos.cpp
        interface/qt/JanelaPrincipal.cpp)
    target_link_libraries(reservas-gui PRIVATE nucleo Qt6::Widgets)
else()
    message(STATUS "Qt6 não encontrado: apenas reservas-cli será compilado.")
endif()
