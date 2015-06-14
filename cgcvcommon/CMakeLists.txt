project(cgcvcommon)

add_subdirectory(tinyxml)

set(cgcvcommon_HEADERS
  cfg_config.h
  cfg_container.h
  cfg_conversion.h
  cfg_testcase.h
)

set(cgcvcommon_SOURCES
  cfg_config.cpp
  cfg_container.cpp
)

include_directories(.)
add_library(cgcvcommon STATIC ${cgcvcommon_HEADERS} ${cgcvcommon_SOURCES})
