cmake_minimum_required (VERSION 2.8)

project(XrdClHttp)

list(APPEND CMAKE_MODULE_PATH
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/cmake)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

# Defaults
if( NOT XRDCLHTTP_SUBMODULE )
  if( NOT DEFINED PLUGIN_VERSION )
    set(PLUGIN_VERSION 5)
  endif()
endif()

# Options
option(BUILD_TESTS "Enable unit tests" OFF)

# Dependencies
find_package(Davix REQUIRED)
include_directories(${Davix_INCLUDE_DIRS}/davix)

find_package(XrdCl REQUIRED)
include_directories(${XrdCl_INCLUDE_DIRS}/xrootd)

add_subdirectory(src)

# if(BUILD_TESTS)
#   ENABLE_TESTING()
#   add_subdirectory(tests)
# endif()
