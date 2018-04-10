/**
 * This file is part of XrdClHttp
 */

#ifndef __HTTP_FILE_PLUG_IN_
#define __HTTP_FILE_PLUG_IN_

#include <limits>
#include <cstdint>

#include <XrdCl/XrdClPlugInInterface.hh>
#include <XrdCl/XrdClDefaultEnv.hh>
#include <XrdCl/XrdClLog.hh>

class HttpFilePlugIn : public XrdCl::FilePlugIn {
 public:
  HttpFilePlugIn();
  virtual ~HttpFilePlugIn();

 private:
  // Topic id for the logger
  static const uint64_t LogXrdClHttp = std::numeric_limits<std::uint64_t>::max();

  XrdCl::Log* pLogger;
};

#endif // __HTTP_FILE_PLUG_IN_
