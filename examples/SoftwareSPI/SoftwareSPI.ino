#include "GPIO.h"
#include "SPI.h"
#include "Software/SPI.h"

// Serial Peripheral Interface, bus manager and device driver support
#define BITORDER MSBFIRST
GPIO<BOARD::D10> ss;
Software::SPI<BOARD::D11, BOARD::D12, BOARD::D13> spi;
SPI::Device<0, BITORDER, SPI::MAX_FREQ, BOARD::D10> dev(spi);

void setup()
{
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

  // SPI bus manager serial data transfer: 16.75 us, 15.06 us
  ss.toggle();
  spi.acquire(0, BITORDER, SPI::MIN_PRESCALE);
  spi.transfer(value);
  spi.release();
  ss.toggle();
  delayMicroseconds(20);

  // SPI device driver serial data transfer: 15.87 us, 14.19 us
  dev.acquire();
  dev.transfer(value);
  dev.release();
  delayMicroseconds(10);

  // SPI device driver serial clock: 68 us, 55.75 us
  dev.acquire();
  dev.transfer(NULL, NULL, sizeof(value));
  dev.release();
  delayMicroseconds(10);

  // SPI device driver serial buffer write: 68.19 us, 58.38 us
  dev.acquire();
  dev.transfer(NULL, &value, sizeof(value));
  dev.release();
  delayMicroseconds(10);

  // SPI device driver serial buffer read: 70.62 us, 58.31 us
  dev.acquire();
  dev.transfer(&res, NULL, sizeof(value));
  dev.release();
  delayMicroseconds(10);

  // SPI device driver serial buffer transfer: 69.25 us, 59.44 us
  dev.acquire();
  dev.transfer(&res, &value, sizeof(value));
  dev.release();

  delayMicroseconds(100);
  value++;
}
