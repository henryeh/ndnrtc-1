// 
// tests-helpers.h
//
//  Created by Peter Gusev on 05 March 2016.
//  Copyright 2013-2016 Regents of the University of California
//

#ifndef __tests_helpers_h__
#define __tests_helpers_h__

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <boost/asio.hpp>

#include <include/params.h>
#include "client/src/config.h"
#include <include/interfaces.h>
#include <include/statistics.h>
#include <webrtc/common_video/libyuv/include/webrtc_libyuv.h>
#include "src/frame-data.h"

#define GT_PRINTF(...)  do { testing::internal::ColoredPrintf(testing::internal::COLOR_GREEN, "[ INFO     ] "); testing::internal::ColoredPrintf(testing::internal::COLOR_YELLOW, __VA_ARGS__); } while(0)

ndnrtc::VideoCoderParams sampleVideoCoderParams();
ClientParams sampleConsumerParams();
ClientParams sampleProducerParams();

webrtc::EncodedImage encodedImage(size_t frameLen, uint8_t*& buffer, bool delta = true);
bool checkVideoFrame(const webrtc::EncodedImage& image);
ndnrtc::VideoFramePacket getVideoFramePacket(size_t frameLen = 4300, double rate = 24.7,
  int64_t pubTs = 488589553, int64_t pubUts = 1460488589);
std::vector<ndnrtc::VideoFrameSegment> sliceFrame(ndnrtc::VideoFramePacket& vp, 
  PacketNumber playNo = 0, PacketNumber pairedSeqNo = 1);
std::vector<ndnrtc::CommonSegment> sliceParity(ndnrtc::VideoFramePacket& vp, 
  boost::shared_ptr<ndnrtc::NetworkData>& parity);
std::vector<boost::shared_ptr<ndn::Data>> dataFromSegments(std::string frameName,
	const std::vector<ndnrtc::VideoFrameSegment>& segments);
std::vector<boost::shared_ptr<ndn::Data>> dataFromParitySegments(std::string frameName,
	const std::vector<ndnrtc::CommonSegment>& segments);
std::vector<boost::shared_ptr<ndn::Interest>> getInterests(std::string frameName,
	unsigned int startSeg, size_t nSeg, unsigned int parityStartSeg = 0, size_t parityNSeg = 0);

namespace testing
{
 namespace internal
 {
  enum GTestColor {
      COLOR_DEFAULT,
      COLOR_RED,
      COLOR_GREEN,
      COLOR_YELLOW
  };

  extern void ColoredPrintf(GTestColor color, const char* fmt, ...);
 }
}

//******************************************************************************
typedef boost::function<void(void)> QueueBlock;
typedef boost::chrono::high_resolution_clock::time_point TPoint;
typedef boost::chrono::high_resolution_clock Clock;
typedef boost::chrono::milliseconds Msec;

class DelayQueue {
public:
  DelayQueue(boost::asio::io_service& io, int delayMs, int deviation = 0):
    timer_(io), delayMs_(delayMs), dev_(deviation), timerSet_(false), active_(true)
    { std::srand(std::time(0)); }
  ~DelayQueue()
  {
    active_ = false;
  }

  void push(QueueBlock block);
  void reset();

private:
  boost::atomic<bool> active_, timerSet_;
  int delayMs_, dev_;
  boost::asio::steady_timer timer_;
  std::map<TPoint, std::vector<QueueBlock>> queue_;

  void pop(const boost::system::error_code& e);
};

typedef boost::function<void(const boost::shared_ptr<ndn::Interest>&)> OnInterestT;
typedef boost::function<void(const boost::shared_ptr<ndn::Data>&, const boost::shared_ptr<ndn::Interest>)> OnDataT;

class DataCache {
public:
  void addInterest(const boost::shared_ptr<ndn::Interest>& interest, OnDataT onData);
  void addData(const boost::shared_ptr<ndn::Data>& data, OnInterestT onInterest = OnInterestT());

private:
  boost::mutex m_;
  std::map<ndn::Name, boost::shared_ptr<ndn::Interest>> interests_;
  std::map<ndn::Name, OnDataT> onDataCallbacks_;
  std::map<ndn::Name, boost::shared_ptr<ndn::Data>> data_;
  std::map<ndn::Name, OnInterestT> onInterestCallbacks_;
};

#endif
