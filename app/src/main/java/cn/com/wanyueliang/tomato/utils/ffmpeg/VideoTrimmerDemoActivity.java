package cn.com.wanyueliang.tomato.utils.ffmpeg;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;

public class VideoTrimmerDemoActivity extends Activity {

	public static final String TAG = "BJX";

	VideoTrimmer mVideoTrimmer;

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main);
		mVideoTrimmer = new VideoTrimmer(this);

		findViewById(R.id.startBtn).setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				new Thread(new Runnable() {

					@SuppressLint("SdCardPath")
					@Override
					public void run() {
					
						String input = "/storage/emulated/0/haitao.mp4";
						String output = "/storage/emulated/0/haitao_crop_001.mp4";
//						boolean result = mVideoTrimmer.trim(input,output,"0.3","3.1");
//						String output = "/storage/emulated/0/haitao_crop_002.mp4";
						boolean result = mVideoTrimmer.trim(1, input,output,"3.4","3.3");

						if (result) {
							Log.e(TAG, "OK");
						} else {
							Log.e(TAG, "NG");
						}
					}
				}).start();
			}
		});
	}

	@Override
	public void onDestroy() {
		super.onDestroy();
	}
}