find_path(
    XrdCl_INCLUDE_DIRS
    NAMES xrootd/XrdCl/XrdClPlugInInterface.hh
    HINTS ${XrdCl_INCLUDE_DIRS}
)

find_library(
    XrdCl_LIBRARIES
    NAMES XrdCl
    HINTS ${XrdCl_LIBRARY_DIRS}
)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(
    XrdCl
    DEFAULT_MSG
    XrdCl_LIBRARIES
    XrdCl_INCLUDE_DIRS
)

if(XrdCl_FOUND)
    mark_as_advanced(XrdCl_LIBRARIES XrdCl_INCLUDE_DIRS)
endif()
