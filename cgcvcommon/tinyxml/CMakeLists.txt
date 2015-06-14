project(tinyxml)

set(tinyxml_SOURCES
  tinystr.h
  tinyxml.h
  tinyxml.cpp
  tinyxmlerror.cpp
  tinyxmlparser.cpp
  tinystr.cpp
)

add_definitions(-DTIXML_USE_STL)

add_library(tinyxml STATIC ${tinyxml_SOURCES})
