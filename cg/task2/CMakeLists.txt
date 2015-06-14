cmake_minimum_required(VERSION 2.8)
project(cgtask2)


find_package(OpenGL REQUIRED)

if (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUXX OR MINGW)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUXX OR MINGW)

if (NOT DEFINED FULL_VERSION)
  set(FULL_VERSION 1)
endif()
add_definitions(-DFULL_VERSION=${FULL_VERSION})

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

add_subdirectory(dep)

include_directories(dep/glfw/include)
include_directories(dep/include)
include_directories(framework)

set(SOURCES
    main.cpp
    task2.h
    task2.cpp
   )
set(CG2_FRAMEWORK_SRC
    framework/Model.cpp
    framework/Mesh.cpp
    framework/Material.cpp
    framework/Joint.cpp
    framework/Shader.cpp
    framework/InFile.cpp
    framework/Window.cpp
    framework/IQMImporter.cpp
    framework/IModelDrawer.cpp
    framework/ModelDrawer.cpp
    framework/GLBuffer.cpp
    framework/Camera.cpp
    framework/Animation.cpp
    framework/Keyframe.cpp
    framework/Light.cpp
    framework/PointLight.cpp
    framework/Image.cpp
    framework/Config.cpp
    framework/Spline.cpp
    framework/SplineDrawer.cpp
   )

set(CG2_FRAMEWORK_HEADERS
    framework/Model.h
    framework/Mesh.h
    framework/Material.h
    framework/Joint.h
    framework/Shader.h
    framework/InFile.h
    framework/Window.h
    framework/IQMImporter.h
    framework/IModelDrawer.h
    framework/ModelDrawer.h
    framework/GLBuffer.h
    framework/Camera.h
    framework/Animation.h
    framework/Keyframe.h
    framework/Input.h
    framework/Light.h
    framework/PointLight.h
    framework/Image.h
    framework/Config.h
    framework/Spline.h
    framework/SplineDrawer.h
   )

set(CG2_DEPENDENCY_SRC
    dep/src/gl3w.c
    dep/src/tinyxml2.cpp
    dep/src/lodepng.cpp
   )

add_executable(cgtask2 ${SOURCES} ${CG2_FRAMEWORK_SRC} ${CG2_FRAMEWORK_HEADERS} ${CG2_DEPENDENCY_SRC})

add_submit_target(cgtask2)



if (UNIX)
	target_link_libraries(cgtask2 glfw ${GLFW_LIBRARIES} dl)
else (UNIX)
	target_link_libraries(cgtask2 glfw ${GLFW_LIBRARIES})
endif (UNIX)
