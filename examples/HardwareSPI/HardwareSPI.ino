#include "GPIO.h"
#include "SPI.h"
#include "Hardware/SPI.h"

// Serial Peripheral Interface, bus manager and device driver support
#define BITORDER MSBFIRST
#define FREQ 2000000L
GPIO<BOARD::D10> ss;
Hardware::SPI spi;
SPI::Device<0, BITORDER, FREQ, BOARD::D10> dev(spi);

void setup()
{
  ss.output();
}

void loop()
{
  // 32-bit/4 byte test data
  static uint32_t value = 0;
  uint32_t res;

  // SS toggle (baseline): 0.125 us
  ss.toggle();
  ss.toggle();
  delayMicroseconds(10);

  // SPI bus manager serial data transfer:
  ss.toggle();
  spi.acquire(0, BITORDER, SPI::MIN_PRESCALE);
  spi.transfer(value);
  spi.release();
  ss.toggle();
  delayMicroseconds(20);

  // SPI device driver serial data transfer:
  dev.acquire();
  dev.transfer(value);
  dev.release();
  delayMicroseconds(10);

  // SPI device driver serial clock:
  dev.acquire();
  dev.transfer(NULL, NULL, sizeof(value));
  dev.release();
  delayMicroseconds(10);

  // SPI device driver serial buffer write:
  dev.acquire();
  dev.transfer(NULL, &value, sizeof(value));
  dev.release();
  delayMicroseconds(10);

  // SPI device driver serial buffer read:
  dev.acquire();
  dev.transfer(&res, NULL, sizeof(value));
  dev.release();
  delayMicroseconds(10);

  // SPI device driver serial buffer transfer:
  dev.acquire();
  dev.transfer(&res, &value, sizeof(value));
  dev.release();

  delayMicroseconds(100);
  value++;
}
