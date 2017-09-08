# Arduino-SPI

The Serial Peripheral Interface library is an abstract interface for SPI
firmware and software implementation, and device driver support.

Version: 1.1

## Classes

* [Abstract Serial Peripheral Interface, SPI](./src/SPI.h)
* [SPI Device Driver Interface, SPI::Device](./src/SPI.h)
* [Software Serial Peripheral Interface, Software::SPI](./src/Software/SPI.h)

## Example Sketches

* [SoftwareSPI](./examples/SoftwareSPI)
* [ShiftIn](./examples/ShiftIn)
* [ShiftOut](./examples/ShiftOut)

## Benchmarks

SPI::Device with Software::SPI bit-rate is approx. 570 kHz (LSB) and
730 kHz (MSB). Transfer measurment use SPI mode(0), and include slave
select and buffer access.

Transfer | bytes | bitorder | us
---------|----------|-------|---
uint8_t | 1 | MSB | 15
uint8_t | 1 | LSB | 17
uint32_t | 4 | MSB | 59
uint32_t | 4 | LSB | 69
