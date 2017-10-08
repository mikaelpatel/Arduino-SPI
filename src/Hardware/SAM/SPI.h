/**
 * @file Hardware/SAM/SPI.h
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

#ifndef HARDWARE_SAM_SPI_H
#define HARDWARE_SAM_SPI_H

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
  SPI() : ::SPI() {}

  /**
   * @override{SPI}
   * Acquire bus access. Yield until bus is released.
   * @param[in] mode of access.
   * @param[in] bitorder of serial data.
   * @param[in] scale clock frequency.
   */
  virtual void acquire(uint8_t mode, uint8_t bitorder, uint8_t scale)
  {
    (void) mode;
    (void) bitorder;
    (void) scale;
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
    (void) value;
    return (0);
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
  }

protected:
};
};
#endif
