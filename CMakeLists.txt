cmake_minimum_required(VERSION 3.5)

project(OpencascadeViewer VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package (OpenCASCADE REQUIRED)
find_package(Boost REQUIRED)
include_directories(${OpenCASCADE_INCLUDE_DIR} ${Boost_INCLUDE_DIRS})
find_package(Qt5 COMPONENTS Widgets OpenGL REQUIRED)

FIND_PACKAGE(OpenSceneGraph COMPONENTS osgDB osgGA osgViewer osg osgUtil osgText)

if (VULKAN_SDK)
     set(ENV{VULKAN_SDK} ${VULKAN_SDK})
endif()
find_package(vsg)
find_package(vsgXchange)
find_package(vsgImGui)
find_package(vsgQt REQUIRED)


set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        examples.cpp
        examples.h
        ocafdelegatemodel.h
        ocafdelegatemodel.cpp
        #labeldelegate.h
        #labeldelegate.cpp
        #ocafapp.h
        #ocafapp.cpp
        #labelindex.h
        qtosgwidget.h
        qtosgwidget.cpp
        #qtvsgwidget.h
        #qtvsgwidget.cpp
        scenegraphdelegatemodel.h
        scenegraphdelegatemodel.cpp
        mainwindow.ui
        ocaftreedialog.h
        ocaftreedialog.cpp
        ocaftreedialog.ui
)

add_executable(OpencascadeViewer
    ${PROJECT_SOURCES})

target_link_libraries(OpencascadeViewer vsg::vsg vsgXchange::vsgXchange vsgQt::vsgQt Qt5::Widgets ${OPENSCENEGRAPH_LIBRARIES} TKBinL TKBin TKBinTObj TKBinXCAF TKBool TKBO TKBRep TKCAF TKCDF TKernel TKFeat TKFillet TKGeomAlgo TKGeomBase TKG2d TKG3d TKHLR TKIGES TKLCAF TKMath TKMesh TKMeshVS TKOffset TKOpenGl TKPrim TKService TKShHealing TKSTEPAttr TKSTEPBase TKSTEP TKSTEP209 TKSTL TKTObj TKTopAlgo TKVRML TKV3d TKXCAF TKXDEIGES TKXDESTEP TKXMesh TKXmlL TKXml TKXmlTObj TKXmlXCAF TKXSBase)
