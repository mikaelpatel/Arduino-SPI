#include "GPIO.h"
#include "SPI.h"
#include "Driver/SRAM.h"

// #define USE_SOFTWARE_SPI
#define USE_HARDWARE_SPI

#if defined(USE_SOFTWARE_SPI)
#include "Software/SPI.h"
#if defined(ARDUINO_attiny)
Software::SPI<BOARD::D1, BOARD::D2, BOARD::D3> spi;
#else
Software::SPI<BOARD::D11, BOARD::D12, BOARD::D13> spi;
#endif
#elif defined(USE_HARDWARE_SPI)
#include "Hardware/SPI.h"
Hardware::SPI spi;
#endif

SRAM<BOARD::D10> sram(spi);
SRAM<BOARD::D10>::Stream<10000> ios(sram);
SRAM<BOARD::D10>::Stream<10000> temps(sram, 10000);

const int N = 100;
uint32_t n, s0, m0, m1, m2, m3;

void setup()
{
  Serial.begin(57600);
  while (!Serial);

  // Print N analog samples to serial stream
  s0 = micros();
  for (int i = 0; i < N; i++) {
    Serial.println(analogRead(A0));
  }
  Serial.println();
  m0 = micros() - s0;
  Serial.flush();

  // Print N analog samples to sram stream
  s0 = micros();
  for (int i = 0; i < N; i++) {
    ios.println(analogRead(A0));
  }
  ios.println();
  m1 = micros() - s0;

  // Transfer data between two sram streams
  n = ios.available();
  s0 = micros();
  while (ios.available())
    temps.write(ios.read());
  m2 = micros() - s0;

  // Print data from sram stream to serial stream
  s0 = micros();
  while (temps.available())
    Serial.write(temps.read());
  m3 = micros() - s0;

  Serial.print(F("Samples, N = "));
  Serial.println(N);
  Serial.print(F("Serial.print, m0 = "));
  Serial.println(m0 / N);
  Serial.print(F("SRAM::Stream.print, m1 = "));
  Serial.println(m1 / N);
  Serial.print(F("SRAM::Stream.available, n = "));
  Serial.println(n);
  Serial.print(F("SRAM::Stream.write/read, m2 = "));
  Serial.println(m2 / n);
  Serial.print(F("SRAM::Stream.read/Serial.write, m3 = "));
  Serial.println(m3 / N);
}

void loop()
{
}
