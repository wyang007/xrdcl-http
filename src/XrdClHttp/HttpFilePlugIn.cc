/**
 * This file is part of XrdClHttp
 */

#include "HttpFilePlugIn.hh"

#include <cassert>

#include "HttpPlugInUtil.hh"
#include "Posix.hh"
#include "XrdCl/XrdClDefaultEnv.hh"
#include "XrdCl/XrdClLog.hh"
#include "XrdCl/XrdClStatus.hh"

namespace {

int MakePosixOpenFlags(XrdCl::OpenFlags::Flags flags) {
  int posix_flags = 0;
  if (flags & XrdCl::OpenFlags::New) {
    posix_flags |= O_CREAT | O_EXCL;
  }
  if (flags & XrdCl::OpenFlags::Delete) {
    posix_flags |= O_CREAT | O_TRUNC;
  }
  if (flags & XrdCl::OpenFlags::Append) {
    posix_flags |= O_APPEND;
  }
  if (flags & XrdCl::OpenFlags::Read) {
    posix_flags |= O_RDONLY;
  }
  if (flags & XrdCl::OpenFlags::Write) {
    posix_flags |= O_WRONLY;
  }
  if (flags & XrdCl::OpenFlags::Update) {
    posix_flags |= O_RDWR;
  }
  return posix_flags;
}

}  // namespace

namespace XrdCl {

HttpFilePlugIn::HttpFilePlugIn()
    : davix_context_(),
      davix_client_(&davix_context_),
      davix_fd_(nullptr),
      is_open_(false),
      url_(),
      properties_(),
      logger_(DefaultEnv::GetLog()) {
  SetUpLogging(logger_);
  logger_->Debug(kLogXrdClHttp, "HttpFilePlugin constructed.");
}

XRootDStatus HttpFilePlugIn::Open(const std::string &url,
                                  OpenFlags::Flags flags, Access::Mode /*mode*/,
                                  ResponseHandler *handler, uint16_t timeout) {
  if (is_open_) {
    logger_->Error(kLogXrdClHttp, "URL %s already open", url.c_str());
    return XRootDStatus(stError, errInvalidOp);
  }

  Davix::RequestParams params;
  if (timeout != 0) {
    struct timespec ts = {timeout, 0};
    params.setOperationTimeout(&ts);
  }

  // O_CREAT is always assumed, we try to create parents paths, too
  auto full_path = XrdCl::URL(url).GetLocation();
  auto pos = full_path.find_last_of('/');
  auto base_dir =
      pos != std::string::npos ? full_path.substr(0, pos) : full_path;
  auto mkdir_status =
      Posix::MkDir(davix_client_, base_dir, XrdCl::MkDirFlags::MakePath,
                   XrdCl::Access::None, timeout);
  if (mkdir_status.IsError()) {
    logger_->Error(kLogXrdClHttp,
                   "Could not create parent directories when opening: %s",
                   url.c_str());
    return mkdir_status;
  }

  if (((flags & OpenFlags::Write) || (flags & OpenFlags::Update)) &&
      (flags & OpenFlags::Delete)) {
    auto stat_info = new StatInfo();
    auto status = Posix::Stat(davix_client_, url, timeout, stat_info);
    if (status.IsOK()) {
      auto unlink_status = Posix::Unlink(davix_client_, url, timeout);
      if (unlink_status.IsError()) {
        logger_->Error(
            kLogXrdClHttp,
            "Could not delete existing destination file: %s. Error: %s",
            url.c_str(), unlink_status.GetErrorMessage().c_str());
        return unlink_status;
      }
    }
  }

  auto posix_open_flags = MakePosixOpenFlags(flags);

  logger_->Debug(kLogXrdClHttp,
                 "Open: URL: %s, XRootD flags: %d, POSIX flags: %d",
                 url.c_str(), flags, posix_open_flags);

  // res == std::pair<fd, XRootDStatus>
  auto res = Posix::Open(davix_client_, url, posix_open_flags, timeout);
  if (!res.first) {
    logger_->Error(kLogXrdClHttp, "Could not open: %s, error: %s", url.c_str(),
                   res.second.ToStr().c_str());
    return res.second;
  }

  davix_fd_ = res.first;

  logger_->Debug(kLogXrdClHttp, "Opened: %s", url.c_str());

  is_open_ = true;
  url_ = url;

  auto status = new XRootDStatus();
  handler->HandleResponse(status, nullptr);

  return XRootDStatus();
}

XRootDStatus HttpFilePlugIn::Close(ResponseHandler *handler,
                                   uint16_t /*timeout*/) {
  if (!is_open_) {
    logger_->Error(kLogXrdClHttp,
                   "Cannot close. URL hasn't been previously opened");
    return XRootDStatus(stError, errInvalidOp);
  }

  logger_->Debug(kLogXrdClHttp, "Closing davix fd: %ld", davix_fd_);

  auto status = Posix::Close(davix_client_, davix_fd_);
  if (status.IsError()) {
    logger_->Error(kLogXrdClHttp, "Could not close davix fd: %ld, error: %s",
                   davix_fd_, status.ToStr().c_str());
    return status;
  }

  is_open_ = false;
  url_.clear();

  handler->HandleResponse(new XRootDStatus(), nullptr);

  return XRootDStatus();
}

XRootDStatus HttpFilePlugIn::Stat(bool /*force*/, ResponseHandler *handler,
                                  uint16_t timeout) {
  if (!is_open_) {
    logger_->Error(kLogXrdClHttp,
                   "Cannot stat. URL hasn't been previously opened");
    return XRootDStatus(stError, errInvalidOp);
  }

  auto stat_info = new StatInfo();
  auto status = Posix::Stat(davix_client_, url_, timeout, stat_info);
  if (status.IsError()) {
    logger_->Error(kLogXrdClHttp, "Stat failed: %s", status.ToStr().c_str());
    return status;
  }

  logger_->Debug(kLogXrdClHttp, "Stat-ed URL: %s", url_.c_str());

  auto obj = new AnyObject();
  obj->Set(stat_info);

  handler->HandleResponse(new XRootDStatus(), obj);

  return XRootDStatus();
}

XRootDStatus HttpFilePlugIn::Read(uint64_t offset, uint32_t size, void *buffer,
                                  ResponseHandler *handler,
                                  uint16_t /*timeout*/) {
  if (!is_open_) {
    logger_->Error(kLogXrdClHttp,
                   "Cannot read. URL hasn't previously been opened");
    return XRootDStatus(stError, errInvalidOp);
  }

  Davix::DavixError *err = nullptr;
  // res = std::pair<int, XRootDStatus>
  auto res = Posix::PRead(davix_client_, davix_fd_, buffer, size, offset);
  if (res.second.IsError()) {
    logger_->Error(kLogXrdClHttp, "Could not read URL: %s, error: %s",
                   url_.c_str(), res.second.ToStr().c_str());
    return res.second;
  }

  int num_bytes_read = res.first;

  logger_->Debug(kLogXrdClHttp, "Read %d bytes, at offset %d, from URL: %s",
                 num_bytes_read, offset, url_.c_str());

  auto status = new XRootDStatus();
  auto chunk_info = new ChunkInfo(offset, num_bytes_read, buffer);
  auto obj = new AnyObject();
  obj->Set(chunk_info);
  handler->HandleResponse(status, obj);

  return XRootDStatus();
}

XRootDStatus HttpFilePlugIn::Write(uint64_t offset, uint32_t size,
                                   const void *buffer, ResponseHandler *handler,
                                   uint16_t timeout) {
  if (!is_open_) {
    logger_->Error(kLogXrdClHttp,
                   "Cannot write. URL hasn't previously been opened");
    return XRootDStatus(stError, errInvalidOp);
  }

  // res == std::pair<int, XRootDStatus>
  auto res =
      Posix::PWrite(davix_client_, davix_fd_, offset, size, buffer, timeout);
  if (res.second.IsError()) {
    logger_->Error(kLogXrdClHttp, "Could not write URL: %s, error: %s",
                   url_.c_str(), res.second.ToStr().c_str());
    return res.second;
  }

  logger_->Debug(kLogXrdClHttp, "Wrote %d bytes, at offset %d, to URL: %s",
                 res.first, offset, url_.c_str());

  handler->HandleResponse(new XRootDStatus(), nullptr);

  return XRootDStatus();
}

XRootDStatus HttpFilePlugIn::Sync(ResponseHandler *handler, uint16_t timeout) {
  (void)handler;
  (void)timeout;

  logger_->Debug(kLogXrdClHttp, "Sync is a no-op for HTTP.");

  return XRootDStatus();
}

XRootDStatus HttpFilePlugIn::VectorRead(const ChunkList &chunks, void *buffer,
                                        ResponseHandler *handler,
                                        uint16_t /*timeout*/) {
  if (!is_open_) {
    logger_->Error(kLogXrdClHttp,
                   "Cannot read. URL hasn't previously been opened");
    return XRootDStatus(stError, errInvalidOp);
  }

  const auto num_chunks = chunks.size();
  std::vector<Davix::DavIOVecInput> input_vector(num_chunks);
  std::vector<Davix::DavIOVecOuput> output_vector(num_chunks);

  for (size_t i = 0; i < num_chunks; ++i) {
    input_vector[i].diov_offset = chunks[i].offset;
    input_vector[i].diov_size = chunks[i].length;
    input_vector[i].diov_buffer = chunks[i].buffer;
  }

  // res == std::pair<int, XRootDStatus>
  auto res = Posix::PReadVec(davix_client_, davix_fd_, chunks, buffer);
  if (res.second.IsError()) {
    logger_->Error(kLogXrdClHttp, "Could not vectorRead URL: %s, error: %s",
                   url_.c_str(), res.second.ToStr().c_str());
    return res.second;
  }

  int num_bytes_read = res.first;

  logger_->Debug(kLogXrdClHttp, "VecRead %d bytes, from URL: %s",
                 num_bytes_read, url_.c_str());

  char *output = static_cast<char *>(buffer);
  for (size_t i = 0; i < num_chunks; ++i) {
    std::memcpy(output + input_vector[i].diov_offset,
                output_vector[i].diov_buffer, output_vector[i].diov_size);
  }

  auto status = new XRootDStatus();
  auto read_info = new VectorReadInfo();
  read_info->SetSize(num_bytes_read);
  read_info->GetChunks() = chunks;
  auto obj = new AnyObject();
  obj->Set(read_info);
  handler->HandleResponse(status, obj);

  return XRootDStatus();
}

bool HttpFilePlugIn::IsOpen() const { return is_open_; }

bool HttpFilePlugIn::SetProperty(const std::string &name,
                                 const std::string &value) {
  properties_[name] = value;
  return true;
}

bool HttpFilePlugIn::GetProperty(const std::string &name,
                                 std::string &value) const {
  const auto p = properties_.find(name);
  if (p == std::end(properties_)) {
    return false;
  }

  value = p->second;
  return true;
}

}  // namespace XrdCl
