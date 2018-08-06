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

XRootDStatus Stat(Davix::DavPosix& davix_client, Davix::RequestParams params,
                  const std::string& url, uint16_t timeout,
                  StatInfo* stat_info) {
  SetTimeout(params, timeout);

  struct stat stats;
  Davix::DavixError* err = nullptr;
  if (davix_client.stat(&params, url, &stats, &err)) {
    delete err;
    return XRootDStatus(stError, errUnknown);
  }

  std::ostringstream data;
  data << stats.st_dev << " " << stats.st_size << " " << stats.st_mode << " "
       << stats.st_mtime;

  if (!stat_info->ParseServerResponse(data.str().c_str())) {
    return XRootDStatus(stError, errDataError);
  }

  return XRootDStatus();
}

XRootDStatus Unlink(Davix::DavPosix& davix_client, Davix::RequestParams params,
                    const std::string& url, uint16_t timeout) {
  SetTimeout(params, timeout);

  Davix::DavixError* err = nullptr;
  if (davix_client.unlink(&params, url, &err)) {
    delete err;
    return XRootDStatus(stError, errUnknown);
  }

  return XRootDStatus();
}

}  // namespace Posix

}  // namespace XrdCl
