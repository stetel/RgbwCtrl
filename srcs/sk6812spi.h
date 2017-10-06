#ifndef __SK6812_SPI_H__
#define __SK6812_SPI_H__

#include "sk6812_leds.h"

class Sk6812_Spi : public Sk6812_Leds {

public:
  
    // render current leds over the spi interface
    // overriding the super class display
    void display();


	Sk6812_Spi(int nrleds);
	~Sk6812_Spi();
private:
  
    // raw array for the spidata
    unsigned char *spidata; 
    int spidatalen;

    int spi_fd;
    
    void transform2raw();

    bool transfer2Spi();
    bool initSpi();
    void closeSpi();

};

#endif /* __SK6812_SPI_H__ */