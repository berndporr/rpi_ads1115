#ifndef __ADS1115RPI_H
#define __ADS1115RPI_H

/*
 * ADS1115 class to read data at a given sampling rate
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
#include <assert.h>

// enable debug messages and error messages to stderr
#ifndef NDEBUG
#define DEBUG
#endif

static const char could_not_open_i2c[] = "Could not open I2C.\n";

#define ISR_TIMEOUT 1000

// default address if ADDR is pulled to GND
#define DEFAULT_ADS1115_ADDRESS 0x48

// default GPIO pin for the ALRT/DRY signal
#define DEFAULT_DATA_READY_GPIO 17




/**
 * ADS1115 initial settings when starting the device.
 **/
struct ADS1115settings {

	/**
	 * I2C bus used (99% always set to one)
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
 * This class reads data from the ADS1115 in the background (separate
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
	 * Called when a new sample is available.
	 * This needs to be implemented in a derived
	 * class by the client. Defined as abstract.
	 * \param sample Voltage from the selected channel
	 **/
	virtual void hasSample(float sample) = 0;

	/**
	 * Selects a different channel at the multiplexer
	 * while running.
	 * Call this in the callback handler hasSample()
	 * to cycle through different channels.
	 * \param channel Sets the channel from A0..A3.
	 **/
	void setChannel(ADS1115settings::Input channel);

	/**
	 * Starts the data acquisition in the background and the
	 * callback is called with new samples.
	 * \param settings A struct with the settings.
	 **/
	void start(ADS1115settings settings = ADS1115settings() );

	/**
	 * Stops the data acquistion
	 **/
	void stop();

private:
	ADS1115settings ads1115settings;

	void dataReady();

	static void gpioISR(int gpio, int level, uint32_t tick, void* userdata) {
		((ADS1115rpi*)userdata)->dataReady();
	}

	void i2c_writeWord(uint8_t reg, unsigned data);
        unsigned i2c_readWord(uint8_t reg);
        int i2c_readConversion();

	const int reg_config = 1;
	const int reg_lo_thres = 2;
	const int reg_hi_thres = 3;

	float fullScaleVoltage() {
		switch (ads1115settings.pgaGain) {
		case ADS1115settings::FSR2_048:
			return 2.048;
		case ADS1115settings::FSR1_024:
			return 1.024;
		case ADS1115settings::FSR0_512:
			return 0.512;
		case ADS1115settings::FSR0_256:
			return 0.256;
		}
		assert(1 == 0);
		return 0;
	}
};


#endif
