/**
 * @file SRAM.h
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
 * (GND)---------------4-|VSS       SI|-5---------------(MOSI)
 *                       +------------+
 * @endcode
 */
template<BOARD::pin_t SS_PIN, uint32_t FREQ = 16000000L>
class SRAM : protected SPI::Device<0, MSBFIRST, FREQ, SS_PIN> {
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
   * @param[in] dest destination buffer pointer.
   * @param[in] src source memory address on device.
   * @param[in] count number of bytes to read from device.
   */
  void read(void* dest, uint32_t src, size_t count)
  {
    uint8_t* header = (uint8_t*) &src;
    src = __builtin_bswap32(src);
    header[0] = READ;
    acquire();
    write(header, sizeof(src));
    read(dest, count);
    release();
  }

  /**
   * Write count number of bytes to SRAM address from buffer.
   * @param[in] dest destination memory address on device.
   * @param[in] src source buffer pointer.
   * @param[in] count number of bytes to write to device.
   */
  void write(uint32_t dest, const void* src, size_t count)
  {
    uint8_t* header = (uint8_t*) &dest;
    dest = __builtin_bswap32(dest);
    header[0] = WRITE;
    acquire();
    write(header, sizeof(dest));
    write(src, count);
    release();
  }

  /**
   * SRAM based Stream of given size. Write/print intermediate data
   * to the stream that may later be read and transfered. Multiple
   * stream may be created on the same device by assigning start
   * address and size.
   * @param[in] SIZE number of bytes in stream.
   */
  template<uint16_t SIZE>
  class Stream : public ::Stream {
  public:
    /**
     * Construct stream on given sram device at the given start
     * address. The storage size of the stream is given as a template
     * parameter.
     * @param[in] sram device for stream.
     * @param[in] addr start address for stream (default 0).
     */
    Stream(SRAM<SS_PIN,FREQ> &sram, uint32_t addr = 0) :
      m_sram(sram),
      m_addr(addr),
      m_put(0),
      m_get(0),
      m_count(0)
    {}

    /**
     * @override{Stream}
     * Write given byte to stream. Return number of bytes written,
     * zero if full.
     * @param[in] byte to write.
     * @return number of bytes written(1).
     */
    virtual size_t write(uint8_t byte)
    {
      if (m_count == SIZE) return (0);
      m_sram.write(m_addr + m_put, &byte, sizeof(byte));
      m_count += 1;
      m_put += 1;
      if (m_put == SIZE) m_put = 0;
      return (sizeof(byte));
    }

    /**
     * @override{Stream}
     * Returns number of bytes available for read().
     * @return bytes available.
     */
    virtual int available()
    {
      return (m_count);
    }

    /**
     * @override{Stream}
     * Return next byte to read if available otherwise negative error
     * code(-1).
     * @return next byte or negative error code.
     */
    virtual int peek()
    {
      if (m_count == 0) return (-1);
      uint8_t res = 0;
      m_sram.read(&res, m_addr + m_get, sizeof(res));
      return (res);
    }

    /**
     * @override{Stream}
     * Return next byte if available otherwise negative error
     * code(-1).
     * @return next byte or negative error code.
     */
    virtual int read()
    {
      if (m_count == 0) return (-1);
      uint8_t res = 0;
      m_sram.read(&res, m_addr + m_get, sizeof(res));
      m_count -= 1;
      m_get += 1;
      if (m_get == SIZE) m_get = 0;
      return (res);
    }

    /**
     * @override{Stream}
     * Flush all data and reset stream.
     */
    virtual void flush()
    {
      m_put = 0;
      m_get = 0;
      m_count = 0;
    }

  protected:
    SRAM<SS_PIN,FREQ>& m_sram;
    uint32_t m_addr;
    uint16_t m_put;
    uint16_t m_get;
    uint16_t m_count;
  };

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
