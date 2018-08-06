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

XRootDStatus HttpFileSystemPlugIn::Truncate(const std::string &path,
                                            uint64_t size,
                                            ResponseHandler *handler,
                                            uint16_t timeout) {
  (void)path;
  (void)size;
  (void)handler;
  (void)timeout;

  logger_->Error(kLogXrdClHttp, "Truncate not supported with HTTP");

  return XRootDStatus(stError, errNotSupported);
}

XRootDStatus HttpFileSystemPlugIn::ChMod(const std::string &path,
                                         Access::Mode mode,
                                         ResponseHandler *handler,
                                         uint16_t timeout) {
  (void)path;
  (void)mode;
  (void)handler;
  (void)timeout;

  logger_->Error(kLogXrdClHttp, "ChMod not supported with HTTP");

  return XRootDStatus(stError, errNotSupported);
}

XRootDStatus HttpFileSystemPlugIn::Stat(const std::string &path,
                                        ResponseHandler *handler,
                                        uint16_t timeout) {
  logger_->Debug(kLogXrdClHttp,
                 "HttpFileSystemPlugIn::Stat - path = %s, timeout = %d",
                 path.c_str(), timeout);

  Davix::Context ctx;
  Davix::DavPosix davix_client(&ctx);

  auto stat_info = new StatInfo();
  auto status =
      Posix::Stat(davix_client, url_.GetLocation(), timeout, stat_info);

  if (status.IsError()) {
    logger_->Error(kLogXrdClHttp, "Stat failed: %s", status.ToStr().c_str());
  }

  auto obj = new AnyObject();
  obj->Set(stat_info);

  handler->HandleResponse(new XRootDStatus(), obj);

  return XRootDStatus();
}

bool HttpFileSystemPlugIn::SetProperty(const std::string &name,
                                       const std::string &value) {
  properties_[name] = value;
  return true;
}

bool HttpFileSystemPlugIn::GetProperty(const std::string &name,
                                       std::string &value) const {
  const auto p = properties_.find(name);
  if (p == std::end(properties_)) {
    return false;
  }

  value = p->second;
  return true;
}

}  // namespace XrdCl
