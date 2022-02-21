#include "ads1115rpi.h"

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

	unsigned r = (0b10000000 << 8); // kick it all off
	r = r | (1 << 2) | (1 << 3); // data ready active high & latching
	r = r | (settings.samplingRate << 5);
	r = r | (settings.pgaGain << 9);
	r = r | (settings.channel << 12) | 1 << 14; // unipolar
	i2c_writeWord(reg_config,r);

#ifdef DEBUG
	fprintf(stderr,"Receiving data.\n");
#endif
	
        gpioSetMode(settings.drdy_gpio,PI_INPUT);
        gpioSetISRFuncEx(settings.drdy_gpio,RISING_EDGE,ISR_TIMEOUT,gpioISR,(void*)this);

}


void ADS1115rpi::setChannel(ADS1115settings::Input channel) {
	unsigned r = i2c_readWord(reg_config);
	r = r & (~(3 << 12));
	r = r | (channel << 12);
	i2c_writeWord(reg_config,r);
	ads1115settings.channel = channel;	
}


void ADS1115rpi::dataReady() {
	float v = (float)i2c_readConversion() / (float)0x7fff * fullScaleVoltage() * 2;
	hasSample(v);
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
                fprintf(stderr,"Could not open %02x.\n",ads1115settings.address);
#endif
                throw could_not_open_i2c;
        }
	char tmp[2];
	tmp[0] = (char)((data & 0xff00) >> 8);
	tmp[1] = (char)(data & 0x00ff);
	int r = i2cWriteI2CBlockData(fd, reg, tmp, 2);
        if (r < 0) {
#ifdef DEBUG
                fprintf(stderr,"Could not write word from %02x. ret=%d.\n",ads1115settings.address,r);
#endif
                throw "Could not read from i2c.";
        }
        i2cClose(fd);
}

unsigned ADS1115rpi::i2c_readWord(uint8_t reg)
{
        int fd = i2cOpen(ads1115settings.i2c_bus, ads1115settings.address, 0);
        if (fd < 0) {
#ifdef DEBUG
                fprintf(stderr,"Could not open %02x.\n",ads1115settings.address);
#endif
                throw could_not_open_i2c;
        }
        int r;
	char tmp[2];
        r = i2cReadI2CBlockData(fd, reg, tmp, 2);
        if (r < 0) {
#ifdef DEBUG
                fprintf(stderr,"Could not read word from %02x. ret=%d.\n",ads1115settings.address,r);
#endif
                throw "Could not read from i2c.";
        }
        i2cClose(fd);
        return (((unsigned)(tmp[0])) << 8) | ((unsigned)(tmp[1]));
}

int ADS1115rpi::i2c_readConversion()
{
	const int reg = 0;
        int fd = i2cOpen(ads1115settings.i2c_bus, ads1115settings.address, 0);
        if (fd < 0) {
#ifdef DEBUG
                fprintf(stderr,"Could not open %02x.\n",ads1115settings.address);
#endif
                throw could_not_open_i2c;
        }
        int r;
	char tmp[2];
        r = i2cReadI2CBlockData(fd, reg, tmp, 2);
        if (r < 0) {
#ifdef DEBUG
                fprintf(stderr,"Could not read ADC value. ret=%d.\n",r);
#endif
                throw "Could not read from i2c.";
        }
        i2cClose(fd);
	//        return (((int)(tmp[0])) << 8) | ((int)(tmp[1]));
        return ((int)(tmp[0]) << 8) | (int)(tmp[1]);
}
