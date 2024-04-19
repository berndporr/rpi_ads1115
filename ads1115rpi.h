#ifndef __ADS1115RPI_H
#define __ADS1115RPI_H

/*
 * ADS1115 class to read data at a given sampling rate
 *
 * Copyright (c) 2007  MontaVista Software, Inc.
 * Copyright (c) 2007  Anton Vorontsov <avorontsov@ru.mvista.com>
 * Copyright (c) 2013-2024  Bernd Porr <mail@berndporr.me.uk>
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
#include <assert.h>
#include <linux/i2c-dev.h>
#include <thread>
#include <gpiod.h>
#include <vector>

// enable debug messages and error messages to stderr
#ifndef NDEBUG
#define DEBUG
#endif

static const char could_not_open_i2c[] = "Could not open I2C.\n";

#define ISR_TIMEOUT 1000

// default address if ADDR is pulled to GND
#define DEFAULT_ADS1115_ADDRESS 0x48

// default GPIO pin for the ALRT/DRY signal
#define DEFAULT_ALERT_RDY_TO_GPIO 17




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
     * Get the sampling rate in Hz
     **/
    inline unsigned getSamplingRate() const {
	const unsigned SamplingRateEnum2Value[8] =
	    {8,16,32,64,128,250,475,860};
	return SamplingRateEnum2Value[samplingRate];
    }

    /**
     * Sampling rate requested
     **/
    SamplingRates samplingRate = FS8HZ;

    /**
     * Full scale range: 2.048V, 1.024V, 0.512V or 0.256V.
     **/
    enum PGA {
	FSR2_048 = 2,
	FSR1_024 = 3,
	FSR0_512 = 4,
	FSR0_256 = 5
    };

    /**
     * Requested full scale range
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
     * Requested input channel (AIN0..AIN3)
     **/
    Input channel = AIN0;

    /**
     * GPIO Chip number which receives the Data Ready signal.
     * For RPI 1-4 it's chip 0. For the RPI5 it's chip number 4.
     **/
    int drdy_chip = 0;

    /**
     * GPIO pin connected to ALERT/RDY
     **/
    int drdy_gpio = DEFAULT_ALERT_RDY_TO_GPIO;
};


/**
 * This class reads data from the ADS1115 in the background (separate
 * thread) and calls a callback function whenever data is available.
 **/
class ADS1115rpi {

public:
    /**
     * Destructor which makes sure the data acquisition
     * stops on exit.
     **/
    ~ADS1115rpi() {
	stop();
    }

    struct ADSCallbackInterface {
	    /**
	     * Called when a new sample is available.
	     * This needs to be implemented in a derived
	     * class by the client. Defined as abstract.
	     * \param sample Voltage from the selected channel.
	     **/
	virtual void hasADS1115Sample(float sample) = 0;
    };

    void registerCallback(ADSCallbackInterface* ci) {
	adsCallbackInterfaces.push_back(ci);
    }

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
     * Returns the current settings
     **/
    ADS1115settings getADS1115settings() const {
	return ads1115settings;
    }

    /**
     * Stops the data acquistion
     **/
    void stop();

private:
    ADS1115settings ads1115settings;

    void dataReady();

    void worker();

    void i2c_writeWord(uint8_t reg, unsigned data);
    unsigned i2c_readWord(uint8_t reg);
    int i2c_readConversion();

    const uint8_t reg_config = 1;
    const uint8_t reg_lo_thres = 2;
    const uint8_t reg_hi_thres = 3;

    float fullScaleVoltage() {
	switch (ads1115settings.pgaGain) {
	case ADS1115settings::FSR2_048:
	    return 2.048f;
	case ADS1115settings::FSR1_024:
	    return 1.024f;
	case ADS1115settings::FSR0_512:
	    return 0.512f;
	case ADS1115settings::FSR0_256:
	    return 0.256f;
	}
	assert(1 == 0);
	return 0;
    }

    struct gpiod_chip *chipDRDY = nullptr;
    struct gpiod_line *pinDRDY = nullptr;

    std::thread thr;

    int fd_i2c = -1;

    bool running = false;

    std::vector<ADSCallbackInterface*> adsCallbackInterfaces;
};


#endif
