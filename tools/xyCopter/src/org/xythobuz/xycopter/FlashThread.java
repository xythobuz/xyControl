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

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;

import com.dropbox.client2.DropboxAPI.Entry;
import com.dropbox.client2.exception.DropboxException;

import android.app.AlertDialog;
import android.content.DialogInterface;

class FlashThread extends Thread {
	
	public MainActivity mMain;
	public int minAddress;
	public int[] firmware;
	public volatile int whichFile = -1;
	public String[] hexfiles;
	
	public FlashThread(MainActivity main) {
		mMain = main;
	}
	
	private void addToLog(String l) {
		mMain.handler.obtainMessage(MainActivity.MESSAGE_READ, l).sendToTarget();
	}
	
	public void run() {
		mMain.handler.obtainMessage(MainActivity.MESSAGE_READ, "\nConnecting to Dropbox...");
		try {
			addToLog("Listing Dropbox files...");
			Entry root = mMain.mDBApi.metadata("/", 1000, null, true, null);
			String[] files = new String[root.contents.size()];
			int i = 0;
			for (Entry ent : root.contents) {
				files[i++] = ent.path;
			}
			filesListed(files);
			while (whichFile == -1);
			getFirmwareFile(hexfiles[whichFile]);
		} catch (DropboxException e) {
			fileListingError(e.getMessage());
		}
	}
	
	public void filesListed(String[] fs) {
		hexfiles = filterOnlyHexFiles(fs);
		AlertDialog.Builder builder = new AlertDialog.Builder(mMain);
		builder.setTitle(R.string.hex_select);
		builder.setItems(hexfiles, new DialogInterface.OnClickListener() {
			public void onClick(DialogInterface dialog, int which) {
				whichFile = which;
			}
		});
		mMain.handler.obtainMessage(MainActivity.MESSAGE_ALERT_DIALOG, builder).sendToTarget();
	}
	
	public void fileListingError(String error) {
		mMain.handler.obtainMessage(MainActivity.MESSAGE_DROPBOX_FAIL, error).sendToTarget();
	}
	
	private void getFirmwareFile(String f) {
		File file = null;
		FileOutputStream outputStream = null;
		addToLog("Downloading \"" + f + "\"...");
		try {
			file = File.createTempFile("xycopter_", ".hex", mMain.getCacheDir());
			outputStream = new FileOutputStream(file);
			mMain.mDBApi.getFile(f, null, outputStream, null);
			parseFirmwareFile(file);
		} catch (Exception e) {
			mMain.handler.obtainMessage(MainActivity.MESSAGE_ERROR, e.getMessage()).sendToTarget();
			e.printStackTrace();
		} finally {
			if (outputStream != null) {
				try {
					outputStream.close();
				} catch (IOException e) {}
			}
			if (file != null) {
				file.delete();
			}
		}
	}
	
	private void parseFirmwareFile(File f) throws FileNotFoundException {
		FileInputStream in = new FileInputStream(f);
		HexParser h = new HexParser(in, f.length());
		if (h.read()) {
			mMain.handler.obtainMessage(MainActivity.MESSAGE_HEX_ERROR, R.string.hex_read).sendToTarget();
			return;
		}
		if (h.parse()) {
			mMain.handler.obtainMessage(MainActivity.MESSAGE_HEX_ERROR, R.string.hex_invalid).sendToTarget();
			return;
		}
		minAddress = h.minAddress();
		firmware = h.extract();
		mMain.handler.obtainMessage(MainActivity.MESSAGE_HEX_PARSED).sendToTarget();
	}
	
	private String[] filterOnlyHexFiles(String[] all) {
		int c = 0;
		for (int i = 0; i < all.length; i++) {
			if (all[i].endsWith(".hex")) {
				c++;
			}
		}
		String[] hex = new String[c];
		c = 0;
		for (int i = 0; i < all.length; i++) {
			if (all[i].endsWith(".hex")) {
				hex[c++] = all[i];
			}
		}
		return hex;
	}
}
