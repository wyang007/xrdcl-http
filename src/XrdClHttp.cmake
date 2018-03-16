set(lib${PROJECT_NAME_LOWER}_sources
    XrdClHttp/XrdClHttp.cc)

add_library(${PROJECT_NAME_LOWER} MODULE ${lib${PROJECT_NAME_LOWER}_sources})

target_link_libraries(${PROJECT_NAME_LOWER} ${Davix_LIBRARIES} ${XrdCl_LIBRARIES})
