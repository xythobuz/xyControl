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
                // Send the obtained bytes to the UI activity
                mmMain.handler.obtainMessage(MainActivity.MESSAGE_READ, -1, -1, line).sendToTarget();
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