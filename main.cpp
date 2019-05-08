
#include "AudioRecorder.h"
#include <string>

using namespace std;

int main(){
    puts("==== Audio Recorder ====");
    avdevice_register_all();

    AudioRecorder recorder{ "testAudio.aac","" };
    try {
        recorder.Open();
    }
    catch (std::exception &e) {
        fprintf(stderr,"[ERROR] %s\n", e.what());
        exit(-1);
    }
    recorder.Start();
    std::this_thread::sleep_for(10s);
    string reason = recorder.GetLastError();
    if (!reason.empty()) {
        fprintf(stderr, "[ERROR] %s\n", reason.c_str());
        exit(-1);
    }

    try {
        recorder.Stop();
    }
    catch (std::exception &e) {
        fprintf(stderr, "[ERROR] %s\n", e.what());
        exit(-1);
    }

    puts("END");
    return 0;
}