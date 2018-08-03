/**
 * This file is part of XrdClHttp
 */

#include "HttpFileSystemPlugIn.hh"

#include <mutex>

#include "XrdCl/XrdClDefaultEnv.hh"
#include "XrdCl/XrdClLog.hh"
#include "XrdCl/XrdClXRootDResponses.hh"

#include "davix.hpp"

#include "HttpFilePlugIn.hh"
#include "HttpPlugInUtil.hh"
#include "Posix.hh"

namespace XrdCl {

HttpFileSystemPlugIn::HttpFileSystemPlugIn(const std::string &url)
    : url_(url), logger_(DefaultEnv::GetLog()) {
  SetUpLogging(logger_);
  logger_->Debug(kLogXrdClHttp,
                 "HttpFileSystemPlugIn constructed with URL: %s.",
                 url_.GetURL().c_str());
}

XRootDStatus HttpFileSystemPlugIn::Stat(const std::string& path,
                                        ResponseHandler *handler,
                                        uint16_t timeout) {
  logger_->Debug(kLogXrdClHttp,
                 "HttpFileSystemPlugIn::Stat - path = %s, timeout = %d",
                 path.c_str(), timeout);

  Davix::Context ctx;
  Davix::DavPosix davix_client(&ctx);

  auto stat_info = new StatInfo();
  auto status = Posix::Stat(davix_client, url_.GetLocation(), timeout, stat_info);

  if (status.IsError()) {
    logger_->Error(kLogXrdClHttp, "Stat failed: %s", status.ToStr().c_str());
  }

  auto obj = new AnyObject();
  obj->Set(stat_info);

  handler->HandleResponse(new XRootDStatus(), obj);

  return XRootDStatus();
}

}  // namespace XrdCl
