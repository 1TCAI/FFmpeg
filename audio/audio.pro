TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    pcm2wav.cpp \
    wav2pcm.cpp \
    av2pcm.cpp \
    media2aac.cpp \
    test.cpp



HEADERS += \
    pcm2wav.h \
    wav2pcm.h \
    av2pcm.h \
    media2aac.h \
    test.h

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
