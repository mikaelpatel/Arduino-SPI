# Arduino-SPI

The Serial Peripheral Interface library is an abstract interface for SPI
hardware and software implementation, and device driver support.

The library contains bus manager implementations for hardware
resources (SPI and USI) and software using Ardino-GPIO.

Version: 1.3

## Classes

* [Abstract Serial Peripheral Interface, SPI](./src/SPI.h)
* [SPI Device Driver Interface, SPI::Device](./src/SPI.h)
* [Hardware Serial Peripheral Interface, Hardware::SPI](./src/Hardware/SPI.h)
* [Software Serial Peripheral Interface, Software::SPI](./src/Software/SPI.h)

## Example Sketches

* [HardwareSPI](./examples/HardwareSPI)
* [SoftwareSPI](./examples/SoftwareSPI)
* [ShiftIn](./examples/ShiftIn)
* [ShiftOut](./examples/ShiftOut)

## Benchmarks

Software SPI bus manager bit-rate is approx. 570 kHz (LSB) and
730 kHzf (MSB). Transfer measurments use SPI mode(0), and include slave
select and buffer access.

Transfer | bytes | bitorder | us
---------|----------|-------|---
uint8_t | 1 | MSB | 15
uint8_t | 1 | LSB | 17
uint32_t | 4 | MSB | 59
uint32_t | 4 | LSB | 69
