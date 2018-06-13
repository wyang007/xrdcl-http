/**
 * This file is part of XrdClHttp
 */

#ifndef __HTTP_FILE_SYSTEM_PLUG_IN_
#define __HTTP_FILE_SYSTEM_PLUG_IN_

#include "XrdCl/XrdClPlugInInterface.hh"

#include "XrdCl/XrdClURL.hh"

namespace XrdCl {
  class Log;

  class HttpFileSystemPlugIn : public FileSystemPlugIn {
    public:
      HttpFileSystemPlugIn(const std::string& url);
      virtual ~HttpFileSystemPlugIn() = default;

      virtual XRootDStatus Stat(const std::string &path,
                                ResponseHandler *handler,
                                uint16_t timeout) override;

    private:
      URL url_;
      Log* logger_;
  };
}

#endif // __HTTP_FILE_SYSTEM_PLUG_IN_
