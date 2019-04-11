set(lib${PROJECT_NAME}_sources
  XrdClHttp/HttpPlugInFactory.cc
  XrdClHttp/HttpPlugInUtil.cc
  XrdClHttp/HttpFilePlugIn.cc
  XrdClHttp/HttpFileSystemPlugIn.cc
  XrdClHttp/Posix.cc)

set(PLUGIN_NAME "${PROJECT_NAME}-${PLUGIN_VERSION}")

add_library(${PLUGIN_NAME} MODULE ${lib${PROJECT_NAME}_sources})

target_link_libraries(${PLUGIN_NAME} ${Davix_LIBRARIES} ${XrdCl_LIBRARIES})

set(LIBDIR_DEFAULT "lib")
# Override this default 'lib' with 'lib64' iff:
#  - we are on Linux system but NOT cross-compiling
#  - we are NOT on debian
#  - we are on a 64 bits system
# reason is: amd64 ABI: http://www.x86-64.org/documentation/abi.pdf
# Note that the future of multi-arch handling may be even
# more complicated than that: http://wiki.debian.org/Multiarch
if(CMAKE_SYSTEM_NAME MATCHES "Linux"
    AND NOT CMAKE_CROSSCOMPILING
    AND NOT EXISTS "/etc/debian_version")
  if(NOT DEFINED CMAKE_SIZEOF_VOID_P)
    message(AUTHOR_WARNING
      "Unable to determine default CMAKE_INSTALL_LIBDIR directory because no target architecture is known. "
      "Please enable at least one language before including GNUInstallDirs.")
  else()
    if("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
      set(LIBDIR_DEFAULT "lib64")
    endif()
  endif()
endif()
message( "LIBDIR_DEFAULT : ${LIBDIR_DEFAULT}" )

install(TARGETS ${PLUGIN_NAME} LIBRARY DESTINATION ${LIBDIR_DEFAULT})
