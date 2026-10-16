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

find_package(LayerShellQt QUIET)
if(LayerShellQt_FOUND)
    message(STATUS "LayerShellQt detectado: habilitando zonas exclusivas Wayland para MenuBar y Dock")
    target_link_libraries(quartz-shell PRIVATE LayerShellQt::Interface)
    target_compile_definitions(quartz-shell PRIVATE HAVE_LAYERSHELLQT)
endif()

install(TARGETS quartz-shell
    RUNTIME DESTINATION bin
)
install(DIRECTORY qml/
    DESTINATION share/quartz-shell/qml
)
