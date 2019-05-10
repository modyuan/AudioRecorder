
#include "AudioRecorder.h"
#include "AudioPlayer.h"
#include <string>

using namespace std;

int main(){
    puts("==== Audio Player ====");
    avdevice_register_all();
    try {
        AudioPlayer player{"testAudio.aac"};
        player.OpenDevice();
        player.PlayAndDecode();
    }
    catch (std::exception &e) {
        fprintf(stderr,"[ERROR] %s\n", e.what());
        exit(-1);
    }

    puts("END");
    return 0;
}

int main1(){
    puts("==== Audio Recorder ====");
    avdevice_register_all();

    AudioRecorder recorder{ "testAudio.aac","" };
    try {
        recorder.Open();
        recorder.Start();

        //record 10 seconds.
        std::this_thread::sleep_for(10s);

        recorder.Stop();
        string reason = recorder.GetLastError();
        if (!reason.empty()) {
            throw std::runtime_error(reason);
        }
    }
    catch (std::exception &e) {
        fprintf(stderr,"[ERROR] %s\n", e.what());
        exit(-1);
    }

    puts("END");
    return 0;
}