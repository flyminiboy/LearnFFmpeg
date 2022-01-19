package com.gpf.ffmpeg

import android.Manifest
import android.content.pm.PackageManager
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.os.Environment
import android.util.Log
import android.widget.Toast
import androidx.core.content.ContextCompat
import com.gpf.ffmpeg.databinding.ActivityMainBinding
import com.permissionx.guolindev.PermissionX
import java.io.File

class MainActivity : AppCompatActivity() {

    private lateinit var binding: ActivityMainBinding

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)

        // Example of a call to a native method
        binding.sampleText.text = FFmpeg.displayFFmpegInfo()

        val mVideoPath = externalCacheDir?.absolutePath + "/a.mp4"
        val yuv = File(externalCacheDir, "a.yuv")

        // 权限判断
        if (ContextCompat.checkSelfPermission(
                this,
                Manifest.permission.WRITE_EXTERNAL_STORAGE
            ) == PackageManager.PERMISSION_GRANTED
        ) {
            // 开始解码

            yuv.deleteOnExit()
            yuv.createNewFile()
            val moutpath = yuv.absolutePath
            Log.e("gpf", moutpath)
            FFmpeg.decodeVideo(mVideoPath, moutpath)
        } else {
            PermissionX.init(this)
                .permissions(Manifest.permission.READ_EXTERNAL_STORAGE)
                .request { allGranted, grantedList, deniedList ->
                    if (allGranted) {
                        // 开始解码
                        yuv.deleteOnExit()
                        yuv.createNewFile()
                        val moutpath = yuv.absolutePath
                        Log.e("gpf", moutpath)
                        FFmpeg.decodeVideo(mVideoPath, moutpath)
                    } else {
                        Toast.makeText(
                            this,
                            "These permissions are denied: $deniedList",
                            Toast.LENGTH_LONG
                        ).show()
                    }
                }
        }

    }

}