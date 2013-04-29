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

    private JComboBox selector;
    private JButton connector;
    private JButton showLog;
    private JLabel rollLabel;
    private JLabel pitchLabel;
    private JLabel yawLabel;
    private JLabel rollCurrent;
    private JLabel pitchCurrent;
    private JLabel yawCurrent;
    private JLabel voltLabel;
    private JLabel voltCurrent;

    public SerialCommunicator serial = null;
    public SerialThread serialThread = null;

    private JProgressBar[] bars = new JProgressBar[3];
    private JLabel[] barLabels = new JLabel[3];
    private String[] barText = {"P", "R", "Y"};

    public JFrame logF;
    public JTextArea logArea;
    public JScrollPane logPane;

    public static final int MESSAGE_READ = 1;
    public static final int MESSAGE_FREQ_READ = 2;
    public static final int MESSAGE_MOTOR_READ = 3;
    public static final int MESSAGE_PIDINTRANGE_READ = 4;
    public static final int MESSAGE_PIDRANGE_READ = 5;
    public static final int MESSAGE_PIDVAL_READ = 6;
    public static final int MESSAGE_PID_READ = 7;
    public static final int MESSAGE_PITCH_READ = 8;
    public static final int MESSAGE_ROLL_READ = 9;
    public static final int MESSAGE_YAW_READ = 10;
    public static final int MESSAGE_VOLT_READ = 11;

    public Remote() {
        super("xyControl Telemetry");

        GraphicsDevice gd = GraphicsEnvironment.getLocalGraphicsEnvironment().getDefaultScreenDevice();
        xOff = gd.getDisplayMode().getWidth() / 4;
        xOff -= width / 2;
        yOff = gd.getDisplayMode().getHeight() / 3;
        yOff -= height / 2;

        serial = new SerialCommunicator();

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
                break;
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
        rollLabel.setBounds(105, 100, 35, 15);
        add(rollLabel);
        pitchLabel = new JLabel();
        pitchLabel.setText("Pitch:");
        pitchLabel.setBounds(105, 130, 35, 15);
        add(pitchLabel);
        yawLabel = new JLabel();
        yawLabel.setText("Yaw:");
        yawLabel.setBounds(105, 160, 35, 15);
        add(yawLabel);

        rollCurrent = new JLabel();
        rollCurrent.setText("-180.00°");
        rollCurrent.setBounds(145, 100, 60, 15);
        add(rollCurrent);
        pitchCurrent = new JLabel();
        pitchCurrent.setText("-180.00°");
        pitchCurrent.setBounds(145, 130, 60, 15);
        add(pitchCurrent);
        yawCurrent = new JLabel();
        yawCurrent.setText("-180.00°");
        yawCurrent.setBounds(145, 160, 60, 15);
        add(yawCurrent);

        voltLabel = new JLabel();
        voltLabel.setText("Batt: ");
        voltLabel.setBounds(105, 190, 35, 15);
        add(voltLabel);
        voltCurrent = new JLabel();
        voltCurrent.setText("11.11V");
        voltCurrent.setBounds(145, 190, 60, 15);
        add(voltCurrent);

        for (int i = 0; i < bars.length; i++) {
            bars[i] = new JProgressBar(SwingConstants.VERTICAL, -180, 180);
            bars[i].setBounds(10 + (30 * i), 95, 20, 290);
            bars[i].setValue(0);
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

    public void sendMessage(int msg, final String line) {
        switch(msg) {
            case MESSAGE_READ:
                log("RX: " + line);
                break;

            case MESSAGE_FREQ_READ:

                break;

            case MESSAGE_MOTOR_READ:

                break;

            case MESSAGE_PIDINTRANGE_READ:

                break;

            case MESSAGE_PIDRANGE_READ:

                break;

            case MESSAGE_PIDVAL_READ:

                break;

            case MESSAGE_PID_READ:

                break;

            case MESSAGE_PITCH_READ:
                try {
                    float p = Float.parseFloat(line);
                    bars[0].setValue(Math.round(p));
                    pitchCurrent.setText(String.format("%.2f", p) + "°");
                } catch (Exception e) {
                    log(e.getMessage());
                }
                break;

            case MESSAGE_ROLL_READ:
                try {
                    float r = Float.parseFloat(line);
                    bars[1].setValue(Math.round(r));
                    rollCurrent.setText(String.format("%.2f", r) + "°");
                } catch (Exception e) {
                    log(e.getMessage());
                }
                break;

            case MESSAGE_YAW_READ :
                try {
                    float y = Float.parseFloat(line);
                    bars[2].setValue(Math.round(y));
                    yawCurrent.setText(String.format("%.2f", y) + "°");
                } catch (Exception e) {
                    log(e.getMessage());
                }
                break;

            case MESSAGE_VOLT_READ :
                try {
                    float v = Float.parseFloat(line);
                    voltCurrent.setText(String.format("%.2f", v) + "V");
                } catch (Exception e) {
                    log(e.getMessage());
                }
                break;

            default:
                log("Unknown Message: " + msg + ": " + line);
                break;
        }
    }

    public void actionPerformed(ActionEvent e) {
        if (e.getSource() == connector) {
            if (serial.isOpen()) {
                serial.closePort();
                connector.setText("Connect");
                log("Disconnected!");
                if (serialThread != null) {
                    serialThread.disconnect();
                    serialThread = null;
                }
            } else {
                if (!serial.openPort((String)selector.getSelectedItem())) {
                    showError("Could not open port!");
                } else {
                    connector.setText("Disconnect");
                    log("Connected!");
                    serialThread = new SerialThread(this);
                    serialThread.start();
                }
            }
        } else if (e.getSource() == showLog) {
            logF.setVisible(true);
        }
    }

    class SerialThread extends Thread {
        public boolean running = true;
        private Remote remote;

        SerialThread(Remote r) {
            remote = r;
        }

        public void run() {
            while (running) {
                // Read from the InputStream
                String line = remote.serial.readLine();
                // Send to Remote
                if (line == null) {
                    remote.log("Error while reading line!");
                    continue;
                }
                if (line.length() < 1) {
                    remote.log("Read a single character!");
                    continue;
                }
                char first = line.charAt(0);
                switch (first) {
                case 'q':
                    remote.sendMessage(Remote.MESSAGE_FREQ_READ, line.substring(1));
                    break;
                case 'r':
                    remote.sendMessage(Remote.MESSAGE_PIDINTRANGE_READ, line.substring(1));
                    break;
                case 's':
                    remote.sendMessage(Remote.MESSAGE_PIDRANGE_READ, line.substring(1));
                    break;
                case 't':
                    remote.sendMessage(Remote.MESSAGE_PIDVAL_READ, line.substring(1));
                    break;
                case 'u':
                    remote.sendMessage(Remote.MESSAGE_PID_READ, line.substring(1));
                    break;
                case 'v':
                    remote.sendMessage(Remote.MESSAGE_MOTOR_READ, line.substring(1));
                    break;
                case 'w':
                    remote.sendMessage(Remote.MESSAGE_PITCH_READ, line.substring(1));
                    break;
                case 'x':
                    remote.sendMessage(Remote.MESSAGE_ROLL_READ, line.substring(1));
                    break;
                case 'y':
                    remote.sendMessage(Remote.MESSAGE_YAW_READ, line.substring(1));
                    break;
                case 'z':
                    remote.sendMessage(Remote.MESSAGE_VOLT_READ, line.substring(1));
                    break;
                default:
                    remote.sendMessage(Remote.MESSAGE_READ, line);
                    break;
                }
            }
        }

        public void disconnect() {
            running = false;
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
