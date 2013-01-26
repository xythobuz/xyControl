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
    public int height = 420;
    public int xOff = 10;
    public int yOff = 10;

    public SerialCommunicator serial;

    private JComboBox selector;
    private JButton connector;
    private JButton showLog;
    private JLabel rollLabel;
    private JLabel pitchLabel;
    private JLabel yawLabel;
    private JLabel rollCurrent;
    private JLabel pitchCurrent;
    private JLabel yawCurrent;
    private JTextField rollTarget;
    private JTextField pitchTarget;
    private JTextField yawTarget;
    private JButton calibrate;
    private JButton setAngles;
    private JButton resetAngles;

    private JProgressBar[] bars = new JProgressBar[4];
    private JLabel[] barLabels = new JLabel[4];
    private String[] barText = {"1", "3", "2", "4"};

    public JFrame logF;
    public JTextArea logArea;
    public JScrollPane logPane;

    public Remote() {
        super("xyControl Telemetry");

        GraphicsDevice gd = GraphicsEnvironment.getLocalGraphicsEnvironment().getDefaultScreenDevice();
        xOff = gd.getDisplayMode().getWidth() / 4;
        xOff -= width / 2;
        yOff = gd.getDisplayMode().getHeight() / 3;
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
        logArea = new JTextArea("Initialized Telemetry!", 10, 30);
        logArea.setLineWrap(true);
        logPane = new JScrollPane(logArea);
        logPane.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_ALWAYS);
        logF.add(logPane);
        logF.setDefaultCloseOperation(JFrame.HIDE_ON_CLOSE);
        logF.pack();
        logF.setLocation(xOff + 10, yOff + height + 10);

        showLog = new JButton();
        showLog.setText("Log");
        showLog.setBounds(10, 40, 100, 30);
        showLog.addActionListener(this);
        add(showLog);

        rollLabel = new JLabel();
        rollLabel.setText("Roll:");
        rollLabel.setBounds(145, 100, 35, 15);
        add(rollLabel);
        pitchLabel = new JLabel();
        pitchLabel.setText("Pitch:");
        pitchLabel.setBounds(145, 130, 35, 15);
        add(pitchLabel);
        yawLabel = new JLabel();
        yawLabel.setText("Yaw:");
        yawLabel.setBounds(145, 160, 35, 15);
        add(yawLabel);

        rollCurrent = new JLabel();
        rollCurrent.setText("-180°");
        rollCurrent.setBounds(185, 100, 40, 15);
        add(rollCurrent);
        pitchCurrent = new JLabel();
        pitchCurrent.setText("-180°");
        pitchCurrent.setBounds(185, 130, 40, 15);
        add(pitchCurrent);
        yawCurrent = new JLabel();
        yawCurrent.setText("-180°");
        yawCurrent.setBounds(185, 160, 40, 15);
        add(yawCurrent);

        rollTarget = new JTextField();
        rollTarget.setText("-180");
        rollTarget.setBounds(235, 100, 45, 20);
        add(rollTarget);
        pitchTarget = new JTextField();
        pitchTarget.setText("-180");
        pitchTarget.setBounds(235, 130, 45, 20);
        add(pitchTarget);
        yawTarget = new JTextField();
        yawTarget.setText("-180");
        yawTarget.setBounds(235, 160, 45, 20);
        add(yawTarget);

        calibrate = new JButton();
        calibrate.setText("Calibrate");
        calibrate.setBounds(290, 95, 100, 30);
        calibrate.addActionListener(this);
        add(calibrate);
        setAngles = new JButton();
        setAngles.setText("Set");
        setAngles.setBounds(290, 125, 100, 30);
        setAngles.addActionListener(this);
        add(setAngles);
        resetAngles = new JButton();
        resetAngles.setText("Reset");
        resetAngles.setBounds(290, 155, 100, 30);
        resetAngles.addActionListener(this);
        add(resetAngles);

        for (int i = 0; i < bars.length; i++) {
            bars[i] = new JProgressBar(SwingConstants.VERTICAL, 0, 255);
            bars[i].setBounds(10 + (30 * i), 95, 20, 290);
            bars[i].setValue(127);
            add(bars[i]);
            barLabels[i] = new JLabel();
            barLabels[i].setText(barText[i]);
            barLabels[i].setBounds(15 + (30 * i), 80, 15, 15);
            add(barLabels[i]);
        }

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
        } else if (e.getSource() == showLog) {
            logF.setVisible(true);
        } else if (e.getSource() == calibrate) {
        } else if (e.getSource() == setAngles) {
        } else if (e.getSource() == resetAngles) {
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
