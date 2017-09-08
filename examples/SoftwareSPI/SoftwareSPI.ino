#include "GPIO.h"
#include "SPI.h"
#include "Software/SPI.h"

// Serial Peripheral Interface, bus manager and device driver support
GPIO<BOARD::D10> ss;
Software::SPI<BOARD::D11, BOARD::D12, BOARD::D13> spi;
SPI::Device<0, LSBFIRST, SPI::MAX_FREQ, BOARD::D10> dev(spi);

void setup()
{
}

void loop()
{
  // 32-bit/4 byte test data
  static uint32_t value = 0;
  uint32_t res;

  // SS toggle (baseline)
  ss.toggle();
  ss.toggle();
  delayMicroseconds(10);

  // SPI bus manager serial data transfer
  ss.toggle();
  spi.acquire(0, LSBFIRST, SPI::MIN_PRESCALE);
  spi.transfer(value);
  spi.release();
  ss.toggle();
  delayMicroseconds(20);

  // SPI device driver serial data transfer
  dev.acquire();
  dev.transfer(value);
  dev.release();
  delayMicroseconds(10);

  // SPI device driver serial buffer write
  dev.acquire();
  dev.transfer(NULL, NULL, sizeof(value));
  dev.release();
  delayMicroseconds(10);

  // SPI device driver serial buffer write
  dev.acquire();
  dev.transfer(NULL, &value, sizeof(value));
  dev.release();
  delayMicroseconds(10);

  // SPI device driver serial buffer read
  dev.acquire();
  dev.transfer(&res, NULL, sizeof(value));
  dev.release();
  delayMicroseconds(10);

  // SPI device driver serial buffer transfer
  dev.acquire();
  dev.transfer(&res, &value, sizeof(value));
  dev.release();

  delayMicroseconds(100);
  value++;
}
