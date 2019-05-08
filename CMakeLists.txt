cmake_minimum_required(VERSION 3.14)
project(AudioRecorder)

set(CMAKE_CXX_STANDARD 14)


IF (WIN32)
    MESSAGE(STATUS "Platform is Windows")
    add_definitions(-DWINDOWS)
ELSEIF (APPLE)
    MESSAGE(STATUS "Platform is MacOS")
    add_definitions(-DMACOS)

    #for ffmpeg
    include_directories(/usr/local/Cellar/ffmpeg/4.1.3/include/)
    link_directories(/usr/local/Cellar/ffmpeg/4.1.3/lib/)

    # for SDL2
    include_directories(/usr/local/Cellar/sdl2/2.0.9/include/)
    link_directories(/usr/local/Cellar/sdl2/2.0.9/lib)
ELSEIF (UNIX)
    MESSAGE(STATUS "Platform is UNIX")
    add_definitions(-DUNIX)
ENDIF ()


if(APPLE)
    add_definitions(-DMACOS)

else()
    add_definitions(-DWINDOWS)

endif()


add_executable(AudioRecorder main.cpp Recorder.cpp Recorder.h AudioRecorder.cpp AudioRecorder.h ffmpeg.h ListAVDevices.cpp ListAVDevices.h)

target_link_libraries(
        AudioRecorder
        avcodec
        avdevice
        avfilter
        avformat
        avutil
        postproc
        swresample
        swscale
        SDL2
)
