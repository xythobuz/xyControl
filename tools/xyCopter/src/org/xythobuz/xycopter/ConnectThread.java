package org.xythobuz.xycopter;

import java.io.IOException;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;

class ConnectThread extends Thread {
    private final BluetoothSocket mmSocket;
    private final BluetoothAdapter mmAdapter;
    private final MainActivity mmMain;
 
    public ConnectThread(BluetoothDevice device, BluetoothAdapter adapter, MainActivity main) {
        // Use a temporary object that is later assigned to mmSocket,
        // because mmSocket is final
        BluetoothSocket tmp = null;
        mmAdapter = adapter;
        mmMain = main;
 
        // Get a BluetoothSocket to connect with the given BluetoothDevice
        try {
            tmp = device.createRfcommSocketToServiceRecord(MainActivity.BLUETOOTH_UUID);
        } catch (IOException e) { }
        mmSocket = tmp;
    }
 
    public void run() {
        // Cancel discovery because it will slow down the connection
        mmAdapter.cancelDiscovery();
 
        try {
            // Connect the device through the socket. This will block
            // until it succeeds or throws an exception
            mmSocket.connect();
        } catch (IOException connectException) {
            // Unable to connect; close the socket and get out
            try {
                mmSocket.close();
            } catch (IOException closeException) {}
            mmMain.handler.obtainMessage(MainActivity.MESSAGE_BLUETOOTH_CONNECTION_FAIL, -1, -1, null).sendToTarget();
            return;
        }
 
        // Do work to manage the connection (in a separate thread)
        mmMain.handler.obtainMessage(MainActivity.MESSAGE_BLUETOOTH_CONNECTED, -1, -1, mmSocket).sendToTarget();
    }
 
    /** Will cancel an in-progress connection, and close the socket */
    public void cancel() {
        try {
            mmSocket.close();
        } catch (IOException e) {}
    }
}