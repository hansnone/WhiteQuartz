add_executable(quartz-shell
    main.cpp
    menubar/MenuBarController.cpp
    menubar/MenuBarController.h
    dock/DockController.cpp
    dock/DockController.h
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
    quartz-style
)

install(TARGETS quartz-shell
    RUNTIME DESTINATION bin
)
install(DIRECTORY qml/
    DESTINATION share/quartz-shell/qml
)
