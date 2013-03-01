package org.xythobuz.xycopter;

import java.io.IOException;
import java.util.Set;
import java.util.UUID;

import android.app.Activity;
import android.app.AlertDialog;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.res.Configuration;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.ScrollView;
import android.widget.TextView;

public class MainActivity extends Activity implements OnClickListener {

	public final static int REQUEST_ENABLE_BT = 1;
	public final static int MESSAGE_READ = 2;
	public final static int MESSAGE_READ_FAIL = 3;
	public final static int MESSAGE_WRITE_FAIL = 4;
	public final static int MESSAGE_BLUETOOTH_CONNECTED = 5;
	public final static int MESSAGE_BLUETOOTH_CONNECTION_FAIL = 6;
	
	public final static UUID BLUETOOTH_UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB"); // Default SPP UUID
	
	private BluetoothAdapter bluetoothAdapter = null;
	private BluetoothDevice pairedDevice = null;
	private BluetoothSocket socket = null;
	private ConnectedThread connectedThread = null;
	
	private byte[] commands = {'a', 'w', 's', 'd', 'x', 'y', 'm', 'v', 'o', 'q'};
	private Button[] buttons = new Button[10];
	private final static int B_LEFT = 0;
	private final static int B_FORW = 1;
	private final static int B_BACK = 2;
	private final static int B_RIGHT = 3;
	private final static int B_UP = 4;
	private final static int B_DOWN = 5;
	private final static int B_TOGGLE = 6;
	private final static int B_BATTERY = 7;
	private final static int B_ANGLES = 8;
	private final static int B_RESET = 9;
	
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
		
		bluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
		if (bluetoothAdapter == null) {
		    // Device does not support Bluetooth
			showErrorAndExit(R.string.bluetooth_error_title, R.string.bluetooth_no_adapter);
		}

		if (!bluetoothAdapter.isEnabled()) {
		    Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
		    startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT);
		} else {
			bluetoothEnabled();
		}
		
		setContentView(R.layout.activity_main);
		
		buttons[B_LEFT] = (Button)findViewById(R.id.bLeft);
		buttons[B_FORW] = (Button)findViewById(R.id.bForw);
		buttons[B_BACK] = (Button)findViewById(R.id.bBack);
		buttons[B_RIGHT] = (Button)findViewById(R.id.bRight);
		buttons[B_UP] = (Button)findViewById(R.id.bUp);
		buttons[B_DOWN] = (Button)findViewById(R.id.bDown);
		buttons[B_TOGGLE] = (Button)findViewById(R.id.bTog);
		buttons[B_BATTERY] = (Button)findViewById(R.id.bVolt);
		buttons[B_ANGLES] = (Button)findViewById(R.id.bAng);
		buttons[B_RESET] = (Button)findViewById(R.id.bReset);
		for (int i = 0; i < buttons.length; i++) {
			buttons[i].setOnClickListener(this);
		}
	}
	
	@Override
	public void onConfigurationChanged(Configuration newConfig) {
	    super.onConfigurationChanged(newConfig);
	}
	
	@Override
	protected void onActivityResult (int requestCode, int resultCode, Intent data) {
		if (requestCode == REQUEST_ENABLE_BT) {
			if (resultCode != Activity.RESULT_OK) {
				showErrorAndExit(R.string.bluetooth_error_title, R.string.bluetooth_turned_off);
			} else {
				bluetoothEnabled();
			}
		}
	}
	
	public void onClick(View v) {
		for (int i = 0; i < buttons.length; i++) {
			if (buttons[i].equals(v)) {
				buttonHandler(i);
				return;
			}
		}
    }
	
	private void bluetoothEnabled() {
		// List paired devices
		Set<BluetoothDevice> pairedDev = bluetoothAdapter.getBondedDevices();
		// If there are paired devices
		if (pairedDev.size() > 0) {
		    // Loop through paired devices
			int i = 0;
			final BluetoothDevice[] pairedDevices = (BluetoothDevice[]) pairedDev.toArray(new BluetoothDevice[0]);
			String[] pairedName = new String[pairedDev.size()];
		    for (BluetoothDevice device : pairedDevices) {
		    	pairedName[i] = device.getName() + " (" + device.getAddress() + ")";
		    }

		    // List AlertDialog
		    AlertDialog.Builder builder = new AlertDialog.Builder(this);
	        builder.setTitle(R.string.bluetooth_select);
	        builder.setCancelable(false);
	        builder.setItems(pairedName, new DialogInterface.OnClickListener() {
	        	public void onClick(DialogInterface dialog, int which) {
	        		TextView intro = (TextView)findViewById(R.id.intro_text);
	        		intro.setText(R.string.intro_connecting);
	        		pairedDevice = pairedDevices[which];
	        		newConnectThread();
	        	}
	        });
	        builder.show();
		} else {
			showErrorAndExit(R.string.bluetooth_error_title, R.string.bluetooth_no_devices);
		}
	}
	
	public void messageHandler(Message msg) {
		if (msg.what == MESSAGE_READ) {
			String dat = (String)msg.obj;
			// Append Text
			final TextView t = (TextView)findViewById(R.id.intro_text);
			t.setText(t.getText() + "\n" + dat);
			// scroll to bottom
			final ScrollView s = (ScrollView)findViewById(R.id.intro_scroll);
			s.post(new Runnable() { 
		        public void run() { 
		            s.smoothScrollTo(0, t.getBottom());
		        }
		    });
		} else if ((msg.what == MESSAGE_READ_FAIL) || (msg.what == MESSAGE_WRITE_FAIL)) {
			IOException e = (IOException)msg.obj;
			e.printStackTrace();
			showErrorAndDo(R.string.bluetooth_error_title, e.getMessage(), null);
		} else if (msg.what == MESSAGE_BLUETOOTH_CONNECTED) {
			TextView t = (TextView)findViewById(R.id.intro_text);
			t.setText(getString(R.string.intro_ready) + " " + pairedDevice.getName() + " (" + pairedDevice.getAddress() + ")");
			socket = (BluetoothSocket)msg.obj;
			connectedThread = new ConnectedThread(socket, this);
			connectedThread.start();
		} else if (msg.what == MESSAGE_BLUETOOTH_CONNECTION_FAIL) {
			TextView t = (TextView)findViewById(R.id.intro_text);
			t.setText(R.string.bluetooth_no_connect);
			showErrorAndDo(R.string.bluetooth_error_title, R.string.bluetooth_no_connect, new Function() {
				public void execute() {
					bluetoothEnabled();
				}
			});
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
		builder.setPositiveButton(R.string.button_ok, new DialogInterface.OnClickListener() {
			public void onClick(DialogInterface dialog, int id) {
				if (f != null)
					f.execute();
			}
	    });
		builder.show();
	}
}
