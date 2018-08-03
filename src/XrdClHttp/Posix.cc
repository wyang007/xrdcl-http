/**
 * This file is part of XrdClHttp
 */

#include "Posix.hh"

#include "XrdCl/XrdClStatus.hh"

#include <davix.hpp>

namespace XrdCl {

namespace Posix {

XRootDStatus Stat(Davix::DavPosix& davix_client, const std::string& url,
                      uint16_t timeout, StatInfo* stat_info) {
  Davix::RequestParams params;
  if (timeout != 0) {
    struct timespec ts = {timeout, 0};
    params.setOperationTimeout(&ts);
  }

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

}  // namespace Posix

}  // namespace XrdCl
