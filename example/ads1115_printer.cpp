/*
 * AD7705 test/demo program for the Raspberry PI
 *
 * Copyright (c) 2007  MontaVista Software, Inc.
 * Copyright (c) 2007  Anton Vorontsov <avorontsov@ru.mvista.com>
 * Copyright (c) 2013-2022  Bernd Porr <mail@berndporr.me.uk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License.
 *
 * Cross-compile with cross-gcc -I/path/to/cross-kernel/include
 */

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "ads1115rpi.h"

// Handler which receives the data and prints it on the
// screen.
class ADS1115PrinterCallback : public ADS1115callback {
	virtual void hasSample(float v) {
		printf("v = %e\n",v);
	}
};

// Creates an instance of the AD7705 class.
// Registers the callback.
// Prints data till the user presses a key.
int main(int argc, char *argv[]) {
	ADS1115rpi ads1115rpi;
	ADS1115PrinterCallback ads1115PrinterCallback;
	ads1115rpi.setCallback(&ads1115PrinterCallback);
        ADS1115settings s;
	s.samplingRate = ADS1115settings::FS8HZ;
	ads1115rpi.start(s);
	getchar();
	ads1115rpi.stop();
	return 0;
}