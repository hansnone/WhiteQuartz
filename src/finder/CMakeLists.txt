add_executable(quartz-finder
    main.cpp
    findercolumnview.cpp
    findercolumnview.h
    quicklookclient.cpp
    quicklookclient.h
)

target_include_directories(quartz-finder PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src/style
)

target_link_libraries(quartz-finder PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::DBus
    quartz-style
)

install(TARGETS quartz-finder
    RUNTIME DESTINATION bin
)
