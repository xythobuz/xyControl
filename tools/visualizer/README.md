# Visualizer

Visualizer is a small Java Swing GUI capable of talking to a serial port and visualizing raw IMU data. It utilizes my own serial library and it's JNI Interface. This is only a small test, succeeded by the Telemetry App.

## Protocol
<table>
<tr><th>PC</th><th>Length</th><th>Direction</th><th>xyControl</th><th>Length</th></tr>
<tr><td>a</td><td>1</td><td>&rarr;</td><td></td><td></td></tr>
<tr><td></td><td></td><td>&larr;</td><td>Accelerometer Data</td><td>6</td></tr>
<tr><td>g</td><td>1</td><td>&rarr;</td><td></td><td></td></tr>
<tr><td></td><td></td><td>&larr;</td><td>Gyroscope Data</td><td>6</td></tr>
<tr><td>m</td><td>1</td><td>&rarr;</td><td></td><td></td></tr>
<tr><td></td><td></td><td>&larr;</td><td>Magnetometer Data</td><td>6</td></tr>
</table>
Data is 16 bit per X, Y, Z. Most significant byte first!
