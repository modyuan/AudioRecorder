cmake_minimum_required(VERSION 3.14)
project(AudioRecorder)

set(CMAKE_CXX_STANDARD 14)


IF (WIN32)
    MESSAGE(STATUS "Platform is Windows")
    add_definitions(-DWINDOWS)
ELSEIF (APPLE)
    MESSAGE(STATUS "Platform is MacOS")
    add_definitions(-DMACOS)

    include_directories(/usr/local/include/)
    link_directories(/usr/local/lib/)
ELSEIF (UNIX)
    MESSAGE(STATUS "Platform is UNIX")
    add_definitions(-DUNIX)
ENDIF ()


add_executable(AudioRecorder main.cpp AudioRecorder.cpp AudioRecorder.h ffmpeg.h ListAVDevices.cpp ListAVDevices.h AudioPlayer.cpp AudioPlayer.h)

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
