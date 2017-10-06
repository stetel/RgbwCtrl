/*
 * RgbwCtrl -- A  CLI frontend command to drive sk6812 leds via the SPI
 *
 * Copyright (C) 2017 Stetel srl
 *
 * Georg Campana <g.campana(AT)stetel.com>
 *
 * This program is free software, distributed under the terms of
 * the GNU General Public License Version 2. See the LICENSE.txt file
 * at the top of the source tree.
 *
 * File:   sk6812_leds.h
 * Author: Georg
 *
 * Created on 6 October 2017, 23.31
*/


#ifndef __SK6812_LEDS_H__
#define __SK6812_LEDS_H__


typedef unsigned char color8;
typedef unsigned long rgbw32;


class Sk6812_Leds {

public:

	struct Led {
		rgbw32 pxvalue;
		
		void set(rgbw32 rgbw);
        void set(color8 r, color8 g, color8 b, color8 w);
		void setR(color8 r);
		void setG(color8 g);
		void setB(color8 b);
		void setW(color8 w);				
	} ;
    
    // clear the leds and decide if it should be displayed immediately
    void clear(bool displaynow=true);
    
    // fill with one specific color
	void print(rgbw32 value);
    
    // returns the leds to the client for painting
    Led* getCanvas();
    
    // returns the number of leds
    int getLength();

    // render current leds
    // this is hw interface specific
    virtual void display();

	Sk6812_Leds(int nrleds);
	~Sk6812_Leds();
protected:
    int length;
    Led* pixels;
};

__inline void Sk6812_Leds::Led::set(rgbw32 px) { pxvalue = px; } 
__inline void Sk6812_Leds::Led::set(color8 r, color8 g, color8 b, color8 w) { pxvalue = ((rgbw32)r << 24) | ((rgbw32)g << 16) | ((rgbw32)b << 8) | ((rgbw32)w) ;}
__inline void Sk6812_Leds::Led::setR(color8 r) { pxvalue = (pxvalue & 0x00ffffff) | ((rgbw32)r << 24);}
__inline void Sk6812_Leds::Led::setG(color8 g) { pxvalue = (pxvalue & 0xff00ffff) | ((rgbw32)g << 16);}
__inline void Sk6812_Leds::Led::setB(color8 b) { pxvalue = (pxvalue & 0xffff00ff) | ((rgbw32)b <<  8);}
__inline void Sk6812_Leds::Led::setW(color8 w) { pxvalue = (pxvalue & 0xffffff00) | ((rgbw32)w );}

__inline Sk6812_Leds::Led* Sk6812_Leds::getCanvas() { return pixels;}
__inline int Sk6812_Leds::getLength() { return length;}

#endif /* __SK6812_LEDS_H__ */