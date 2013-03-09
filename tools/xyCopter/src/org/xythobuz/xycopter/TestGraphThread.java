/*
 * ConnectThread.java
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

class TestGraphThread extends Thread {

	private MainActivity m;
	private boolean keepRunning = true;

	public TestGraphThread(MainActivity main) {
		m = main;
	}

	public void run() {
		while (keepRunning) {
			for (double x = 0; (x <= Math.PI * 2) && keepRunning; x += 0.1) {
				double a = Math.sin(x);
				double b = Math.cos(x);
				double c = 0.0;
				a = Math.toDegrees(a);
				b = Math.toDegrees(b);
				c = Math.toDegrees(c);
				m.handler.obtainMessage(MainActivity.MESSAGE_PITCH_READ, -1,
						-1, Double.toString(a)).sendToTarget();
				m.handler.obtainMessage(MainActivity.MESSAGE_ROLL_READ, -1, -1,
						Double.toString(b)).sendToTarget();
				m.handler.obtainMessage(MainActivity.MESSAGE_YAW_READ, -1, -1,
						Double.toString(c)).sendToTarget();
				try {
					sleep(50);
				} catch (InterruptedException e) {
				}
			}
		}
	}

	public void cancel() {
		keepRunning = false;
	}
}
