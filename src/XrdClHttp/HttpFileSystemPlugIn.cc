/**
 * This file is part of XrdClHttp
 */

#include "HttpFileSystemPlugIn.hh"

#include <mutex>

#include "XrdCl/XrdClDefaultEnv.hh"
#include "XrdCl/XrdClLog.hh"
#include "XrdCl/XrdClXRootDResponses.hh"

#include "HttpFilePlugIn.hh"
#include "HttpPlugInUtil.hh"
#include "Posix.hh"

namespace XrdCl {

HttpFileSystemPlugIn::HttpFileSystemPlugIn(const std::string &url)
    : ctx_(), davix_client_(&ctx_), url_(url), logger_(DefaultEnv::GetLog()) {
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

XRootDStatus HttpFileSystemPlugIn::Rm(const std::string &path,
                                      ResponseHandler *handler,
                                      uint16_t timeout) {
  const auto full_path = url_.GetLocation() + path;

  logger_->Debug(kLogXrdClHttp,
                 "HttpFileSystemPlugIn::Rm - path = %s, timeout = %d",
                 full_path.c_str(), timeout);

  auto status = Posix::Unlink(davix_client_, full_path, timeout);

  if (status.IsError()) {
    logger_->Error(kLogXrdClHttp, "Rm failed: %s", status.ToStr().c_str());
  }

  handler->HandleResponse(new XRootDStatus(status), nullptr);

  return XRootDStatus();
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
  const auto full_path = url_.GetLocation() + path;

  logger_->Debug(kLogXrdClHttp,
                 "HttpFileSystemPlugIn::Stat - path = %s, timeout = %d",
                 full_path.c_str(), timeout);

  auto stat_info = new StatInfo();
  auto status =
      Posix::Stat(davix_client_, full_path, timeout, stat_info);

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
