cmake_minimum_required (VERSION 2.8)

project(XrdCl-HTTP)
string(TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWER)

list(APPEND CMAKE_MODULE_PATH
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/cmake)

# Options
option(BUILD_TESTS "Enable unit tests" OFF)

# Dependencies
find_package(Davix REQUIRED)
include_directories(${Davix_INCLUDE_DIRS})

find_package(XrdCl REQUIRED)
include_directories(${XrdCl_INCLUDE_DIRS}/xrootd)

add_subdirectory(src)

if(BUILD_TESTS)
  ENABLE_TESTING()
  add_subdirectory(tests)
endif()
