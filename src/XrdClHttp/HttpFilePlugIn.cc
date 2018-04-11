/**
 * This file is part of XrdClHttp
 */

#include "HttpFilePlugIn.hh"

#include <cassert>

#include <mutex>

#include "XrdCl/XrdClStatus.hh"

static std::once_flag logging_topic_init;

namespace XrdCl {

HttpFilePlugIn::HttpFilePlugIn()
    : davix_context_(),
      davix_client_(&davix_context_),
      davix_fd_(nullptr),
      is_open_(false),
      url_(),
      properties_(),
      logger_(DefaultEnv::GetLog())
{
  // Assert that there is no existing topic
  std::call_once(logging_topic_init, [this] {
      if (logger_) {
        logger_->SetTopicName(kLogXrdClHttp, "XrdClHttp");
      }
    });

  logger_->Debug(kLogXrdClHttp, "HttpFilePlugin constructed.");
}

HttpFilePlugIn::~HttpFilePlugIn() {}

XRootDStatus HttpFilePlugIn::Open( const std::string &url,
                                   OpenFlags::Flags   flags,
                                   Access::Mode       /*mode*/,
                                   ResponseHandler   *handler,
                                   uint16_t           timeout )
{
  if (is_open_) {
    logger_->Error(kLogXrdClHttp, "Error: URL %s already open", url.c_str());
    return XRootDStatus(stError, errInvalidOp);
  }

  Davix::RequestParams params;
  if (timeout != 0) {
    struct timespec ts = {timeout, 0};
    params.setOperationTimeout(&ts);
  }

  Davix::DavixError* err = nullptr;
  davix_fd_ = davix_client_.open(&params, url, flags, &err);
  if (!davix_fd_) {
    logger_->Error(kLogXrdClHttp, "Could not open: %s, error: %s",
                   url.c_str(), err->getErrMsg().c_str());
    return XRootDStatus(stError, errUnknown);
  }

  logger_->Debug(kLogXrdClHttp, "Opened: %s", url.c_str());

  is_open_ = true;
  url_ = url;

  auto status = new XRootDStatus();
  handler->HandleResponse(status, nullptr);

  return XRootDStatus();
}

XRootDStatus HttpFilePlugIn::Close( ResponseHandler *handler,
                                    uint16_t         /*timeout*/ )
{
  if (! is_open_) {
    logger_->Error(kLogXrdClHttp, "Error: Cannot close. URL hasn't been previously opened",
                   url_.c_str());
    return XRootDStatus(stError, errInvalidOp);
  }

  Davix::DavixError* err = nullptr;

  if (davix_client_.close(davix_fd_, &err)) {
    logger_->Error(kLogXrdClHttp, "Could not close davix fd: %ld, error: %s",
                   davix_fd_, err->getErrMsg().c_str());
    return XRootDStatus(stError, errUnknown);
  }

  logger_->Debug(kLogXrdClHttp, "Closed davix fd: %ld", davix_fd_);

  is_open_ = false;
  url_.clear();

  auto status = new XRootDStatus();
  handler->HandleResponse(status, nullptr);

  return XRootDStatus();
}

XRootDStatus HttpFilePlugIn::Stat( bool             /*force*/,
                                   ResponseHandler *handler,
                                   uint16_t         timeout )
{
  if (! is_open_) {
    logger_->Error(kLogXrdClHttp, "Error: Cannot stat. URL hasn't been previously opened",
                   url_.c_str());
    return XRootDStatus(stError, errInvalidOp);
  }

  Davix::RequestParams params;
  if (timeout != 0) {
    struct timespec ts = {timeout, 0};
    params.setOperationTimeout(&ts);
  }

  struct stat stats;
  Davix::DavixError* err = nullptr;
  if (davix_client_.stat(&params, url_, &stats, &err)) {
    logger_->Error(kLogXrdClHttp, "Could not stat URL: %s, error: %s",
                   url_.c_str(), err->getErrMsg().c_str());
    return XRootDStatus(stError, errUnknown);
  }

  logger_->Debug(kLogXrdClHttp, "Stat-ed URL: %s", url_.c_str());

  std::ostringstream data;
  data << stats.st_dev << " " << stats.st_size << " "
       << stats.st_mode << " " << stats.st_mtime;

  auto stat_info = new StatInfo();
  if (!stat_info->ParseServerResponse(data.str().c_str())) {
    logger_->Error(kLogXrdClHttp, "Could not parse stat reply for URL: %s",
                   url_.c_str());
    return XRootDStatus(stError, errDataError);
  }

  auto obj = new AnyObject();
  obj->Set(stat_info);

  auto status = new XRootDStatus();
  handler->HandleResponse( status, obj );

  return XRootDStatus();
}

XRootDStatus HttpFilePlugIn::Read( uint64_t         offset,
                                   uint32_t         size,
                                   void            *buffer,
                                   ResponseHandler *handler,
                                   uint16_t         timeout )
{
  (void)offset; (void)size; (void)buffer; (void)handler; (void)timeout;

  logger_->Debug(kLogXrdClHttp, "Read not implemented.");

  return XRootDStatus( stError, errNotImplemented );
}

XRootDStatus HttpFilePlugIn::Write( uint64_t         offset,
                                    uint32_t         size,
                                    const void      *buffer,
                                    ResponseHandler *handler,
                                    uint16_t         timeout )
{
  (void)offset; (void)size; (void)buffer; (void)handler; (void)timeout;

  logger_->Debug(kLogXrdClHttp, "Write not implemented.");

  return XRootDStatus( stError, errNotImplemented );
}

XRootDStatus HttpFilePlugIn::Sync( ResponseHandler *handler,
                                   uint16_t         timeout )
{
  (void)handler; (void)timeout;

  logger_->Debug(kLogXrdClHttp, "Sync not implemented.");

  return XRootDStatus( stError, errNotImplemented );
}

XRootDStatus HttpFilePlugIn::Truncate( uint64_t         size,
                                       ResponseHandler *handler,
                                       uint16_t         timeout )
{
  (void)size; (void)handler; (void)timeout;

  logger_->Debug(kLogXrdClHttp, "Truncate not implemented.");

  return XRootDStatus( stError, errNotImplemented );
}

XRootDStatus HttpFilePlugIn::VectorRead( const ChunkList &chunks,
                                         void            *buffer,
                                         ResponseHandler *handler,
                                         uint16_t         timeout )
{
  (void)chunks; (void)buffer; (void)handler; (void)timeout;

  logger_->Debug(kLogXrdClHttp, "VectorRead not implemented.");

  return XRootDStatus( stError, errNotImplemented );
}

XRootDStatus HttpFilePlugIn::Fcntl( const Buffer    &arg,
                                    ResponseHandler *handler,
                                    uint16_t         timeout )
{
  (void)arg; (void)handler; (void)timeout;

  logger_->Debug(kLogXrdClHttp, "Fcntl not implemented.");

  return XRootDStatus( stError, errNotImplemented );
}

XRootDStatus HttpFilePlugIn::Visa( ResponseHandler *handler,
                                   uint16_t         timeout )
{
  (void)handler; (void)timeout;

  logger_->Debug(kLogXrdClHttp, "Visa not implemented.");

  return XRootDStatus( stError, errNotImplemented );
}

bool HttpFilePlugIn::IsOpen() const
{
  return is_open_;
}

bool HttpFilePlugIn::SetProperty( const std::string &name,
                                  const std::string &value )
{
  properties_[name] = value;
  return true;
}

bool HttpFilePlugIn::GetProperty( const std::string &name,
                                  std::string &value ) const
{
  const auto p = properties_.find(name);
  if (p == std::end(properties_)) {
    return false;
  }

  value = p->second;
  return true;
}

}
