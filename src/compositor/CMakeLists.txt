add_library(quartz-compositor SHARED
    stagemanagereffect.cpp
    stagemanagereffect.h
    quartzdecoration.cpp
    quartzdecoration.h
)

target_include_directories(quartz-compositor PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src/style
)

target_link_libraries(quartz-compositor PUBLIC
    Qt6::Core
    Qt6::Gui
    Qt6::DBus
    quartz-style
)

install(TARGETS quartz-compositor
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)
install(FILES stagemanagereffect.h quartzdecoration.h
    DESTINATION include/quartz/compositor
)
