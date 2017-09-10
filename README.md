# Arduino-SPI

The Serial Peripheral Interface library is an abstract interface for SPI
hardware and software implementation, and device driver support.

The library contains bus manager implementations for hardware
resources (SPI and USI) and software using Ardino-GPIO.

Version: 1.4

## Classes

* [Abstract SPI Bus Manager Interface, SPI](./src/SPI.h)
* [Abstract SPI Device Driver Interface, SPI::Device](./src/SPI.h)
* [Hardware SPI Bus Manager Interface, Hardware::SPI](./src/Hardware/SPI.h)
* [Software SPI Bus Manager Interface, Software::SPI](./src/Software/SPI.h)

## Example Sketches

* [Benchmark](./examples/Benchmark)
* [ShiftIn](./examples/ShiftIn)
* [ShiftOut](./examples/ShiftOut)

## Benchmarks

#### Software SPI Bus Manager

Operation (LSB @ 570 kHz) | Bytes | us
----------|-------|----
transfer | 1 | 16
read | 4 | 68
write | 4  | 67
transfer | 4  | 67

Operation (MSB @ 720 kHz) | Bytes | us
----------|-------|----
transfer | 1 | 14
read | 4 | 55
write | 4  | 56
transfer | 4  | 57

#### Hardware SPI Bus Manager

Operation (LSB/MSB @ 8 MHz) | Bytes | us
----------|-------|----
transfer | 1 | 3.44
read | 4 | 13.37
write | 4  | 13.56
transfer | 4  | 14.06
