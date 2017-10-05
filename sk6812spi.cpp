// using standard exceptions
#include <iostream>
#include <exception>

#include "sk6812spi.h"

// if you change these then you need to ammend the bit stuffing functions
// which are now optimized to work with 4 bits (nibble) per symbol and 8 symbols per color
#define BITS_PER_SYMBOL     4
#define SYMBOLS_PER_COLOR   8

enum SymbolBits {
    B_HIGH = 0xC,  // 1 1 0 0
    B_LOW  = 0x8   // 1 0 0 0
};

#define SK6812_FREQ     800000
#define SPI_FREQ    (SK6812_FREQ * BITS_PER_SYMBOL)

#define SK6812_RESET_USECS   80
#define SPI_RESET_SYMBOLS  ((SK6812_RESET_USECS * SK6812_FREQ)/1000000)
#define SPI_RESET_BITS   (SPI_RESET_SYMBOLS * BITS_PER_SYMBOL)
#define COLORS_PER_LED      4

class InitException : public std::exception  {

  virtual const char* what() const throw()
  {
    return "Can't init (no mem or spi port)";
  }

} initex;



Sk6812_Leds::Sk6812_Leds(int nrleds) {

    if(initSpi() == false) {
        throw initex;
    }

    pixels = new Led[nrleds];
    length = nrleds;

    int spi_data_bits = length * COLORS_PER_LED * SYMBOLS_PER_COLOR * BITS_PER_SYMBOL ;
    spidatalen =  (spi_data_bits + SPI_RESET_BITS + 7)/8 ;
    // we need to allocate space for the bits of the symbols plus the reset time (load to registers).
    // Then we round up to the next byte
    spidata = new unsigned char[spidatalen];
}

Sk6812_Leds::~Sk6812_Leds() {
    delete[] spidata;
    delete[] pixels;
    closeSpi();
}


void Sk6812_Leds::display() {
    transform2raw();
    transfer2Spi();
}

void Sk6812_Leds::clear(bool displaynow) {
    print(0);
    if(displaynow) display(); 
}

void Sk6812_Leds::print(rgbw32 value) {
    for(int c = 0; c < length; c++) {
        pixels[c].set(0x00000000);
    }
}

void Sk6812_Leds::transform2raw() {
    unsigned char *data2send = spidata;
    Led* currentled = pixels;
    bool halfbyte = false;

    // for each led
    for(int c=0; c < length; c++) {
        rgbw32 pxvalue = currentled->pxvalue;
        currentled++;
        // for each symbol in each color        
        for(int valbit=0; valbit < 32; valbit++) {
            SymbolBits bits2push =  B_LOW;
            // we read the highest bit and shift then
            if(pxvalue & 0x80000000) {bits2push =  B_HIGH; }
            pxvalue <<= 1;

            *data2send <<= BITS_PER_SYMBOL;
            *data2send |= bits2push;

            if(halfbyte) { data2send++ ;}
            halfbyte = !halfbyte;
        }
    }

    // reset (load registers) bits set to low    
    for(int valbit=0; valbit < SPI_RESET_SYMBOLS; valbit++) {
    
        *data2send <<= BITS_PER_SYMBOL;
        *data2send |= B_LOW;

        if(halfbyte) { data2send++ ;}
        halfbyte = !halfbyte;
    }
}


// SPI stuff
// TODO: encapsulate the SPI 
// NOTE: includes are intentionally here and not on top
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/spi/spidev.h>
#include <string.h>
#include <unistd.h>

bool Sk6812_Leds::transfer2Spi() {
    int ret;
    struct spi_ioc_transfer tr;

    ::memset(&tr, 0, sizeof(struct spi_ioc_transfer));
    tr.tx_buf = (unsigned long)spidata;
    tr.rx_buf = 0;
    tr.len = spidatalen;

    ret = ioctl(spi_fd, SPI_IOC_MESSAGE(1), &tr);
    if (ret < 1)
    {
        fprintf(stderr, "Can't send spi message");
        return false;
    }
    
    return true;
}

bool Sk6812_Leds::initSpi() {

    uint8_t mode = SPI_MODE_0;
    uint8_t bits = 8;
    uint32_t speed = SK6812_FREQ * BITS_PER_SYMBOL;


    spi_fd = ::open("/dev/spidev0.0", O_RDWR);
    if (spi_fd < 0) {
        fprintf(stderr, "Cannot open /dev/spidev0.0. spi kernel module not loaded?\n");
        return false;
    }

    // SPI mode
    if (::ioctl(spi_fd, SPI_IOC_WR_MODE, &mode) < 0)
    {
        fprintf(stderr, "Cannot set spi wr mode\n");
        return false;
    }
    if (::ioctl(spi_fd, SPI_IOC_RD_MODE, &mode) < 0)
    {
        fprintf(stderr, "Cannot set spi rd mode\n");
        return false;
    }

    // Bits per word
    if (::ioctl(spi_fd, SPI_IOC_WR_BITS_PER_WORD, &bits) < 0)
    {
        fprintf(stderr, "Cannot set spi wr bits per word\n");
        return false;
    }
    if (::ioctl(spi_fd, SPI_IOC_RD_BITS_PER_WORD, &bits) < 0)
    {
        fprintf(stderr, "Cannot set spi rd bits per word\n");
        return false;
    }

    // Max speed Hz
    if (::ioctl(spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) < 0)
    {
        fprintf(stderr, "Cannot set spi wr speed freq\n");
        return false;
    }
    if (::ioctl(spi_fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed) < 0)
    {
        fprintf(stderr, "Cannot set spi rd speed freq\n");
        return false;
    }

    return true;
}

void Sk6812_Leds::closeSpi() {
    if (spi_fd > 0) {
        ::close(spi_fd);
        spi_fd = -1;
    }
}