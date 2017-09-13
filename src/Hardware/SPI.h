/**
 * @file Hardware/SPI.h
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

#ifndef HARDWARE_SPI_H
#define HARDWARE_SPI_H

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
    m_busy(false)
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
   * @param[in] scale clock frequency.
   */
  virtual void acquire(uint8_t mode, uint8_t bitorder, uint8_t scale)
  {
    // Wait for bus manager to be released
    while (m_busy) yield();
    m_busy = true;

    // Calculate clock setting for given scale
    uint8_t spr = 0;
    scale >>= 2;
    while (scale != 0) {
      spr++;
      scale >>= 1;
    }

    // Set control registers: mode, bitorder and clock
    SPCR = _BV(SPE)
         | _BV(MSTR)
         | (bitorder == LSBFIRST ? _BV(DORD) : 0)
         | ((mode & 3) << CPHA)
         | ((spr >> 1) & 3);
    SPSR = ((spr & 0x01) == 0);
  }

  /**
   * @override{SPI}
   * Release bus access.
   */
  virtual void release()
  {
    m_busy = false;
  }

  /**
   * @override{SPI}
   * Send given byte to device. Returns byte received from device.
   * @param[in] value to send to device.
   * @return received value.
   */
  virtual uint8_t transfer(uint8_t value)
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
      *dp++ = SPDR;
      SPDR = value;
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
    uint8_t value;
    SPDR = 0;
    while (--count) {
      loop_until_bit_is_set(SPSR, SPIF);
      value = SPDR;
      SPDR = 0;
      *bp++ = value;
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
  /** Bus manager semaphore. */
  volatile bool m_busy;

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
    m_busy(false)
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
   * @param[in] scale clock frequency.
   */
  virtual void acquire(uint8_t mode, uint8_t bitorder, uint8_t scale)
  {
    // Wait for bus manager to be released
    while (m_busy) yield();
    m_busy = true;

    // Not used: only MSBFIRST bitorder and max frequency
    (void) bitorder;
    (void) scale;

    // Set clock polarity
    m_sck.write(mode & 0x02);

    // Cache clocking command
    m_usicr = (_BV(USIWM0) | _BV(USICS1) | _BV(USICLK) | _BV(USITC));
    if (mode == 1 || mode == 2) m_usicr |= _BV(USICS0);
  }

  /**
   * @override{SPI}
   * Release bus access.
   */
  virtual void release()
  {
    m_busy = false;
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
  /** Bus manager semaphore. */
  volatile bool m_busy;

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
