#include "GPIO.h"
#include "SRPI.h"
#include "SPI.h"
#include "benchmark.h"

// Configuration: SPI/BITORDER
#define USE_SOFTWARE_SPI
// #define BITORDER LSBFIRST
#define BITORDER MSBFIRST

#if defined(USE_SOFTWARE_SPI)
#include "Software/SPI.h"
Software::SPI<BOARD::D11, BOARD::D12, BOARD::D13> spi;
#else
#include "Hardware/SPI.h"
Hardware::SPI spi;
#endif

GPIO<BOARD::D10> ss;
SPI::Device<0, BITORDER, SPI::MAX_FREQ, BOARD::D10> dev(spi);
SRPI<BITORDER, BOARD::D12, BOARD::D13> srpi;

#define SS_PIN 10
#define MOSI_PIN 11
#define MISO_PIN 12
#define SCK_PIN 13

void setup()
{
  Serial.begin(57600);
  while (!Serial);
  ss.output();
  spi.acquire(0, BITORDER, spi.prescale(SPI::MAX_FREQ));
  BENCHMARK_BASELINE(1000);
}

void loop()
{
  uint8_t value;

  BENCHMARK("1. Arduino core shiftIn", 1000) {
    ss.toggle();
    value = shiftIn(MISO_PIN, SCK_PIN, BITORDER);
    ss.toggle();
  }

  BENCHMARK("2. SPI input operator", 1000) {
    ss.toggle();
    spi >> value;
    ss.toggle();
  }

  BENCHMARK("3, SRPI input operator", 1000) {
    ss.toggle();
    srpi >> value;
    ss.toggle();
  }

  Serial.println();
  delay(2000);
}
