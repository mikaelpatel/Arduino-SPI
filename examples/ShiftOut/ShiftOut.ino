#include "GPIO.h"
#include "SRPO.h"
#include "SPI.h"

// Configuration: SPI/BITORDER
#define USE_SOFTWARE_SPI
// #define USE_HARDWARE_SPI
// #define BITORDER LSBFIRST
#define BITORDER MSBFIRST

#if defined(USE_SOFTWARE_SPI)
#include "Software/SPI.h"
#if defined(ARDUINO_attiny)
GPIO<BOARD::D0> ss;
Software::SPI<BOARD::D1, BOARD::D2, BOARD::D3> spi;
SPI::Device<0, BITORDER, SPI::MAX_FREQ, BOARD::D0> dev(spi);
SRPO<BITORDER, BOARD::D1, BOARD::D2> srpo;
#else
GPIO<BOARD::D10> ss;
Software::SPI<BOARD::D11, BOARD::D12, BOARD::D13> spi;
SPI::Device<0, BITORDER, SPI::MAX_FREQ, BOARD::D10> dev(spi);
SRPO<BITORDER, BOARD::D11, BOARD::D13> srpo;
#endif
#elif defined(USE_HARDWARE_SPI)
#include "Hardware/SPI.h"
GPIO<BOARD::D10> ss;
Hardware::SPI spi;
SPI::Device<0, BITORDER, SPI::MAX_FREQ, BOARD::D10> dev(spi);
SRPO<BITORDER, BOARD::D11, BOARD::D13> srpo;
#endif

#if defined(ARDUINO_attiny)
#define MOSI_PIN 11
#define MISO_PIN 12
#define SCK_PIN 13
#else
#define MOSI_PIN 1
#define MISO_PIN 2
#define SCK_PIN 3
#endif

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
  spi << value;
  ss.toggle();
  delayMicroseconds(10);

  // 8 us
  ss.toggle();
  srpo << value;
  ss.toggle();
  delayMicroseconds(100);
}
