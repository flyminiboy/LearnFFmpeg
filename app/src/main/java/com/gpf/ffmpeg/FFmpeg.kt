package com.gpf.ffmpeg

import android.view.Surface

object FFmpeg {

    external fun displayFFmpegInfo(): String

    external fun decodeVideo(path:String)

    external fun decodeVideoAndPlay(path:String, surface:Surface)

    external fun decodeAudio(path:String)

    // Used to load the 'ffmpeg' library on application startup.
    init {
        System.loadLibrary("ffmpeg")
    }

}