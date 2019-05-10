
#ifndef AUDIORECORDER_AUDIOPLAYER_H
#define AUDIORECORDER_AUDIOPLAYER_H

#include <string>
#include <queue>
#include <thread>
#include <chrono>
#include <mutex>
#include "ffmpeg.h"

extern "C" {
#define SDL_MAIN_HANDLED
#define __STDC_CONSTANT_MACROS
#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include <SDL2/SDL_types.h>
}

using AudioCB = void(*)(uint8_t** data, int len);

class AudioPlayer {
private:

    std::string  filepath; // filepath of aac

    AVFormatContext *audioFormatCtx;
    AVStream        *audioStream;
    AVCodecContext  *audioCodecCtx;
    SwrContext      *audioConverter;

    std::mutex lock;
    std::queue<uint8_t**> fifo;
    std::atomic_bool  isRun;
    std::atomic_bool  isPlayFinish;

    SDL_AudioSpec wanted_spec;


    void SdlAudio();
    void GetData(uint8_t* buffer, int len);
    friend void fill_audio(void *udata, Uint8 *stream, int len);

public:

    AudioPlayer(std::string file) :filepath(file),audioStream(nullptr) {}

    void OpenDevice();
    void PlayAndDecode();

};

#endif //AUDIORECORDER_AUDIOPLAYER_H
