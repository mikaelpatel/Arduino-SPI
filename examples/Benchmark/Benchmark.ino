#include "GPIO.h"
#include "SPI.h"

// Configuration:
// #define USE_SOFTWARE_SPI
#define USE_HARDWARE_SPI
// #define BITORDER LSBFIRST
#define BITORDER MSBFIRST

#if defined(USE_SOFTWARE_SPI)
#include "Software/SPI.h"
#if defined(ARDUINO_attiny)
GPIO<BOARD::D0> ss;
Software::TWI<BOARD::D1, BOARD::D2> twi;
SPI::Device<0, BITORDER, SPI::MAX_FREQ, BOARD::D0> dev(spi);
#else
GPIO<BOARD::D10> ss;
Software::SPI<BOARD::D11, BOARD::D12, BOARD::D13> spi;
SPI::Device<0, BITORDER, SPI::MAX_FREQ, BOARD::D10> dev(spi);
#endif
#elif defined(USE_HARDWARE_SPI)
#include "Hardware/SPI.h"
GPIO<BOARD::D10> ss;
Hardware::SPI spi;
SPI::Device<0, BITORDER, SPI::MAX_FREQ, BOARD::D10> dev(spi);
#endif

void setup()
{
}

void loop()
{
  // 32-bit/4 byte test data
  static uint32_t value = 0;
  uint32_t res;

  // SPI bus manager serial data transfer
  ss.toggle();
  spi.acquire(0, BITORDER, SPI::MIN_CLOCK_SCALE);
  spi.transfer(value);
  spi.release();
  ss.toggle();
  delayMicroseconds(20);

  // SPI device driver serial data transfer
  dev.acquire();
  dev.transfer(value);
  dev.release();
  delayMicroseconds(10);

  // SPI device driver serial buffer read
  dev.acquire();
  dev.read(&res, sizeof(value));
  dev.release();
  delayMicroseconds(10);

  // SPI device driver serial buffer write
  dev.acquire();
  dev.write(&value, sizeof(value));
  dev.release();
  delayMicroseconds(10);

  // SPI device driver serial buffer transfer
  dev.acquire();
  dev.transfer(&res, &value, sizeof(value));
  dev.release();

  delayMicroseconds(100);
  value++;
}
