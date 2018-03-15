set(lib${PROJECT_NAME_LOWER}_sources dummy.cc)

add_library(${PROJECT_NAME_LOWER} ${lib${PROJECT_NAME_LOWER}_sources})

target_link_libraries(${PROJECT_NAME_LOWER} Davix_LIBRARIES XrdCl_LIBRARIES)
