/**
 * This file is part of XrdClHttp
 */

#include "HttpPlugInFactory.hh"

#include "XrdVersion.hh"

#include "HttpFilePlugIn.hh"
#include "HttpFileSystemPlugIn.hh"

XrdVERSIONINFO(XrdClGetPlugIn, XrdClGetPlugIn)

extern "C"
{
  void *XrdClGetPlugIn( const void* /*arg*/ )
  {
    return static_cast<void*>( new HttpPlugInFactory());
  }
}

HttpPlugInFactory::~HttpPlugInFactory() {
}

XrdCl::FilePlugIn* HttpPlugInFactory::CreateFile( const std::string &/*url*/ ) {
  return new XrdCl::HttpFilePlugIn();
}

XrdCl::FileSystemPlugIn* HttpPlugInFactory::CreateFileSystem( const std::string& url ) {
  return new XrdCl::HttpFileSystemPlugIn(url);
}
