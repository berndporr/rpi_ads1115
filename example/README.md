# Example program

![alt tag](setup.jpg)

Connect a sinewave generator to A0 with DC offset.

Run:
```
sudo ./ads1115_printer > /tmp/d.dat
gnuplot
> plot "/tmp/d.dat" w l
```
which plots the data with gnuplot.

![alt tag](sine.png)
