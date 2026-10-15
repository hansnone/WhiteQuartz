add_library(quartz-style SHARED
    quartzstyle.cpp
    quartzstyle.h
    quartzmetrics.h
)

target_include_directories(quartz-style PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(quartz-style PUBLIC
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# Instalación protegida (solo si se invoca make/ninja install dentro de la VM)
install(TARGETS quartz-style
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)
install(FILES quartzmetrics.h quartzstyle.h
    DESTINATION include/quartz/style
)
