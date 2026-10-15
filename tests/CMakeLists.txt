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
