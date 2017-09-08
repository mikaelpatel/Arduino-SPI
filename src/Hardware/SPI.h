/**
 * @file Hardware/SPI.h
 * @version 1.0
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
   * @param[in] prescale clock frequency.
   */
  virtual void acquire(uint8_t mode, uint8_t bitorder, uint8_t prescale)
  {
    while (m_busy) yield();
    m_busy = true;
    int8_t spr = 0;
    prescale >>= 2;
    while (prescale != 0) {
      spr++;
      prescale >>= 1;
    }
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

};
#endif
