
#include "AudioPlayer.h"

using std::operator""ms;
const AVSampleFormat requireAudioFmt = AV_SAMPLE_FMT_S32; // SDL only accpet interleaved audio data.

void AudioPlayer::OpenDevice() {

    audioFormatCtx = nullptr;
    int ret = avformat_open_input(&audioFormatCtx, filepath.c_str(), nullptr, nullptr);
    if (ret != 0) {
        throw std::runtime_error("can not open input format!");
    }
    ret = avformat_find_stream_info(audioFormatCtx, nullptr);
    if (ret < 0) {
        throw std::runtime_error("fail to find stream info.");
    }
    for (uint16_t i = 0; i < audioFormatCtx->nb_streams; i++) {
        if (audioFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            audioStream = audioFormatCtx->streams[i];
            break;
        }
    }
    if (audioStream == nullptr) {
        throw std::runtime_error("fail to find stream.");
    }
    AVCodec * audioCodec = avcodec_find_decoder(audioStream->codecpar->codec_id);
    audioCodecCtx = avcodec_alloc_context3(audioCodec);
    avcodec_parameters_to_context(audioCodecCtx, audioStream->codecpar);

    if (avcodec_open2(audioCodecCtx, audioCodec, nullptr) < 0) {
        throw std::runtime_error("fail to open codec.");
    }


    // config Converter
    audioConverter = swr_alloc_set_opts(nullptr,
                                        av_get_default_channel_layout(audioCodecCtx->channels),
                                        requireAudioFmt,  // aac encoder only receive this format
                                        audioCodecCtx->sample_rate,
                                        av_get_default_channel_layout(audioCodecCtx->channels),
                                        (AVSampleFormat)audioStream->codecpar->format,
                                        audioStream->codecpar->sample_rate,
                                        0, nullptr);
    swr_init(audioConverter);


    isRun = true;
    isPlayFinish = false;

}



void AudioPlayer::PlayAndDecode()
{

    SdlAudio();

    AVPacket *packet_in = av_packet_alloc();
    AVFrame *frame_out = av_frame_alloc();
    int ret;
    while (isRun) {
        ret = av_read_frame(audioFormatCtx, packet_in);
        if (ret < 0) {
            isRun = false;
            break; //throw std::runtime_error("fail to read frame frome file.");
        }

        ret = avcodec_send_packet(audioCodecCtx, packet_in);
        if (ret < 0) throw std::runtime_error("fail to send packet in decode");

        ret = avcodec_receive_frame(audioCodecCtx, frame_out); //frame_out->format must be FLTP for aac
        if (ret < 0) throw std::runtime_error("fail to receive frame in decode");

        av_packet_unref(packet_in);
        // transform audio format.
        uint8_t **cSamples = nullptr;
        ret = av_samples_alloc_array_and_samples(&cSamples, nullptr, audioCodecCtx->channels, frame_out->nb_samples, requireAudioFmt, 0);
        if (ret < 0) {
            throw std::runtime_error("Fail to alloc samples by av_samples_alloc_array_and_samples.");
        }
        ret = swr_convert(audioConverter, cSamples, frame_out->nb_samples, (const uint8_t**)frame_out->extended_data, frame_out->nb_samples);
        if (ret < 0) {
            throw std::runtime_error("Fail to swr_convert.");
        }

        //100 packet is about 2s
        while (fifo.size()>100) {
            std::this_thread::sleep_for(500ms);
        }

        lock.lock();
        fifo.push(cSamples);
        lock.unlock();

        av_frame_unref(frame_out);
    }

    av_packet_free(&packet_in);
    av_frame_free(&frame_out);

    while (!isPlayFinish) {
        std::this_thread::sleep_for(100ms);
    }

}

void AudioPlayer::GetData(uint8_t * buf, int len)
{
    memset(buf, 0, len);
    lock.lock();
    if (!fifo.empty()) {
        uint8_t ** data = fifo.front();
        fifo.pop();
        lock.unlock();
        memcpy(buf, data[0], len);
        av_freep(&data[0]);
    }
    else {
        lock.unlock();
        if (!isRun) {
            SDL_PauseAudio(1);
            puts("Play Finish.");
            isPlayFinish = true;
        }
    }

}

void  fill_audio(void *udata, Uint8 *stream, int len) {

    auto decoder = (AudioPlayer*)udata;
    decoder->GetData(stream, len);
}

void AudioPlayer::SdlAudio()
{
    if (SDL_Init(SDL_INIT_AUDIO)) {
        throw std::runtime_error("fail to init SDL.");
    }

    SDL_memset(&wanted_spec, 0, sizeof(wanted_spec));
    wanted_spec.freq = audioStream->codecpar->sample_rate;
    wanted_spec.format = AUDIO_S32SYS;
    wanted_spec.channels = audioStream->codecpar->channels;
    wanted_spec.silence = 0;
    wanted_spec.samples = 1024; // samples per channel. It equals to frame->nb_samples.
    wanted_spec.callback = fill_audio;
    wanted_spec.userdata = this;

    if (SDL_OpenAudio(&wanted_spec, nullptr) != 0) {
        throw std::runtime_error("fail to open audio device in SDL.");
    }
    SDL_PauseAudio(0);
}
