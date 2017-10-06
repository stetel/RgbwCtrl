# Rgbctrl

This is a cli frontend and spi driver for strips or arrays using the sk6812 leds via a SPI interface.
It has been tested on the Raspberry 3 but would work most likely also work on the RPI 1, RPI 2 and probably also on other dev boards like the Orange PI
These leds are in the 5050 format and have not only rgb color components but also the white part.
On chinese sites you can find types with cold white, warm white and natural white.
You can find a datasheet here: [ The Sk6812 datashee](https://cdn-shop.adafruit.com/product-files/1138/SK6812+LED+datasheet+.pdf)

## Getting Started
Simply clone the repository by using git clone on your system.

### Prerequisites

It is important to have everythinds you need to compile c++ files  (apt-get install is your friend).
Since the SPI interface is used it is important that the spi headers are present.

```
IMPORTANT: On the raspberry pi the SPI interface frequency shifts when the cpu throttles to a different frequency. 
This is not acceptable since the choosen frequency must be exact to properly drive. To prevent this behavior add "core_freq=250"  to the file /boot/config.txt.
It is also recommended to enlarge the spi buffer of the kernel adding  spidev.bufsiz=32768 to /boot/cmdline.txt
```

### building

Once you have the source code cloned and all dependencies cleared you can simply type

```
make
```
to build the project. The rgbwctrl command is located in the build directory.

To clean everything you can use the clean target
```
make clean
```

## Connecting the leds

Only one line of the SPI interface is used: MOSI which is pin number 19 on the RPI 3
The leds must be powered by an external power supply, the RPI is not able to provide enough power unless you only play with 2/3 leds.
Seems that a level shifter is not needed and that the 3.3V level is perfectly fine to drive the data input of the first led in the chain.

## Usage

The rgbwctrl command accepts simply commands from the stdin. This allows to directly tests the leds manually, to pipe the output of another app diretly to the rgbwctrl or to use the bash redirection in order to load commands from a file. When EOF is reached the command exits.
Example to drive a strip of 10 leds:
```
./rgbwctrl -x 10 -y 1 -c
```
now you can give commands in order to dirve the leds directly from the cli.
See "examples.txt" for some examples and the syntax below.

## Syntax

* **C** = Clear, clears the led matrix (you must use d to actually send the cleared values to the leds)
* **D** = Display, displays/renders on the leds what is in the matrix
* **F \<hex\>** = Fill, fills the whole matrix with the given color value in the hex format 0xRRGGBBWW
* **W \<millis\>** = Wait, wait for given milliseconds (must be in decimal format)
* **\<hex\>,\<hex\>,...** Values to load into each led starting from the first. C,D and F do reset the counter to 0

## Contributing

Fill free to submit valuable pull requests.
The code is written in C++ and the main class can be extended (like the spi version is extending it) to support other king of hw interfaces.

## Authors

* **Georg Campana** - *Stetel srl* - [www.stetel.com](https://www.stetel.com)


## License

This project is dual licensed under the GPL2 License or under a commercial license - see the [LICENSE](LICENSE) file for details

## Acknowledgments

* Jeremy Garff's rpi_ws281x inspired me to build this cli frontend specifically for the sk6812 leds drived by the spi of the RPI 3
* The project is a side-product of a much larger internal project

