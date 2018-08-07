/**
 * This file is part of XrdClHttp
 */

#include "Posix.hh"

#include "XrdCl/XrdClStatus.hh"
#include "XrdCl/XrdClURL.hh"

#include <davix.hpp>

#include <string>

namespace {

std::vector<std::string> SplitString(const std::string& input,
                                     const std::string& delimiter) {
  size_t start = 0;
  size_t end = 0;
  size_t length = 0;

  auto result = std::vector<std::string>{};

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

void SetTimeout(Davix::RequestParams& params, uint16_t timeout) {
  if (timeout != 0) {
    struct timespec ts = {timeout, 0};
    params.setOperationTimeout(&ts);
  }
}

}  // namespace

namespace XrdCl {

namespace Posix {

XRootDStatus MkDir(Davix::DavPosix& davix_client, const std::string& path,
                   XrdCl::MkDirFlags::Flags flags, XrdCl::Access::Mode /*mode*/,
                   uint16_t timeout) {
  Davix::RequestParams params;
  SetTimeout(params, timeout);

  auto url = XrdCl::URL(path);

  if (flags & XrdCl::MkDirFlags::MakePath) {
    // Also create intermediate directories

    auto dirs = SplitString(url.GetPath(), "/");

    std::string dirs_cumul;
    for (const auto& d : dirs) {
      dirs_cumul += "/" + d;
      url.SetPath(dirs_cumul);
      Davix::DavixError* err = nullptr;
      if (davix_client.mkdir(&params, url.GetLocation(), S_IRWXU, &err)) {
        if (err->getStatus() != Davix::StatusCode::FileExist) {
          auto errStatus = XRootDStatus(stError, errInternal, err->getStatus(),
                                        err->getErrMsg());
          delete err;
          return errStatus;
        }
      }
    }
  } else {
    // Only create final directory
    Davix::DavixError* err = nullptr;
    if (davix_client.mkdir(&params, url.GetURL(), S_IRWXU, &err)) {
      if (err->getStatus() != Davix::StatusCode::FileExist) {
        auto errStatus = XRootDStatus(stError, errInternal, err->getStatus(),
                                      err->getErrMsg());
        delete err;
        return errStatus;
      }
    }
  }

  return XRootDStatus();
}

XRootDStatus RmDir(Davix::DavPosix& davix_client, const std::string& path,
                   uint16_t timeout) {
  Davix::RequestParams params;
  SetTimeout(params, timeout);

  auto url = XrdCl::URL(path);
  Davix::DavixError* err = nullptr;
  if (davix_client.rmdir(&params, url.GetLocation(), &err)) {
    auto errStatus = XRootDStatus(stError, errInternal, err->getStatus(),
                                  err->getErrMsg());
    delete err;
    return errStatus;
  }

  return XRootDStatus();
}

XRootDStatus Rename(Davix::DavPosix& davix_client, const std::string& source,
                    const std::string& dest, uint16_t timeout) {
  Davix::RequestParams params;
  SetTimeout(params, timeout);

  Davix::DavixError* err = nullptr;
  if (davix_client.rename(&params, source, dest, &err)) {
    auto errStatus =
        XRootDStatus(stError, errInternal, err->getStatus(), err->getErrMsg());
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
    auto errStatus =
        XRootDStatus(stError, errInternal, err->getStatus(), err->getErrMsg());
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
    auto errStatus =
        XRootDStatus(stError, errInternal, err->getStatus(), err->getErrMsg());
    delete err;
    return errStatus;
  }

  return XRootDStatus();
}

}  // namespace Posix

}  // namespace XrdCl
