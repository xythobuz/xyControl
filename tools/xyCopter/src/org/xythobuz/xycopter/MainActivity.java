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

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
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
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.inputmethod.EditorInfo;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.ScrollView;
import android.widget.TextView;

import com.dropbox.client2.DropboxAPI;
import com.dropbox.client2.DropboxAPI.Entry;
import com.dropbox.client2.android.AndroidAuthSession;
import com.dropbox.client2.exception.DropboxException;
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
	public final static int MESSAGE_BOOTLOADER_READ = 14;
	public final static int MESSAGE_DROPBOX_CALLBACK = 15;
	public final static int MESSAGE_DROPBOX_FAIL = 16;
	public final static int MESSAGE_DROPBOX_RECEIVED = 17;

	private final static String APP_KEY = "gnbnnowfgpv5jej";
	private final static String APP_SECRET = "uxy6uf661xyd46q";
	private final static AccessType ACCESS_TYPE = AccessType.DROPBOX;
	private DropboxAPI<AndroidAuthSession> mDBApi;

	private final static String PREFS_NAME = "xyDropboxPrefs";
	private final static String PREF_KEY = "DropboxKey";
	private final static String PREF_SECRET = "DropboxSecret";

	public final static UUID BLUETOOTH_UUID = UUID
			.fromString("00001101-0000-1000-8000-00805F9B34FB"); // Default SPP
																	// UUID

	private BluetoothAdapter bluetoothAdapter = null;
	private BluetoothDevice pairedDevice = null;
	private BluetoothSocket socket = null;
	private ConnectedThread connectedThread = null;
	private GraphView graphView = null;
	private GraphViewSeries rollSeries = null;
	private GraphViewSeries pitchSeries = null;
	private GraphViewSeries yawSeries = null;
	private double graphIncrement = 0.2;
	private double rollX = graphIncrement, pitchX = graphIncrement,
			yawX = graphIncrement;

	private final String ParameterCommand = "n";
	private final byte[] commands = { 'a', 'w', 's', 'd', 'x', 'y', 'p', 'm',
			'q' };
	private final Button[] buttons = new Button[9];
	private final static int B_LEFT = 0;
	private final static int B_FORW = 1;
	private final static int B_BACK = 2;
	private final static int B_RIGHT = 3;
	private final static int B_UP = 4;
	private final static int B_DOWN = 5;
	private final static int B_ANGLES = 6;
	private final static int B_TOGGLE = 7;
	private final static int B_RESET = 8;

	private TestGraphThread testGraphThread = null;

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
			TextView t = (TextView) findViewById(R.id.intro_text);
			t.setText(t.getText() + "\n" + getString(R.string.intro_wait));
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
		for (int i = 0; i < buttons.length; i++) {
			buttons[i].setOnClickListener(this);
		}

		GraphViewData[] a = new GraphViewData[1], b = new GraphViewData[1], c = new GraphViewData[1];
		a[0] = new GraphViewData(0.0, 0.0);
		b[0] = new GraphViewData(0.0, 0.0);
		c[0] = new GraphViewData(0.0, 0.0);
		rollSeries = new GraphViewSeries("Roll", new GraphViewSeriesStyle(
				Color.rgb(200, 50, 0), 3), a);
		pitchSeries = new GraphViewSeries("Pitch", new GraphViewSeriesStyle(
				Color.rgb(0, 200, 50), 3), b);
		yawSeries = new GraphViewSeries("Yaw", new GraphViewSeriesStyle(
				Color.rgb(50, 0, 200), 3), c);
		graphView = new LineGraphView(this, "Angles");
		graphView.addSeries(rollSeries);
		graphView.addSeries(pitchSeries);
		graphView.addSeries(yawSeries);
		graphView.setScrollable(true);
		graphView.setScalable(true);
		graphView.setShowLegend(true);
		graphView.setLegendAlign(LegendAlign.BOTTOM);
		graphView.setViewPort(0.0, 10.0);
		LinearLayout layout = (LinearLayout) findViewById(R.id.upperOuterLayout);
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
				TextView t = (TextView) findViewById(R.id.intro_text);
				t.setText(t.getText() + "\n" + getString(R.string.intro_wait));
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
		TextView intro = (TextView) findViewById(R.id.intro_text);

		switch (item.getItemId()) {
		case R.id.clear:
			intro.setText("");
			GraphViewData[] a = new GraphViewData[1],
			b = new GraphViewData[1],
			c = new GraphViewData[1];
			a[0] = new GraphViewData(0.0, 0.0);
			b[0] = new GraphViewData(0.0, 0.0);
			c[0] = new GraphViewData(0.0, 0.0);
			rollSeries.resetData(a);
			pitchSeries.resetData(b);
			yawSeries.resetData(c);
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
			} else {
				startConnection();
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

	private class DropboxThread extends Thread {
		public void run() {
			try {
				Entry root = mDBApi.metadata("/", 1000, null, true, null);
				String[] files = new String[root.contents.size()];
				int i = 0;
				for (Entry ent : root.contents) {
					files[i++] = ent.path;
				}
				handler.obtainMessage(MESSAGE_DROPBOX_CALLBACK, files)
						.sendToTarget();
			} catch (DropboxException e) {
				handler.obtainMessage(MESSAGE_DROPBOX_FAIL, e.getMessage())
						.sendToTarget();
			}
		}
	}

	private class DropboxReceiveThread extends Thread {

		String file;
		FileOutputStream out;

		public DropboxReceiveThread(String f, FileOutputStream s) {
			file = f;
			out = s;
		}

		public void run() {
			try {
				mDBApi.getFile(file, null, out, null);
				handler.obtainMessage(MESSAGE_DROPBOX_RECEIVED).sendToTarget();
			} catch (DropboxException e) {
				handler.obtainMessage(MESSAGE_DROPBOX_FAIL, e.getMessage())
						.sendToTarget();
				e.printStackTrace();
			}
		}
	}

	private void getDropboxFiles() {
		new DropboxThread().start();
	}

	private String[] filterOnlyHexFiles(String[] all) {
		int c = 0;
		for (int i = 0; i < all.length; i++) {
			if (all[i].endsWith(".hex")) {
				c++;
			}
		}
		String[] hex = new String[c];
		c = 0;
		for (int i = 0; i < all.length; i++) {
			if (all[i].endsWith(".hex")) {
				hex[c++] = all[i];
			}
		}
		return hex;
	}

	private void selectFirmwareFile(final String[] files) {
		AlertDialog.Builder builder = new AlertDialog.Builder(this);
		builder.setTitle(R.string.hex_select);
		builder.setCancelable(false);
		String[] fileNames = new String[files.length];
		for (int i = 0; i < files.length; i++) {
			fileNames[i] = files[i].substring(1);
		}
		builder.setItems(fileNames, new DialogInterface.OnClickListener() {
			public void onClick(DialogInterface dialog, int which) {
				getFirmwareFile(files[which]);
			}
		});
		builder.show();
	}

	private void flashFirmware() {
		getDropboxFiles();
	}

	public void dropboxFirmwareCallback(String[] files) {
		if (files != null) {
			files = filterOnlyHexFiles(files);
			selectFirmwareFile(files);
		}
	}

	FileOutputStream outputStream = null;
	File file = null;
	
	private void getFirmwareFile(String f) {
		try {
			file = File.createTempFile("xycopter_", ".hex", getCacheDir());
			outputStream = new FileOutputStream(file);
			new DropboxReceiveThread(f, outputStream).start();
		} catch (Exception e) {
			showErrorAndDo(R.string.general_error, e.getMessage(), null);
			e.printStackTrace();
		}
	}

	public void dropboxFileReceived() {
		try {
			parseFirmwareFile(file);
		} catch (Exception e) {
			e.printStackTrace();
		}
		if (outputStream != null) {
			try {
				outputStream.close();
			} catch (IOException e) { }
		}
		if (file != null) {
			file.delete();
		}
	}

	private void parseFirmwareFile(File f) throws Exception {
		FileInputStream in = new FileInputStream(f);
		HexParser h = new HexParser(in, f.length());
		if (h.read()) {
			showErrorAndDo(R.string.hex_title, R.string.hex_read, null);
			return;
		}
		if (h.parse()) {
			showErrorAndDo(R.string.hex_title, R.string.hex_invalid, null);
			return;
		}
		int min = h.minAddress();
		int[] firmware = h.extract();
		if (firmware != null) {
			firmwareBlob = firmware;
			hexStart = min;
			hexLen = firmware.length;
			if (connectedThread != null) {
				flashFirmwareFile();
			} else {
				showErrorAndDo(R.string.bluetooth_error_title,
						"Please connect!", null);
			}
		}
	}

	private void addToLog(String l) {
		final TextView t = (TextView) findViewById(R.id.intro_text);
		t.setText(t.getText() + "\n" + l);
		final ScrollView s = (ScrollView) findViewById(R.id.intro_scroll);
		s.post(new Runnable() {
			public void run() {
				s.smoothScrollTo(0, t.getBottom());
			}
		});
	}

	private int flashState = 0;
	private int[] firmwareBlob;
	int hexStart, hexLen, pagesWritten;

	private void flashFirmwareFile() {
		addToLog("\nResetting Target...!");
		connectedThread.write("q");
		addToLog("Trying to connect...!");
		try {
			Thread.sleep(200);
		} catch (InterruptedException e) {
		}
		connectedThread.setTarget(true); // Reroute Output
		connectedThread.write("q");
		flashState = 1;
	}

	private void gotBootloaderPing() {
		addToLog("Got answer. Acknowledging...");
		connectedThread.write("c");
		flashState = 2;
	}

	private void gotAcknowledge() {
		addToLog("Connected! Sending address...");
		byte[] tmp = { (byte) ((hexStart & 0xFF000000) >>> 24),
				(byte) ((hexStart & 0x00FF0000) >>> 16),
				(byte) ((hexStart & 0x0000FF00) >>> 8),
				(byte) (hexStart & 0x000000FF) };
		connectedThread.write(tmp);
		flashState = 3;
	}

	private void gotStartAck() {
		addToLog("Done. Sending length...");
		byte[] tmp = { (byte) ((hexLen & 0xFF000000) >>> 24),
				(byte) ((hexLen & 0x00FF0000) >>> 16),
				(byte) ((hexLen & 0x0000FF00) >>> 8),
				(byte) (hexLen & 0x000000FF) };
		connectedThread.write(tmp);
		flashState = 4;
	}

	private void gotLengthAck() {
		byte[] data = new byte[firmwareBlob.length];
		for (int i = 0; i < data.length; i++) {
			data[i] = (byte) firmwareBlob[i];
		}
		addToLog("Done. Sending Firmware Blob...");
		connectedThread.write(data);
		flashState = 5;
		pagesWritten = 0;
	}

	private void dataFromBootloader(int c) {
		if ((flashState == 1) && (c == 'o')) {
			gotBootloaderPing();
		} else if ((flashState == 2) && (c == 'a')) {
			gotAcknowledge();
		} else if ((flashState == 3) && (c == 'o')) {
			gotStartAck();
		} else if ((flashState == 4) && (c == 'o')) {
			gotLengthAck();
		} else if (flashState == 5) {
			if (c == 'o') {
				pagesWritten++;
				addToLog("Next page written (" + pagesWritten + ")!");
				if ((pagesWritten * 256) >= hexLen) {
					addToLog("Upload finished...!");
					connectedThread.setTarget(false);
				}
			} else if (c == 'e') {
				addToLog("Interrupted by target!");
			} else {
				addToLog("Unknown response... (" + c + ")");
			}
		} else {
			addToLog("Invalid (" + flashState + "/" + c + ")? Trying again!");
			flashState = 1;
			connectedThread.write("q");
		}
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
		final EditText p = new EditText(this);
		final EditText i = new EditText(this);
		final EditText d = new EditText(this);
		layout.setOrientation(LinearLayout.VERTICAL);
		p.setInputType(EditorInfo.TYPE_NUMBER_FLAG_DECIMAL
				| EditorInfo.TYPE_NUMBER_FLAG_SIGNED
				| EditorInfo.TYPE_CLASS_NUMBER);
		i.setInputType(EditorInfo.TYPE_NUMBER_FLAG_DECIMAL
				| EditorInfo.TYPE_NUMBER_FLAG_SIGNED
				| EditorInfo.TYPE_CLASS_NUMBER);
		d.setInputType(EditorInfo.TYPE_NUMBER_FLAG_DECIMAL
				| EditorInfo.TYPE_NUMBER_FLAG_SIGNED
				| EditorInfo.TYPE_CLASS_NUMBER);
		layout.addView(p);
		layout.addView(i);
		layout.addView(d);
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
							sendPIDValues(pVal, iVal, dVal);
						} catch (NumberFormatException e) {
							showErrorAndDo(R.string.pid_title, e.getMessage(),
									null);
						}
					}
				});
		builder.setNegativeButton(R.string.button_cancel, null);
		builder.setView(layout);
		builder.show();
	}

	private void sendPIDValues(double p, double i, double d) {
		DecimalFormatSymbols decimalFormatSymbols = new DecimalFormatSymbols();
		decimalFormatSymbols.setDecimalSeparator('.');
		DecimalFormat decimalFormat = new DecimalFormat("0.000",
				decimalFormatSymbols);
		String pString = decimalFormat.format(p);
		String iString = decimalFormat.format(i);
		String dString = decimalFormat.format(d);
		if (connectedThread != null)
			connectedThread.write(ParameterCommand + pString + " " + iString
					+ " " + dString);
	}

	private void startConnection() {
		// List paired devices
		Set<BluetoothDevice> pairedDev = bluetoothAdapter.getBondedDevices();
		// If there are paired devices
		if (pairedDev.size() > 1) {
			// Loop through paired devices
			int i = 0;
			final BluetoothDevice[] pairedDevices = (BluetoothDevice[]) pairedDev
					.toArray(new BluetoothDevice[0]);
			String[] pairedName = new String[pairedDev.size()];
			for (BluetoothDevice device : pairedDevices) {
				pairedName[i] = device.getName() + " (" + device.getAddress()
						+ ")";
			}

			// List AlertDialog
			AlertDialog.Builder builder = new AlertDialog.Builder(this);
			builder.setTitle(R.string.bluetooth_select);
			builder.setCancelable(false);
			builder.setItems(pairedName, new DialogInterface.OnClickListener() {
				public void onClick(DialogInterface dialog, int which) {
					TextView intro = (TextView) findViewById(R.id.intro_text);
					intro.setText(intro.getText() + "\n"
							+ getString(R.string.intro_connect));
					pairedDevice = pairedDevices[which];
					newConnectThread();
				}
			});
			builder.show();
		} else if (pairedDev.size() > 0) {
			TextView t = (TextView) findViewById(R.id.intro_text);
			t.setText(t.getText() + "\n" + getString(R.string.intro_connect));
			BluetoothDevice[] pairedDevices = (BluetoothDevice[]) pairedDev
					.toArray(new BluetoothDevice[0]);
			pairedDevice = pairedDevices[0];
			newConnectThread();
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
			TextView t = (TextView) findViewById(R.id.intro_text);
			t.setText(t.getText() + "\n" + getString(R.string.intro_ready)
					+ " " + pairedDevice.getName() + " ("
					+ pairedDevice.getAddress() + ")\n");
			socket = (BluetoothSocket) msg.obj;
			connectedThread = new ConnectedThread(socket, this);
			connectedThread.start();
		} else if (msg.what == MESSAGE_BLUETOOTH_CONNECTION_FAIL) {
			TextView t = (TextView) findViewById(R.id.intro_text);
			t.setText(t.getText() + "\n"
					+ getString(R.string.bluetooth_no_connect));
			showErrorAndDo(R.string.bluetooth_error_title,
					R.string.bluetooth_no_connect, null);
		} else if (msg.what == MESSAGE_ROLL_READ) {
			TextView t = (TextView) findViewById(R.id.firstText);
			t.setText((String) msg.obj + " " + (char) 0x00B0);
			double y = Double.parseDouble((String) msg.obj);
			rollSeries.appendData(new GraphViewData(rollX, y), true);
			rollX += graphIncrement;
		} else if (msg.what == MESSAGE_PITCH_READ) {
			TextView t = (TextView) findViewById(R.id.secondText);
			t.setText((String) msg.obj + " " + (char) 0x00B0);
			double y = Double.parseDouble((String) msg.obj);
			pitchSeries.appendData(new GraphViewData(pitchX, y), true);
			pitchX += graphIncrement;
		} else if (msg.what == MESSAGE_YAW_READ) {
			TextView t = (TextView) findViewById(R.id.thirdText);
			t.setText((String) msg.obj + " " + (char) 0x00B0);
			double y = Double.parseDouble((String) msg.obj);
			yawSeries.appendData(new GraphViewData(yawX, y), true);
			yawX += graphIncrement;
		} else if (msg.what == MESSAGE_VOLT_READ) {
			TextView t = (TextView) findViewById(R.id.fourthText);
			t.setText((String) msg.obj + " V");
		} else if (msg.what == MESSAGE_MOTOR_READ) {
			TextView t = (TextView) findViewById(R.id.fifthText);
			t.setText((String) msg.obj);
		} else if (msg.what == MESSAGE_PID_READ) {
			TextView t = (TextView) findViewById(R.id.sixthText);
			t.setText((String) msg.obj);
		} else if (msg.what == MESSAGE_PIDVAL_READ) {
			TextView t = (TextView) findViewById(R.id.seventhText);
			t.setText((String) msg.obj);
		} else if (msg.what == MESSAGE_BOOTLOADER_READ) {
			dataFromBootloader(msg.arg1);
		} else if (msg.what == MESSAGE_DROPBOX_CALLBACK) {
			dropboxFirmwareCallback((String[]) msg.obj);
		} else if (msg.what == MESSAGE_DROPBOX_FAIL) {
			showErrorAndDo(R.string.dropbox_title, (String) msg.obj, null);
		} else if (msg.what == MESSAGE_DROPBOX_RECEIVED) {
			dropboxFileReceived();
		}
	}

	public void buttonHandler(int id) {
		byte[] d = new byte[1];
		d[0] = commands[id];
		connectedThread.write(d);
	}

	private void newConnectThread() {
		new ConnectThread(pairedDevice, bluetoothAdapter, this).start();
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
