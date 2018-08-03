/**
 * This file is part of XrdClHttp
 */

#ifndef __HTTP_STAT_
#define __HTTP_STAT_

#include "XrdCl/XrdClXRootDResponses.hh"

#include <cstdint>
#include <string>

namespace Davix {

class DavPosix;
}

namespace XrdCl {

class StatInfo;

namespace Posix {

XRootDStatus Stat(Davix::DavPosix& davix_client, const std::string& url,
                  uint16_t timeout, StatInfo* stat_info);

}  // namespace Posix

}  // namespace XrdCl

#endif  // __HTTP_STAT_
