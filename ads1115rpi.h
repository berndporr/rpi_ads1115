#ifndef __AD7705COMM_H
#define __AD7705COMM_H

/*
 * AD7705 class to read data at a given sampling rate
 *
 * Copyright (c) 2007  MontaVista Software, Inc.
 * Copyright (c) 2007  Anton Vorontsov <avorontsov@ru.mvista.com>
 * Copyright (c) 2013-2022  Bernd Porr <mail@berndporr.me.uk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License.
 *
 */
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pigpio.h>

#ifndef NDEBUG
#define DEBUG
#endif

static const char could_not_open_i2c[] = "Could not open I2C.\n";

#define ISR_TIMEOUT 1000

#define DEFAULT_ADS1115_ADDRESS 0x48

#define DEFAULT_DATA_READY_GPIO 17

/**
 * Callback for new samples which needs to be implemented by the main program.
 * The function hasSample needs to be overloaded in the main program.
 **/
class ADS1115callback {
public:
	/**
	 * Called after a sample has arrived.
	 **/
	virtual void hasSample(float sample) = 0;
};

/**
 * ADS1115 initial settings when starting the device.
 **/
struct ADS1115settings {

	/**
	 * I2C bus used (99% always one)
	 **/
	int i2c_bus = 1;
	
	/**
	 * I2C address of the ads1115
	 **/
	uint8_t address = DEFAULT_ADS1115_ADDRESS;
	
	/**
	 * Sampling rates
	 **/
	enum SamplingRates {
		FS8HZ   = 0,
		FS16HZ  = 1,
		FS32HZ  = 2,
		FS64HZ  = 3,
		FS128HZ = 4,
		FS250HZ = 5,
		FS475HZ = 6,
		FS860HZ = 7
	};

	/**
	 * Sampling rate requested
	 **/
	SamplingRates samplingRate = FS8HZ;

	/**
	 * Gains of the PGA
	 **/
	enum PGA {
		FSR6_144 = 0,
		FSR4_096 = 1,
		FSR2_048 = 2,
		FSR1_024 = 3,
		FSR0_512 = 4,
		FSR0_256 = 5
	};

	/**
	 * Requested gain
	 **/
	PGA pgaGain = FSR2_048;

	/**
	 * Channel indices
	 **/
	enum Input {
		AIN0 = 0,
		AIN1 = 1,
		AIN2 = 2,
		AIN3 = 3
	};

	/**
	 * Requested input channel (0 or 1)
	 **/
	Input channel = AIN0;

	/**
	 * If set to true the pigpio is initialised
	 **/
	bool initPIGPIO = true;

	/**
	 * Default GPIO pin for data ready
	 **/
	int drdy_gpio = DEFAULT_DATA_READY_GPIO;
};


/**
 * This class reads data from the AD7705 in the background (separate
 * thread) and calls a callback function whenever data is available.
 **/
class ADS1115rpi {

public:
	/**
	 * Constructor with the spiDevice. The default device
	 * is /dev/spidev0.0.
	 * \param spiDevice The raw /dev spi device.
	 **/
	ADS1115rpi();

	/**
	 * Destructor which makes sure the data acquisition
	 * has stopped.
	 **/
	~ADS1115rpi() {
		stop();
	}
       
	/**
	 * Sets the callback which is called whenever there is a sample.
	 * \param cb Pointer to the callback interface.
	 **/
	void setCallback(ADS1115callback* cb);

	/**
	 * Starts the data acquisition in the background and the
	 * callback is called with new samples.
	 * \param samplingRate The sampling rate of the ADC.
	 **/
	void start(ADS1115settings settings = ADS1115settings() );

	/**
	 * Stops the data acquistion
	 **/
	void stop();

private:
	ADS1115callback* ads1115callback = nullptr;
	ADS1115settings ads1115settings;

	void dataReady();

	static void gpioISR(int gpio, int level, uint32_t tick, void* userdata) {
		((ADS1115rpi*)userdata)->dataReady();
	}

	void i2c_writeWord(uint8_t reg, unsigned data);
        unsigned i2c_readWord(uint8_t reg);

	const int reg_config = 1;
	const int reg_lo_thres = 2;
	const int reg_hi_thres = 3;
};


#endif
