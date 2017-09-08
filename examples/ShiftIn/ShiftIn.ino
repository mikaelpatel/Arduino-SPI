#include "GPIO.h"
#include "SRPI.h"
#include "SPI.h"
#include "Software/SPI.h"

GPIO<BOARD::D10> ss;
SRPI<LSBFIRST, BOARD::D13, BOARD::D11> srpi;
Software::SPI<BOARD::D11, BOARD::D12, BOARD::D13> spi;
#define SCK_PIN 11
#define MOSI_PIN 12
#define MISO_PIN 13

void setup()
{
  ss.output();
  spi.acquire(0, LSBFIRST, SPI::MIN_PRESCALE);
}

void loop()
{
  uint8_t value;

  // 107 us, 13.31 us per bit, 75 kHz
  ss.toggle();
  value = shiftIn(MISO_PIN, SCK_PIN, LSBFIRST);
  ss.toggle();
  delayMicroseconds(100);

  // 15.75 us, 1.75 us per bit, 570 kHz
  ss.toggle();
  value = spi.transfer(0);
  ss.toggle();
  delayMicroseconds(100);

  // 5 us, 0.5625 us per bit, 1.8 MHz
  ss.toggle();
  srpi >> value;
  ss.toggle();
  delayMicroseconds(100);
}
