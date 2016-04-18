#include <stdio.h>
#include <unistd.h>
extern "C" {
#include <ffmpeg/include/libavutil/opt.h>
#include <ffmpeg/include/libavcodec/avcodec.h>
#include <ffmpeg/include/libavutil/channel_layout.h>
#include <ffmpeg/include/libavutil/common.h>
#include <ffmpeg/include/libavutil/imgutils.h>
#include <ffmpeg/include/libavutil/mathematics.h>
#include <ffmpeg/include/libavutil/samplefmt.h>
#include <ffmpeg/include/libavcodec/avcodec.h>
#include <ffmpeg/include/libavformat/avformat.h>
//#include <libswscale/swscale.h>
#include <ffmpeg/include/libavutil/pixfmt.h>
#include <ffmpeg/include/libswresample/swresample.h>
}
#include <jni.h>

#include <android/log.h>
#include <stdio.h>
#define TAG "VideoTrimmer-JNI"
#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG ,TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO  ,TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN  ,TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR ,TAG, __VA_ARGS__)

#include "cn_com_wanyueliang_tomato_utils_ffmpeg_VideoTrimmer.h"
#include "ffmpeg/include/libavutil/log.h"

extern "C" void register_exit_callback(void (*cb)(int ret));
extern "C" int ffmpeg_main(int argc, char **argv);
JavaVM *myVm = NULL;
jobject g_obj = NULL;

extern "C" jint JNIEXPORT JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
	myVm = vm;
	return JNI_VERSION_1_4;
}

void exitcallback(int a) {
	JNIEnv *g_env;
	myVm->AttachCurrentThread(&g_env, NULL);
	jclass cls = g_env->GetObjectClass(g_obj);
	jmethodID methodId = g_env->GetMethodID(cls, "onNativeCrash", "(I)V");
	if (NULL == methodId) {
		return;
	}
	g_env->CallVoidMethod(g_obj, methodId, a);
	g_env->DeleteLocalRef(cls);
	g_env->DeleteGlobalRef(g_obj);
	int check = myVm->GetEnv((void**) &g_env, JNI_VERSION_1_6);
	if (check != JNI_EDETACHED) {
		//int detach = myVm->DetachCurrentThread();
	}
}

extern "C" void av_log_callback(void* ptr, int level, const char* fmt,
		va_list vl) {
	//LOGE(fmt,vl);
}

void log_message(const char* message) {
	__android_log_write(ANDROID_LOG_ERROR, "VideoTrimmer", message);
}

void reverse(char s[]) {
	int i, j;
	char c;

	for (i = 0, j = strlen(s) - 1; i < j; i++, j--) {
		c = s[i];
		s[i] = s[j];
		s[j] = c;
	}
}

/* itoa:  convert n to characters in s */
void itoa(int n, char s[]) {
	int i, sign;

	if ((sign = n) < 0) /* record sign */
		n = -n; /* make n positive */
	i = 0;
	do { /* generate digits in reverse order */
		s[i++] = n % 10 + '0'; /* get next digit */
	} while ((n /= 10) > 0); /* delete it */
	if (sign < 0)
		s[i++] = '-';
	s[i] = '\0';
	reverse(s);
}

JNIEXPORT jint JNICALL Java_cn_com_wanyueliang_tomato_utils_ffmpeg_VideoTrimmerService_trim(
		JNIEnv *env, jobject obj, jint cmdType, jstring inputFile,
		jstring outFile, jstring startTime, jstring length) {

	g_obj = env->NewGlobalRef(obj);
	av_log_set_callback(av_log_callback);
	register_exit_callback(exitcallback);

	log_message("--------FFmpeg Start---------");

	int numberOfArgs = 15;

	if (cmdType == 1) {
		numberOfArgs = 15;
	} else if (cmdType == 2) {
		numberOfArgs = 13;
	}

	char** arguments = (char**)calloc(numberOfArgs, sizeof(char*));
	char start[5];
	const char *in, *out, *cropstart, *duration;

	in = env->GetStringUTFChars(inputFile, 0);
	out = env->GetStringUTFChars(outFile, 0);
	cropstart = env->GetStringUTFChars(startTime, 0);
	duration = env->GetStringUTFChars(length, 0);

	//ffmpeg -ss 0.0 -i in -t 3.0   -vcodec copy  -acodec copy -y out
	//ffmpeg -ss 0.0 -t 3.0 -i in -qscale 0 -async 1 -strict -2 -y out

	if (cmdType == 1) {
		arguments[0] = (char*)"ffmpeg";
		arguments[1] = (char*)"-ss";
		arguments[2] = (char*)cropstart;
		arguments[3] = (char*)"-t";
		arguments[4] = (char*)duration;
		arguments[5] = (char*)"-i";
		arguments[6] = (char*)in;
		arguments[7] = (char*)"-qscale";
		arguments[8] = (char*)"0";
		arguments[9] = (char*)"-async";
		arguments[10] =(char*) "1";
		arguments[11] =(char*) "-strict";
		arguments[12] =(char*) "-2";
		arguments[13] =(char*) "-y";
		arguments[14] =(char*) out;
	} else if (cmdType == 2) {
		arguments[0] =(char*)"ffmpeg";
		arguments[1] =(char*)"-ss";
		arguments[2] =(char*)cropstart;
		arguments[3] =(char*)"-i";
		arguments[4] =(char*)in;
		arguments[5] =(char*)"-t";
		arguments[6] =(char*)duration;
		arguments[7] =(char*)"-vcodec";
		arguments[8] =(char*)"copy";
		arguments[9] =(char*)"-acodec";
		arguments[10] =(char*) "copy";
		arguments[11] =(char*) "-y";
		arguments[12] =(char*) out;
	}

//	arguments[7] = duration;
//	arguments[8] = "-vcodec";
//	arguments[9] = "copy";
//	arguments[10] = "-acodec";
//	arguments[11] = "copy";
//	arguments[12] = out;
//	arguments[11] = "-strict";
//	arguments[12] = "experimental";
//	arguments[13] = "-ab";
//	arguments[14] = "12k";
//  arguments[11] = "-metadata";
//  arguments[12] = "rotate=0";

	int i;
	for (i = 0; i < numberOfArgs; i++) {
		log_message(arguments[i]);
	}
	log_message("--------FFmpeg End---------");

	ffmpeg_main(numberOfArgs, arguments);
	free(arguments);
	env->ReleaseStringUTFChars(inputFile, in);
	env->ReleaseStringUTFChars(outFile, out);
	env->ReleaseStringUTFChars(startTime, cropstart);
	env->ReleaseStringUTFChars(length, duration);
	return true;
}
