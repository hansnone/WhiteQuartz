find_package(Qt6 COMPONENTS Test REQUIRED)

# Test 1: Métricas de estilo
add_executable(tst_quartzstyle tst_quartzstyle.cpp)
target_include_directories(tst_quartzstyle PRIVATE ${CMAKE_SOURCE_DIR}/src/style)
target_link_libraries(tst_quartzstyle PRIVATE Qt6::Core Qt6::Widgets Qt6::Test quartz-style)
add_test(NAME tst_quartzstyle COMMAND tst_quartzstyle)

# Test 2: Modelo de Ajustes del Sistema
add_executable(tst_settings_model tst_settings_model.cpp ${CMAKE_SOURCE_DIR}/src/settings/settingsmodel.cpp)
target_include_directories(tst_settings_model PRIVATE ${CMAKE_SOURCE_DIR}/src/settings)
target_link_libraries(tst_settings_model PRIVATE Qt6::Core Qt6::Qml Qt6::Test)
add_test(NAME tst_settings_model COMMAND tst_settings_model)

# Test 3: Matemáticas de Stage Manager y Superelipse
add_executable(tst_stagemanager_math tst_stagemanager_math.cpp)
target_include_directories(tst_stagemanager_math PRIVATE ${CMAKE_SOURCE_DIR}/src/compositor ${CMAKE_SOURCE_DIR}/src/style)
target_link_libraries(tst_stagemanager_math PRIVATE Qt6::Core Qt6::Gui Qt6::Test quartz-compositor)
add_test(NAME tst_stagemanager_math COMMAND tst_stagemanager_math)

# Test 4: Conmutación de los 4 Modos de Finder
add_executable(tst_finder_modes tst_finder_modes.cpp ${CMAKE_SOURCE_DIR}/src/finder/findercolumnview.cpp)
target_include_directories(tst_finder_modes PRIVATE ${CMAKE_SOURCE_DIR}/src/finder ${CMAKE_SOURCE_DIR}/src/style)
target_link_libraries(tst_finder_modes PRIVATE Qt6::Core Qt6::Widgets Qt6::Test quartz-style)
add_test(NAME tst_finder_modes COMMAND tst_finder_modes)

# Test 5: Controlador de Ajustes D-Bus y Configuración
add_executable(tst_settingsconfig tst_settingsconfig.cpp ${CMAKE_SOURCE_DIR}/src/settings/settingsconfigcontroller.cpp)
target_include_directories(tst_settingsconfig PRIVATE ${CMAKE_SOURCE_DIR}/src/settings)
target_link_libraries(tst_settingsconfig PRIVATE Qt6::Core Qt6::DBus Qt6::Test)
add_test(NAME tst_settingsconfig COMMAND tst_settingsconfig)

# Test 6: Controladores de Shell (Spotlight, QuickLook y Dock)
add_executable(tst_shell_controllers
    tst_shell_controllers.cpp
    ${CMAKE_SOURCE_DIR}/src/shell/spotlight/SpotlightController.cpp
    ${CMAKE_SOURCE_DIR}/src/shell/quicklook/QuickLookController.cpp
    ${CMAKE_SOURCE_DIR}/src/shell/dock/DockController.cpp
    ${CMAKE_SOURCE_DIR}/src/shell/menubar/MenuBarController.cpp
    ${CMAKE_SOURCE_DIR}/src/shell/menubar/appmenuregistrar.cpp
)
target_include_directories(tst_shell_controllers PRIVATE ${CMAKE_SOURCE_DIR}/src/shell ${CMAKE_SOURCE_DIR}/src/style)
target_link_libraries(tst_shell_controllers PRIVATE Qt6::Core Qt6::Gui Qt6::DBus Qt6::Test)
add_test(NAME tst_shell_controllers COMMAND tst_shell_controllers)
