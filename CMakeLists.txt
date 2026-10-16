cmake_minimum_required(VERSION 3.22)
project(QuartzDE VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Seguridad: advertencias estrictas
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Dependencias Qt6
find_package(Qt6 6.5 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Quick
    Qml
    DBus
    Concurrent
)

# Dependencias KDE Frameworks 6 opcionales/condicionales si están disponibles
find_package(KF6CoreAddons QUIET)
find_package(KF6Config QUIET)
find_package(KF6WindowSystem QUIET)
find_package(KF6PulseAudioQt QUIET)
find_package(KF6Screen QUIET)

# Incluir submódulos del sistema
add_subdirectory(src/style)
add_subdirectory(src/compositor)
add_subdirectory(src/settings)
add_subdirectory(src/shell)
add_subdirectory(src/finder)

# Pruebas automatizadas C++
enable_testing()
add_subdirectory(tests)
