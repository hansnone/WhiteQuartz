add_library(quartz-compositor SHARED
    stagemanagereffect.cpp
    stagemanagereffect.h
    quartzdecoration.cpp
    quartzdecoration.h
)

find_package(KF6CoreAddons QUIET)
find_package(KF6Config QUIET)
find_package(KDecoration3 QUIET)

find_library(KWIN_LIBRARY NAMES kwin)
find_path(KWIN_INCLUDE_DIR NAMES effect/effect.h PATH_SUFFIXES kwin)

target_include_directories(quartz-compositor PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src/style
    ${KWIN_INCLUDE_DIR}
    ${KWIN_INCLUDE_DIR}/kwin
    /usr/include/kwin
)

target_link_libraries(quartz-compositor PUBLIC
    Qt6::Core
    Qt6::Gui
    Qt6::DBus
    quartz-style
)

if(TARGET KDecoration3::KDecoration)
    target_link_libraries(quartz-compositor PUBLIC KDecoration3::KDecoration)
endif()
if(TARGET KF6::CoreAddons)
    target_link_libraries(quartz-compositor PUBLIC KF6::CoreAddons)
endif()
if(TARGET KF6::ConfigCore)
    target_link_libraries(quartz-compositor PUBLIC KF6::ConfigCore)
endif()
if(KWIN_LIBRARY)
    target_link_libraries(quartz-compositor PUBLIC ${KWIN_LIBRARY})
endif()

# Plugin 1: Decoración de ventanas KDecoration3
if(TARGET KDecoration3::KDecoration AND TARGET KF6::CoreAddons)
    add_library(quartzdecoration MODULE
        quartzdecoration_plugin.cpp
        quartzdecoration_plugin.h
        decoration.json
    )
    target_include_directories(quartzdecoration PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/src/style
    )
    target_link_libraries(quartzdecoration PRIVATE
        quartz-compositor
        quartz-style
        Qt6::Core
        Qt6::Gui
        KDecoration3::KDecoration
        KF6::CoreAddons
    )
    install(TARGETS quartzdecoration
        DESTINATION lib/qt6/plugins/org.kde.kdecoration3
    )
endif()

# Plugin 2: Efecto de compositor KWin (OffscreenEffect)
if(KWIN_LIBRARY AND TARGET KF6::CoreAddons AND TARGET KF6::ConfigCore)
    add_library(quartz-stagemanager MODULE
        stagemanager_plugin.cpp
        stagemanager_plugin.h
        compositor.qrc
        metadata.json
    )
    target_include_directories(quartz-stagemanager PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/src/style
        ${KWIN_INCLUDE_DIR}
        ${KWIN_INCLUDE_DIR}/kwin
        /usr/include/kwin
    )
    target_link_libraries(quartz-stagemanager PRIVATE
        quartz-compositor
        quartz-style
        Qt6::Core
        Qt6::Gui
        Qt6::Qml
        Qt6::DBus
        ${KWIN_LIBRARY}
        KF6::CoreAddons
        KF6::ConfigCore
    )
    set_target_properties(quartz-stagemanager PROPERTIES
        AUTOMOC_MACRO_NAMES "Q_OBJECT;KWIN_EFFECT_FACTORY;KWIN_EFFECT_FACTORY_SUPPORTED_ENABLED"
    )
    install(TARGETS quartz-stagemanager
        DESTINATION lib/qt6/plugins/kwin/effects/plugins
    )
endif()

install(TARGETS quartz-compositor
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)
install(FILES stagemanagereffect.h quartzdecoration.h
    DESTINATION include/quartz/compositor
)

