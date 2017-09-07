/**
 * @file Software/SPI.h
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

#ifndef SOFTWARE_SPI_H
#define SOFTWARE_SPI_H

#include "GPIO.h"
#include "SPI.h"

#ifndef CHARBITS
#define CHARBITS 8
#endif

/**
 * Software Serial Perpheral Interface (SPI) class using GPIO.
 * @param[in] SCL_PIN board pin for clock.
 * @param[in] MOSI_PIN board pin for master output slave input.
 * @param[in] MISO_PIN board pin for master input slave output.
 */
namespace Software {
template<BOARD::pin_t SCL_PIN,
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
    m_scl.output();
    m_mosi.output();
    m_miso.input();
    m_dev = NULL;
  }

  /**
   * @override{SPI}
   * Acquire bus access. Yield until bus is released.
   * @param[in] dev device driver.
   * @param[in] mode of access.
   */
  virtual void acquire(void* dev, uint8_t mode)
  {
    while (m_dev != NULL) yield();
    m_dev = dev;
    m_cpha = mode & 1;
    m_scl = mode & 2;
    m_msbfirst = mode & 4;
  }

  /**
   * @override{SPI}
   * Release bus access.
   */
  virtual void release()
  {
    m_dev = NULL;
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
    if (m_msbfirst) {
      do {
	if (cpha) m_scl.toggle();
	m_mosi = value & 0x80;
	m_scl.toggle();
	value <<= 1;
	value |= (m_miso ? 0x01 : 0x00);
	if (!cpha) m_scl.toggle();
      } while (--bits);
    }
    else {
      do {
	if (cpha) m_scl.toggle();
	m_mosi = value & 0x01;
	m_scl.toggle();
	value >>= 1;
	value |= (m_miso ? 0x80 : 0x00);
	if (!cpha) m_scl.toggle();
      } while (--bits);
    }
    return (value);
  }

protected:
  /** Clock pin. */
  GPIO<SCL_PIN> m_scl;

  /** Master Output Slave Input pin. */
  GPIO<MOSI_PIN> m_mosi;

  /** Master Input Slave Output pin. */
  GPIO<MISO_PIN> m_miso;

  /** Bus manager semaphore. */
  volatile void* m_dev;

  /** Clock phase flag. */
  bool m_cpha;

  /** MSB first bitorder flag. */
  bool m_msbfirst;
};

};
#endif
