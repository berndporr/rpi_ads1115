#include "ads1115rpi.h"

ADS1115rpi::ADS1115rpi() {
}

void ADS1115rpi::setCallback(ADS1115callback* cb) {
	ads1115callback = cb;
}

void ADS1115rpi::start(ADS1115settings settings) {
	ads1115settings = settings;

	if (settings.initPIGPIO) {
                int cfg = gpioCfgGetInternals();
                cfg |= PI_CFG_NOSIGHANDLER;
                gpioCfgSetInternals(cfg);
                int r = gpioInitialise();
                if (r < 0) {
                        char msg[] = "Cannot init pigpio.";
#ifdef DEBUG
                        fprintf(stderr,"%s\n",msg);
#endif
                        throw msg;
                }
        }

#ifdef DEBUG
	fprintf(stderr,"Init .\n");
#endif
	// Enable RDY
	i2c_writeWord(reg_lo_thres, 0x0000);
	i2c_writeWord(reg_hi_thres, 0x8000);

	unsigned r = (0b10000000 << 8);
	r = r | (settings.samplingRate << 5) | (1 << 2) | (1 << 3);
	i2c_writeWord(reg_config,r);

#ifdef DEBUG
	fprintf(stderr,"Receiving data.\n");
#endif
	
        gpioSetMode(settings.drdy_gpio,PI_INPUT);
        gpioSetISRFuncEx(settings.drdy_gpio,RISING_EDGE,ISR_TIMEOUT,gpioISR,(void*)this);

}

void ADS1115rpi::dataReady() {
	float v = i2c_readWord(0);
	if (ads1115callback) {
		ads1115callback->hasSample(v);
	}
}


void ADS1115rpi::stop() {
	gpioSetISRFuncEx(ads1115settings.drdy_gpio,RISING_EDGE,-1,NULL,(void*)this);
        if (ads1115settings.initPIGPIO) {
                gpioTerminate();
        }
}


// i2c read and write protocols
void ADS1115rpi::i2c_writeWord(uint8_t reg, unsigned data)
{
        int fd = i2cOpen(ads1115settings.i2c_bus, ads1115settings.address, 0);
        if (fd < 0) {
#ifdef DEBUG
                fprintf(stderr,"Could not write %02x to %02x,%02x,%02x\n",data,device.i2c_bus,address,subAddress);
#endif
                throw could_not_open_i2c;
        }
        i2cWriteWordData(fd, reg, data);
        i2cClose(fd);
}

unsigned ADS1115rpi::i2c_readWord(uint8_t reg)
{
        int fd = i2cOpen(ads1115settings.i2c_bus, ads1115settings.address, 0);
        if (fd < 0) {
#ifdef DEBUG
                fprintf(stderr,"Could not read byte from %02x,%02x,%02x\n",device.i2c_bus,address,subAddress);
#endif
                throw could_not_open_i2c;
        }
        int data; // `data` will store the register data
        data = i2cReadWordData(fd, reg);
        if (data < 0) {
#ifdef DEBUG
                fprintf(stderr,"Could not read byte from %02x,%02x,%02x. ret=%d.\n",device.i2c_bus,address,subAddress,data);
#endif
                throw "Could not read from i2c.";
        }
        i2cClose(fd);
        return (unsigned)data;           
}
