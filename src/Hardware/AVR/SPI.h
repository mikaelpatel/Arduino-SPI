/**
 * @file Hardware/AVR/SPI.h
 * @version 1.1
 *
 * @section License
 * Copyright (C) 2017, Mikael Patel
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 */

#ifndef HARDWARE_AVR_SPI_H
#define HARDWARE_AVR_SPI_H

#include "GPIO.h"
#include "SPI.h"

/**
 * Hardware Serial Perpheral Interface (SPI) class.
 */
namespace Hardware {

#if defined(SPDR)
class SPI : public ::SPI {
public:
  /**
   * Serial Perpheral Interface (SPI) constructor.
   */
  SPI() :
    ::SPI()
  {
    m_ss.output();
    m_sck.output();
    m_mosi.output();
    m_miso.input();
  }

  /**
   * @override{SPI}
   * Calculate bus prescale from device frequency.
   * @param[in] frequency device access.
   * @return prescale
   */
  virtual uint8_t prescale(uint32_t frequency)
  {
    uint16_t scale = F_CPU / frequency;
    if (scale <= 2)  return (0b100);
    if (scale <= 4)  return (0b000);
    if (scale <= 8)  return (0b101);
    if (scale <= 16) return (0b001);
    if (scale <= 32) return (0b110);
    if (scale <= 64) return (0b111);
    return (0b011);
  }

  /**
   * @override{SPI}
   * Acquire bus access. Yield until bus is released.
   * @param[in] mode of access.
   * @param[in] bitorder of serial data.
   * @param[in] prescale clock prescale.
   */
  virtual void acquire(uint8_t mode, uint8_t bitorder, uint8_t prescale)
  {
    // Wait for bus manager to be released
    lock();

    // Set control registers: mode, bitorder and clock
    SPCR = _BV(SPE)
         | _BV(MSTR)
         | (bitorder == LSBFIRST ? _BV(DORD) : 0)
         | ((mode & 0x03) << CPHA)
         | (prescale & 0x03);
    SPSR = ((prescale >> 3) & 0x01);
  }

  /**
   * @override{SPI}
   * Send given byte to device. Returns byte received from device.
   * @param[in] value to send to device.
   * @return received value.
   */
  virtual uint8_t transfer(uint8_t value)
    __attribute__((always_inline))
  {
    SPDR = value;
    __asm__ __volatile__("nop");
    loop_until_bit_is_set(SPSR, SPIF);
    return (SPDR);
  }

  /**
   * @override{SPI}
   * Transfer given number of bytes from source buffer to
   * device. Store received bytes in destination buffer.
   * @param[in] dest destination buffer.
   * @param[in] src source buffer.
   * @param[in] count number of bytes to transfer.
   */
  virtual void transfer(void* dest, const void* src, size_t count)
  {
    if (count == 0 || dest == NULL || src == NULL) return;
    const uint8_t* sp = (const uint8_t*) src;
    uint8_t* dp = (uint8_t*) dest;
    uint8_t value = *sp++;
    SPDR = value;
    while (--count) {
      value = *sp++;
      loop_until_bit_is_set(SPSR, SPIF);
      SPDR = value;
      *dp++ = SPDR;
    }
    loop_until_bit_is_set(SPSR, SPIF);
    *dp = SPDR;
  }

  /**
   * @override{SPI}
   * Read given number of bytes from device and store in buffer.
   * @param[in] buf buffer pointer.
   * @param[in] count number of bytes.
   */
  virtual void read(void* buf, size_t count)
  {
    if (count == 0 || buf == NULL) return;
    uint8_t* bp = (uint8_t*) buf;
    SPDR = 0;
    while (--count) {
      __asm__ __volatile__("nop");
      loop_until_bit_is_set(SPSR, SPIF);
      SPDR = 0;
      *bp++ = SPDR;
    }
    loop_until_bit_is_set(SPSR, SPIF);
    *bp = SPDR;
  }

  /**
   * @override{SPI}
   * Write given number of bytes from buffer to device.
   * @param[in] buf buffer pointer.
   * @param[in] count number of bytes.
   */
  virtual void write(const void* buf, size_t count)
  {
    if (count == 0 || buf == NULL) return;
    uint8_t* bp = (uint8_t*) buf;
    uint8_t value = *bp++;
    SPDR = value;
    while (--count) {
      value = *bp++;
      loop_until_bit_is_set(SPSR, SPIF);
      SPDR = value;
    }
    loop_until_bit_is_set(SPSR, SPIF);
  }

protected:
  /** Slave select pin. */
  GPIO<BOARD::SS> m_ss;

  /** Clock pin. */
  GPIO<BOARD::SCK> m_sck;

  /** Master Output Slave Input pin. */
  GPIO<BOARD::MOSI> m_mosi;

  /** Master Input Slave Output pin. */
  GPIO<BOARD::MISO> m_miso;
};

#elif defined(USIDR)

class SPI : public ::SPI {
public:
  /**
   * Serial Perpheral Interface (SPI) constructor.
   */
  SPI() :
    ::SPI()
  {
    m_ss.output();
    m_sck.output();
    m_mosi.output();
    m_miso.input();
  }

  /**
   * @override{SPI}
   * Acquire bus access. Yield until bus is released.
   * @param[in] mode of access.
   * @param[in] bitorder of serial data.
   * @param[in] prescale clock prescale.
   */
  virtual void acquire(uint8_t mode, uint8_t bitorder, uint8_t prescale)
  {
    // Wait for bus manager to be released
    lock();

    // Not used: only MSBFIRST bitorder and max frequency
    (void) bitorder;
    (void) prescale;

    // Set clock polarity
    m_sck.write(mode & 0x02);

    // Cache clocking command
    m_usicr = (_BV(USIWM0) | _BV(USICS1) | _BV(USICLK) | _BV(USITC));
    if (mode == 1 || mode == 2) m_usicr |= _BV(USICS0);
  }

  /**
   * @override{SPI}
   * Send given byte to device. Returns byte received from device.
   * @param[in] value to send to device.
   * @return received value.
   */
  virtual uint8_t transfer(uint8_t value)
  {
    USIDR = value;
    USISR = _BV(USIOIF);
    register uint8_t cntl = m_usicr;
    do {
      USICR = cntl;
    } while ((USISR & _BV(USIOIF)) == 0);
    return (USIDR);
  }

protected:
  /** Slave select pin. */
  GPIO<BOARD::SS> m_ss;

  /** Clock pin. */
  GPIO<BOARD::SCK> m_sck;

  /** Master Output Slave Input pin. */
  GPIO<BOARD::MOSI> m_mosi;

  /** Master Input Slave Output pin. */
  GPIO<BOARD::MISO> m_miso;

  /** USI clock command. */
  uint8_t m_usicr;
};
#endif

};
#endif
