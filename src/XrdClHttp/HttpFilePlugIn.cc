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
    : pLogger(DefaultEnv::GetLog())
{
  // Assert that there is no existing topic
  std::call_once(logging_topic_init, [this] {
      if (pLogger) {
        pLogger->SetTopicName(LogXrdClHttp, "XrdClHttp");
      }
    });

  pLogger->Debug(LogXrdClHttp, "HttpFilePlugin constructed.");
}

HttpFilePlugIn::~HttpFilePlugIn() {}

XRootDStatus HttpFilePlugIn::Open( const std::string &url,
                                   OpenFlags::Flags   flags,
                                   Access::Mode       mode,
                                   ResponseHandler   *handler,
                                   uint16_t           timeout )
{
  (void)url; (void)flags; (void)mode; (void)handler; (void)timeout;
  return XRootDStatus( stError, errNotImplemented );
}

XRootDStatus HttpFilePlugIn::Close( ResponseHandler *handler,
                                    uint16_t         timeout )
{
  (void)handler; (void)timeout;
  return XRootDStatus( stError, errNotImplemented );
}

XRootDStatus HttpFilePlugIn::Stat( bool             force,
                                   ResponseHandler *handler,
                                   uint16_t         timeout )
{
  (void)force; (void)handler; (void)timeout;
  return XRootDStatus( stError, errNotImplemented );
}

XRootDStatus HttpFilePlugIn::Read( uint64_t         offset,
                                   uint32_t         size,
                                   void            *buffer,
                                   ResponseHandler *handler,
                                   uint16_t         timeout )
{
  (void)offset; (void)size; (void)buffer; (void)handler; (void)timeout;
  return XRootDStatus( stError, errNotImplemented );
}

XRootDStatus HttpFilePlugIn::Write( uint64_t         offset,
                                    uint32_t         size,
                                    const void      *buffer,
                                    ResponseHandler *handler,
                                    uint16_t         timeout )
{
  (void)offset; (void)size; (void)buffer; (void)handler; (void)timeout;
  return XRootDStatus( stError, errNotImplemented );
}

XRootDStatus HttpFilePlugIn::Sync( ResponseHandler *handler,
                                   uint16_t         timeout )
{
  (void)handler; (void)timeout;
  return XRootDStatus( stError, errNotImplemented );
}

XRootDStatus HttpFilePlugIn::Truncate( uint64_t         size,
                                       ResponseHandler *handler,
                                       uint16_t         timeout )
{
  (void)size; (void)handler; (void)timeout;
  return XRootDStatus( stError, errNotImplemented );
}

XRootDStatus HttpFilePlugIn::VectorRead( const ChunkList &chunks,
                                         void            *buffer,
                                         ResponseHandler *handler,
                                         uint16_t         timeout )
{
  (void)chunks; (void)buffer; (void)handler; (void)timeout;
  return XRootDStatus( stError, errNotImplemented );
}

XRootDStatus HttpFilePlugIn::Fcntl( const Buffer    &arg,
                                    ResponseHandler *handler,
                                    uint16_t         timeout )
{
  (void)arg; (void)handler; (void)timeout;
  return XRootDStatus( stError, errNotImplemented );
}

XRootDStatus HttpFilePlugIn::Visa( ResponseHandler *handler,
                                   uint16_t         timeout )
{
  (void)handler; (void)timeout;
  return XRootDStatus( stError, errNotImplemented );
}

bool HttpFilePlugIn::IsOpen() const
{
  return false;
}

bool HttpFilePlugIn::SetProperty( const std::string &name,
                                  const std::string &value )
{
  (void)name; (void)value;
  return false;
}

bool HttpFilePlugIn::GetProperty( const std::string &name,
                                  std::string &value ) const
{
  (void)name; (void)value;
  return false;
}

}
