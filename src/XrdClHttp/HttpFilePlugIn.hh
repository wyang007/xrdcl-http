/**
 * This file is part of XrdClHttp
 */

#ifndef __HTTP_FILE_PLUG_IN_
#define __HTTP_FILE_PLUG_IN_

#include <limits>
#include <cstdint>

#include "XrdCl/XrdClDefaultEnv.hh"
#include "XrdCl/XrdClFile.hh"
#include "XrdCl/XrdClFileSystem.hh"
#include "XrdCl/XrdClLog.hh"
#include "XrdCl/XrdClPlugInInterface.hh"

namespace XrdCl {

class HttpFilePlugIn : public FilePlugIn {
 public:
  HttpFilePlugIn();
  virtual ~HttpFilePlugIn();

  //------------------------------------------------------------------------
  //! @see XrdCl::File::Open
  //------------------------------------------------------------------------
  virtual XRootDStatus Open( const std::string &url,
                             OpenFlags::Flags   flags,
                             Access::Mode       mode,
                             ResponseHandler   *handler,
                             uint16_t           timeout ) override;

  //------------------------------------------------------------------------
  //! @see XrdCl::File::Close
  //------------------------------------------------------------------------
  virtual XRootDStatus Close( ResponseHandler *handler,
                              uint16_t         timeout ) override;

  //------------------------------------------------------------------------
  //! @see XrdCl::File::Stat
  //------------------------------------------------------------------------
  virtual XRootDStatus Stat( bool             force,
                             ResponseHandler *handler,
                             uint16_t         timeout ) override;

  //------------------------------------------------------------------------
  //! @see XrdCl::File::Read
  //------------------------------------------------------------------------
  virtual XRootDStatus Read( uint64_t         offset,
                             uint32_t         size,
                             void            *buffer,
                             ResponseHandler *handler,
                             uint16_t         timeout ) override;

  //------------------------------------------------------------------------
  //! @see XrdCl::File::Write
  //------------------------------------------------------------------------
  virtual XRootDStatus Write( uint64_t         offset,
                              uint32_t         size,
                              const void      *buffer,
                              ResponseHandler *handler,
                              uint16_t         timeout ) override;

  //------------------------------------------------------------------------
  //! @see XrdCl::File::Sync
  //------------------------------------------------------------------------
  virtual XRootDStatus Sync( ResponseHandler *handler,
                             uint16_t         timeout ) override;

  //------------------------------------------------------------------------
  //! @see XrdCl::File::Truncate
  //------------------------------------------------------------------------
  virtual XRootDStatus Truncate( uint64_t         size,
                                 ResponseHandler *handler,
                                 uint16_t         timeout ) override;

  //------------------------------------------------------------------------
  //! @see XrdCl::File::VectorRead
  //------------------------------------------------------------------------
  virtual XRootDStatus VectorRead( const ChunkList &chunks,
                                   void            *buffer,
                                   XrdCl::ResponseHandler *handler,
                                   uint16_t         timeout ) override;

  //------------------------------------------------------------------------
  //! @see File::Fcntl
  //------------------------------------------------------------------------
  virtual XRootDStatus Fcntl( const Buffer    &arg,
                              ResponseHandler *handler,
                              uint16_t         timeout ) override;

  //------------------------------------------------------------------------
  //! @see XrdCl::File::Visa
  //------------------------------------------------------------------------
  virtual XRootDStatus Visa( ResponseHandler *handler,
                             uint16_t         timeout ) override;

  //------------------------------------------------------------------------
  //! @see XrdCl::File::IsOpen
  //------------------------------------------------------------------------
  virtual bool IsOpen() const override;

  //------------------------------------------------------------------------
  //! @see XrdCl::File::SetProperty
  //------------------------------------------------------------------------
  virtual bool SetProperty( const std::string &name,
                            const std::string &value ) override;

  //------------------------------------------------------------------------
  //! @see XrdCl::File::GetProperty
  //------------------------------------------------------------------------
  virtual bool GetProperty( const std::string &name,
                            std::string &value ) const override;

 private:
  // Topic id for the logger
  static const uint64_t LogXrdClHttp = std::numeric_limits<std::uint64_t>::max();

  Log* pLogger;
};

}

#endif // __HTTP_FILE_PLUG_IN_
