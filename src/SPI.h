/**
 * @file SPI.h
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

#ifndef SPI_H
#define SPI_H

#include "GPIO.h"

/**
 * Serial Perpheral Interface (SPI) abstract class.
 */
class SPI {
public:
  /**
   * @override{SPI}
   * Acquire bus access with given mode.
   * @param[in] dev device driver.
   * @param[in] mode of access.
   */
  virtual void acquire(void* dev, uint8_t mode) = 0;

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
    const uint8_t* sp = (const uint8_t*) src;
    uint8_t* dp = (uint8_t*) dest;
    while (count--) {
      uint8_t value = (sp != NULL ? *sp++ : 0);
      value = transfer(value);
      if (dp != NULL) *dp++ = value;
    }
  }

  /**
   * Serial Perpheral Interface (SPI) device driver class.
   * @param[in] MODE clock mode (polarity and phase).
   * @param[in] SS_PIN board pin for device chip select.
   */
  template<uint8_t MODE, uint8_t BITORDER, BOARD::pin_t SS_PIN>
  class Device {
  public:
    /**
     * Construct Serial Perpheral Interface (SPI) device driver
     * instance and initiate chip select pin.
     * @param[in] spi bus manager.
     * @param[in] ss initial chip select pin state (default HIGH).
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
      m_spi.acquire(this, MODE | (BITORDER << 2));
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

    /** Chip select pin. */
    GPIO<SS_PIN> m_ss;
  };
};
#endif
