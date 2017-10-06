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
 * File:   main.cpp
 * Author: Georg
 *
 * Created on 6 October 2017, 23.31
*/

#include "sk6812spi.h"

#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <stdarg.h>
#include <getopt.h>
#include <iostream>


#define VERSION_MAJOR 1
#define VERSION_MINOR 0
#define VERSION_MICRO 0


static char VERSIONSTR[] = "XX.YY.ZZ";

bool clear_on_exit = 0;
int width = 10;
int height = 10;
bool running = true;

Sk6812_Spi* ledmatrix = NULL;


static void ctrl_c_handler(int signum)
{
	(void)(signum);
    running = false;
}

static void setup_handlers(void)
{
	struct sigaction new_action; 
	new_action.sa_handler = ctrl_c_handler;

	sigemptyset(&new_action.sa_mask);
	new_action.sa_flags = 0;

    sigaction(SIGINT, &new_action, NULL);
    sigaction(SIGTERM, &new_action, NULL);
}


static void parseargs(int argc, char **argv)
{
	int index;
	int c;

	static struct option longopts[] =
	{
        {"help", no_argument, 0, 'h'},
        {"clear", no_argument, 0, 'c'},
		{"height", required_argument, 0, 'y'},
		{"width", required_argument, 0, 'x'},
		{"version", no_argument, 0, 'v'},
		{0, 0, 0, 0}
	};

	while (1)
	{

		index = 0;
		c = getopt_long(argc, argv, "chvx:y:", longopts, &index);

		if (c == -1)
			break;

		switch (c)
		{
		case 0:
			/* handle flag options (array's 3rd field non-0) */
			break;

		case 'h':
			std::cerr << argv[0] << " Stetel srl version " << VERSIONSTR << std::endl;
			std::cerr << "Usage: " << argv[0] << std::endl  << 
				"-h (--help)    - this information\n"
				"-x (--width)   - matrix width (default 10)\n"
				"-y (--height)  - matrix height (default 10)\n"
				"-c (--clear)   - clear matrix on exit.\n"
				"-v (--version) - version information\n" ;
			exit(-1);

		case 'c':
			clear_on_exit=1;
			break;

		case 'y':
			if (optarg) {
				height = atoi(optarg);
				if (height <= 0) {
					std::cerr << "invalid height " << height << std::endl;
					exit (-1);
				}
			}
			break;

		case 'x':
			if (optarg) {
				width = atoi(optarg);
				if (width <= 0) {
					std::cerr << "invalid width " << width << std::endl;
					exit (-1);
				}
			}
			break;

		case 'v':
			std::cerr << argv[0] << " version " << VERSIONSTR << std::endl;
			exit(-1);

		case '?':
			/* getopt_long already reported error? */
			exit(-1);

		default:
			exit(-1);
		}
	}
}

#define IDX(x,y)    (y*height + x)

int main(int argc, char *argv[])
{
    sprintf(VERSIONSTR, "%d.%d.%d", VERSION_MAJOR, VERSION_MINOR, VERSION_MICRO);

	parseargs(argc, argv);
	
	setup_handlers(); // we can interrupt via ctrl-c

	// let's get an instance of the controller class
	ledmatrix = new Sk6812_Spi(width*height);
	
	// this is to clear the pixels (all off)
	// NOTE: by adding false we do not call display() so it doesn't go out
	ledmatrix->clear(false);

	// the canvas can be used to draw directly on the matrix
	// note that the controller is not aware about the matrix
	// everything is linear --> use facility macros like IDX() above
    Sk6812_Leds::Led* canvas = ledmatrix->getCanvas();

	int nrofleds = width*height;
	
	char cmd;
	int loadingindex=0;
	bool parseerror = false;
	while (std::cin.get(cmd) && running && !parseerror)
	{
		switch(cmd){
			case 'C':  // Clear matrix
			case 'c':
				ledmatrix->clear(false);
				loadingindex=0;
				break;
			case 'F':  // fill matrix with a particular color
			case 'f':
			{
				unsigned long pxcolor;
				std::cin >> std::hex >> pxcolor;
				if(std::cin.fail()) {parseerror = true; break;}
				ledmatrix->print(pxcolor);
				loadingindex=0;
				break;
			}
			case 'D': // Display the matrix (load the leds)
			case 'd':
				ledmatrix->display();
				loadingindex=0;
				break;
			case 'W': // Wait for given milliseconds
			case 'w': 
			{
				unsigned long delay;
				std::cin >> std::dec >> delay;
				if(std::cin.fail()) {parseerror = true; break;}
				usleep(delay * 1000);
				break;
			}
			case '0':
				break;
			case 'x':
			{
				unsigned long pxcolor;
				std::cin >> std::hex >> pxcolor;
				if(std::cin.fail()) {parseerror = true; break;}
				if(loadingindex < nrofleds) { canvas[loadingindex++].set(pxcolor) ;}
				break;
			}
			case '#':
				while(std::cin.get(cmd)) { if(cmd == '\n') {break;} }
			break;
		}
	}

	if(parseerror) std::cerr << "Parsing error" << std::endl;

	if(clear_on_exit) {ledmatrix->clear();	}

	if(ledmatrix) { delete ledmatrix; } 

    std::cout << std::endl << "Thanks for using me (Cignogay)" << std::endl;
    return 0;
}