
add_subdirectory_if_exists(common)

if (BUILD_CG1)
  add_subdirectory_if_exists(task1a)
  add_subdirectory_if_exists(task1b)
endif (BUILD_CG1)

if (BUILD_CG2)
  add_subdirectory_if_exists(task2)
endif (BUILD_CG2)
