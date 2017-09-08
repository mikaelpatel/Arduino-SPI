#include "GPIO.h"
#include "SRPO.h"
#include "SPI.h"
#include "Software/SPI.h"

GPIO<BOARD::D10> ss;
SRPO<LSBFIRST, BOARD::D12, BOARD::D11> srpo;
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
  uint8_t value = 0xaa;

  // 115 us, 14-15 us per bit, 74 kHz
  ss.toggle();
  shiftOut(MOSI_PIN, SCK_PIN, LSBFIRST, value);
  ss.toggle();
  delayMicroseconds(100);

  // 15 us, 1.563 us per bit, 640 kHz
  ss.toggle();
  spi.transfer(value);
  ss.toggle();
  delayMicroseconds(100);

  // 8 us, 1 us per bit, 1 MHz
  ss.toggle();
  srpo << value;
  ss.toggle();
  delayMicroseconds(100);
}
