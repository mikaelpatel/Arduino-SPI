#include "GPIO.h"
#include "SRPO.h"
#include "SPI.h"
#include "Software/SPI.h"

GPIO<BOARD::D10> ss;
SRPO<LSBFIRST, BOARD::D11, BOARD::D13> srpo;
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
  uint8_t value = 0xaa;

  // 109 us
  ss.toggle();
  shiftOut(MOSI_PIN, SCK_PIN, BITORDER, value);
  ss.toggle();
  delayMicroseconds(10);

  // 15 us
  ss.toggle();
  spi.transfer(value);
  ss.toggle();
  delayMicroseconds(10);

  // 8 us
  ss.toggle();
  srpo << value;
  ss.toggle();
  delayMicroseconds(100);
}
