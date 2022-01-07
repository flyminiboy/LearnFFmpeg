#include <jni.h>
#include <cstring>
#include "util/LogUtil.h"

extern "C" {
#include <libavcodec/version.h>
#include <libavformat/version.h>
#include <libavutil/version.h>
#include <libavfilter/version.h>
#include <libswresample/version.h>
#include <libswscale/version.h>
#include "libavcodec/avcodec.h"
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_gpf_ffmpeg_FFmpeg_displayFFmpegInfo(JNIEnv *env, jobject thiz) {
    char strBuffer[1024 * 4] = {0};
    strcat(strBuffer, "libavcodec : ");
    strcat(strBuffer, AV_STRINGIFY(LIBAVCODEC_VERSION));
    strcat(strBuffer, "\nlibavformat : ");
    strcat(strBuffer, AV_STRINGIFY(LIBAVFORMAT_VERSION));
    strcat(strBuffer, "\nlibavutil : ");
    strcat(strBuffer, AV_STRINGIFY(LIBAVUTIL_VERSION));
    strcat(strBuffer, "\nlibavfilter : ");
    strcat(strBuffer, AV_STRINGIFY(LIBAVFILTER_VERSION));
    strcat(strBuffer, "\nlibswresample : ");
    strcat(strBuffer, AV_STRINGIFY(LIBSWRESAMPLE_VERSION));
    strcat(strBuffer, "\nlibswscale : ");
    strcat(strBuffer, AV_STRINGIFY(LIBSWSCALE_VERSION));
    strcat(strBuffer, "\navcodec_configure : \n");
    strcat(strBuffer, avcodec_configuration());
    strcat(strBuffer, "\navcodec_license : ");
    strcat(strBuffer, avcodec_license());
    LOGCATE("GetFFmpegVersion\n%s", strBuffer);

    return env->NewStringUTF(strBuffer);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_gpf_ffmpeg_FFmpeg_decodeVideo(JNIEnv *env, jobject thiz, jstring path) {

}
extern "C"
JNIEXPORT void JNICALL
Java_com_gpf_ffmpeg_FFmpeg_decodeVideoAndPlay(JNIEnv *env, jobject thiz, jstring path,
                                              jobject surface) {

}
extern "C"
JNIEXPORT void JNICALL
Java_com_gpf_ffmpeg_FFmpeg_decodeAudio(JNIEnv *env, jobject thiz, jstring path) {

}