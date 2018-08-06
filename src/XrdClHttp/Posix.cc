/**
 * This file is part of XrdClHttp
 */

#include "Posix.hh"

#include "XrdCl/XrdClStatus.hh"

#include <davix.hpp>

namespace {

void SetTimeout(Davix::RequestParams& params, uint16_t timeout) {
  if (timeout != 0) {
    struct timespec ts = {timeout, 0};
    params.setOperationTimeout(&ts);
  }
}

}  // namespace

namespace XrdCl {

namespace Posix {

XRootDStatus Rename(Davix::DavPosix& davix_client, const std::string& source,
                    const std::string& dest, uint16_t timeout) {
  Davix::RequestParams params;
  SetTimeout(params, timeout);

  Davix::DavixError* err = nullptr;
  if (davix_client.rename(&params, source, dest, &err)) {
    auto errStatus = XRootDStatus(stError, errInternal, err->getStatus(), err->getErrMsg());
    delete err;
    return errStatus;
  }

  return XRootDStatus();
}

XRootDStatus Stat(Davix::DavPosix& davix_client, const std::string& url,
                  uint16_t timeout, StatInfo* stat_info) {
  Davix::RequestParams params;
  SetTimeout(params, timeout);

  struct stat stats;
  Davix::DavixError* err = nullptr;
  if (davix_client.stat(&params, url, &stats, &err)) {
    auto errStatus = XRootDStatus(stError, errInternal, err->getStatus(), err->getErrMsg());
    delete err;
    return errStatus;
  }

  std::ostringstream data;
  data << stats.st_dev << " " << stats.st_size << " " << stats.st_mode << " "
       << stats.st_mtime;

  if (!stat_info->ParseServerResponse(data.str().c_str())) {
    return XRootDStatus(stError, errDataError);
  }

  return XRootDStatus();
}

XRootDStatus Unlink(Davix::DavPosix& davix_client, const std::string& url,
                    uint16_t timeout) {
  Davix::RequestParams params;
  SetTimeout(params, timeout);

  Davix::DavixError* err = nullptr;
  if (davix_client.unlink(&params, url, &err)) {
    auto errStatus = XRootDStatus(stError, errInternal, err->getStatus(), err->getErrMsg());
    delete err;
    return errStatus;
  }

  return XRootDStatus();
}

}  // namespace Posix

}  // namespace XrdCl
