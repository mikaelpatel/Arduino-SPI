/**
 * @file SRAM.h
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

#ifndef SRAM_H
#define SRAM_H

#include "SPI.h"

/**
 * Driver for the Microchip 23LC1024, 1M bit SPI Serial SRAM.
 * @param[in] SS_PIN slave select board pin.
 * @section Circuit
 * @code
 *                          23LC1024
 *                       +------------+
 * (SS)----------------1-|CS   U   VCC|-8----------------(VCC)
 * (MISO)--------------2-|SO      HOLD|-7---------(VCC/PULLUP)
 * (VCC/PULLUP)--------3-|NU       SCK|-6----------------(SCK)
 * (GND)---------------4-|VSS       SI|-3---------------(MOSI)
 *                       +------------+
 * @endcode
 */
template<BOARD::pin_t SS_PIN>
class SRAM : protected SPI::Device<0, MSBFIRST, 16000000L, SS_PIN> {
public:
  /** Maximum device clock frequency. */
  static const uint32_t MAX_FREQ = 16000000L;

  /**
   * Construct and initiate SRAM device driver with given slave select
   * board pin and bus manager.
   */
  SRAM(SPI& spi) :
    SPI::Device<0, MSBFIRST, MAX_FREQ, SS_PIN>(spi)
  {}

  /**
   * Read count number of bytes from SRAM address to buffer.
   * @param[in] addr memory address on device.
   * @param[in] buf destination buffer pointer.
   * @param[in] count number of bytes to read from device.
   */
  void read(uint32_t addr, void* buf, size_t count)
  {
    addr = __builtin_bswap32(addr);
    acquire();
    transfer(READ);
    write(((uint8_t*) &addr) + 1, 3);
    read(buf, count);
    release();
  }

  /**
   * Write count number of bytes to SRAM address from buffer.
   * @param[in] addr memory address on device.
   * @param[in] buf source buffer pointer.
   * @param[in] count number of bytes to write to device.
   */
  void write(uint32_t addr, const void* buf, size_t count)
  {
    addr = __builtin_bswap32(addr);
    acquire();
    transfer(WRITE);
    write(((uint8_t*) &addr) + 1, 3);
    write(buf, count);
    release();
  }

protected:
  enum {
    READ = 0x03,		//!< Read data from memory
    WRITE = 0x02,		//!< Write data to memory
    RDMR = 0x05,		//!< Read mode register
    WRMR = 0x01			//!< Write mode register
  };

  using SPI::Device<0,MSBFIRST,MAX_FREQ,SS_PIN>::acquire;
  using SPI::Device<0,MSBFIRST,MAX_FREQ,SS_PIN>::transfer;
  using SPI::Device<0,MSBFIRST,MAX_FREQ,SS_PIN>::read;
  using SPI::Device<0,MSBFIRST,MAX_FREQ,SS_PIN>::write;
  using SPI::Device<0,MSBFIRST,MAX_FREQ,SS_PIN>::release;
};
#endif
