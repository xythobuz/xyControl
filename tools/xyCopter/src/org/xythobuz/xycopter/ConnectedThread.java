package org.xythobuz.xycopter;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;

import android.bluetooth.BluetoothSocket;

class ConnectedThread extends Thread {
    private final BluetoothSocket mmSocket;
    private final BufferedReader mmIn;
    private final OutputStream mmOutStream;
    private final MainActivity mmMain;
 
    public ConnectedThread(BluetoothSocket socket, MainActivity main) {
        mmSocket = socket;
        mmMain = main;
        InputStream tmpIn = null;
        OutputStream tmpOut = null;
 
        // Get the input and output streams, using temp objects because
        // member streams are final
        try {
            tmpIn = socket.getInputStream();
            tmpOut = socket.getOutputStream();
        } catch (IOException e) { }
 
        mmIn = new BufferedReader(new InputStreamReader(tmpIn));
        mmOutStream = tmpOut;
    }
 
    public void run() {
        // Keep listening to the InputStream until an exception occurs
        while (true) {
            try {
                // Read from the InputStream
                String line = mmIn.readLine();
                // Send to MainActivity
                if (line.length() < 1) {
                	continue;
                }
                char first = line.charAt(0);
                switch (first) {
                case 'v':
                	mmMain.handler.obtainMessage(MainActivity.MESSAGE_MOTOR_READ, -1, -1, line.substring(1)).sendToTarget();
                	break;
                case 'w':
                	mmMain.handler.obtainMessage(MainActivity.MESSAGE_PITCH_READ, -1, -1, line.substring(1)).sendToTarget();
                	break;
                case 'x':
                	mmMain.handler.obtainMessage(MainActivity.MESSAGE_ROLL_READ, -1, -1, line.substring(1)).sendToTarget();
                	break;
                case 'y':
                	mmMain.handler.obtainMessage(MainActivity.MESSAGE_YAW_READ, -1, -1, line.substring(1)).sendToTarget();
                	break;
                case 'z':
                	mmMain.handler.obtainMessage(MainActivity.MESSAGE_VOLT_READ, -1, -1, line.substring(1)).sendToTarget();
                	break;
                default:
                	mmMain.handler.obtainMessage(MainActivity.MESSAGE_READ, -1, -1, line).sendToTarget();
                	break;
                }
            } catch (IOException e) {
            	mmMain.handler.obtainMessage(MainActivity.MESSAGE_READ_FAIL, -1, -1, e).sendToTarget();
                break;
            }
        }
    }
 
    /* Call this from the main activity to send data to the remote device */
    public void write(byte[] bytes) {
        try {
            mmOutStream.write(bytes);
        } catch (IOException e) {
        	mmMain.handler.obtainMessage(MainActivity.MESSAGE_WRITE_FAIL, -1, -1, e).sendToTarget();
        }
    }
 
    /* Call this from the main activity to shutdown the connection */
    public void cancel() {
        try {
            mmSocket.close();
        } catch (IOException e) {}
    }
}