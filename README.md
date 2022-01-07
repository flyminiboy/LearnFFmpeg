## MAC编译FFmpeg移植到Android

### 编译环境

* macOS Monterey 版本12.1(21C52)
* ffmpeg version 4.4.1
* android-ndk-r21-darwin-x86_64

### 下载NDK

[NDK最新版本](https://developer.android.com/ndk/downloads)

[NDK历史版本](https://github.com/android/ndk/wiki/Unsupported-Downloads)


### 下载FFmpeg

[FFmpeg](https://evermeet.cx/ffmpeg/)

下载好源码后，进入根目录，找到一个名为 **congfigure** 的文件，这是一个shell脚本，用于生成一些 FFmpeg 编译需要的配置文件。

> 网上很多教程会对此文件进行修改，但是3.4版本后不需要更改configure文件内容，下载解压后就ok，主要是后面脚本配置两步

### 编辑编译脚本

在源码根目录新建 **build_android.sh**

**Android 的 NDK 已经迭代了很多版本，在 r17c 以后，Google正式移除 GCC ，不再支持 GCC ，新版本的 NDK 都是使用 CLANG 进行编译。**

#### 查看编译工具目录

```
/Users/guopengfei/Library/Android/sdk/ndk/21.4.7075529/toolchains/llvm/prebuilt/darwin-x86_64/bin
```

**注意上述路径的NDK目录切换为自己本地路径**

根据不同的CPU架构区和不同的Android版本，区分了不同的clang工具，根据自己需要选择就好了。

本文示例选择的是CPU 架构 armv7a和arm64-v8a，Android版本 21

##### armv7a
```
armv7a-linux-androideabi21-clang   
armv7a-linux-androideabi21-clang++
```

##### arm64-v8a

```
aarch64-linux-android21-clang      
aarch64-linux-android21-clang++
```

**其他相关的注意事项都已经在编译脚本的注释里面说明了**

```
#!/bin/bash

export NDK=/Users/guopengfei/Library/Android/sdk/ndk/21.4.7075529 #这里配置你的 NDK 路径
# 编译工具链目录
TOOLCHAIN=$NDK/toolchains/llvm/prebuilt/darwin-x86_64 #这个地方需要注意，网上好多教程都写的是 linux-x86_64，这个地方需要和自己编译平台相匹配，网上的脚本是Linux平台下面运行的，我们这个是在MAC平台运行的所以指定为 darwin-x86_64

function build_android
{

# FFmpeg 配置修改开启相关选项 可以参考一下 ijkplayer
./configure \
--prefix=$PREFIX \
--enable-neon  \
--enable-hwaccels  \
--disable-postproc \
--disable-debug \
--enable-small \
--enable-jni \
--enable-mediacodec \
--enable-decoder=h264_mediacodec \
--enable-static \
--enable-shared \
--disable-doc \
--enable-ffmpeg \
--disable-ffplay \
--disable-ffprobe \
--disable-avdevice \
--disable-doc \
--disable-symver \
--cross-prefix=$CROSS_PREFIX \
--target-os=android \
--arch=$ARCH \
--cpu=$CPU \
--cc=$CC \
--cxx=$CXX \
--enable-cross-compile \
--sysroot=$SYSROOT \
--extra-cflags="-Os -fpic $OPTIMIZE_CFLAGS" \
--extra-ldflags="$ADDI_LDFLAGS"

# 清空上次编译结果
make clean
# 指定编译开启核 16核 -j4 4核 
make -j16
# 安装
make install

echo "============================ build android $CPU success =========================="

}


#arm64-v8a
ARCH=arm64
CPU=armv8-a
API=21
CC=$TOOLCHAIN/bin/aarch64-linux-android$API-clang
CXX=$TOOLCHAIN/bin/aarch64-linux-android$API-clang++
# sysroot被称为逻辑根目录，只在链接过程中起作用，作为交叉编译工具链搜索库文件的根路径，如配置--sysroot=dir，则dir作为逻辑根目录，链接器将在dir/usr/lib中搜索库文件。
# 这个选项是用来设置目标平台根目录的
#                未设置--sysroot	   设置了--sysroot=dir后
# 头文件搜索路径	  /usr/include	     dir/usr/include
# 依赖库搜索路径	  /usr/lib	         dir/usr/lib
# 接下来，您需要定义自己的 sysroot。sysroot 是包含您的目标系统头文件及库的目录。若要定义 sysroot，您必须知道原生支持的目标 Android API 级别；而可用的原生 API 会因 Android API 级别而异。

# 针对相应 Android API 级别的原生 API 库位于 $NDK/platforms/ 下；
# 每个 API 级别目录又包含针对各种 CPU 和架构的子目录。标头位于 $NDK/sysroot。
# 交叉编译环境目录
SYSROOT=$NDK/toolchains/llvm/prebuilt/darwin-x86_64/sysroot
# AArch64是Armv8-A架构（https://en.wikipedia.org/wiki/ARM_architecture#ARMv8-A）中引入的64位状态。
# 向后兼容Armv7-A和先前的32位Arm架构的32位状态称为AArch32。
# 因此，用于64位ISA的GNU gcc 是aarch64。Linux内核社区选择将其内核端口称为该体系结构arm64，而不是aarch64，因此这是一些arm64用法的来源。
# 据我所知，用于aarch64的Apple后端称为arm64，而LLVM 编译器社区开发的后端称为aarch64（因为它是64位ISA的规范名称），后来将arm64和 aarch64 两者合并，现在的后端称为aarch64。 。
# 因此aarch64和arm64指的是同一件事。
CROSS_PREFIX=$TOOLCHAIN/bin/aarch64-linux-android-
PREFIX=$(pwd)/android/$CPU
OPTIMIZE_CFLAGS="-march=$CPU"
build_android

#armv7-a
ARCH=arm
CPU=armv7-a
API=21
CC=$TOOLCHAIN/bin/armv7a-linux-androideabi$API-clang
CXX=$TOOLCHAIN/bin/armv7a-linux-androideabi$API-clang++
SYSROOT=$NDK/toolchains/llvm/prebuilt/darwin-x86_64/sysroot
CROSS_PREFIX=$TOOLCHAIN/bin/arm-linux-androideabi-
PREFIX=$(pwd)/android/$CPU
OPTIMIZE_CFLAGS="-mfloat-abi=softfp -mfpu=vfp -marm -march=$CPU "
build_android

```

关于配置再说一下如何设置各个选项开启和关闭的状态

举例说明

```
--enable-static  // 开启静态链接库
--disable-shared // 关闭动态链接库
```

没错，就是**enable**和**disable**

### 编译

#### 添加执行权限

```
chmod +x build_android.sh 
```

#### 编译

```
./build_android.sh
```

编译成功以后会生成一个**Android**文件夹，里面包含上面脚本里面编译的CPU架构的文件夹，比如当前示例得到俩个**armv7-a**，**armv8-a**。

里面包括**include**存放头文件的，**lib**存放静态库和动态库

### 集成到Android

* 新建一个Android NDK项目

> New Project->Native C++

* 编译好的库导入到项目

将**include**拷贝到**cpp**目录下面

关于动态库的放置位置，需要特殊注意一下。具体参考如下

[自动打包 CMake 使用的预构建依赖项](https://developer.android.com/studio/releases/gradle-plugin#cmake-imported-targets)

早期版本的 Android Gradle 插件要求您使用 jniLibs 明确打包您的 CMake 外部原生 build 使用的所有预构建库。您的库可能位于模块的 src/main/jniLibs 目录中，也可能位于在 build.gradle 文件中配置的某个其他目录中：

```
sourceSets {
    main {
        // The libs directory contains prebuilt libraries that are used by the
        // app's library defined in CMakeLists.txt via an IMPORTED target.
        jniLibs.setSrcDirs(listOf("libs"))
    }
}
```

有了 Android Gradle 插件 4.0，上述配置不再是必需的，并且会导致构建失败：

```
* What went wrong:
Execution failed for task ':app:mergeDebugNativeLibs'.
> A failure occurred while executing com.android.build.gradle.internal.tasks.Workers$ActionFacade
   > More than one file was found with OS independent path 'lib/x86/libprebuilt.so'
```

外部原生 build 现在会自动打包这些库，因此使用 jniLibs 明确打包库会导致重复。为了避免构建错误，请将预构建库移至 jniLibs 之外的位置，或从 build.gradle 文件中移除 jniLibs 配置。

基于以上原因，我们新建**nativeLibs**目录，将**lib**目录下面的以**.so**结尾的动态库拷贝到**nativeLibs**目录

### 配置CMakeLists.txt

关于**CMake**的配置可以参考Android开发者网站的用户指南

[CMake](https://developer.android.com/studio/projects/configure-cmake)

```
# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.10.2)

# 支持gnu++11
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")

# 1. 定义so库和头文件所在目录，方面后面使用
set(ffmpeg_lib_dir ${CMAKE_SOURCE_DIR}/../nativeLibs/${ANDROID_ABI})

# 为了让 CMake 能够在编译时找到头文件，您需要使用 include_directories() 命令并包含相应头文件的路径
include_directories(
        include
        ${CMAKE_SOURCE_DIR}/util
)

# Declares and names the project.

project("ffmpeg")

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

# 添加预构建库的步骤与为 CMake 指定其他要构建的原生库的步骤相似。
# 不过，由于库已构建，因此您需要使用 IMPORTED 标志指示 CMake 您只想要将此库导入到您的项目中
# 3. 添加ffmpeg相关的so库
add_library( avutil
        SHARED
        IMPORTED )
# 然后，您需要使用 set_target_properties() 命令指定库的路径
set_target_properties( avutil
        PROPERTIES IMPORTED_LOCATION
        ${ffmpeg_lib_dir}/libavutil.so )

add_library( swresample
        SHARED
        IMPORTED )
set_target_properties( swresample
        PROPERTIES IMPORTED_LOCATION
        ${ffmpeg_lib_dir}/libswresample.so )

add_library( avcodec
        SHARED
        IMPORTED )
set_target_properties( avcodec
        PROPERTIES IMPORTED_LOCATION
        ${ffmpeg_lib_dir}/libavcodec.so )
add_library( avformat
        SHARED
        IMPORTED)
set_target_properties( avformat
        PROPERTIES IMPORTED_LOCATION
        ${ffmpeg_lib_dir}/libavformat.so )
add_library( avfilter
        SHARED
        IMPORTED)
set_target_properties( avfilter
        PROPERTIES IMPORTED_LOCATION
        ${ffmpeg_lib_dir}/libavfilter.so )

add_library( swscale
        SHARED
        IMPORTED)
set_target_properties( swscale
        PROPERTIES IMPORTED_LOCATION
        ${ffmpeg_lib_dir}/libswscale.so )

add_library( # Sets the name of the library.
        ffmpeg

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        native-lib.cpp)

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

# 将预构建库关联到自己的原生库
target_link_libraries( # Specifies the target library.
        ffmpeg

        # 4. 连接 FFmpeg 相关的库
        avformat
        avcodec
        swresample
        avutil
        avfilter
        swscale

        android
        OpenSLES

        # Links the target library to the log library
        # included in the NDK.
        ${log-lib})
```





