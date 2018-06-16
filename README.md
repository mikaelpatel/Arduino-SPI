# Arduino-SPI

The Serial Peripheral Interface library is an abstract interface for
bus managers and device driver support. the library contains bus
manager implementations for hardware resources (AVR SPI and USI) and
software using Ardino-GPIO.

The design of this library allows multiple bus managers both hardware
and software, and device drivers directly support bus managers.

Version: 1.8

## Classes

* [Abstract SPI Bus Manager and Device Driver Interface, SPI](./src/SPI.h)
* [Hardware SPI Bus Manager, Hardware::SPI](./src/Hardware/SPI.h)
* [Software SPI Bus Manager, Software::SPI](./src/Software/SPI.h)

## Example Sketches

* [Benchmark](./examples/Benchmark)
* [ShiftIn](./examples/ShiftIn)
* [ShiftOut](./examples/ShiftOut)

[ATtiny](./examples/ATtiny) variants.

## Benchmarks

Benchmarks measurements below do not include bus acquire-release
(multitasking support), and slave select pin handling. The
measurements are the transfer time in micro-seconds.

#### Software SPI Bus Manager

Operation (LSB) | Bytes | us
----------|-------|----
transfer | 1 | 13.6
transfer | 10 | 147.9
transfer | 100 | 1491.8
read | 100 | 1511.7
write | 100 | 1459.6

Operation (MSB) | Bytes | us
----------|-------|----
transfer | 1 | 12.8
transfer | 10 | 139.0
transfer | 100 | 1345.4
read | 100 | 1304.2
write | 100 | 1313.2

Wiring (MSB) | us | SPI | us | Xn
------ |----|------|----|----
shiftIn | 86.340 | spi >> var | 12.892 | 6.7
shiftOut | 109.288 | spi << val | 13.144 | 8.3

Wiring (MSB) | us | SPR | us | Xn
------ |----|------|----|----
shiftIn | 86.340 | srpi >> var | 4.844 | 17.8
shiftOut | 109.288 | srpo << val | 8.676 | 12.6

#### Hardware SPI Bus Manager

Operation (LSB/MSB @ 8 MHz) | Bytes | us
----------|-------|----
transfer | 1 | 1.8
transfer | 10 | 15.3
transfer | 100 | 139.8
read | 100 | 133.1
write | 100  | 145.6

Wiring (MSB) | us | SPI | us | Xn
------ |----|------|----|----
shiftIn | 86.340 | spi >> var | 2.832 | 30
shiftOut | 109.288 | spi << val | 2.832 | 38

## Dependencies

* [Arduino-GPIO](https://github.com/mikaelpatel/Arduino-GPIO)
