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

namespace {

template <typename Container>
Container SplitString(const std::string &input,
                      const std::string &delimiter) {
  size_t start = 0;
  size_t end = 0;
  size_t length = 0;

  auto result = Container{};

  do {
    end = input.find(delimiter, start);

    if (end != std::string::npos)
      length = end - start;
    else
      length = input.length() - start;

    if (length) result.push_back(input.substr(start, length));

    start = end + delimiter.size();
  } while (end != std::string::npos);

  return result;
}

}  // namespace

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
  logger_->Debug(
      kLogXrdClHttp,
      "HttpFileSystemPlugIn::MkDir - path = %s, flags = %d, timeout = %d",
      path.c_str(), flags, timeout);

  if (flags & MkDirFlags::MakePath) {
    auto dirs = SplitString<std::vector<std::string>>(path, "/");
    for (const auto &d : dirs) {
      logger_->Debug(kLogXrdClHttp, "DIR: %s", d.c_str());
    }
  }

  handler->HandleResponse(new XRootDStatus(), nullptr);
  return XRootDStatus();
}

XRootDStatus HttpFileSystemPlugIn::RmDir(const std::string &path,
                                         ResponseHandler *handler,
                                         uint16_t timeout) {
  handler->HandleResponse(new XRootDStatus(), nullptr);
  return XRootDStatus();
}

XRootDStatus HttpFileSystemPlugIn::DirList(const std::string &path,
                                           DirListFlags::Flags flags,
                                           ResponseHandler *handler,
                                           uint16_t timeout) {
  // Handle the cases of stat calls for each item and that of
  // recursive listing
  handler->HandleResponse(new XRootDStatus(), nullptr);
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
  auto status = Posix::Stat(davix_client_, full_path, timeout, stat_info);

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
