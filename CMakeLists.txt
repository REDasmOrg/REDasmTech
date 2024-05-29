cmake_minimum_required(VERSION 3.25)

project(redasm_gui)

include(cmake/Dependencies.cmake)
setup_dependencies()

qt_add_executable(${PROJECT_NAME})

qt_add_resources(${PROJECT_NAME} "resources"
    PREFIX "/"

    FILES
        "res/fonts/FontAwesomeBrands.otf"
        "res/fonts/FontAwesome.otf"
        "res/themes/light.json"
        "res/themes/dark.json"
        "res/logo_dark.png"
        "res/logo.png"
)

add_subdirectory(lib)
add_subdirectory(QHexView)

target_sources(${PROJECT_NAME} 
    PRIVATE
        src/main.cpp
        src/utils.cpp
        src/actions.cpp
        src/settings.cpp
        src/themeprovider.cpp
        src/mainwindow.cpp
        src/fontawesome.cpp
        src/statusbar.cpp

        src/hexview/flagsbuffer.cpp

        src/models/recentfilesdelegate.cpp
        src/models/recentfilesmodel.cpp
        src/models/segmentsmodel.cpp
        src/models/symbolsfiltermodel.cpp
        src/models/symbolsmodel.cpp

        src/dialogs/flcdialog.cpp
        src/dialogs/gotodialog.cpp
        src/dialogs/tabledialog.cpp
        src/dialogs/loaderdialog.cpp
        src/dialogs/analyzerdialog.cpp
        src/dialogs/memorymapdialog.cpp

        src/widgets/contextview.cpp
        src/widgets/dashboardwidget.cpp
        src/widgets/welcomeview.cpp
        src/widgets/surface/surfacewidget.cpp
        src/widgets/surface/surfaceview.cpp
        src/widgets/surface/surfacepath.cpp
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        Qt6::Widgets
        redasm::lib
        QHexView
)

set_target_properties(${PROJECT_NAME}
    PROPERTIES
        WIN32_EXECUTABLE ON
        MACOSX_BUNDLE ON
)

include(cmake/Compiler.cmake)
setup_compiler(${PROJECT_NAME})