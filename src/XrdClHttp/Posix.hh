/**
 * This file is part of XrdClHttp
 */

#ifndef __HTTP_STAT_
#define __HTTP_STAT_

#include "XrdCl/XrdClFileSystem.hh"
#include "XrdCl/XrdClXRootDResponses.hh"

#include <cstdint>
#include <string>

namespace Davix {

class DavPosix;
}

namespace XrdCl {

class StatInfo;

namespace Posix {

XRootDStatus MkDir(Davix::DavPosix& davix_client, const std::string& path,
                   MkDirFlags::Flags flags, Access::Mode mode,
                   uint16_t timeout);

XRootDStatus Rename(Davix::DavPosix& davix_client, const std::string& source,
                    const std::string& dest, uint16_t timeout);

XRootDStatus Stat(Davix::DavPosix& davix_client, const std::string& url,
                  uint16_t timeout, StatInfo* stat_info);

XRootDStatus Unlink(Davix::DavPosix& davix_client, const std::string& url,
                    uint16_t timeout);

}  // namespace Posix

}  // namespace XrdCl

#endif  // __HTTP_STAT_
