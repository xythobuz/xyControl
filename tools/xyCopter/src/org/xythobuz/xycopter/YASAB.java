/*
 * YASAB.java
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
import java.io.InputStream;
import java.io.OutputStream;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;

public class YASAB extends Thread {

	private MainActivity mMain;
	private int min;
	private int[] firmware;
	private BluetoothDevice device;
	private BluetoothAdapter adapter;
	private BluetoothSocket socket;
	private InputStream input;
	private OutputStream output;

	private static final int STATE_PING = 0;
	private static final int STATE_ACK = 1;
	private static final int STATE_ADDRESS = 2;
	private static final int STATE_LENGTH = 3;
	private static final int STATE_FIRMWARE = 4;
	private static final int STATES = 5;
	private int state = 0;

	private static final int DELAY = 250;

	private static final int XOFF = 0x13;
	private static final int XON = 0x11;
	private boolean flowStopped = false;
	
	private static final int OKAY = 111; // o
	private static final int CONFIRM = 99; // c
	private static final int ACK = 97; // a
	private static final int PING = 113; // q
	private static final int ERROR = 101; // e

	public YASAB(int minAddress, int[] fw, BluetoothDevice dev,
			BluetoothAdapter adapt, MainActivity m) {
		mMain = m;
		min = minAddress;
		firmware = fw;
		device = dev;
		adapter = adapt;
	}

	public void run() {
		try {
			socket = device
					.createRfcommSocketToServiceRecord(MainActivity.BLUETOOTH_UUID);
			adapter.cancelDiscovery();
			socket.connect();
			input = socket.getInputStream();
			output = socket.getOutputStream();
		} catch (Exception e) {
			mMain.handler.obtainMessage(MainActivity.MESSAGE_ERROR,
					e.getMessage()).sendToTarget();
			return;
		}

		String s = "\n";
		s += "=== YASAB for Android ===\n";
		s += "Start: " + min + "\n";
		s += "End: " + (min + firmware.length - 1) + "\n";
		s += "Length: " + firmware.length + "\n";
		addToLog(s);

		java.util.Date startDate = new java.util.Date();
		
		while (state < STATES) {
			while (state == STATE_PING) {
				addToLog("Pinging Bootloader...");
				write(PING);
				sleep(DELAY * 2);
				if (available() > 0) {
					int c = read();
					if (c == OKAY) {
						state = STATE_ACK;
						addToLog("Got response!");
					} else {
						addToLog("Unknown response (" + c + ")...?");
					}
				}
			}
			while (state == STATE_ACK) {
				addToLog("Acknowledging...");
				write(CONFIRM);
				sleep(DELAY);
				int c = read();
				if (c == ACK) {
					state = STATE_ADDRESS;
					addToLog("Got Acknowledge!");
				} else {
					addToLog("Unknown response (" + c + ")...?");
					state = STATE_PING;
				}
			}
			while (state == STATE_ADDRESS) {
				addToLog("Sending start address...");
				write((min & 0xFF000000) >>> 24);
				write((min & 0x00FF0000) >>> 16);
				write((min & 0x0000FF00) >>> 8);
				write(min & 0x000000FF);
				int c = read();
				if (c == OKAY) {
					state = STATE_LENGTH;
					addToLog("Address acknowledged!");
				} else {
					state = STATES;
					addToLog("Invalid response (" + c + ")!");
				}
			}
			while (state == STATE_LENGTH) {
				addToLog("Sending length...");
				write((firmware.length & 0xFF000000) >>> 24);
				write((firmware.length & 0x00FF0000) >>> 16);
				write((firmware.length & 0x0000FF00) >>> 8);
				write(firmware.length & 0x000000FF);
				int c = read();
				if (c == OKAY) {
					state = STATE_FIRMWARE;
					addToLog("Length acknowledged!");
				} else {
					state = STATES;
					addToLog("Invalid response (" + c + ")!");
				}
			}
			int byteCounter = 0;
			int pageCounter = 0;
			while (state == STATE_FIRMWARE) {
				if (!flowStopped) {
					write(firmware[byteCounter++]);
					if (byteCounter >= firmware.length) {
						state = STATES;
						java.util.Date stopDate = new java.util.Date();
						long diff = stopDate.getTime() - startDate.getTime();
						double timediff = diff / 1000;
						addToLog("YASAB finished after " + timediff + " seconds!\n");
					}
				}
				if (available() > 0) {
					int c = read();
					if (c == XON) {
						flowStopped = false;
					} else if (c == XOFF) {
						flowStopped = true;
					} else if (c == ERROR) {
						addToLog("Bootloader aborted!");
						state = STATES;
					} else if (c == OKAY) {
						pageCounter++;
						addToLog(pageCounter + "p " + byteCounter + "b " + ((100 * byteCounter) / firmware.length) + "%");
					} else {
						addToLog("Unknown response (" + c + ")!");
					}
				}
			}
		}
		try {
			socket.close();
		} catch (IOException e) {}
	}

	private void sleep(int ms) {
		try {
			super.sleep(ms);
		} catch (InterruptedException e) {
		}
	}

	private int available() {
		try {
			return input.available();
		} catch (IOException e) {
			state = STATES;
			mMain.handler.obtainMessage(MainActivity.MESSAGE_ERROR,
					e.getMessage()).sendToTarget();
			return 0;
		}
	}

	private int read() {
		try {
			return input.read();
		} catch (IOException e) {
			state = STATES;
			mMain.handler.obtainMessage(MainActivity.MESSAGE_ERROR,
					e.getMessage()).sendToTarget();
			return 0;
		}
	}

	private void write(int c) {
		try {
			output.write(c);
		} catch (IOException e) {
			state = STATES;
			mMain.handler.obtainMessage(MainActivity.MESSAGE_ERROR,
					e.getMessage()).sendToTarget();
		}
	}

	private void addToLog(String l) {
		mMain.handler.obtainMessage(MainActivity.MESSAGE_READ, l).sendToTarget();
	}
}
