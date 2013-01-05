# Visualizer

Visualizer is a small menu-driven Unix command line utility, utilizing GNUs [ncurses](http://www.gnu.org/software/ncurses/) and my own [Unix Serial Library](https://github.com/xythobuz/Snippets/tree/master/WinUnixSerial).
It allows you to connect to an xyControl with an attached Inertial Measurement Unit, running the visualizer.hex firmware.
Data from the IMU can then be displayed, more or less graphically.

**Beware!** This is full of dirty hacks and probably breaks when using a terminal window not big enough...

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
