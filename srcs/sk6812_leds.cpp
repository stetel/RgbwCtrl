#include "sk6812_leds.h"
#include <iostream>
#include <iomanip>

Sk6812_Leds::Sk6812_Leds(int nrleds) {

    pixels = new Led[nrleds];
    length = nrleds;
}

Sk6812_Leds::~Sk6812_Leds() {
    delete[] pixels;
}

void Sk6812_Leds::clear(bool displaynow) {
    print(0);
    if(displaynow) display(); 
}

void Sk6812_Leds::print(rgbw32 value) {
    for(int c=0; c < length; c++) {
        pixels[c].set(value);
    }
}

// default implementation
// must be overridden by the subclass
void Sk6812_Leds::display() {

    std::cout << "Values:" << std::endl;
    for(int c=0; c < length; c++) {
        std::cout << "0x" << std::setw(8) <<  std::setfill('0') << std::hex  <<  (unsigned long)pixels[c].pxvalue << ",";
    }
    std::cout << std::endl << std::setw(0) << std::setfill(' ');
}
