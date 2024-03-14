TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    cutvideo.cpp \
    media2h264.cpp \
    media2yuv.cpp \
    remuxing.cpp \
    videocolorspacechange.cpp \
    videoresolutionchange.cpp \
    yuv2h264.cpp



win32{
INCLUDEPATH += $$PWD/ffmpeg-4.3.1-win32-dev\include
LIBS += $$PWD/ffmpeg-4.3.1-win32-dev\lib\avformat.lib   \
$$PWD/ffmpeg-4.3.1-win32-dev\lib\avcodec.lib   \
$$PWD/ffmpeg-4.3.1-win32-dev\lib\avdevice.lib   \
$$PWD/ffmpeg-4.3.1-win32-dev\lib\avfilter.lib   \
$$PWD/ffmpeg-4.3.1-win32-dev\lib\avutil.lib   \
$$PWD/ffmpeg-4.3.1-win32-dev\lib\postproc.lib   \
$$PWD/ffmpeg-4.3.1-win32-dev\lib\swresample.lib   \
$$PWD/ffmpeg-4.3.1-win32-dev\lib\swscale.lib
}

HEADERS += \
    cutvideo.h \
    media2h264.h \
    media2yuv.h \
    remuxing.h \
    videocolorspacechange.h \
    videoresolutionchange.h \
    yuv2h264.h
