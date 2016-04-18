package cn.com.wanyueliang.tomato.utils.ffmpeg;

import android.app.Activity;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.IBinder;
import android.os.RemoteException;
import android.util.Log;

public class VideoTrimmer {

    private static final String TAG = "VideoTrimmer";
    private boolean sucessed = false;
    private final Activity activity;

    private IVideoTrimmerManager mVideoTrimmerManager;
    private final FinishListener mFinishListener;
    private int cmdType;
    private String inputFile;
    private String outputFile;
    private String start;
    private String duration;

    ///*编辑.h时用这个，不使用这个，生成.so的时候使用*/
//    static {
//        System.loadLibrary("bjx");    //defaultConfig.ndk.moduleName
//    }

    private final ServiceConnection mServiceConn = new ServiceConnection() {

        @Override
        public void onServiceConnected(ComponentName name, IBinder service) {
            mVideoTrimmerManager = IVideoTrimmerManager.Stub.asInterface(service);
            try {
                mVideoTrimmerManager.setOnFinishListener(mFinishListener);
            } catch (RemoteException e) {
                Log.i(TAG, "", e);
                try {
                    mFinishListener.onFinish(false);
                } catch (RemoteException e1) {
                    Log.i(TAG, "", e1);
                }
            }
            try {
                mVideoTrimmerManager.trim(cmdType, inputFile, outputFile, start, duration);
            } catch (RemoteException e) {
                Log.i(TAG, "", e);
                try {
                    mFinishListener.onFinish(false);
                } catch (RemoteException e1) {
                    Log.i(TAG, "", e1);
                }
            }
        }

        @Override
        public void onServiceDisconnected(ComponentName name) {
            mVideoTrimmerManager = null;
            try {
                mFinishListener.onFinish(false);
            } catch (RemoteException e1) {
                Log.i(TAG, "", e1);
            }
        }

    };

    private class FinishListener extends IOnFinishListener.Stub {

        @Override
        public void onFinish(boolean successed) throws RemoteException {
            activity.unbindService(mServiceConn);
            synchronized (mServiceConn) {
                sucessed = successed;
                mServiceConn.notify();
            }
        }
    }

    public VideoTrimmer(Activity activity) {
        this.activity = activity;
        mFinishListener = new FinishListener();
    }

//    /*编辑.h时用这个，把另一个注销*/
//        public native boolean trim(int cmdType, String inputFile, String outputFile, String start, String duration);
    public boolean trim(int cmdType, String inputFile, String outputFile, String start, String duration) {
        this.cmdType = cmdType;
        this.inputFile = inputFile;
        this.outputFile = outputFile;
        this.start = start;
        this.duration = duration;
        sucessed = false;
        activity.bindService(new Intent(VideoTrimmer.this.activity, VideoTrimmerService.class), mServiceConn, Context.BIND_AUTO_CREATE);
        synchronized (mServiceConn) {
            try {
                mServiceConn.wait();
            } catch (InterruptedException e) {
                Log.e(TAG, "", e);
                activity.unbindService(mServiceConn);
                return false;
            }
        }
        return sucessed;
    }

}