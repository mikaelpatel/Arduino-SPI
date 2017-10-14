#include "GPIO.h"
#include "SPI.h"
#include "benchmark.h"

// Configuration: SPI/BITORDER
// #define USE_SOFTWARE_SPI
// #define BITORDER LSBFIRST
#define BITORDER MSBFIRST

#if defined(USE_SOFTWARE_SPI)
#include "Software/SPI.h"
#if defined(ARDUINO_attiny)
#include "Software/Serial.h"
Software::Serial<BOARD::D0> Serial;
GPIO<BOARD::D1> ss;
Software::SPI<BOARD::D2, BOARD::D3, BOARD::D4> spi;
SPI::Device<0, BITORDER, SPI::MAX_FREQ, BOARD::D1> dev(spi);
#else
GPIO<BOARD::D10> ss;
Software::SPI<BOARD::D11, BOARD::D12, BOARD::D13> spi;
SPI::Device<0, BITORDER, SPI::MAX_FREQ, BOARD::D10> dev(spi);
#endif

#else
#include "Hardware/SPI.h"
GPIO<BOARD::D10> ss;
Hardware::SPI spi;
SPI::Device<0, BITORDER, SPI::MAX_FREQ, BOARD::D10> dev(spi);
#endif

void setup()
{
  Serial.begin(57600);
  while (!Serial);
  BENCHMARK_BASELINE(1000);
}

void loop()
{
  // Measure SPI bus manager performance with and without slave select
  // and device setting

  const uint8_t PRESCALE = spi.prescale(SPI::MAX_FREQ);
  uint8_t src[100];
  uint8_t dest[100];

  ss.toggle();
  spi.acquire(0, BITORDER, PRESCALE);
  BENCHMARK("1a. SPI bus manager data transfer only (1 byte)", 1000) {
    spi.transfer(src[0]);
  }

  BENCHMARK("1b. - transfer (10 byte)", 1000) {
    spi.transfer(dest, src, 10);
  }

  BENCHMARK("1c. - transfer (100 byte)", 1000) {
    spi.transfer(dest, src, 100);
  }

  BENCHMARK("1d. - read (100 byte)", 1000) {
    spi.read(dest, 100);
  }

  BENCHMARK("1e. - write only (100 byte)", 1000) {
    spi.write(src, 100);
  }
  spi.release();
  ss.toggle();

  BENCHMARK("2a. SPI bus manager data transfer (1 byte)", 1000) {
    ss.toggle();
    spi.acquire(0, BITORDER, PRESCALE);
    spi.transfer(src[0]);
    spi.release();
    ss.toggle();
  }

  BENCHMARK("2b. - transfer (10 byte)", 1000) {
    ss.toggle();
    spi.acquire(0, BITORDER, PRESCALE);
    spi.transfer(dest, src, 10);
    spi.release();
    ss.toggle();
  }

  BENCHMARK("2c. - transfer (100 byte)", 1000) {
    ss.toggle();
    spi.acquire(0, BITORDER, PRESCALE);
    spi.transfer(dest, src, 100);
    spi.release();
    ss.toggle();
  }

  BENCHMARK("2d. - read (100 byte)", 1000) {
    ss.toggle();
    spi.acquire(0, BITORDER, PRESCALE);
    spi.read(dest, 100);
    spi.release();
    ss.toggle();
  }

  BENCHMARK("2e. - write (100 byte)", 1000) {
    ss.toggle();
    spi.acquire(0, BITORDER, PRESCALE);
    spi.write(dest, 100);
    spi.release();
    ss.toggle();
  }

  dev.acquire();
  BENCHMARK("3a. SPI device driver data transfer only (1 byte)", 1000) {
    dev.transfer(src[0]);
  }

  BENCHMARK("3b. - transfer (10 bytes)", 1000) {
    dev.transfer(dest, src, 10);
  }

  BENCHMARK("3c. - transfer (100 bytes)", 1000) {
    dev.transfer(dest, src, 100);
  }

  BENCHMARK("3d. - read (100 bytes)", 1000) {
    dev.read(dest, 100);
  }

  BENCHMARK("3e. - write (100 bytes)", 1000) {
    dev.write(src, 100);
  }
  dev.release();

  BENCHMARK("4a. SPI device driver data transfer (1 byte)", 1000) {
    dev.acquire();
    dev.transfer(src[0]);
    dev.release();
  }

  BENCHMARK("4b. - transfer (10 bytes)", 1000) {
    dev.acquire();
    dev.transfer(dest, src, 10);
    dev.release();
  }

  BENCHMARK("4c. - transfer (100 bytes)", 1000) {
    dev.acquire();
    dev.transfer(dest, src, 100);
    dev.release();
  }

  BENCHMARK("4d. - read (100 bytes)", 1000) {
    dev.acquire();
    dev.read(dest, 100);
    dev.release();
  }

  BENCHMARK("4e. - write (100 bytes)", 1000) {
    dev.acquire();
    dev.write(src, 100);
    dev.release();
  }

  Serial.println();
  delay(2000);
}
