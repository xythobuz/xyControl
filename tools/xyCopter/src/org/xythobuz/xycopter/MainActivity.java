package org.xythobuz.xycopter;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Set;
import java.util.UUID;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.app.AlertDialog;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.text.method.ScrollingMovementMethod;
import android.util.TypedValue;
import android.view.Menu;
import android.view.View;
import android.widget.Button;
import android.widget.ScrollView;
import android.widget.TextView;

@SuppressLint("HandlerLeak")
public class MainActivity extends Activity {

	public final static int REQUEST_ENABLE_BT = 1;
	public final static int MESSAGE_READ = 2;
	public final static int MESSAGE_READ_FAIL = 3;
	public final static int MESSAGE_WRITE_FAIL = 4;
	
	private static final float TEXT_SIZE = 16;
	private static final UUID BLUETOOTH_UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB"); // Default SPP UUID
	
	private BluetoothAdapter bluetoothAdapter = null;
	private BluetoothDevice pairedDevice = null;
	private BluetoothSocket socket = null;
	private ConnectedThread connectedThread = null;
	
	private Button[] buttons = new Button[10];
	private byte[] commands = new byte[10];
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
	
	public Handler handler = new Handler() {
		public void handleMessage(Message msg) {
			messageHandler(msg);
		}
	};
	
	@Override
	protected void onDestroy() {
		super.onDestroy();
		try {
			socket.close();
		} catch (IOException e) {
			e.printStackTrace();
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
		commands[B_LEFT] = 'a';
		commands[B_FORW] = 'w';
		commands[B_BACK] = 's';
		commands[B_RIGHT] = 'd';
		commands[B_UP] = 'x';
		commands[B_DOWN] = 'y';
		commands[B_TOGGLE] = 'm';
		commands[B_BATTERY] = 'v';
		commands[B_ANGLES] = 'o';
		commands[B_RESET] = 'q';
		for (int i = 0; i < buttons.length; i++) {
			buttons[i].setOnClickListener(new View.OnClickListener() {
	             public void onClick(View v) {
	                 for (int i = 0; i < buttons.length; i++) {
	                	 if (buttons[i].equals(v)) {
	                		 buttonHandler(i);
	                		 return;
	                	 }
	                 }
	             }
	         });
		}
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
	        		new ConnectThread(pairedDevice).start();
	        	}
	        });
	        builder.show();
		} else {
			showErrorAndExit(R.string.bluetooth_error_title, R.string.bluetooth_no_devices);
		}
	}
	
	private class ConnectThread extends Thread {
	    public ConnectThread(BluetoothDevice device) {
	        try {
	            socket = device.createRfcommSocketToServiceRecord(BLUETOOTH_UUID);
	        } catch (IOException e) {
	        	showErrorAndExit(R.string.bluetooth_error_title, e.getMessage());
	        }
	    }
	 
	    public void run() {
	        // Cancel discovery because it will slow down the connection
	        bluetoothAdapter.cancelDiscovery();
	        try {
	            socket.connect();
	        } catch (IOException e) {
	        	e.printStackTrace();
	            handler.post(bluetoothConnectionFailed);
	            return;
	        }
	        handler.post(bluetoothConnected);
	    }
	}
	
	private final Runnable bluetoothConnectionFailed = new Runnable() { public void run() {
		TextView t = (TextView)findViewById(R.id.intro_text);
		t.setText(R.string.bluetooth_no_connect);
		showErrorAndDo(R.string.bluetooth_error_title, R.string.bluetooth_no_connect, new Function() {
			public void execute() {
				bluetoothEnabled();
			}
		});
	}};
	
	private final Runnable bluetoothConnected = new Runnable() { public void run() {
		TextView t = (TextView)findViewById(R.id.intro_text);
		t.setTextSize(TypedValue.COMPLEX_UNIT_PX, TEXT_SIZE);
		t.setText(R.string.intro_ready);

		connectedThread = new ConnectedThread();
		connectedThread.start();
	}};
	
	public void messageHandler(Message msg) {
		if (msg.what == MESSAGE_READ) {
			dataReceived((byte[])msg.obj);
		} else if ((msg.what == MESSAGE_READ_FAIL) || (msg.what == MESSAGE_WRITE_FAIL)) {
			IOException e = (IOException)msg.obj;
			e.printStackTrace();
			showErrorAndExit(R.string.bluetooth_error_title, e.getMessage());
		}
	}
	
	private class ConnectedThread extends Thread {
	    private InputStream mmInStream;
	    private OutputStream mmOutStream;
	 
	    public ConnectedThread() {
	        try {
	        	mmInStream = socket.getInputStream();
	        	mmOutStream = socket.getOutputStream();
	        } catch (IOException e) {
	        	e.printStackTrace();
	        }
	    }
	 
	    public void run() {
	        byte[] buffer = new byte[1024];
	        
	        while (true) {
	            try {
	                // Read from the InputStream
	            	mmInStream.read(buffer);
	                Message msg = handler.obtainMessage(MESSAGE_READ, -1, -1, buffer);
	                handler.sendMessage(msg);
	            } catch (IOException e) {
	            	Message msg = handler.obtainMessage(MESSAGE_READ_FAIL, -1, -1, e);
	            	handler.sendMessage(msg);
	                break;
	            }
	        }
	    }
	 
	    public void write(byte[] bytes) {
	        try {
	            mmOutStream.write(bytes);
	        } catch (IOException e) {
	        	Message msg = handler.obtainMessage(MESSAGE_WRITE_FAIL, -1, -1, e);
            	handler.sendMessage(msg);
	        }
	    }
	}

	private void dataReceived(byte[] data) {
		String dat = new String(data);
		final TextView t = (TextView)findViewById(R.id.intro_text);
		t.setText(t.getText() + dat);
		
		// scroll to bottom
		final ScrollView s = (ScrollView)findViewById(R.id.intro_scroll);
		s.post(new Runnable() { 
	        public void run() { 
	            s.smoothScrollTo(0, t.getBottom());
	        }
	    });
	}
	
	public void buttonHandler(int id) {
		byte[] d = new byte[1];
		d[0] = commands[id];
		connectedThread.write(d);
	}
	
	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.activity_main, menu);
		return true;
	}
	
	private void showErrorAndExit(int title, int message) {
		showErrorAndDo(title, message, new Function() {
			public void execute() {
				finish();
			}
		});
	}
	
	private void showErrorAndExit(int title, String message) {
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
				f.execute();
			}
	    });
		builder.show();
	}
}
