/*
 * MainActivity.java
 *
 * Copyright (c) 2013, Thomas Buck <xythobuz@me.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
package org.xythobuz.xycopter;

import java.io.IOException;
import java.text.DecimalFormat;
import java.text.DecimalFormatSymbols;
import java.util.Set;
import java.util.UUID;

import android.app.Activity;
import android.app.AlertDialog;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.res.Configuration;
import android.graphics.Color;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.text.Editable;
import android.text.TextWatcher;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.inputmethod.EditorInfo;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.ScrollView;
import android.widget.TextView;

import com.dropbox.client2.DropboxAPI;
import com.dropbox.client2.android.AndroidAuthSession;
import com.dropbox.client2.session.AccessTokenPair;
import com.dropbox.client2.session.AppKeyPair;
import com.dropbox.client2.session.Session.AccessType;
import com.jjoe64.graphview.GraphView;
import com.jjoe64.graphview.GraphView.GraphViewData;
import com.jjoe64.graphview.GraphView.LegendAlign;
import com.jjoe64.graphview.GraphViewSeries;
import com.jjoe64.graphview.GraphViewSeries.GraphViewSeriesStyle;
import com.jjoe64.graphview.LineGraphView;

public class MainActivity extends Activity implements OnClickListener {

	public final static int REQUEST_ENABLE_BT = 1;
	public final static int MESSAGE_READ = 2;
	public final static int MESSAGE_READ_FAIL = 3;
	public final static int MESSAGE_WRITE_FAIL = 4;
	public final static int MESSAGE_BLUETOOTH_CONNECTED = 5;
	public final static int MESSAGE_BLUETOOTH_CONNECTION_FAIL = 6;
	public final static int MESSAGE_ROLL_READ = 7;
	public final static int MESSAGE_PITCH_READ = 8;
	public final static int MESSAGE_YAW_READ = 9;
	public final static int MESSAGE_VOLT_READ = 10;
	public final static int MESSAGE_MOTOR_READ = 11;
	public final static int MESSAGE_PID_READ = 12;
	public final static int MESSAGE_PIDVAL_READ = 13;
	public final static int MESSAGE_PIDRANGE_READ = 14;
	public final static int MESSAGE_HEX_ERROR = 15;
	public final static int MESSAGE_DROPBOX_FAIL = 16;
	public final static int MESSAGE_ERROR = 17;
	public final static int MESSAGE_HEX_PARSED = 18;
	public final static int MESSAGE_ALERT_DIALOG = 19;
	public final static int MESSAGE_PIDINTRANGE_READ = 20;
	public final static int MESSAGE_FREQ_READ = 21;
	public final static int MESSAGE_ANGLES_READ = 22;

	private final static String APP_KEY = "gnbnnowfgpv5jej";
	private final static String APP_SECRET = "uxy6uf661xyd46q";
	private final static AccessType ACCESS_TYPE = AccessType.DROPBOX;
	public DropboxAPI<AndroidAuthSession> mDBApi;

	private final static String PREFS_NAME = "xyDropboxPrefs";
	private final static String PREF_KEY = "DropboxKey";
	private final static String PREF_SECRET = "DropboxSecret";

	public final static UUID BLUETOOTH_UUID = UUID
			.fromString("00001101-0000-1000-8000-00805F9B34FB"); // Default SPP
																	// UUID

	public boolean connected = false;
	private BluetoothAdapter bluetoothAdapter = null;
	private BluetoothDevice pairedDevice = null;
	private BluetoothSocket socket = null;
	private ConnectedThread connectedThread = null;
	private GraphView graphView = null;

	public final double STATUSFREQ = 10;
	public final double FLIGHTFREQ = 100;

	private static int SERIES_ROLL = 0;
	private static int SERIES_PITCH = 1;
	private static int SERIES_YAW = 2;
	private static int SERIES_PIDROLL = 3;
	private static int SERIES_PIDPITCH = 4;
	private static int SERIES_M1 = 5;
	// private static int SERIES_M2 = 6;
	// private static int SERIES_M3 = 7;
	// private static int SERIES_M4 = 8;
	private static int SERIES_VPITCH = 9;
	private static int SERIES_VROLL = 10;
	private static int SERIES_VYAW = 11;
	private GraphViewSeries[] graphViewSeries = new GraphViewSeries[12];
	GraphViewData[][] graphViewDatas = new GraphViewData[12][];
	private String[] graphViewName = { "Roll", "Pitch", "Yaw", "PID-Roll",
			"PID-Pitch", "M1", "M2", "M3", "M4", "vP", "vR", "vY" };
	private GraphViewSeriesStyle[] graphViewStyle = {
			new GraphViewSeriesStyle(Color.RED, 2),
			new GraphViewSeriesStyle(Color.GREEN, 2),
			new GraphViewSeriesStyle(Color.BLUE, 2),
			new GraphViewSeriesStyle(Color.MAGENTA, 2),
			new GraphViewSeriesStyle(Color.YELLOW, 2),
			new GraphViewSeriesStyle(Color.BLACK, 1),
			new GraphViewSeriesStyle(Color.CYAN, 1),
			new GraphViewSeriesStyle(Color.GRAY, 1),
			new GraphViewSeriesStyle(Color.WHITE, 1),
			new GraphViewSeriesStyle(Color.LTGRAY, 1),
			new GraphViewSeriesStyle(Color.LTGRAY, 1),
			new GraphViewSeriesStyle(Color.LTGRAY, 1)};
	private double graphIncrement = 1 / STATUSFREQ;
	private double graphX = graphIncrement;

	private final String ParameterCommand = "n";
	private final byte[] commands = { 'a', 'w', 's', 'd', 'x', 'y', 'p', 'm',
			'q', 'z', 'o' };
	private final Button[] buttons = new Button[commands.length];
	private final static int B_LEFT = 0;
	private final static int B_FORW = 1;
	private final static int B_BACK = 2;
	private final static int B_RIGHT = 3;
	private final static int B_UP = 4;
	private final static int B_DOWN = 5;
	private final static int B_ANGLES = 6;
	private final static int B_TOGGLE = 7;
	private final static int B_RESET = 8;
	private final static int B_ZERO = 9;
	private final static int B_SILENT = 10;

	private TestGraphThread testGraphThread = null;
	private FlashThread flashThread = null;
	private YASAB yasab = null;

	private String lastKnownP = "5.0";
	private String lastKnownI = "0.0013";
	private String lastKnownD = "-13.0";
	private String lastKnownMin = "-32768";
	private String lastKnownMax = "32767";
	private String lastKnownIntMin = "-32768";
	private String lastKnownIntMax = "32767";

	public Handler handler = new Handler(new Handler.Callback() {
		public boolean handleMessage(Message msg) {
			messageHandler(msg);
			return true;
		}
	});

	@Override
	protected void onDestroy() {
		super.onDestroy();
		if (socket != null) {
			if (connectedThread != null) {
				connectedThread.interrupt();
				connectedThread = null;
			}
			try {
				socket.close();
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);

		bluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
		if (bluetoothAdapter == null) {
			// Device does not support Bluetooth
			showErrorAndExit(R.string.bluetooth_error_title,
					R.string.bluetooth_no_adapter);
		}

		if (!bluetoothAdapter.isEnabled()) {
			Intent enableBtIntent = new Intent(
					BluetoothAdapter.ACTION_REQUEST_ENABLE);
			startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT);
		} else {
			final TextView t = (TextView) findViewById(R.id.intro_text);
			t.setText(t.getText() + "\n" + getString(R.string.intro_wait));
			final ScrollView s = (ScrollView) findViewById(R.id.intro_scroll);
			s.post(new Runnable() {
				public void run() {
					s.smoothScrollTo(0, t.getBottom());
				}
			});
		}

		buttons[B_LEFT] = (Button) findViewById(R.id.bLeft);
		buttons[B_FORW] = (Button) findViewById(R.id.bForw);
		buttons[B_BACK] = (Button) findViewById(R.id.bBack);
		buttons[B_RIGHT] = (Button) findViewById(R.id.bRight);
		buttons[B_UP] = (Button) findViewById(R.id.bUp);
		buttons[B_DOWN] = (Button) findViewById(R.id.bDown);
		buttons[B_ANGLES] = (Button) findViewById(R.id.bAng);
		buttons[B_TOGGLE] = (Button) findViewById(R.id.bTog);
		buttons[B_RESET] = (Button) findViewById(R.id.bReset);
		buttons[B_ZERO] = (Button) findViewById(R.id.bZero);
		buttons[B_SILENT] = (Button) findViewById(R.id.bSilent);
		for (int i = 0; i < buttons.length; i++) {
			buttons[i].setOnClickListener(this);
		}

		graphView = new LineGraphView(this, "xyCopter Data Dump");
		for (int i = 0; i < graphViewDatas.length; i++) {
			graphViewDatas[i] = new GraphViewData[1];
			graphViewDatas[i][0] = new GraphViewData(0.0, 0.0);
			graphViewSeries[i] = new GraphViewSeries(graphViewName[i],
					graphViewStyle[i], graphViewDatas[i]);
			graphView.addSeries(graphViewSeries[i]);
		}
		graphView.setScrollable(true);
		graphView.setScalable(true);
		graphView.setShowLegend(true);
		graphView.setLegendAlign(LegendAlign.BOTTOM);
		graphView.setViewPort(0.0, 5.0);
		graphView.setCenterZero(true);
		LinearLayout layout = (LinearLayout) findViewById(R.id.graphLayout);
		layout.addView(graphView);

		AppKeyPair appKeys = new AppKeyPair(APP_KEY, APP_SECRET);
		AndroidAuthSession session = new AndroidAuthSession(appKeys,
				ACCESS_TYPE);
		mDBApi = new DropboxAPI<AndroidAuthSession>(session);
	}

	@Override
	protected void onResume() {
		super.onResume();
		if (mDBApi.getSession().authenticationSuccessful()) {
			try {
				mDBApi.getSession().finishAuthentication();
				AccessTokenPair tokens = mDBApi.getSession()
						.getAccessTokenPair();
				storeKeys(tokens.key, tokens.secret);
				flashFirmware();
			} catch (IllegalStateException e) {
				showErrorAndDo(R.string.dropbox_title, e.getMessage(), null);
			}
		}
	}

	@Override
	public void onConfigurationChanged(Configuration newConfig) {
		super.onConfigurationChanged(newConfig);
	}

	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data) {
		if (requestCode == REQUEST_ENABLE_BT) {
			if (resultCode != Activity.RESULT_OK) {
				showErrorAndExit(R.string.bluetooth_error_title,
						R.string.bluetooth_turned_off);
			} else {
				final TextView t = (TextView) findViewById(R.id.intro_text);
				t.setText(t.getText() + "\n" + getString(R.string.intro_wait));
				final ScrollView s = (ScrollView) findViewById(R.id.intro_scroll);
				s.post(new Runnable() {
					public void run() {
						s.smoothScrollTo(0, t.getBottom());
					}
				});
			}
		}
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		MenuInflater inflater = getMenuInflater();
		inflater.inflate(R.menu.activity_main, menu);
		return true;
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		final TextView intro = (TextView) findViewById(R.id.intro_text);
		final ScrollView s = (ScrollView) findViewById(R.id.intro_scroll);

		switch (item.getItemId()) {
		case R.id.clear:
			intro.setText("");
			s.post(new Runnable() {
				public void run() {
					s.smoothScrollTo(0, intro.getBottom());
				}
			});
			for (int i = 0; i < graphViewSeries.length; i++) {
				graphViewDatas[i][0] = new GraphViewData(0.0, 0.0);
				graphViewSeries[i].resetData(graphViewDatas[i]);
			}
			return true;
		case R.id.parameters:
			displayPIDAlert();
			return true;
		case R.id.disconnect:
		case R.id.connect:
			if (connectedThread != null) {
				connectedThread.cancel();
				connectedThread.interrupt();
				connectedThread = null;
				intro.setText(intro.getText() + "\n"
						+ getString(R.string.intro_disconnect));
				s.post(new Runnable() {
					public void run() {
						s.smoothScrollTo(0, intro.getBottom());
					}
				});
			} else {
				startConnection(new Function() {
					public void execute() {
						newConnectThread();
					}
				});
			}
			return true;
		case R.id.testgraph:
			if (testGraphThread == null) {
				testGraphThread = new TestGraphThread(this);
				testGraphThread.start();
			} else {
				testGraphThread.cancel();
				testGraphThread.interrupt();
				testGraphThread = null;
			}
			return true;
		case R.id.firmware:
			AccessTokenPair atp = getStoredKeys();
			if (atp == null) {
				mDBApi.getSession().startAuthentication(this);
			} else {
				mDBApi.getSession().setAccessTokenPair(atp);
				flashFirmware();
			}
			return true;
		default:
			return super.onOptionsItemSelected(item);
		}
	}

	private void storeKeys(String key, String secret) {
		SharedPreferences settings = getSharedPreferences(PREFS_NAME, 0);
		SharedPreferences.Editor editor = settings.edit();
		editor.putString(PREF_KEY, key);
		editor.putString(PREF_SECRET, secret);
		editor.commit();
	}

	private AccessTokenPair getStoredKeys() {
		SharedPreferences settings = getSharedPreferences(PREFS_NAME, 0);
		String key = settings.getString(PREF_KEY, null);
		String secret = settings.getString(PREF_SECRET, null);
		if ((key != null) && (secret != null)) {
			AccessTokenPair atp = new AccessTokenPair(key, secret);
			return atp;
		} else {
			return null;
		}
	}

	private void flashFirmware() {
		flashThread = new FlashThread(this);
		flashThread.start();
	}

	private void startConnection(final Function f) {
		// List paired devices
		Set<BluetoothDevice> pairedDev = bluetoothAdapter.getBondedDevices();
		// If there are paired devices
		if (pairedDev.size() > 1) {
			// Loop through paired devices
			final BluetoothDevice[] pairedDevices = (BluetoothDevice[]) pairedDev
					.toArray(new BluetoothDevice[0]);
			String[] pairedName = new String[pairedDev.size()];
			for (int i = 0; i < pairedDev.size(); i++) {
				pairedName[i] = pairedDevices[i].getName() + " (" + pairedDevices[i].getAddress()
						+ ")";
			}

			// List AlertDialog
			AlertDialog.Builder builder = new AlertDialog.Builder(this);
			builder.setTitle(R.string.bluetooth_select);
			builder.setCancelable(false);
			builder.setItems(pairedName, new DialogInterface.OnClickListener() {
				public void onClick(DialogInterface dialog, int which) {
					final TextView intro = (TextView) findViewById(R.id.intro_text);
					intro.setText(intro.getText() + "\n"
							+ getString(R.string.intro_connect));
					final ScrollView s = (ScrollView) findViewById(R.id.intro_scroll);
					s.post(new Runnable() {
						public void run() {
							s.smoothScrollTo(0, intro.getBottom());
						}
					});
					pairedDevice = pairedDevices[which];
					f.execute();
				}
			});
			builder.show();
		} else if (pairedDev.size() > 0) {
			final TextView t = (TextView) findViewById(R.id.intro_text);
			t.setText(t.getText() + "\n" + getString(R.string.intro_connect));
			final ScrollView s = (ScrollView) findViewById(R.id.intro_scroll);
			s.post(new Runnable() {
				public void run() {
					s.smoothScrollTo(0, t.getBottom());
				}
			});
			BluetoothDevice[] pairedDevices = (BluetoothDevice[]) pairedDev
					.toArray(new BluetoothDevice[0]);
			pairedDevice = pairedDevices[0];
			f.execute();
		} else {
			showErrorAndExit(R.string.bluetooth_error_title,
					R.string.bluetooth_no_devices);
		}
	}

	public void messageHandler(Message msg) {
		if (msg.what == MESSAGE_READ) {
			String dat = (String) msg.obj;
			// Append Text
			final TextView t = (TextView) findViewById(R.id.intro_text);
			t.setText(t.getText() + "\n" + dat);
			// scroll to bottom
			final ScrollView s = (ScrollView) findViewById(R.id.intro_scroll);
			s.post(new Runnable() {
				public void run() {
					s.smoothScrollTo(0, t.getBottom());
				}
			});
		} else if ((msg.what == MESSAGE_READ_FAIL)
				|| (msg.what == MESSAGE_WRITE_FAIL)) {
			IOException e = (IOException) msg.obj;
			e.printStackTrace();
			showErrorAndDo(R.string.bluetooth_error_title, e.getMessage(), null);
		} else if (msg.what == MESSAGE_BLUETOOTH_CONNECTED) {
			connected = true;
			final TextView t = (TextView) findViewById(R.id.intro_text);
			t.setText(t.getText() + "\n" + getString(R.string.intro_ready)
					+ " " + pairedDevice.getName() + " ("
					+ pairedDevice.getAddress() + ")\n");
			final ScrollView s = (ScrollView) findViewById(R.id.intro_scroll);
			s.post(new Runnable() {
				public void run() {
					s.smoothScrollTo(0, t.getBottom());
				}
			});
			socket = (BluetoothSocket) msg.obj;
			connectedThread = new ConnectedThread(socket, this);
			connectedThread.start();
		} else if (msg.what == MESSAGE_BLUETOOTH_CONNECTION_FAIL) {
			connected = false;
			final TextView t = (TextView) findViewById(R.id.intro_text);
			t.setText(t.getText() + "\n"
					+ getString(R.string.bluetooth_no_connect));
			final ScrollView s = (ScrollView) findViewById(R.id.intro_scroll);
			s.post(new Runnable() {
				public void run() {
					s.smoothScrollTo(0, t.getBottom());
				}
			});
			showErrorAndDo(R.string.bluetooth_error_title,
					R.string.bluetooth_no_connect, null);
		} else if (msg.what == MESSAGE_ROLL_READ) {
			TextView t = (TextView) findViewById(R.id.firstText);
			t.setText((String) msg.obj + " " + (char) 0x00B0);
			try {
				double y = Double.parseDouble((String) msg.obj);
				graphViewSeries[SERIES_ROLL].appendData(new GraphViewData(
						graphX, y), true);
			} catch (NumberFormatException e) {
			}
		} else if (msg.what == MESSAGE_PITCH_READ) {
			TextView t = (TextView) findViewById(R.id.secondText);
			t.setText((String) msg.obj + " " + (char) 0x00B0);
			try {
				double y = Double.parseDouble((String) msg.obj);
				graphViewSeries[SERIES_PITCH].appendData(new GraphViewData(
						graphX, y), true);
			} catch (NumberFormatException e) {
			}
		} else if (msg.what == MESSAGE_YAW_READ) {
			TextView t = (TextView) findViewById(R.id.thirdText);
			t.setText((String) msg.obj + " " + (char) 0x00B0);
			try {
				double y = Double.parseDouble((String) msg.obj);
				graphViewSeries[SERIES_YAW].appendData(new GraphViewData(
						graphX, y), true);
				graphX += graphIncrement;
			} catch (NumberFormatException e) {
			}
		} else if (msg.what == MESSAGE_VOLT_READ) {
			TextView t = (TextView) findViewById(R.id.fourthText);
			String s = (String) msg.obj;
			String out = s + "V";
			t.setText(out);
			try {
				double v = Double.parseDouble(s);
				double accuMax = 12.6;
				double accuMin = 9.6; // Still some buffer...
				if ((v >= accuMin) && (v <= accuMax)) {
					double accuDiff = accuMax - accuMin;
					double percent = (v - accuMin) * 100 / accuDiff;
					out += " (" + Math.round(percent) + "%)";
					t.setText(out);
				}
				if (v > 11.1) {
					t.setTextColor(Color.GREEN);
				} else if (v > 9.9) {
					t.setTextColor(Color.YELLOW);
				} else {
					t.setTextColor(Color.RED);
				}
			} catch (NumberFormatException e) {
			}
		} else if (msg.what == MESSAGE_MOTOR_READ) {
			TextView t = (TextView) findViewById(R.id.fifthText);
			String s = (String) msg.obj;
			t.setText(s);
			String[] mStrings = s.split("\\s+");
			for (int i = 0; (i < 4) && (i < mStrings.length); i++) {
				try {
					double d = Double.parseDouble(mStrings[i]);
					graphViewSeries[SERIES_M1 + i].appendData(
							new GraphViewData(graphX, d), true);
				} catch (NumberFormatException e) {
				}
			}
		} else if (msg.what == MESSAGE_PID_READ) {
			TextView t = (TextView) findViewById(R.id.sixthText);
			String s = (String) msg.obj;
			t.setText(s);
			String[] mStrings = s.split("\\s+");
			for (int i = 0; (i < 2) && (i < mStrings.length); i++) {
				try {
					double d = Double.parseDouble(mStrings[i]);
					int series;
					if (i == 0)
						series = SERIES_PIDPITCH;
					else
						series = SERIES_PIDROLL;
					graphViewSeries[series].appendData(new GraphViewData(
							graphX, d), true);
				} catch (NumberFormatException e) {
				}
			}
		} else if (msg.what == MESSAGE_PIDVAL_READ) {
			TextView t = (TextView) findViewById(R.id.seventhText);
			String s = (String) msg.obj;
			t.setText(s);
			String[] mStrings = s.split("\\s+");
			for (int i = 0; (i < 3) && (i < mStrings.length); i++) {
				if (i == 0)
					lastKnownP = mStrings[i];
				else if (i == 1)
					lastKnownI = mStrings[i];
				else if (i == 2)
					lastKnownD = mStrings[i];
			}
		} else if (msg.what == MESSAGE_PIDRANGE_READ) {
			String s = (String) msg.obj;
			String[] mStrings = s.split("\\s+");
			for (int i = 0; (i < 2) && (i < mStrings.length); i++) {
				if (i == 0)
					lastKnownMin = mStrings[i];
				else if (i == 1)
					lastKnownMax = mStrings[i];
			}
		} else if (msg.what == MESSAGE_PIDINTRANGE_READ) {
			String s = (String) msg.obj;
			String[] mStrings = s.split("\\s+");
			for (int i = 0; (i < 2) && (i < mStrings.length); i++) {
				if (i == 0)
					lastKnownIntMin = mStrings[i];
				else if (i == 1)
					lastKnownIntMax = mStrings[i];
			}
		} else if (msg.what == MESSAGE_FREQ_READ) {
			TextView t = (TextView) findViewById(R.id.eigthText);
			String s = (String) msg.obj;
			String[] mStrings = s.split("\\s+");
			if (mStrings.length == 2) {
				try {
					double flight = Double.parseDouble(mStrings[0]);
					double status = Double.parseDouble(mStrings[1]);
					double duration = (flight * FLIGHTFREQ) + (status * STATUSFREQ);
					s += " ("
							+ duration
							+ "/1000 --> "
							+ (1000 - duration)
							+ " free)";
				} catch (NumberFormatException e) {
				}
			}
			t.setText(s);
		} else if (msg.what == MESSAGE_ANGLES_READ) {
			String s = (String)msg.obj;
			String[] mStrings = s.split("\\s+");
			if (mStrings.length == 3) {
				try {
					double pitch = Double.parseDouble(mStrings[0]);
					double roll = Double.parseDouble(mStrings[1]);
					double yaw = Double.parseDouble(mStrings[2]);
					graphViewSeries[SERIES_VPITCH].appendData(
							new GraphViewData(graphX, pitch), true);
					graphViewSeries[SERIES_VROLL].appendData(
							new GraphViewData(graphX, roll), true);
					graphViewSeries[SERIES_VYAW].appendData(
							new GraphViewData(graphX, yaw), true);
				} catch (NumberFormatException e) {
				}
			}
		} else if (msg.what == MESSAGE_HEX_ERROR) {
			showErrorAndDo(R.string.hex_title, (String) msg.obj, null);
		} else if (msg.what == MESSAGE_DROPBOX_FAIL) {
			showErrorAndDo(R.string.dropbox_title, (String) msg.obj, null);
		} else if (msg.what == MESSAGE_ERROR) {
			showErrorAndDo(R.string.general_error, (String) msg.obj, null);
		} else if (msg.what == MESSAGE_HEX_PARSED) {
			if (connectedThread != null) {
				connectedThread.cancel();
				connectedThread.interrupt();
				connectedThread = null;
			}
			startConnection(new Function() {
				public void execute() {
					newYASAB();
				}
			});
		} else if (msg.what == MESSAGE_ALERT_DIALOG) {
			AlertDialog.Builder builder = (AlertDialog.Builder) msg.obj;
			builder.show();
		}
	}

	public void buttonHandler(int id) {
		if (id < commands.length) {
			byte[] d = new byte[1];
			d[0] = commands[id];
			connectedThread.write(d);
		}
	}

	private void newYASAB() {
		yasab = new YASAB(flashThread.minAddress, flashThread.firmware,
				pairedDevice, bluetoothAdapter, this);
		yasab.start();
	}

	private void newConnectThread() {
		new ConnectThread(pairedDevice, bluetoothAdapter, this).start();
	}

	public void onClick(View v) {
		if (connectedThread != null) {
			for (int i = 0; i < buttons.length; i++) {
				if (buttons[i].equals(v)) {
					buttonHandler(i);
					return;
				}
			}
		}
	}

	private void displayPIDAlert() {
		final LinearLayout layout = new LinearLayout(this);
		final LinearLayout layoutB = new LinearLayout(this);
		final LinearLayout outerLayout = new LinearLayout(this);
		final EditText p = new EditText(this);
		final EditText i = new EditText(this);
		final EditText d = new EditText(this);
		final EditText min = new EditText(this);
		final EditText max = new EditText(this);
		final EditText intMin = new EditText(this);
		final EditText intMax = new EditText(this);
		final CheckBox linkRange = new CheckBox(this);
		layout.setOrientation(LinearLayout.VERTICAL);
		layoutB.setOrientation(LinearLayout.VERTICAL);
		outerLayout.setOrientation(LinearLayout.HORIZONTAL);
		p.setInputType(EditorInfo.TYPE_NUMBER_FLAG_DECIMAL
				| EditorInfo.TYPE_NUMBER_FLAG_SIGNED
				| EditorInfo.TYPE_CLASS_NUMBER);
		i.setInputType(EditorInfo.TYPE_NUMBER_FLAG_DECIMAL
				| EditorInfo.TYPE_NUMBER_FLAG_SIGNED
				| EditorInfo.TYPE_CLASS_NUMBER);
		d.setInputType(EditorInfo.TYPE_NUMBER_FLAG_DECIMAL
				| EditorInfo.TYPE_NUMBER_FLAG_SIGNED
				| EditorInfo.TYPE_CLASS_NUMBER);
		min.setInputType(EditorInfo.TYPE_NUMBER_FLAG_DECIMAL
				| EditorInfo.TYPE_NUMBER_FLAG_SIGNED
				| EditorInfo.TYPE_CLASS_NUMBER);
		max.setInputType(EditorInfo.TYPE_NUMBER_FLAG_DECIMAL
				| EditorInfo.TYPE_NUMBER_FLAG_SIGNED
				| EditorInfo.TYPE_CLASS_NUMBER);
		intMin.setInputType(EditorInfo.TYPE_NUMBER_FLAG_DECIMAL
				| EditorInfo.TYPE_NUMBER_FLAG_SIGNED
				| EditorInfo.TYPE_CLASS_NUMBER);
		intMax.setInputType(EditorInfo.TYPE_NUMBER_FLAG_DECIMAL
				| EditorInfo.TYPE_NUMBER_FLAG_SIGNED
				| EditorInfo.TYPE_CLASS_NUMBER);
		p.setText(lastKnownP);
		i.setText(lastKnownI);
		d.setText(lastKnownD);
		min.setText(lastKnownMin);
		max.setText(lastKnownMax);
		intMin.setText(lastKnownIntMin);
		intMax.setText(lastKnownIntMax);
		linkRange.setText("Link Ranges");
		linkRange.setChecked(false);
		min.addTextChangedListener(new TextWatcher() {
			public void afterTextChanged(Editable s) {
				if (linkRange.isChecked()) {
					if (min.isFocused()) {
						intMin.setText(min.getText());
					}
				}
			}

			public void beforeTextChanged(CharSequence s, int start, int count,
					int after) {
			}

			public void onTextChanged(CharSequence arg0, int arg1, int arg2,
					int arg3) {
			}
		});
		max.addTextChangedListener(new TextWatcher() {
			public void afterTextChanged(Editable s) {
				if (linkRange.isChecked()) {
					if (max.isFocused()) {
						intMax.setText(max.getText());
					}
				}
			}

			public void beforeTextChanged(CharSequence s, int start, int count,
					int after) {
			}

			public void onTextChanged(CharSequence arg0, int arg1, int arg2,
					int arg3) {
			}
		});
		intMin.addTextChangedListener(new TextWatcher() {
			public void afterTextChanged(Editable s) {
				if (linkRange.isChecked()) {
					if (intMin.isFocused()) {
						min.setText(intMin.getText());
					}
				}
			}

			public void beforeTextChanged(CharSequence s, int start, int count,
					int after) {
			}

			public void onTextChanged(CharSequence arg0, int arg1, int arg2,
					int arg3) {
			}
		});
		intMax.addTextChangedListener(new TextWatcher() {
			public void afterTextChanged(Editable s) {
				if (intMax.isFocused()) {
					max.setText(intMax.getText());
				}
			}

			public void beforeTextChanged(CharSequence s, int start, int count,
					int after) {
			}

			public void onTextChanged(CharSequence arg0, int arg1, int arg2,
					int arg3) {
			}
		});
		layout.addView(p);
		layout.addView(i);
		layout.addView(d);
		layout.addView(linkRange);
		layoutB.addView(min);
		layoutB.addView(max);
		layoutB.addView(intMin);
		layoutB.addView(intMax);

		@SuppressWarnings("deprecation")
		LinearLayout.LayoutParams lp = new LinearLayout.LayoutParams(
				LinearLayout.LayoutParams.FILL_PARENT,
				LinearLayout.LayoutParams.FILL_PARENT);
		lp.weight = 1.0f;

		outerLayout.addView(layout, lp);
		outerLayout.addView(layoutB, lp);
		AlertDialog.Builder builder = new AlertDialog.Builder(this);
		builder.setTitle(R.string.pid_title);
		builder.setCancelable(true);
		builder.setPositiveButton(R.string.button_ok,
				new DialogInterface.OnClickListener() {
					public void onClick(DialogInterface dialog, int id) {
						try {
							double pVal = Double.parseDouble(p.getText()
									.toString());
							double iVal = Double.parseDouble(i.getText()
									.toString());
							double dVal = Double.parseDouble(d.getText()
									.toString());
							double minVal = Double.parseDouble(min.getText()
									.toString());
							double maxVal = Double.parseDouble(max.getText()
									.toString());
							double intMinVal = Double.parseDouble(intMin
									.getText().toString());
							double intMaxVal = Double.parseDouble(intMax
									.getText().toString());
							sendPIDValues(pVal, iVal, dVal, minVal, maxVal,
									intMinVal, intMaxVal);
						} catch (NumberFormatException e) {
							showErrorAndDo(R.string.pid_title, e.getMessage(),
									null);
						}
					}
				});
		builder.setNegativeButton(R.string.button_cancel, null);
		builder.setView(outerLayout);
		builder.show();
	}

	private void sendPIDValues(double p, double i, double d, double min,
			double max, double iMin, double iMax) {
		DecimalFormatSymbols decimalFormatSymbols = new DecimalFormatSymbols();
		decimalFormatSymbols.setDecimalSeparator('.');
		DecimalFormat decimalFormat = new DecimalFormat("0.000",
				decimalFormatSymbols);
		String pString = decimalFormat.format(p);
		String iString = decimalFormat.format(i);
		String dString = decimalFormat.format(d);
		String minString = decimalFormat.format(min);
		String maxString = decimalFormat.format(max);
		String iMinString = decimalFormat.format(iMin);
		String iMaxString = decimalFormat.format(iMax);
		if (connectedThread != null)
			connectedThread.write(ParameterCommand + pString + " " + iString
					+ " " + dString + " " + minString + " " + maxString + " "
					+ iMinString + " " + iMaxString + "\n");
	}

	private void showErrorAndExit(int title, int message) {
		showErrorAndDo(title, message, new Function() {
			public void execute() {
				finish();
			}
		});
	}

	private interface Function {
		public void execute();
	}

	private void showErrorAndDo(int title, int message, Function func) {
		showErrorAndDo(getString(title), getString(message), func);
	}

	private void showErrorAndDo(int title, String message, Function func) {
		showErrorAndDo(getString(title), message, func);
	}

	private void showErrorAndDo(String title, String message, Function func) {
		final Function f = func;
		AlertDialog.Builder builder = new AlertDialog.Builder(this);
		builder.setTitle(title);
		builder.setMessage(message);
		builder.setCancelable(false);
		builder.setPositiveButton(R.string.button_ok,
				new DialogInterface.OnClickListener() {
					public void onClick(DialogInterface dialog, int id) {
						if (f != null)
							f.execute();
					}
				});
		builder.show();
	}
}
