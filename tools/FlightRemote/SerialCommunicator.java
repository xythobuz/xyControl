/*
 * SerialCommunicator.java
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

import javax.swing.*;
import java.awt.*;

class SerialCommunicator {
    private Remote remote;
    private boolean opened = false;

    public final int ERROR = -1;
    public final int TIMEOUT = -2;
    public int lastError = 0;

    public SerialCommunicator(Remote r) {
        remote = r;
    }

    public boolean openPort(String port) {
        if (!HelperUtility.openPort(port)) {
            return false;
        } else {
            opened = true;
            return true;
        }
    }

    public void closePort() {
        HelperUtility.closePort();
        opened = false;
    }

    public boolean isOpen() {
        return opened;
    }

    public String readLine() {
        // Read until \n, return as String, with \n stripped
        if (opened) {
            StringBuilder ret = new StringBuilder();
            while(true) {
                short[] data = readData(1);
                if ((data == null) || (data.length) != 1) {
                    return null;
                }
                char c = (char)data[0];
                if (c != '\n') {
                    ret.append(c);
                } else {
                    return ret.toString();
                }
            }
        } else {
            return null;
        }
    }

    public boolean writeChar(int c) {
        int errorCount = 10;
        short[] dat = new short[1];
        dat[0] = (short)c;
        return writeData(dat, 1);
    }

    public short[] readData(int length) {
        if (opened) {
            short[] tmp = HelperUtility.readData(length);
            if (tmp == null) {
                lastError = TIMEOUT;
                return null;
            } else if (tmp.length != length) {
                lastError = ERROR;
                return null;
            } else {
                return tmp;
            }
        } else {
            return null;
        }
    }

    private boolean writeData(short[] data, int length) {
        if (opened) {
            boolean tmp = HelperUtility.writeData(data, length);
            if (!tmp) {
                remote.showError("Could not write data!");
            }
            return tmp;
        } else {
            return false;
        }
    }
}
