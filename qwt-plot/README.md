# QT ADS1115 plotter

## Required packages

Install the QT5 and Qwt development packages:

```
apt-get install qtdeclarative5-dev
apt-get install libqwt-qt5-dev
```

Make sure that you have installed the ads1115 library (i.e. `sudo make install`) in `/usr/local`.

## Build it

```
cmake .
make
```

## Run it

```
./ads1115plotter
```
