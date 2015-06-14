
if (BUILD_CV1)
  add_subdirectory_if_exists(task1a)
  add_subdirectory_if_exists(task1b)
endif (BUILD_CV1)

if (BUILD_CV2)
  add_subdirectory_if_exists(task2)
endif (BUILD_CV2)
