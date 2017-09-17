#include "GPIO.h"
#include "SRPI.h"
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
SRPI<BITORDER, BOARD::D1, BOARD::D2> srpi;
#else
GPIO<BOARD::D10> ss;
Software::SPI<BOARD::D11, BOARD::D12, BOARD::D13> spi;
SPI::Device<0, BITORDER, SPI::MAX_FREQ, BOARD::D10> dev(spi);
SRPI<BITORDER, BOARD::D12, BOARD::D13> srpi;
#endif
#elif defined(USE_HARDWARE_SPI)
#include "Hardware/SPI.h"
GPIO<BOARD::D10> ss;
Hardware::SPI spi;
SPI::Device<0, BITORDER, SPI::MAX_FREQ, BOARD::D10> dev(spi);
SRPI<BITORDER, BOARD::D12, BOARD::D13> srpi;
#endif

#if defined(ARDUINO_attiny)
#define MOSI_PIN 1
#define MISO_PIN 2
#define SCK_PIN 3
#else
#define MOSI_PIN 11
#define MISO_PIN 12
#define SCK_PIN 13
#endif

void setup()
{
  ss.output();
  spi.acquire(0, BITORDER, SPI::MIN_CLOCK_SCALE);
}

void loop()
{
  uint8_t value;

  // 84 us, 95 kHz
  ss.toggle();
  value = shiftIn(MISO_PIN, SCK_PIN, BITORDER);
  ss.toggle();
  delayMicroseconds(10);

  // 13 us, 762 kHz
  ss.toggle();
  spi >> value;
  ss.toggle();
  delayMicroseconds(10);

  // 4.7 us, 1.78 HHz
  ss.toggle();
  srpi >> value;
  ss.toggle();
  delayMicroseconds(100);
}
