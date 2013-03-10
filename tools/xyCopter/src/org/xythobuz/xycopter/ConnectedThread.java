/*
 * ConnectedThread.java
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
	private boolean cancled = false;
	private boolean routing = false;

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
		} catch (IOException e) {
		}

		mmIn = new BufferedReader(new InputStreamReader(tmpIn));
		mmOutStream = tmpOut;
	}

	public void run() {
		// Keep listening to the InputStream until an exception occurs
		while (true) {
			try {
				if (!routing) {
					// Read from the InputStream
					String line = mmIn.readLine();
					// Send to MainActivity
					if (line.length() < 1) {
						continue;
					}
					char first = line.charAt(0);
					switch (first) {
					case 't':
						mmMain.handler.obtainMessage(
								MainActivity.MESSAGE_PIDVAL_READ, -1, -1,
								line.substring(1)).sendToTarget();
						break;
					case 'u':
						mmMain.handler.obtainMessage(
								MainActivity.MESSAGE_PID_READ, -1, -1,
								line.substring(1)).sendToTarget();
						break;
					case 'v':
						mmMain.handler.obtainMessage(
								MainActivity.MESSAGE_MOTOR_READ, -1, -1,
								line.substring(1)).sendToTarget();
						break;
					case 'w':
						mmMain.handler.obtainMessage(
								MainActivity.MESSAGE_PITCH_READ, -1, -1,
								line.substring(1)).sendToTarget();
						break;
					case 'x':
						mmMain.handler.obtainMessage(
								MainActivity.MESSAGE_ROLL_READ, -1, -1,
								line.substring(1)).sendToTarget();
						break;
					case 'y':
						mmMain.handler.obtainMessage(
								MainActivity.MESSAGE_YAW_READ, -1, -1,
								line.substring(1)).sendToTarget();
						break;
					case 'z':
						mmMain.handler.obtainMessage(
								MainActivity.MESSAGE_VOLT_READ, -1, -1,
								line.substring(1)).sendToTarget();
						break;
					default:
						mmMain.handler.obtainMessage(MainActivity.MESSAGE_READ,
								-1, -1, line).sendToTarget();
						break;
					}
				} else {
					// reroute incoming data for firmware flashing
					int c = mmIn.read();
					mmMain.handler.obtainMessage(MainActivity.MESSAGE_BOOTLOADER_READ, c, -1, null).sendToTarget();
				}
			} catch (IOException e) {
				if (!cancled)
					mmMain.handler.obtainMessage(
							MainActivity.MESSAGE_READ_FAIL, -1, -1, e)
							.sendToTarget();
				break;
			}
		}
	}

	public void setTarget(boolean reroute) {
		routing = reroute;
	}

	/* Call this from the main activity to send data to the remote device */
	public void write(byte[] bytes) {
		try {
			mmOutStream.write(bytes);
		} catch (IOException e) {
			mmMain.handler.obtainMessage(MainActivity.MESSAGE_WRITE_FAIL, -1,
					-1, e).sendToTarget();
		}
	}

	public void write(String s) {
		write(s.getBytes());
	}

	/* Call this from the main activity to shutdown the connection */
	public void cancel() {
		cancled = true;
		try {
			mmSocket.close();
		} catch (IOException e) {
		}
	}
}
