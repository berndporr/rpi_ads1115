#include "ads1115rpi.h"

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <format>


void ADS1115rpi::start(ADS1115settings settings) {
	ads1115settings = settings;

	char gpioFilename[20];
	snprintf(gpioFilename, 19, "/dev/i2c-%d", settings.i2c_bus);
	fd_i2c = open(gpioFilename, O_RDWR);
	if (fd_i2c < 0) {
	    char i2copen[] = "Could not open I2C.\n";
#ifdef DEBUG
	    fprintf(stderr,i2open);
#endif
	    throw std::invalid_argument(i2copen);
	}
	
	if (ioctl(fd_i2c, I2C_SLAVE, settings.address) < 0) {
	    char i2cslave[] = "Could not access I2C adress.\n";
#ifdef DEBUG
	    fprintf(stderr,i2cslave);
#endif
	    throw std::invalid_argument(i2cslave);
	}
	
#ifdef DEBUG
	fprintf(stderr,"Init.\n");
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

	const std::string chipPath = std::format("/dev/gpiochip{}", settings.drdy_chip);
	const std::string consumername = std::format("gpioconsumer_{}_{}", settings.drdy_chip, settings.drdy_gpio);

	// Config the pin as input and detecting falling and rising edegs
	gpiod::line_config line_cfg;
	line_cfg.add_line_settings(
				   settings.drdy_gpio,
				   gpiod::line_settings()
				   .set_direction(gpiod::line::direction::INPUT)
				   .set_edge_detection(gpiod::line::edge::RISING));
    
	chip = std::make_shared<gpiod::chip>(chipPath);
	    
	auto builder = chip->prepare_request();
	builder.set_consumer(consumername);
	builder.set_line_config(line_cfg);
	request = std::make_shared<gpiod::line_request>(builder.do_request());
	
	running = true;

	thr = std::thread(&ADS1115rpi::worker,this);
}


void ADS1115rpi::setChannel(ADS1115settings::Input channel) {
	unsigned r = i2c_readWord(reg_config);
	r = r & (~(3 << 12));
	r = r | (channel << 12);
	i2c_writeWord(reg_config,r);
	ads1115settings.channel = channel;	
}


void ADS1115rpi::dataReady() {
	float v = (float)i2c_readConversion() / (float)0x7fff * fullScaleVoltage();
	if (adsCallbackInterface) {
	    adsCallbackInterface(v);
	}
}


void ADS1115rpi::worker() {
    while (running) {
	bool r = request->wait_edge_events(std::chrono::milliseconds(ISR_TIMEOUT_MS));
	if (r)
	    {
		gpiod::edge_event_buffer buffer;
		request->read_edge_events(buffer, 1);
		// callback
		dataReady();
	    }
    }
}


void ADS1115rpi::stop() {
    if (!running) return;
    running = false;
    thr.join();
    request->release();
    chip->close();
    close(fd_i2c);
}


// i2c read and write protocols
void ADS1115rpi::i2c_writeWord(uint8_t reg, unsigned data)
{
	uint8_t tmp[3];
	tmp[0] = reg;
	tmp[1] = (char)((data & 0xff00) >> 8);
	tmp[2] = (char)(data & 0x00ff);
	long int r = write(fd_i2c,&tmp,3);
        if (r < 0) {
#ifdef DEBUG
                fprintf(stderr,"Could not write word from %02x. ret=%d.\n",ads1115settings.address,r);
#endif
                throw std::invalid_argument("Could not write to i2c.");
        }
}

unsigned ADS1115rpi::i2c_readWord(uint8_t reg)
{
	uint8_t tmp[2];
	tmp[0] = reg;
	write(fd_i2c,&tmp,1);
        long int r = read(fd_i2c, tmp, 2);
        if (r < 0) {
#ifdef DEBUG
                fprintf(stderr,"Could not read word from %02x. ret=%d.\n",ads1115settings.address,r);
#endif
                throw std::invalid_argument("Could not read from i2c.");
        }
        return (((unsigned)(tmp[0])) << 8) | ((unsigned)(tmp[1]));
}

int ADS1115rpi::i2c_readConversion()
{
	const int reg = 0;
	char tmp[3];
	tmp[0] = reg;
	write(fd_i2c,&tmp,1);
        long int r = read(fd_i2c, tmp, 2);
        if (r < 0) {
#ifdef DEBUG
                fprintf(stderr,"Could not read ADC value. ret=%d.\n",r);
#endif
                throw std::invalid_argument("Could not read from i2c.");
        }
        return ((int)(tmp[0]) << 8) | (int)(tmp[1]);
}
