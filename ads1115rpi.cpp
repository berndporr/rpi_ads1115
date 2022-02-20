#include "ads1115rpi.h"

ADS1115rpi::ADS1115rpi() {
}

void ADS1115rpi::setCallback(ADS1115callback* cb) {
	ads1115callback = cb;
}

void ADS1115rpi::start(ADS1115settings settings) {
	ads1115settings = settings;
#ifdef DEBUG
	fprintf(stderr,"Sending reset.\n");
#endif

#ifdef DEBUG
	fprintf(stderr,"Receiving data.\n");
#endif
	
}


void ADS1115rpi::stop() {
}
