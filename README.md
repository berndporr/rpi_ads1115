# Data acquisition with the ADS1115 on the raspberry PI with callback handlers

![alt tag](ads1115.jpg)

The ADS1115 a sigma delta converter which has
differential inputs, a PGA and programmable sampling rate from 8Hz..860Hz. It's
perfect for slowly changing inputs such as pressure, temperature,
heart rate etc.

This repo offers the class `ADS1115rpi` which manages the
communication with the ADS1115. The user just needs to
implement the callback `hasSample(float sample)` which is called
at the requested sampling rate.

The class uses the ALERT/RDY of the ADS1115 connected to GPIO 17 to
establish the sampling rate. The ADS1115 ALERT/RDY pin is configured
that it triggers the callback after a sample has become available.

## Building:

To build:

    cmake .

    make

## Install

    sudo make install

## Usage example

In the subdir `example` is a simple application which prints
the ADC data to the screen or you can pipe it into a file.

    cd example
    sudo ./ads1115_printer

## General usage

The online doc is here: https://berndporr.github.io/rpi_ads1115/

You need to implement a callback handler:
```
virtual void hasSample(float v) {
};
```

Start the data acquisition:
```
myAD7705comm.start(settings)
```
once `start` has been called `hasSample` will be called at the
specified sampling rate.

Stop the data acquisition:
```
myAD7705comm.stop();
```

## Author: Bernd Porr

   - bernd.porr@glasgow.ac.uk
   - mail@berndporr.me.uk
   - www.berndporr.me.uk
