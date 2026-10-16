add_executable(quartz-shell
    main.cpp
    menubar/MenuBarController.cpp
    menubar/MenuBarController.h
    menubar/appmenuregistrar.cpp
    menubar/appmenuregistrar.h
    dock/DockController.cpp
    dock/DockController.h
    spotlight/SpotlightController.cpp
    spotlight/SpotlightController.h
    quicklook/QuickLookController.cpp
    quicklook/QuickLookController.h
    notifications/NotificationServer.cpp
    notifications/NotificationServer.h
    notifications/NotificationModel.cpp
    notifications/NotificationModel.h
    controlcenter/SystemHardwareController.cpp
    controlcenter/SystemHardwareController.h
    shortcuts/GlobalShortcuts.cpp
    shortcuts/GlobalShortcuts.h
    launchpad/LaunchpadController.cpp
    launchpad/LaunchpadController.h
    missioncontrol/MissionControlController.cpp
    missioncontrol/MissionControlController.h
    corners/HotCornersController.cpp
    corners/HotCornersController.h
    sni/StatusNotifierWatcher.cpp
    sni/StatusNotifierWatcher.h
    services/QuartzServices.cpp
    services/QuartzServices.h
    resources.qrc
)

target_compile_definitions(quartz-shell PRIVATE
    QUARTZ_SHELL_QML_DIR="${CMAKE_CURRENT_SOURCE_DIR}/qml"
    QUARTZ_SHELL_INSTALL_QML_DIR="${CMAKE_INSTALL_PREFIX}/share/quartz-shell/qml"
)

target_include_directories(quartz-shell PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src/style
)

target_link_libraries(quartz-shell PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Quick
    Qt6::Qml
    Qt6::DBus
    quartz-style
)

option(QUARTZ_ALLOW_MISSING_DEPS "Permitir compilacion del shell omitiendo dependencias criticas del sistema" OFF)

if(QUARTZ_ALLOW_MISSING_DEPS)
    set(_DEP_MODE "")
    message(WARNING "QUARTZ_ALLOW_MISSING_DEPS activado: La falta de dependencias no detendra CMake, pero la sesion puede carecer de atajos o capas Wayland.")
else()
    set(_DEP_MODE "REQUIRED")
endif()

find_package(LayerShellQt ${_DEP_MODE})
if(LayerShellQt_FOUND)
    message(STATUS "LayerShellQt detectado: habilitando zonas exclusivas Wayland para MenuBar y Dock")
    target_link_libraries(quartz-shell PRIVATE LayerShellQt::Interface)
    target_compile_definitions(quartz-shell PRIVATE HAVE_LAYERSHELLQT)
else()
    message(WARNING "LayerShellQt NO encontrado. El shell compilara sin soporte de layer-shell.")
endif()

find_package(KF6WindowSystem ${_DEP_MODE})
if(KF6WindowSystem_FOUND)
    message(STATUS "KF6WindowSystem detectado: habilitando efecto Blur Behind (Vibrancy) en shell")
    target_link_libraries(quartz-shell PRIVATE KF6::WindowSystem)
    target_compile_definitions(quartz-shell PRIVATE HAVE_KF6WINDOWSYSTEM)
else()
    message(WARNING "KF6WindowSystem NO encontrado. El shell compilara sin soporte de desenfoque.")
endif()

find_package(KF6GlobalAccel ${_DEP_MODE})
if(KF6GlobalAccel_FOUND)
    message(STATUS "KF6GlobalAccel detectado: habilitando soporte de atajos de teclado globales")
    target_link_libraries(quartz-shell PRIVATE KF6::GlobalAccel)
    target_compile_definitions(quartz-shell PRIVATE HAVE_KF6GLOBALACCEL)
else()
    message(WARNING "KF6GlobalAccel NO encontrado. El shell compilara sin registro de atajos globales.")
endif()

install(TARGETS quartz-shell
    RUNTIME DESTINATION bin
)
install(DIRECTORY qml/
    DESTINATION share/quartz-shell/qml
)
