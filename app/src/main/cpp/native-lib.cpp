#include <jni.h>
#include <cstring>
#include "stdio.h"
#include "util/LogUtil.h"

extern "C" {
#include <libavcodec/version.h>
#include <libavformat/version.h>
#include <libavutil/version.h>
#include <libavfilter/version.h>
#include <libswresample/version.h>
#include <libswscale/version.h>
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavcodec/codec.h"
#include "libswscale/swscale.h"
#include <libavutil/imgutils.h>
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
Java_com_gpf_ffmpeg_FFmpeg_decodeVideo(JNIEnv *env, jobject thiz, jstring path, jstring out_path) {

    const char *url = env->GetStringUTFChars(path, nullptr);

    AVFormatContext *formatContext = nullptr;
    formatContext = avformat_alloc_context();

    int result = avformat_open_input(&formatContext, url, nullptr, nullptr);
    if (result != 0) { // 开启失败
        return;
    }

    result = avformat_find_stream_info(formatContext, nullptr);
    if (result < 0) { //
        return;
    }

    //数据流的类型
    AVMediaType mediaType = AVMEDIA_TYPE_VIDEO;
    //数据流索引
    int streamIndex = -1;

    // 获取音视频流索引
    for (int i = 0; i < formatContext->nb_streams; ++i) {
        if (formatContext->streams[i]->codecpar->codec_type == mediaType) {
            streamIndex = i;
            break;
        }
    }

    AVCodecParameters *codecParameters = formatContext->streams[streamIndex]->codecpar;

    const AVCodec *codec = avcodec_find_decoder(codecParameters->codec_id);
    if (codec == nullptr) {
        return;
    }

    AVCodecContext *codecContext = avcodec_alloc_context3(codec);

    // 使用指定编解码器的参数填充编解码器上下文
    result = avcodec_parameters_to_context(codecContext, codecParameters);
    if (result < 0) {
        return;
    }

    // 使用给定的编解码器初始化编解码器上下文
    result = avcodec_open2(codecContext, codec, nullptr);
    if (result != 0) {
        return;
    }

    AVFrame *frame = av_frame_alloc();

    //YUV420
//    AVFrame *frameYUV = av_frame_alloc();
//    const uint8_t *frameYUVBuffer;

    //==================================== 分配空间 ==================================//
    //一帧图像数据大小
//    int bufferSize = av_image_get_buffer_size(AV_PIX_FMT_YUV420P,
//                                              codecContext->width,
//                                              codecContext->height, 1);
//    frameYUVBuffer = (const uint8_t *) av_malloc(bufferSize * sizeof(const uint8_t *));
    //会将pFrameRGB的数据按RGB格式自动"关联"到buffer  即nv12Frame中的数据改变了
    //out_buffer中的数据也会相应的改变
//    av_image_fill_arrays(frameYUV->data, frameYUV->linesize,
//                         frameYUVBuffer, AV_PIX_FMT_YUV420P,
//                         codecContext->width,
//                         codecContext->height,
//                         1);

    // 分配并返回一个 SwsContext。 您需要它使用 sws_scale() 执行缩放/转换操作
    // AV_PIX_FMT_YUV420P
    // AV_PIX_FMT_RGBA
    // SWS_BICUBIC
    // SWS_FAST_BILINEAR
//    SwsContext *swsContext = sws_getContext(codecContext->width,
//                                            codecContext->height,
//                                            codecContext->pix_fmt,
//                                            codecContext->width,
//                                            codecContext->height,
//                                            AV_PIX_FMT_YUV420P,
//                                            SWS_BICUBIC, nullptr, nullptr, nullptr);
//
    const char *output_cstr = env->GetStringUTFChars(out_path, nullptr);

    FILE *fpYUV = fopen(output_cstr, "w+b");

    // 因为FFmpeg软解后的帧格式为YUV420P，也就不用进行格式转换了，直接将解码后的数据写入本地就行了

    AVPacket *packet = av_packet_alloc();

    // 返回流的下一帧数据 <0 error或者读到结尾
    while (av_read_frame(formatContext, packet) >= 0) {
        // 判断当前得到的帧数据是否是当前解码器解析的流
        if (packet->stream_index == streamIndex) {
            // 提供原始数据包数据作为解码器的输入
            result = avcodec_send_packet(codecContext, packet);
            if (result == 0) {
                // 从解码器返回解码后的输出数据
                while (avcodec_receive_frame(codecContext, frame) == 0) {
                    int y_size = codecContext->width * codecContext->height;
                    fwrite(frame->data[0],1,y_size,fpYUV);//y
                    fwrite(frame->data[1],1,y_size/4,fpYUV);//u
                    fwrite(frame->data[2],1,y_size/4,fpYUV);//v

//                    sws_scale(swsContext, frame->data, frame->linesize, 0,
//                              codecContext->height,
//                              frameYUV->data, frameYUV->linesize);
//
//                    //输出到YUV文件
//                    //AVFrame像素帧写入文件
//                    //data解码后的图像像素数据 (音频采样数据)
//                    //Y 亮度 U 色度 (压缩了) 人对亮度更加敏感
//                    //U V 个数是Y的1/4
//                    int y_size = codecContext->width * codecContext->height;
//
//                    fwrite(frameYUV->data[0],1,y_size,fpYUV);//y
//                    fwrite(frameYUV->data[1],1,y_size,fpYUV);//uv

                }
            }
        }

        av_packet_unref(packet);
    }

    // --------------------------------------资源释放操作------------------------------------------

    // 关闭文件句柄
    fclose(fpYUV);

//    av_frame_free(&frameYUV);
//    frameYUV = nullptr;

//    if (frameYUVBuffer != nullptr) {
//        free(frameYUVBuffer);
//    }

//    if (swsContext != nullptr) {
//        sws_freeContext(swsContext);
//    }

    if (frame != nullptr) {
        av_frame_free(&frame);
        frame = nullptr;
    }

    if (packet != nullptr) {
        av_packet_free(&packet);
        packet = nullptr;
    }

    if (codecContext != nullptr) {
        // 关闭给定的 AVCodecContext 并释放与其关联的所有数据（但不是 AVCodecContext 本身）。
        // 在尚未打开的 AVCodecContext 上调用此函数将释放在 avcodec_alloc_context3() 中分配的编解码器特定数据和非 NULL 编解码器
        avcodec_close(codecContext);
        // 释放编解码器上下文和与之关联的所有内容，并将 NULL 写入提供的指针。
        avcodec_free_context(&codecContext);
        codecContext = nullptr;
        codec = nullptr;
    }

    if (formatContext != nullptr) {
        // 关闭一个打开的输入 AVFormatContext。 释放它及其所有内容并将 *s 设置为 NULL。
        avformat_close_input(&formatContext);
        // 释放 AVFormatContext 及其所有流
        avformat_free_context(formatContext);
        formatContext = nullptr;
    }


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