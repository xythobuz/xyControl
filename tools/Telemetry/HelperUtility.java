/*
 * HelperUtility.java
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

/**
 * Helper class which runs our native library, which is loaded from inside the Jar.
 * @author Thomas Buck
 * @version 1.0
 */
import java.io.*;
import java.nio.channels.*;
import java.nio.*;
import java.util.StringTokenizer;

public class HelperUtility {

    // Load libraries, copy from Jar if needed
    // Inspired by:
    // http://stackoverflow.com/questions/1611357/how-to-make-a-jar-file-that-include-dll-files
    static {
        loadFromJar();
    }

    /**
     * When packaged into JAR extracts DLLs, places these into
     */
    private static void loadFromJar() {
        // we need to put DLL to temp dir
        String os = System.getProperty("os.name").toLowerCase();
        String path = "CC_";
        if (os.indexOf("windows") > -1) {
            loadLib(path, "Serial.dll");
        } else if (os.indexOf("mac") > -1) {
            loadLib(path, "libSerial.jnilib");
        } else { // Hopefully unix/linux...
            loadLib(path, "libSerial.so");
        }
    }

    /**
     * Puts library to temp dir and loads to memory
     * @param path Put in front of file name
     * @param Complete name of file to load (w/ lib & .dll)
     */
    private static void loadLib(String path, String name) {
        try {
            // have to use a stream
            InputStream in = HelperUtility.class.getResourceAsStream(name);
            File fileOut = new File(System.getProperty("java.io.tmpdir") + "/" + path + name);
            OutputStream out = new FileOutputStream(fileOut);
            ReadableByteChannel inChannel = Channels.newChannel(in);
            WritableByteChannel outChannel = Channels.newChannel(out);
            fastChannelCopy(inChannel, outChannel);
            inChannel.close();
            outChannel.close();
            path = fileOut.getPath();
            try {
                System.load(path);
            } catch (UnsatisfiedLinkError e) {
                System.out.println("ERROR: Library does not exist!");
                return;
            } catch (SecurityException e) {
                System.out.println("ERROR: Not allowed to load Library!");
                return;
            } catch (NullPointerException e) {
                System.out.println("ERROR: Library name is null!");
                return;
            } catch (Exception e) {
                System.out.println("ERROR: " + e.toString());
                return;
            }
            // System.out.println("Loaded Serial Library at \"" + path + "\"");
            return;
        } catch (Exception e) {
            System.out.println("ERROR: Failed to load Serial Library:");
            e.printStackTrace();
            return;
        }
    }

    /**
     * Get the names of all available serial ports.
     *
     * @return Array of port names. First entry is "No serial ports!" if no
     *         others
     */
    // Get ports as single String from getPortsOS() and put them in an array
    public static String[] getPorts() {
        String portLines = getPortsOS();
        if (portLines == null) {
            String[] ports = { "No serial ports!" };
            return ports;
        } else {
            StringTokenizer sT = new StringTokenizer(portLines, "\n");
            int size = sT.countTokens();
            String[] ports = new String[size];
            for (int i = 0; i < size; i++) {
                ports[i] = sT.nextToken();
            }
            return ports;
        }
    }

    /**
     * Get all the existing serial port names
     *
     * @return List of port names. \n between entries
     */
    private static String getPortsOS() {
        String os = System.getProperty("os.name").toLowerCase();
        try {
            if (os.indexOf("windows") > -1) {
                return getThePorts("COM");
            } else if (os.indexOf("mac") > -1) {
                return getThePorts("tty.");
            } else {
                return getThePorts("rfcomm");
            }
        } catch (UnsatisfiedLinkError e) {
            System.out.println("ERROR: Library not loaded! (getPorts)");
            return "Serial Library Error!\n";
        }
    }

    private static native String getThePorts(String search);

    /**
     * Open Connection to a port
     *
     * @return TRUE if successful
     * @param name
     *            Port to open
     */
    public static boolean openPort(String name) {
        try {
            return openPortNative(name);
        } catch (UnsatisfiedLinkError e) {
            System.out.println("ERROR: Library not loaded! (openPort)");
            return false;
        }
    }

    private static native boolean openPortNative(String name);

    /**
     * Close Connection to port
     */
    public static void closePort() {
        try {
            closePortNative();
        } catch (UnsatisfiedLinkError e) {
            System.out.println("ERROR: Library not loaded! (closePort)");
        }
    }

    private static native void closePortNative();

    /**
     * Read data from Cube
     *
     * @param length Amount of data to read
     * @return Data read, empty on error
     */
    public static short[] readData(int length) {
        try {
            return readDataNative(length);
        } catch (UnsatisfiedLinkError e) {
            System.out.println("ERROR: Library not loaded! (readData)");
            return null;
        }
    }

    private static native short[] readDataNative(int length);

    /**
     * Write data to Cube
     *
     * @param data
     *            Data to write
     * @param length
     *            Length of data
     *
     * @return TRUE on success, FALSE on error
     */
    public static boolean writeData(short[] data, int length) {
        try {
            return writeDataNative(data, length);
        } catch (UnsatisfiedLinkError e) {
            System.out.println("ERROR: Library not loaded! (writeData)");
            return false;
        }
    }

    private static native boolean writeDataNative(short[] data, int length);

    // http://thomaswabner.wordpress.com/2007/10/09/fast-stream-copy-using-javanio-channels/
    private static void fastChannelCopy(ReadableByteChannel src, WritableByteChannel dest) throws IOException {
        ByteBuffer buffer = ByteBuffer.allocateDirect(16 * 1024);
        while (src.read(buffer) != -1) {
            // prepare the buffer to be drained
            buffer.flip();
            // write to the channel, may block
            dest.write(buffer);
            // If partial transfer, shift remainder down
            // If buffer is empty, same as doing clear()
            buffer.compact();
        }
        // EOF will leave buffer in fill state
        buffer.flip();
        // make sure the buffer is fully drained.
        while (buffer.hasRemaining()) {
            dest.write(buffer);
        }
    }
}
