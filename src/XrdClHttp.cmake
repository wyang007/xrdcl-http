set(lib${PROJECT_NAME}_sources
  XrdClHttp/HttpPlugInFactory.cc
  XrdClHttp/HttpPlugInUtil.cc
  XrdClHttp/HttpFilePlugIn.cc
  XrdClHttp/HttpFileSystemPlugIn.cc
  XrdClHttp/HttpStat.cc)

set(PLUGIN_NAME "${PROJECT_NAME}-${PLUGIN_VERSION}")

add_library(${PLUGIN_NAME} MODULE ${lib${PROJECT_NAME}_sources})

target_link_libraries(${PLUGIN_NAME} ${Davix_LIBRARIES} ${XrdCl_LIBRARIES})

install(TARGETS ${PLUGIN_NAME} LIBRARY DESTINATION lib)
