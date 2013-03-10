/*
 * HexParser.java
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

import java.io.FileInputStream;

class HexParser {

	private FileInputStream in;
	private int fileLength = 0;
	private String[] file = null;
	private HexLine[] parsedLine = null;
	
	private class HexLine {
		public boolean valid = true;
		public int length = 0;
		public int address = 0;
		public int type = 0;
		public int[] data = null;
	}
	
	public HexParser(FileInputStream f, long fl) {
		in = f;
		fileLength = (int)fl;
	}
	
	// Return true if error
	public boolean read() {
		try {
			byte[] bytes = new byte[fileLength];
			int read = in.read(bytes, 0, fileLength);
			while (read < fileLength) {
				int tmp = in.read(bytes, read, (fileLength - read));
				if (tmp != -1) {
					read += tmp;
				} else {
					return true;
				}
			}
			file = new String(bytes).split("\\n");
			if (file == null) {
				return true;
			} else {
				parsedLine = new HexLine[file.length];
			}
		} catch (Exception e) {
			return true;
		} finally {
			try {
				in.close();
			} catch (Exception e) {}
		}
		return false;
	}
	
	public boolean parse() {
		for (int i = 0; i < file.length; i++) {
			parsedLine[i] = parseLine(i);
			if (parsedLine[i].valid == false) {
				return true;
			}
		}
		return false;
	}
	
	public int[] extract() {
		int offset = 0;
	    int start = minAddress();
	    int[] d = new int[dataLength()];
	    for (int i = 0; i < parsedLine.length; i++) {
	        HexLine h = parsedLine[i];
	        int a = h.address + (offset << 4);

	        if (h.type == 0x00) {
	            for (int j = 0; j < h.length; j++) {
	                d[(a - start) + j] = h.data[j];
	            }
	        } else if (h.type == 0x02) {
	            offset = parseDigit(i, 0, 4);
	        }
	    }
	    return d;
	}
	
	private int parseDigit(int l, int o, int len) {
		int val = 0;
	    for (int i = 0; i < len; i++) {
	        char c = file[l].charAt(o + i);
	        if ((c >= '0') && (c <= '9')) {
	            c -= '0';
	        } else if ((c >= 'A') && (c <= 'F')) {
	            c -= 'A' - 10;
	        } else if ((c >= 'a') && (c <= 'f')) {
	            c -= 'a' - 10;
	        }
	        val = (16 * val) + c;
	    }
	    return val;
	}
	
	private HexLine parseLine(int l) {
		HexLine h = new HexLine();
		
		if (file[l].charAt(0) != ':') {
			h.valid = false;
			return h;
		}
		
		h.length = parseDigit(l, 1, 2);
		h.address = parseDigit(l, 3, 4);
		h.type = parseDigit(l, 7, 2);
		int checksum = h.length + (h.address & 0x00FF) + h.type;
		checksum += ((h.address & 0xFF00) >> 8);
		
		h.data = new int[h.length];
		for (int i = 0; i < h.length; i++) {
			h.data[i] = parseDigit(l, 9 + (2 * i), 2);
			checksum += h.data[i];
		}
		
		checksum += parseDigit(l, 9 + (2 * h.length), 2);
		
		if ((checksum & 0x00FF) != 0) {
			h.valid = false;
		}
		
		return h;
	}
	
	public int minAddress() {
		int minAdd = 0;
	    int offset = 0;
	    for (int i = 0; i < file.length; i++) {
	        HexLine h = parsedLine[i];
	        int a = h.address + (offset << 4);
	        if (h.type == 0x00) {
	            if (a < minAdd) {
	                minAdd = a;
	            }
	        } else if (h.type == 0x02) {
	            offset = parseDigit(i, 0, 4);
	        }
	    }
	    return minAdd;
	}
	
	public int dataLength() {
		int l = 0;
		for (int i = 0; i < parsedLine.length; i++) {
			HexLine h = parsedLine[i];
			if (h.type == 0) {
				l += h.length;
			}
		}
		return l;
	}
}
