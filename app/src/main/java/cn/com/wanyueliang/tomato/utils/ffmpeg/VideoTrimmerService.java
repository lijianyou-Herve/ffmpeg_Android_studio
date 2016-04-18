package cn.com.wanyueliang.tomato.utils.ffmpeg;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.os.RemoteCallbackList;
import android.os.RemoteException;
import android.util.Log;

public class VideoTrimmerService extends Service {

	private static final String TAG = "MediaMixerService";

	static {
		System.loadLibrary("ffmpeg_bjx");
	}

	final RemoteCallbackList<IOnFinishListener> listeners = new RemoteCallbackList<IOnFinishListener>();

	public class VideoTrimmerManager extends IVideoTrimmerManager.Stub {

		@Override
		public void trim(final int cmdType, final String inputFile, final String outputFile, final String start, final String duration) throws RemoteException {
			new Thread(new Runnable() {

				@Override
				public void run() {
					VideoTrimmerService.this.trim(cmdType, inputFile, outputFile, start, duration);
				}
			}).start();

		}

		@Override
		public void setOnFinishListener(IOnFinishListener listener) throws RemoteException {
			listeners.register(listener);
		}
	}

	public void onNativeCrash(int res) {
		final int N = listeners.beginBroadcast();
		for (int i = 0; i < N; i++) {
			try {
				listeners.getBroadcastItem(i).onFinish(res == 0);
			} catch (RemoteException e) {
				Log.e(TAG, "mixMedia", e);
			}
		}
		listeners.finishBroadcast();
		/*try {
			Thread.sleep(5000);
		} catch (InterruptedException e) {
			Log.e(TAG, "mixMedia", e);
		}
		Log.e(TAG, "kill");
		stopSelf();
		android.os.Process.killProcess(android.os.Process.myPid());*/
	}

	@Override
	public void onCreate() {
		super.onCreate();
	}

	@Override
	public IBinder onBind(Intent intent) {
		return new VideoTrimmerManager();
	}

	@Override
	public int onStartCommand(Intent intent, int flags, int startId) {
		super.onStartCommand(intent, flags, startId);
		return START_NOT_STICKY;
	}

	@Override
	public void onDestroy() {
		super.onDestroy();
	}

	private native int trim(int cmdType, String inputFile, String outputFile, String start, String duration);
}
