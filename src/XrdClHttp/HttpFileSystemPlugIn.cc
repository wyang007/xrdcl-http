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

XRootDStatus HttpFileSystemPlugIn::Mv(const std::string &source,
                                      const std::string &dest,
                                      ResponseHandler *handler,
                                      uint16_t timeout) {
  const auto full_source_path = url_.GetLocation() + source;
  const auto full_dest_path = url_.GetLocation() + dest;

  logger_->Debug(kLogXrdClHttp,
                 "HttpFileSystemPlugIn::Mv - src = %s, dest = %s, timeout = %d",
                 full_source_path.c_str(), full_dest_path.c_str(), timeout);

  auto status =
      Posix::Rename(davix_client_, full_source_path, full_dest_path, timeout);

  if (status.IsError()) {
    logger_->Error(kLogXrdClHttp, "Mv failed: %s", status.ToStr().c_str());
    return status;
  }

  handler->HandleResponse(new XRootDStatus(status), nullptr);

  return XRootDStatus();
}

XRootDStatus HttpFileSystemPlugIn::Rm(const std::string &path,
                                      ResponseHandler *handler,
                                      uint16_t timeout) {
  auto url = url_;
  url.SetPath(path);

  logger_->Debug(kLogXrdClHttp,
                 "HttpFileSystemPlugIn::Rm - path = %s, timeout = %d",
                 url.GetURL().c_str(), timeout);

  auto status = Posix::Unlink(davix_client_, url.GetURL(), timeout);

  if (status.IsError()) {
    logger_->Error(kLogXrdClHttp, "Rm failed: %s", status.ToStr().c_str());
    return status;
  }

  handler->HandleResponse(new XRootDStatus(status), nullptr);

  return XRootDStatus();
}

XRootDStatus HttpFileSystemPlugIn::MkDir(const std::string &path,
                                         MkDirFlags::Flags flags,
                                         Access::Mode mode,
                                         ResponseHandler *handler,
                                         uint16_t timeout) {
  auto url = url_;
  url.SetPath(path);

  logger_->Debug(
      kLogXrdClHttp,
      "HttpFileSystemPlugIn::MkDir - path = %s, flags = %d, timeout = %d",
      url.GetURL().c_str(), flags, timeout);

  auto status = Posix::MkDir(davix_client_, url.GetURL(), flags, mode, timeout);
  if (status.IsError()) {
    logger_->Error(kLogXrdClHttp, "MkDir failed: %s", status.ToStr().c_str());
    return status;
  }

  handler->HandleResponse(new XRootDStatus(status), nullptr);

  return XRootDStatus();
}

XRootDStatus HttpFileSystemPlugIn::RmDir(const std::string &path,
                                         ResponseHandler *handler,
                                         uint16_t timeout) {
  auto url = url_;
  url.SetPath(path);

  logger_->Debug(kLogXrdClHttp,
                 "HttpFileSystemPlugIn::RmDir - path = %s, timeout = %d",
                 url.GetURL().c_str(), timeout);

  auto status = Posix::RmDir(davix_client_, url.GetURL(), timeout);
  if (status.IsError()) {
    logger_->Error(kLogXrdClHttp, "RmDir failed: %s", status.ToStr().c_str());
    return status;
  }

  handler->HandleResponse(new XRootDStatus(status), nullptr);
  return XRootDStatus();
}

XRootDStatus HttpFileSystemPlugIn::DirList(const std::string &path,
                                           DirListFlags::Flags flags,
                                           ResponseHandler *handler,
                                           uint16_t timeout) {
  auto url = url_;
  url.SetPath(path);
  const auto full_path = url.GetLocation();

  logger_->Debug(
      kLogXrdClHttp,
      "HttpFileSystemPlugIn::DirList - path = %s, flags = %d, timeout = %d",
      full_path.c_str(), flags, timeout);

  const bool details = flags & DirListFlags::Stat;
  const bool recursive = flags & DirListFlags::Recursive;

  // res == std::pair<DirectoryList*, XRootDStatus>
  auto res =
      Posix::DirList(davix_client_, full_path, details, recursive, timeout);
  if (res.second.IsError()) {
    logger_->Error(kLogXrdClHttp, "Could not list dir: %s, error: %s",
                   full_path.c_str(), res.second.ToStr().c_str());
    return res.second;
  }

  auto obj = new AnyObject();
  obj->Set(res.first);

  handler->HandleResponse(new XRootDStatus(), obj);
  return XRootDStatus();
}

XRootDStatus HttpFileSystemPlugIn::Stat(const std::string &path,
                                        ResponseHandler *handler,
                                        uint16_t timeout) {
  //const auto full_path = url_.GetLocation() + path;
  const auto full_path = url_.GetProtocol() + "://" +
                         url_.GetHostName() + ":" +
                         std::to_string(url_.GetPort()) + "/" + path;

  logger_->Debug(kLogXrdClHttp,
                 "HttpFileSystemPlugIn::Stat - path = %s, timeout = %d",
                 full_path.c_str(), timeout);

  auto stat_info = new StatInfo();
  XRootDStatus status;
  //auto status = Posix::Stat(davix_client_, full_path, timeout, stat_info);

  if (status.IsError()) {
    logger_->Error(kLogXrdClHttp, "Stat failed: %s", status.ToStr().c_str());
    return status;
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
