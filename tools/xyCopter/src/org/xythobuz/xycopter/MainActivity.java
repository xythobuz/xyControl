package org.xythobuz.xycopter;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
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
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.TypedValue;
import android.view.Menu;
import android.widget.ScrollView;
import android.widget.TextView;

public class MainActivity extends Activity {

	public static int REQUEST_ENABLE_BT = 1;
	public static int MESSAGE_READ = 2;
	
	private static final float TEXT_SIZE = 14;
	private static final UUID BLUETOOTH_UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB"); // Default SPP UUID
	
	private BluetoothAdapter bluetoothAdapter = null;
	private BluetoothDevice pairedDevice = null;
	private BluetoothSocket socket = null;
	private ConnectedThread connectedThread = null;

	public final Handler handler = new Handler();
	
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
		
		byte[] test = {'v'};
		connectedThread.write(test);
	}};
	
	public void handleMessage(Message msg) {
		if (msg.what == MESSAGE_READ) {
			dataReceived((byte[])msg.obj);
		}
	}
	
	private class ConnectedThread extends Thread {
	    private BufferedInputStream mmInStream;
	    private BufferedOutputStream mmOutStream;
	 
	    public ConnectedThread() {
	        try {
	        	mmInStream = new BufferedInputStream(socket.getInputStream());
	        	mmOutStream = new BufferedOutputStream(socket.getOutputStream());
	        } catch (IOException e) {
	        	e.printStackTrace();
	        }
	    }
	 
	    public void run() {
	        byte[] buffer = new byte[1024];  // buffer store for the stream
	 
	        // Keep listening to the InputStream until an exception occurs
	        while (true) {
	            try {
	                // Read from the InputStream
	            	mmInStream.read(buffer);
	                System.out.println("Received data!");
	                handler.obtainMessage(MESSAGE_READ, -1, -1, buffer).sendToTarget();
	            } catch (IOException e) {
	            	e.printStackTrace();
	                break;
	            }
	        }
	    }
	 
	    public void write(byte[] bytes) {
	        try {
	            mmOutStream.write(bytes);
	        } catch (IOException e) {
	        	e.printStackTrace();
	        }
	    }
	}

	private void dataReceived(byte[] data) {
		String dat = new String(data);
		final TextView t = (TextView)findViewById(R.id.intro_text);
		t.setText(t.getText() + dat);
		
		// Scroll to bottom
		final ScrollView s = (ScrollView)findViewById(R.id.intro_scroll);
		s.post(new Runnable() { 
	        public void run() { 
	            s.smoothScrollTo(0, t.getBottom());
	        }
	    });
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
