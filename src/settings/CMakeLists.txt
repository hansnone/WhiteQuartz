add_executable(quartz-settings
    main.cpp
    settingsmodel.cpp
    settingsmodel.h
    settingsconfigcontroller.cpp
    settingsconfigcontroller.h
    resources.qrc
)

target_compile_definitions(quartz-settings PRIVATE
    QUARTZ_SETTINGS_QML_DIR="${CMAKE_CURRENT_SOURCE_DIR}/qml"
)

target_include_directories(quartz-settings PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src/style
)

target_link_libraries(quartz-settings PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Quick
    Qt6::Qml
    Qt6::DBus
    quartz-style
)

install(TARGETS quartz-settings
    RUNTIME DESTINATION bin
)
install(DIRECTORY qml/
    DESTINATION share/quartz-settings/qml
)
