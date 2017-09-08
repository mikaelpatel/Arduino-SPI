/**
 * @file SPI.h
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

#ifndef SPI_H
#define SPI_H

#include "GPIO.h"

/**
 * Serial Perpheral Interface (SPI) abstract class.
 */
class SPI {
public:
  /** Minimum clock prescale. */
  static const uint8_t MIN_PRESCALE = 2;

  /** Maximum clock frequency. */
  static const uint32_t MAX_FREQ = F_CPU / MIN_PRESCALE;

  /**
   * @override{SPI}
   * Acquire bus access with given mode.
   * @param[in] mode of access.
   * @param[in] bitorder of serial data.
   * @param[in] prescale clock.
   */
  virtual void acquire(uint8_t mode, uint8_t bitorder, uint8_t prescale) = 0;

  /**
   * @override{SPI}
   * Release bus access.
   */
  virtual void release() = 0;

  /**
   * @override{SPI}
   * Send given byte to device. Returns byte received from device.
   * @param[in] value to send to device.
   * @return received value.
   */
  virtual uint8_t transfer(uint8_t value) = 0;

  /**
   * Send given number of bytes from source buffer to device. Store
   * received bytes in destination buffer. A read operation may be
   * performed by passing NULL as the source buffer. A write operation
   * by passing NULL as the destination buffer.
   * @param[in] dest destination buffer.
   * @param[in] src source buffer.
   * @param[in] count number of bytes to transfer.
   */
  void transfer(void* dest, const void* src, size_t count)
  {
    if (count == 0) return;
    const uint8_t* sp = (const uint8_t*) src;
    uint8_t* dp = (uint8_t*) dest;
    if (dp != NULL) {
      if (sp != NULL)
	do *dp++ = transfer(*sp++); while (--count);
      else
	do *dp++ = transfer(0); while (--count);
    }
    else {
      if (sp != NULL)
	do transfer(*sp++); while (--count);
      else
	do transfer(0); while (--count);
    }
  }

  /**
   * Serial Perpheral Interface (SPI) device driver class.
   * @param[in] MODE clock mode (polarity and phase).
   * @param[in] BITORDER LSB or MSB first.
   * @param[in] FREQ max clock frequency.
   * @param[in] SS_PIN board pin for device slave select.
   */
  template<uint8_t MODE, uint8_t BITORDER, uint32_t FREQ, BOARD::pin_t SS_PIN>
  class Device {
  public:
    /**
     * Construct Serial Perpheral Interface (SPI) device driver
     * instance and initiate slave select pin.
     * @param[in] spi bus manager.
     * @param[in] ss initial slave select pin state (default HIGH).
     */
    Device(SPI& spi, bool ss = HIGH) :
      m_spi(spi)
    {
      m_ss.output();
      m_ss = ss;
    }

    /**
     * Acquire bus manager and slave device.
     */
    void acquire()
    {
      m_spi.acquire(MODE, BITORDER, FREQ / F_CPU);
      m_ss.toggle();
    }

    /**
     * Release bus manager and slave device.
     */
    void release()
    {
      m_ss.toggle();
      m_spi.release();
    }

    /**
     * Send given byte to device. Returns byte received from device.
     * @param[in] value to send to device.
     * @return received value.
     */
    uint8_t transfer(uint8_t value)
    {
      return (m_spi.transfer(value));
    }

    /**
     * Send given number of bytes from source buffer to device. Store
     * received bytes in destination buffer. A read operation may be
     * performed by passing NULL as the source buffer. A write operation
     * by passing NULL as the destination buffer.
     * @param[in] dest destination buffer.
     * @param[in] src source buffer.
     * @param[in] count number of bytes to transfer.
     */
    void transfer(void* dest, const void* src, size_t count)
    {
      m_spi.transfer(dest, src, count);
    }

  protected:
    /** Bus manager. */
    SPI& m_spi;

    /** Slave select pin. */
    GPIO<SS_PIN> m_ss;
  };
};
#endif
