#ifndef AUDIORECORDER_AUDIORECORDER_H
#define AUDIORECORDER_AUDIORECORDER_H

#ifdef WINDOWS
#include "ListAVDevices.h"
#endif

#include "ffmpeg.h"
#include <string>
#include <atomic>
#include <mutex>
#include <thread>
#include <cstdint>

using std::string;

class AudioRecorder {

private:
    string outfile;
    string deviceName;
    string failReason;

    AVFormatContext *audioInFormatCtx;
    AVStream        *audioInStream;
    AVCodecContext  *audioInCodecCtx;

    SwrContext      *audioConverter;
    AVAudioFifo     *audioFifo;

    AVFormatContext *audioOutFormatCtx;
    AVStream        *audioOutStream;
    AVCodecContext  *audioOutCodecCtx;

    std::atomic_bool     isRun;
    std::thread         *audioThread;

    void StartEncode();



public:

    AudioRecorder(string filepath, string device)
            :outfile(filepath),deviceName(device),failReason(""),isRun(false){}

    void Open();
    void Start();
    void Stop();

    ~AudioRecorder() {
        Stop();
    }

    std::string GetLastError() { return failReason; }
};
#endif //AUDIORECORDER_AUDIORECORDER_H
