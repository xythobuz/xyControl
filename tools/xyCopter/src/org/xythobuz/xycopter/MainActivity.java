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
import android.graphics.Color;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.ScrollView;
import android.widget.TextView;

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

    public final static UUID BLUETOOTH_UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB"); // Default SPP UUID

    private BluetoothAdapter bluetoothAdapter = null;
    private BluetoothDevice pairedDevice = null;
    private BluetoothSocket socket = null;
    private ConnectedThread connectedThread = null;
    private GraphView graphView = null;
    private GraphViewSeries rollSeries = null;
    private GraphViewSeries pitchSeries = null;
    private GraphViewSeries yawSeries = null;
    private double graphIncrement = 0.2;
    private double rollX = graphIncrement, pitchX = graphIncrement, yawX = graphIncrement;

    private byte[] commands = {'a', 'w', 's', 'd', 'x', 'y', 'p', 'm', 'q'};
    private Button[] buttons = new Button[9];
    private final static int B_LEFT = 0;
    private final static int B_FORW = 1;
    private final static int B_BACK = 2;
    private final static int B_RIGHT = 3;
    private final static int B_UP = 4;
    private final static int B_DOWN = 5;
    private final static int B_ANGLES = 6;
    private final static int B_TOGGLE = 7;
    private final static int B_RESET = 8;

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
            showErrorAndExit(R.string.bluetooth_error_title, R.string.bluetooth_no_adapter);
        }

        if (!bluetoothAdapter.isEnabled()) {
            Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT);
        } else {
        	TextView t = (TextView)findViewById(R.id.intro_text);
        	t.setText(t.getText() + "\n" + getString(R.string.intro_wait));
        }

        buttons[B_LEFT] = (Button)findViewById(R.id.bLeft);
        buttons[B_FORW] = (Button)findViewById(R.id.bForw);
        buttons[B_BACK] = (Button)findViewById(R.id.bBack);
        buttons[B_RIGHT] = (Button)findViewById(R.id.bRight);
        buttons[B_UP] = (Button)findViewById(R.id.bUp);
        buttons[B_DOWN] = (Button)findViewById(R.id.bDown);
        buttons[B_ANGLES] = (Button)findViewById(R.id.bAng);
        buttons[B_TOGGLE] = (Button)findViewById(R.id.bTog);
        buttons[B_RESET] = (Button)findViewById(R.id.bReset);
        for (int i = 0; i < buttons.length; i++) {
            buttons[i].setOnClickListener(this);
        }
        
        int dataSize = 1;
        GraphViewData[] a = new GraphViewData[dataSize], b = new GraphViewData[dataSize], c = new GraphViewData[dataSize];
        for (int i = 0; i < dataSize; i++) {
        	a[i] = new GraphViewData(0.0, 0.0);
        	b[i] = new GraphViewData(0.0, 0.0);
        	c[i] = new GraphViewData(0.0, 0.0);
        }
        
        rollSeries = new GraphViewSeries("Roll", new GraphViewSeriesStyle(Color.rgb(200, 50, 0), 3), a);
        pitchSeries = new GraphViewSeries("Pitch", new GraphViewSeriesStyle(Color.rgb(0, 200, 50), 3), b);
        yawSeries = new GraphViewSeries("Yaw", new GraphViewSeriesStyle(Color.rgb(50, 0, 200), 3), c);
        graphView = new LineGraphView(this, "Angles");
        graphView.addSeries(rollSeries);
        graphView.addSeries(pitchSeries);
        graphView.addSeries(yawSeries);
        graphView.setScrollable(true);
        graphView.setScalable(true);
        graphView.setShowLegend(true);
        graphView.setLegendAlign(LegendAlign.BOTTOM);
        graphView.setViewPort(0.0, 10.0);
        LinearLayout layout = (LinearLayout)findViewById(R.id.upperOuterLayout);
        layout.addView(graphView);
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
            	TextView t = (TextView)findViewById(R.id.intro_text);
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
        // Handle item selection
        switch (item.getItemId()) {
        case R.id.disconnect: case R.id.connect:
            if (connectedThread != null) {
            	connectedThread.cancel();
            	connectedThread.interrupt();
            	connectedThread = null;
            	TextView t = (TextView)findViewById(R.id.intro_text);
                t.setText(t.getText() + "\n" + getString(R.string.intro_disconnect));
            } else {
            	startConnection();
            }
            return true;
        default:
            return super.onOptionsItemSelected(item);
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

    private void startConnection() {
        // List paired devices
        Set<BluetoothDevice> pairedDev = bluetoothAdapter.getBondedDevices();
        // If there are paired devices
        if (pairedDev.size() > 1) {
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
                    intro.setText(intro.getText() + "\n" + getString(R.string.intro_connect));
                    pairedDevice = pairedDevices[which];
                    newConnectThread();
                }
            });
            builder.show();
        } else if (pairedDev.size() > 0) {
        	TextView t = (TextView)findViewById(R.id.intro_text);
            t.setText(t.getText() + "\n" + getString(R.string.intro_connect));
        	BluetoothDevice[] pairedDevices = (BluetoothDevice[])pairedDev.toArray(new BluetoothDevice[0]);
            pairedDevice = pairedDevices[0];
            newConnectThread();
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
            t.setText(t.getText() + "\n" + getString(R.string.intro_ready) + " " + pairedDevice.getName() + " (" + pairedDevice.getAddress() + ")\n");
            socket = (BluetoothSocket)msg.obj;
            connectedThread = new ConnectedThread(socket, this);
            connectedThread.start();
        } else if (msg.what == MESSAGE_BLUETOOTH_CONNECTION_FAIL) {
            TextView t = (TextView)findViewById(R.id.intro_text);
            t.setText(t.getText() + "\n" + getString(R.string.bluetooth_no_connect));
            showErrorAndDo(R.string.bluetooth_error_title, R.string.bluetooth_no_connect, null);
        } else if (msg.what == MESSAGE_ROLL_READ) {
            TextView t = (TextView)findViewById(R.id.firstText);
            t.setText((String)msg.obj + " " + (char)0x00B0);
            double y = Double.parseDouble((String)msg.obj);
            rollSeries.appendData(new GraphViewData(rollX, y), true);
            rollX += graphIncrement;
        } else if (msg.what == MESSAGE_PITCH_READ) {
            TextView t = (TextView)findViewById(R.id.secondText);
            t.setText((String)msg.obj + " " + (char)0x00B0);
            double y = Double.parseDouble((String)msg.obj);
            pitchSeries.appendData(new GraphViewData(pitchX, y), true);
            pitchX += graphIncrement;
        } else if (msg.what == MESSAGE_YAW_READ) {
            TextView t = (TextView)findViewById(R.id.thirdText);
            t.setText((String)msg.obj + " " + (char)0x00B0);
            double y = Double.parseDouble((String)msg.obj);
            yawSeries.appendData(new GraphViewData(yawX, y), true);
            yawX += graphIncrement;
        } else if (msg.what == MESSAGE_VOLT_READ) {
            TextView t = (TextView)findViewById(R.id.fourthText);
            t.setText((String)msg.obj + " V");
        } else if (msg.what == MESSAGE_MOTOR_READ) {
            TextView t = (TextView)findViewById(R.id.fifthText);
            t.setText((String)msg.obj);
        } else if (msg.what == MESSAGE_PID_READ) {
        	TextView t = (TextView)findViewById(R.id.sixthText);
        	t.setText((String)msg.obj);
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
