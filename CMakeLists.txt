project(cgcv)
cmake_minimum_required(VERSION 2.6)

if (NOT WIN32)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

# we need one submit target per task:
cmake_policy(SET CMP0002 OLD)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/modules)

# For OpenCV support at pluto.tugraz.at
set(OpenCV_DIR /software/opencv/share/opencv)

# compile full version (1) or student version (0):
set(FULL_VERSION 1)
if(NOT CMAKE_BUILD_TYPE)
set(CMAKE_BUILD_TYPE "Debug" CACHE STRING
      "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel."   FORCE)
endif(NOT CMAKE_BUILD_TYPE)
set(SOURCE_WILDCARDS *.h *.H *.hpp *.hh *.hxx *.c *.C *.cpp *.cc *.cxx)

macro(ADD_SUBMIT_TARGET winname)
  if(NOT WIN32)
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/" "" SUBMIT_SOURCES "${SOURCES}")
    add_custom_target(submit COMMAND tar cfj submission.tar.bz2 CMakeLists.txt ${SUBMIT_SOURCES})
  else(NOT WIN32)
    #experimental
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/" "" SUBMIT_SOURCES "${SOURCES}")
    list(REMOVE_DUPLICATES SUBMIT_SOURCES)
    add_custom_target("submit${winname}" COMMAND cd ${CMAKE_CURRENT_SOURCE_DIR} & ${CMAKE_SOURCE_DIR}/tar -c -fsubmission.tar CMakeLists.txt ${SUBMIT_SOURCES} & ${CMAKE_SOURCE_DIR}/bzip2 -f submission.tar)
  endif(NOT WIN32)
endmacro(ADD_SUBMIT_TARGET)

macro(ADD_SUBDIRECTORY_IF_EXISTS dir)
  if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${dir})
    add_subdirectory(${dir})
  endif(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${dir})
endmacro(ADD_SUBDIRECTORY_IF_EXISTS)

MACRO(add_pch header target sources)
  if (MSVC)
    foreach (src_file ${${sources}})
      set_source_files_properties(
        ${src_file}
        PROPERTIES
        COMPILE_FLAGS "/Yu${header}"
      )
    endforeach (src_file ${${sources}})

    set_source_files_properties(
      ${target}
      PROPERTIES
      COMPILE_FLAGS "/Yc${header}"
    )

    LIST(APPEND ${sources} ${target})
  endif (MSVC)
ENDMACRO(add_pch)

# Boost:
#find_package(Boost REQUIRED)
#include_directories(${Boost_INCLUDE_DIR})
#link_directories(${Boost_LIBRARY_DIRS})

add_definitions(-DFULL_VERSION=${FULL_VERSION} -DTIXML_USE_STL)

# used for xml configuration
add_subdirectory(cgcvcommon)

# For OpenCV support at pluto.tugraz.at
include_directories(/software/opencv/include)

if(WIN32)
  add_definitions(-D_CRT_SECURE_NO_DEPRECATE -D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS)
  add_subdirectory(cgcvcommon/zlib-1.2.6)
  add_subdirectory(cgcvcommon/lpng158)
  set(PNG_LIBRARIES lpng)
  set(ZLIB_LIBRARIES zlib)
  set(PNG_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/cgcvcommon/lpng158")
  set(ZLIB_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/cgcvcommon/zlib-1.2.6")
else(WIN32)
  find_package(PNG)
  include_directories(${PNG_INCLUDE_DIR})
  find_package(ZLIB)
  include_directories(${ZLIB_INCLUDE_DIR})
endif(WIN32)

option(BUILD_CG1 "Build CG1" ON)
option(BUILD_CG2 "Build CG2" ON)
option(BUILD_CV1 "Build CV1" ON)
option(BUILD_CV2 "Build CV2" ON)

if (BUILD_CG1 OR BUILD_CG2)
  add_subdirectory_if_exists(cg)
endif (BUILD_CG1 OR BUILD_CG2)

if (BUILD_CV1 OR BUILD_CV2)
  add_subdirectory_if_exists(cv)
endif (BUILD_CV1 OR BUILD_CV2)

add_custom_target(clean-all COMMAND make clean \; find \\\( -name CMakeFiles -or -name CMakeCache.txt -or -name Makefile -or -name cmake_install.cmake -or -name progress.make -or -name _CPack_Packages -or -name '*~' -or -name submission.tar.bz2 \\\) -prune -exec rm -rf {} '\;')
