/**
 * This file is part of XrdCl-HTTP
 */

#ifndef __XRD_CL_HTTP__
#define __XRD_CL_HTTP__

#include <XrdCl/XrdClPlugInInterface.hh>
#include "XrdVersion.hh"

namespace XrdCl {

  class HttpFilePlugIn : public FilePlugIn {
    virtual ~HttpFilePlugIn();
  };

  class HttpFileSystemPlugIn : public FileSystemPlugIn {
    virtual ~HttpFileSystemPlugIn();
  };

  class HttpPlugInFactory : public PlugInFactory {
    virtual ~HttpPlugInFactory();
  };

}

#endif // __XRD_CL_HTTP__
