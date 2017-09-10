#include "GPIO.h"
#include "SRPI.h"
#include "SPI.h"
#include "Software/SPI.h"

GPIO<BOARD::D10> ss;
SRPI<LSBFIRST, BOARD::D12, BOARD::D13> srpi;
Software::SPI<BOARD::D11, BOARD::D12, BOARD::D13> spi;
#define MOSI_PIN 11
#define MISO_PIN 12
#define SCK_PIN 13
#define BITORDER LSBFIRST

void setup()
{
  ss.output();
  spi.acquire(0, BITORDER, SPI::MIN_CLOCK_SCALE);
}

void loop()
{
  uint8_t value;

  // 85 us
  ss.toggle();
  value = shiftIn(MISO_PIN, SCK_PIN, BITORDER);
  ss.toggle();
  delayMicroseconds(10);

  // 16 us
  ss.toggle();
  value = spi.transfer(0);
  ss.toggle();
  delayMicroseconds(10);

  // 5 us
  ss.toggle();
  srpi >> value;
  ss.toggle();
  delayMicroseconds(100);
}
