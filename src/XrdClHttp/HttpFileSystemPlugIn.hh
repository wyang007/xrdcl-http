/**
 * This file is part of XrdClHttp
 */

#ifndef __HTTP_FILE_SYSTEM_PLUG_IN_
#define __HTTP_FILE_SYSTEM_PLUG_IN_

#include "XrdCl/XrdClPlugInInterface.hh"

namespace XrdCl {
  class Log;

  class HttpFileSystemPlugIn : public FileSystemPlugIn {
    public:
      HttpFileSystemPlugIn();
      virtual ~HttpFileSystemPlugIn() = default;

      virtual XRootDStatus Stat(const std::string &path,
                                ResponseHandler *handler,
                                uint16_t timeout) override;

    private:
        Log* logger_;
  };
}

#endif // __HTTP_FILE_SYSTEM_PLUG_IN_
