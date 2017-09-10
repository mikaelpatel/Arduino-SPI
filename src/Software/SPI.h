/**
 * @file Software/SPI.h
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

#ifndef SOFTWARE_SPI_H
#define SOFTWARE_SPI_H

#include "GPIO.h"
#include "SPI.h"

#ifndef CHARBITS
#define CHARBITS 8
#endif

/**
 * Software Serial Perpheral Interface (SPI) class using GPIO.
 * @param[in] SCK_PIN board pin for clock.
 * @param[in] MOSI_PIN board pin for master output slave input.
 * @param[in] MISO_PIN board pin for master input slave output.
 */
namespace Software {
template<BOARD::pin_t SCK_PIN,
	 BOARD::pin_t MOSI_PIN,
	 BOARD::pin_t MISO_PIN>
class SPI : public ::SPI {
public:
  /**
   * Serial Perpheral Interface (SPI) constructor. Initiate pin
   * input/output mode.
   */
  SPI()
  {
    m_busy = false;
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
    (void) scale;
    while (m_busy) yield();
    m_busy = true;
    m_sck = mode & 2;
    m_cpha = mode & 1;
    m_bitorder = bitorder;
  }

  /**
   * @override{SPI}
   * Release bus access.
   */
  virtual void release()
  {
    m_busy = false;
    m_mosi = LOW;
  }

  /**
   * @override{SPI}
   * Send given byte to device. Returns byte received from device.
   * @param[in] value to send to device.
   * @return received value.
   */
  virtual uint8_t transfer(uint8_t value)
  {
    const bool cpha = m_cpha;
    uint8_t bits = CHARBITS;
    if (m_bitorder == MSBFIRST) {
      do {
	if (cpha) m_sck.toggle();
	m_mosi = value & 0x80;
	m_sck.toggle();
	value <<= 1;
	value |= m_miso;
	if (!cpha) m_sck.toggle();
      } while (--bits);
    }
    else {
      do {
	if (cpha) m_sck.toggle();
	m_mosi = value & 0x01;
	m_sck.toggle();
	value >>= 1;
	value |= (m_miso ? 0x80 : 0x00);
	if (!cpha) m_sck.toggle();
      } while (--bits);
    }
    return (value);
  }

protected:
  /** Clock pin. */
  GPIO<SCK_PIN> m_sck;

  /** Master Output Slave Input pin. */
  GPIO<MOSI_PIN> m_mosi;

  /** Master Input Slave Output pin. */
  GPIO<MISO_PIN> m_miso;

  /** Bus manager semaphore. */
  volatile bool m_busy;

  /** Clock phase flag. */
  uint8_t m_cpha;

  /** Serial data bitorder flag. */
  uint8_t m_bitorder;
};

};
#endif
