#include "GPIO.h"
#include "SPI.h"
#include "Software/SPI.h"

Software::SPI<BOARD::D11, BOARD::D12, BOARD::D13> spi;
SPI::Device<0, LSBFIRST, BOARD::D10> dev(spi);

void setup()
{
}

void loop()
{
  static uint32_t value = 0;
  uint32_t res;

  spi.acquire(&dev, 0);
  spi.transfer(value);
  spi.release();
  delayMicroseconds(20);

  dev.acquire();
  dev.transfer(value);
  dev.release();
  delayMicroseconds(10);

  dev.acquire();
  dev.transfer(NULL, &value, sizeof(value));
  dev.release();
  delayMicroseconds(10);

  dev.acquire();
  dev.transfer(&res, NULL, sizeof(value));
  dev.release();

  dev.acquire();
  dev.transfer(&res, &value, sizeof(value));
  dev.release();

  delayMicroseconds(100);
  value++;
}
