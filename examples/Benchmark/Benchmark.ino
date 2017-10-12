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
  static uint32_t value = 0;
  uint32_t res;

  BENCHMARK("1a. SPI bus manager serial data transfer (1 byte)", 1000) {
    ss.toggle();
    spi.acquire(0, BITORDER, PRESCALE);
    spi.transfer(value);
    spi.release();
    ss.toggle();
  }

  ss.toggle();
  spi.acquire(0, BITORDER, PRESCALE);
  BENCHMARK("1b. - transfer only (1 byte)", 1000) {
    spi.transfer(value);
  }
  spi.release();
  ss.toggle();

  BENCHMARK("2a. SPI device driver serial data transfer (1 byte)", 1000) {
    dev.acquire();
    dev.transfer(value);
    dev.release();
  }

  dev.acquire();
  BENCHMARK("2b. - transfer only (1 byte)", 1000) {
    dev.transfer(value);
  }
  dev.release();

  BENCHMARK("3a. SPI device driver serial buffer read (4 bytes)", 1000) {
    dev.acquire();
    dev.read(&res, sizeof(value));
    dev.release();
  }

  dev.acquire();
  BENCHMARK("3b. - read only (4 bytes)", 1000) {
    dev.read(&res, sizeof(value));
  }
  dev.release();

  BENCHMARK("4a. SPI device driver serial buffer write (4 bytes)", 1000) {
    dev.acquire();
    dev.write(&value, sizeof(value));
    dev.release();
  }

  dev.acquire();
  BENCHMARK("4b. - write only (4 bytes)", 1000) {
    dev.write(&value, sizeof(value));
  }
  dev.release();

  BENCHMARK("5a. SPI device driver serial buffer transfer (4 bytes)", 1000) {
    dev.acquire();
    dev.transfer(&res, &value, sizeof(value));
    dev.release();
  }

  dev.acquire();
  BENCHMARK("5b. - transfer only (4 bytes)", 1000) {
    dev.transfer(&res, &value, sizeof(value));
  }
  dev.release();

  Serial.println();
  delay(2000);
  value++;
}
