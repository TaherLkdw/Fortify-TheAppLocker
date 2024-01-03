cmake_minimum_required(VERSION 3.16)

project(Fortify VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Felgo REQUIRED)

# Project identifier and version
# More information: https://felgo.com/doc/felgo-publishing/#project-configuration
set(PRODUCT_IDENTIFIER "com.lkdw.application.applocker.Fortify")
set(PRODUCT_VERSION_NAME "1.0.0")
set(PRODUCT_VERSION_CODE 1)

# Set either "test" or "publish" stage:
set(PRODUCT_STAGE "test")

# Optionally set a license key that is used instead of the license key from
# main.qml file (App::licenseKey for your app or GameWindow::licenseKey for your game)
# Only used for local builds and Felgo Cloud Builds (https://felgo.com/cloud-builds)
# Not used if using Felgo Live
set(PRODUCT_LICENSE_KEY "2D22691797E3DF842A438B1D1BD57B29AC9BC318300A6886F5685DFB344F40BF67B9CD07A513B12C79D905142ABE0CAD64A964C05CB1C697D7D77AFECE8E3A097F3E1E4884FD67530FAA0CA1E78ED544415CD6101619117C4B313B277F21BCE4387A85B4B67512EF604F15ABE1EF5E8362AD640F3AE2D41F1BAB74F6C42EDE3B9F01BE451207A3B8D0E4061C34764395435A16E869B71DEF3DC255D498C67486BB2F6301D5AC78465C419CE1D27009B7BCFEA6E8F39DECDFB2557A6696FAF34927F471AA7E238DD5201E3B9EB6AD64F57EEE3673DE0E7AC5DBA5BBB2298EF6A2DDD6C8BC1F79AB83603680194288187F3635FF6AC9F7831E8EDD22ABE0D7B5BA48A6B115BAF6A1C0A2C1CE9016AEDAB9039BE27F4A872C54DB0AF9A3FF045F3430031C11B8D7C009155E63D0013BAA4B8B0F19EC01F03ACA491DF09A440F5769")

set(FELGO_PLUGINS
)

# Find all QML/JS files for the QML compiler:
file(GLOB_RECURSE QmlFiles RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} qml/*.qml qml/*.js)

# Find all non-QML/JS files in the qml and assets folder to add as resources:
file(GLOB_RECURSE AssetsFiles RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} assets/* qml/*)
list(REMOVE_ITEM AssetsFiles ${QmlFiles})

file(GLOB_RECURSE SourceFiles RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/*)

qt_add_executable(appFortify
    main.cpp
    ${SourceFiles}
    ${QmlFiles}
    ${AssetsFiles}
)

felgo_configure_executable(appFortify)

# Deploy resources to build folder/package directly
# comment for publishing
#deploy_resources("${QmlFiles};${AssetsFiles}")

# Add QML files and resources to QML module to included them via QRC automatically:
qt_add_qml_module(appFortify
    URI Fortify
    VERSION 1.0
# uncomment for publishing:
    QML_FILES ${QmlFiles}
    RESOURCES ${AssetsFiles}
    NO_RESOURCE_TARGET_PATH
)

# NOTE: for PUBLISHING, perform the following steps:
# 1. comment the deploy_resources line above, to avoid shipping your qml files with the application (instead they get compiled to the app binary)
# 2. uncomment the QML_FILES and RESOURCES above; this compiles your qml files and js files to the app binary and protects your source code
# 3. change the setMainQmlFile() call in main.cpp to the one starting with "qrc:/" - this loads the qml files from the resources
# for more details see the "Deployment Guides" in the Felgo Documentation

# during development, use the deploy_resources because you then get shorter compilation times (the qml files do not need to be compiled to the binary but are just copied)
# also, for quickest deployment on Desktop disable the "Shadow Build" option in Projects/Builds - you can then select "Run Without Deployment" from the Build menu in Qt Creator if you only changed QML files; this speeds up application start, because your app is not copied & re-compiled but just re-interpreted

target_compile_definitions(appFortify
    PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)

target_link_libraries(appFortify PRIVATE Felgo)

# uncomment this line to add the Live Client Module and use live reloading with your custom C++ code
# for the remaining steps to build a custom Live Code Reload app see here: https://felgo.com/custom-code-reload-app/
#find_package(FelgoLive REQUIRED)
#target_link_libraries(appFortify PRIVATE FelgoLive)
