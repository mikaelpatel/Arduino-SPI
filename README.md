# Arduino-SPI

The Serial Peripheral Interface library is an abstract interface for
bus managers and device driver support. the library contains bus
manager implementations for hardware resources (AVR SPI and USI) and
software using Ardino-GPIO.

Version: 1.8

## Classes

* [Abstract SPI Bus Manager and Device Driver Interface, SPI](./src/SPI.h)
* [Hardware SPI Bus Manager, Hardware::SPI](./src/Hardware/SPI.h)
* [Software SPI Bus Manager, Software::SPI](./src/Software/SPI.h)

## Example Sketches

* [Benchmark](./examples/Benchmark)
* [ShiftIn](./examples/ShiftIn)
* [ShiftOut](./examples/ShiftOut)

## Benchmarks

Benchmarks include bus acquire-release (multitasking support), and
slave select pin handling.

#### Software SPI Bus Manager

Operation (LSB @ 570 kHz) | Bytes | us
----------|-------|----
transfer | 1 | 16.62
read | 4 | 70.19
write | 4  | 68.75
transfer | 4  | 70.75

Operation (MSB @ 760 kHz) | Bytes | us
----------|-------|----
transfer | 1 | 13.81
read | 4 | 55.94
write | 4  | 56.31
transfer | 4  | 58.5

Wiring | us | SPI | us | Xn
------ |----|------|----|----
shiftIn | 84.19 | var = spi.transfer(0) | 15.75 | 5.35
shiftOut | 108.9 | spi.transfer(var) | 15.00 | 7.26

#### Hardware SPI Bus Manager

Operation (LSB/MSB @ 8 MHz) | Bytes | us
----------|-------|----
transfer | 1 | 3.44
read | 4 | 8.50
write | 4  | 8.44
transfer | 4  | 8.96

## Dependencies

* [Arduino-GPIO](https://github.com/mikaelpatel/Arduino-GPIO)
