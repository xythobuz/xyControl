/*
 * Remote.java
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
import javax.swing.event.*;
import java.awt.*;
import java.awt.event.*;
import javax.imageio.*;
import java.io.*;

public class Remote extends JFrame implements ActionListener {

    private final String preferredPort = "/dev/tty.usbserial-AE01539L";

    public int width = 420;
    public int height = 330;
    public int xOff = 10;
    public int yOff = 10;

    public SerialCommunicator serial;

    private JComboBox selector;
    private JButton connector;

    public JFrame logF;
    public JTextArea logArea;
    public JScrollPane logPane;

    public Remote() {
        super("xyControl Telemetry");

        GraphicsDevice gd = GraphicsEnvironment.getLocalGraphicsEnvironment().getDefaultScreenDevice();
        xOff = gd.getDisplayMode().getWidth() / 3;
        xOff -= width / 2;
        yOff = gd.getDisplayMode().getHeight() / 2;
        yOff -= height / 2;

        serial = new SerialCommunicator(this);

        setBounds(xOff, yOff, width, height);
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setLayout(null);
        setResizable(false);
        Container c = getContentPane();

        String[] ports = HelperUtility.getPorts();
        if ((ports == null) || (ports.length == 0)) {
            showError("No serial ports found!");
            System.exit(1);
        }
        java.util.List<String> list = java.util.Arrays.asList(ports);
        java.util.Collections.reverse(list);
        ports = (String[])list.toArray();
        for (int i = 0; i < ports.length; i++) {
            if (ports[i].equals(preferredPort) && (i != 0)) {
                String tmp = ports[0];
                ports[0] = ports[i];
                ports[i] = tmp;
            }
        }
        selector = new JComboBox(ports);
        selector.setBounds(10, 10, 245, 30);
        add(selector);

        connector = new JButton();
        connector.setText("Connect");
        connector.setBounds(265, 10, 145, 30);
        connector.addActionListener(this);
        add(connector);

        // Shutdown Hook to close an opened serial port
        Runtime.getRuntime().addShutdownHook(new Thread(new ShutdownThread(this), "Serial Closer"));

        logF = new JFrame("Log");
        logF.setLayout(new FlowLayout());
        logArea = new JTextArea("Initialized FlightRemote!", 10, 30);
        logArea.setLineWrap(true);
        logPane = new JScrollPane(logArea);
        logPane.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_ALWAYS);
        logF.add(logPane);
        logF.setDefaultCloseOperation(JFrame.HIDE_ON_CLOSE);
        logF.pack();
        logF.setLocation(xOff + 10, yOff + height + 10);
        logF.setVisible(true);

        setVisible(true);
    }

    public void log(String l) {
        logArea.append("\n" + l);
        logArea.setCaretPosition(logArea.getDocument().getLength());
    }

    public void actionPerformed(ActionEvent e) {
        if (e.getSource() == connector) {
            if (serial.isOpen()) {
                serial.closePort();
                connector.setText("Connect");
                log("Disconnected!");
            } else {
                if (!serial.openPort((String)selector.getSelectedItem())) {
                    showError("Could not open port!");
                } else {
                    connector.setText("Disconnect");
                    log("Connected!");
                }
            }
        }
    }

    public void showError(String error) {
        JOptionPane.showMessageDialog(null, error, "Error", JOptionPane.ERROR_MESSAGE);
        log("Error: " + error);
    }

    public static void main(String[] args) {
        Remote r = new Remote();
    }
}

// Close serial connection on shutdown
class ShutdownThread implements Runnable {
    private Remote remote = null;

    public ShutdownThread(Remote r) {
        remote = r;
    }

    public void run() {
        if (remote.serial != null) {
            if (remote.serial.isOpen()) {
                remote.serial.closePort();
            }
        }
    }
}
