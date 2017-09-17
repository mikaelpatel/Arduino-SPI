#include "GPIO.h"
#include "SPI.h"
#include "Driver/SRAM.h"

// Configuration: SPI
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

const size_t BUF_MAX = 1000;
static uint8_t buf[BUF_MAX];

void setup()
{
  Serial.begin(57600);
  while (!Serial);

  for (size_t i = 0; i < BUF_MAX; i++) buf[i] = i;
}

void loop()
{
  // 11.25/11.25 us, protocol overhead
  sram.write(0x0000, buf, 0);
  sram.read(0x0000, buf, 0);

  // 12.94/13 us/byte, 77 kbyte/s, 1 byte
  sram.write(0x0001, buf, 1);
  sram.read(0x0001, buf, 1);

  // 2.587/2.625 us/byte, 400 kbyte/s, 10 byte
  sram.write(0x0010, buf, 10);
  sram.read(0x0010, buf, 10);

  // 1.552/1.612 us/byte, 625 kbyte/s, 100 byte
  sram.write(0x0100, buf, 100);
  sram.read(0x00100, buf, 100);

  // 1.455/1.522 us/byte, 667 kbyte/s, 1000 byte
  sram.write(0x1000, buf, sizeof(buf));
  sram.read(0x1000, buf, sizeof(buf));

  // Print buffer and increment values
  for (size_t i = 0; i < BUF_MAX; i++) {
    Serial.print(buf[i]);
    Serial.print(' ');
    buf[i] = buf[i] + 1;
  }
  Serial.println();
  delay(500);
}
