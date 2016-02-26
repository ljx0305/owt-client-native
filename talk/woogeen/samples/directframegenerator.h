/*
 *Intel License
*/
#ifndef DIRECTFRAMGENERATOR_H_
#define DIRECTFRAMGENERATOR_H_

#include <stdio.h>
#include <iostream>
#include <mutex>
//#include <boost/thread.hpp>
#include "woogeen/base/framegeneratorinterface.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavdevice/avdevice.h>
#include <libavformat/avformat.h>
#include <libavutil/mathematics.h>
}


class TimeoutHandler {
public:
  TimeoutHandler(int32_t timeout)
    : m_timeout(timeout), m_lastTime(time(NULL)) {}

  void reset(int32_t timeout) {
    m_timeout = timeout;
    m_lastTime = time(NULL);
  }

  static int checkInterrupt(void* handler) {
    return handler && static_cast<TimeoutHandler*>(handler)->isTimeout();
  }

private:
  bool isTimeout() {
    int32_t delay = time(NULL) - m_lastTime;
    return delay > m_timeout;
  }

  int32_t m_timeout;
  int64_t m_lastTime;
};

class DirectFrameGenerator : public woogeen::base::FrameGeneratorInterface {
public:
  struct Options {
    std::string url;
    std::string transport;
    uint32_t bufferSize;
    int width;
    int height;
    int fps;
    bool useLocal;
    VideoFrameCodec type;
    Options()
      : url(""),
      transport("udp"),
      bufferSize(2 * 1024 * 1024),
      width(0),
      height(0),
      useLocal(false),
      type(I420) {}
  };

  DirectFrameGenerator(const Options& options);
  ~DirectFrameGenerator();

  int GetFrameSize();

  void Init();
  void ReadFrame();
  void GenerateNextFrame(uint8** frame_buffer);

  int GetHeight();
  int GetWidth();
  int GetFps();
  VideoFrameCodec GetType();

private:
  int m_width;
  int m_height;
  int m_fps;
  VideoFrameCodec m_type;
  int m_frame_data_size;
  std::string m_url;
  AVDictionary* m_transportOpts;
  bool m_localCamera;
  std::mutex m_mutex;
 // boost::thread m_thread;
  AVFormatContext* m_context;
  AVInputFormat* m_inputFormat;
  TimeoutHandler* m_timeoutHandler;
  AVPacket m_avPacket;
  int m_videoStreamIndex;
  int m_audioStreamIndex;
#ifdef CAPTURE_FROM_IPCAM
  FILE *output;
#endif
  //AVCodec
  //AVCodec* m_decoder;
  //AVCodecContext *m_decoderContext;
};

#endif // DIRECTFRAMGENERATOR_H_