package cn.com.wanyueliang.tomato.utils.ffmpeg;

import cn.com.wanyueliang.tomato.utils.ffmpeg.IOnFinishListener;

interface IVideoTrimmerManager
{
    void trim(int cmdType, String inputFile, String outputFile, String start, String duration);
    void setOnFinishListener(IOnFinishListener listener);
}