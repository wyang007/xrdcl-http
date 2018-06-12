/**
 * This file is part of XrdClHttp
 */

#include "HttpFileSystemPlugIn.hh"

#include <mutex>

#include "HttpFilePlugIn.hh"
#include "HttpPlugInUtil.hh"
#include "XrdCl/XrdClDefaultEnv.hh"
#include "XrdCl/XrdClLog.hh"
#include "xrootd/XrdCl/XrdClXRootDResponses.hh"

namespace XrdCl {

HttpFileSystemPlugIn::HttpFileSystemPlugIn() : logger_(DefaultEnv::GetLog()) {
  SetUpLogging(logger_);
  logger_->Debug(kLogXrdClHttp, "HttpFileSystemPlugIn constructed.");
}

XRootDStatus HttpFileSystemPlugIn::Stat(const std::string &path,
                                        ResponseHandler *handler,
                                        uint16_t timeout) {
  logger_->Debug(kLogXrdClHttp, "HttpFileSystemPlugIn::Stat - path = %s, timeout = %d",
                 path.c_str(), timeout);
  File file(true);
  auto status1 = file.Open(path, OpenFlags::Read, Access::UR, timeout);

  auto stat_info = new StatInfo();
  file.Stat(false, stat_info, timeout);

  ResponseHandler close_handler;
  file.Close(&close_handler, timeout);

  auto obj = new AnyObject();
  obj->Set(stat_info);
  auto status2 = new XRootDStatus();
  handler->HandleResponse(status2, obj);

  return XRootDStatus();
}

}  // namespace XrdCl
