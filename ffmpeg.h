//
// Created by 原照萌 on 2019-05-08.
//

#ifndef AUDIORECORDER_FFMPEG_H
#define AUDIORECORDER_FFMPEG_H


#pragma warning(disable:4819)

extern "C" {
#define __STDC_CONSTANT_MACROS
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <libavutil/avutil.h>
#include <libavutil/file.h>
#include <libavutil/time.h>
#include <libavutil/audio_fifo.h>
#include <libavutil/imgutils.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
#include <libavdevice/avdevice.h>

}


#endif //AUDIORECORDER_FFMPEG_H
