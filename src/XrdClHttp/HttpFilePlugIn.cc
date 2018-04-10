/**
 * This file is part of XrdClHttp
 */

#include "HttpFilePlugIn.hh"

#include <cassert>

#include <mutex>

static std::once_flag logging_topic_init;

HttpFilePlugIn::HttpFilePlugIn()
    : pLogger(XrdCl::DefaultEnv::GetLog())
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
