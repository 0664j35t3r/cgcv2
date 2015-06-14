project(lpng)

file(GLOB lpng_SOURCES *.c)

include_directories(../zlib-1.2.6)
include_directories(.)
add_library(lpng STATIC ${lpng_SOURCES})
