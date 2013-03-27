/*
Copyright (c) 2008, Intel Corporation
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.
    * Neither the name of Intel Corporation nor the names of its contributors
      may be used to endorse or promote products derived from this software
      without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

// ChangeLog GreenSocs
// 2009-01-21 Christian Schroeder: changed namespace
// 2009-01-29 Christian Schroeder: renamings
// 2009-01-29 FURTHER CHANGES SEE SVN LOG!


/* ========================================================================================
 * File:     read_modify_write_adapter.h
 * Project:    DRF_PORT
 * Date:    
 * Purpose:    Provide adapter based on I_register_container_bus_access interface for
 *        connecting byte-address to word-aligned address targets
 * Comments:
 *
 *
 * Authors:      
 * Creation Date:  
 *
 * @author $Author$
 *
 * @version $Revision$
 *
 * @date $Date$
 *
 * ========================================================================================
 * Additional Information:
 *
 *
 */


#include <iostream>


#include "I_register_container_bus_access.h"


#ifndef READ_MODIFY_WRITE_ADAPTER_H_
#define READ_MODIFY_WRITE_ADAPTER_H_

namespace gs {
namespace reg {

  class read_modify_write_adapter
  : public gs::reg::I_register_container_bus_access
  {
  public:
    
    read_modify_write_adapter(gs::reg::I_register_container_bus_access& _reg_bind);
    virtual ~read_modify_write_adapter();
    

    virtual bool bus_read( unsigned int& _data, unsigned int _address, unsigned int _byte_enable, transaction_type* _transaction = NULL, bool _delayed = true);
    virtual bool bus_write( unsigned int _data, unsigned int _address, unsigned int _byte_enable, transaction_type* _transaction = NULL, bool _delayed = true);    
    
  protected:
    unsigned int be2length(unsigned int _byte_enable);
    unsigned int word2byte_data(unsigned int _word_data,
        unsigned int _addr_offset);
    unsigned int byte2word_data(unsigned int _byte_data,
        unsigned int _byte_enable);
    unsigned int byte2word_address(unsigned int _byte_address);
    unsigned int byte_address2offset(unsigned int _byte_address);
    unsigned int byte_enable2mask(unsigned int _byte_enable);  
    
    void report_unhandled_be(unsigned int _byte_enable);
    
    
    gs::reg::I_register_container_bus_access& m_register_access;
    
    std::ostringstream msg_gen;
    
  };
  
  
  inline read_modify_write_adapter::read_modify_write_adapter(gs::reg::I_register_container_bus_access& _reg_bind) 
  : m_register_access(_reg_bind)
  {
  }
  
  inline read_modify_write_adapter::~read_modify_write_adapter()
  {
  }


  
  inline bool read_modify_write_adapter::bus_read( unsigned int& _data, unsigned int _address, unsigned int _byte_enable, transaction_type* _transaction, bool _delayed)
  {
    unsigned int word_data;
    unsigned int word_address = byte2word_address(_address);
    unsigned int offset = byte_address2offset(_address);
    const unsigned int READ_BYTE_ENABLE = 0xF;
    bool status = m_register_access.bus_read(word_data, word_address, READ_BYTE_ENABLE, _transaction, _delayed);
    _data = word2byte_data(word_data, offset);
    return status;
  }
  
  

  inline bool read_modify_write_adapter::bus_write( unsigned int _data, unsigned int _address, unsigned int _byte_enable, transaction_type* _transaction, bool _delayed)
  {
    unsigned int read_data;
    unsigned int word_address = byte2word_address(_address);
    unsigned int offset = byte_address2offset(_address);
    const unsigned int BYTE_ENABLE = 0xF;
    bool status = m_register_access.bus_read(read_data, word_address, BYTE_ENABLE, _transaction, _delayed); // TODO CS: is it correct to forward _transaction and _delay here?
    unsigned int data_mask = byte_enable2mask(_byte_enable) << (offset*8);
    unsigned int word_data = _data << (offset*8);
    unsigned int write_data = (read_data & (~data_mask)) | (word_data & data_mask);  // merge old & new data using masking
    
    if (!m_register_access.bus_write(write_data, word_address, BYTE_ENABLE, _transaction, _delayed))  // only overwrite status if false
    {
      status = false;
    }

    return status;    
  }
  
  

  inline unsigned int read_modify_write_adapter::be2length(unsigned int _byte_enable) {
    switch (_byte_enable) {
    case 0x1:
      return 1;

    case 0x3:
      return 2;

    case 0xf:
      return 4;

    default:
      report_unhandled_be(_byte_enable);
      return 0;
    }
  }

  
  inline unsigned int read_modify_write_adapter::word2byte_data(unsigned int _word_data,
      unsigned int _addr_offset) 
  {
    return _word_data >> (_addr_offset*8);
  }

  inline unsigned int read_modify_write_adapter::byte2word_data(unsigned int _byte_data,
      unsigned int _byte_enable) {
    switch (_byte_enable) {
    case 0x1:
    case 0x3:
    case 0xF:
      return _byte_data;
    case 0x2:
      return _byte_data << 1*8;
    case 0x4:
    case 0xC:
      return _byte_data << 2*8;
    case 0x8:
      return _byte_data << 3*8;
    default:
      report_unhandled_be(_byte_enable);
    }
    return 0xFFFFFFFF;
  }
  
  
  
  inline unsigned int read_modify_write_adapter::byte2word_address(unsigned int _byte_address)
  {
    return _byte_address & ~(0x3);
  }

  inline unsigned int read_modify_write_adapter::byte_address2offset(unsigned int _byte_address)
  {
    return _byte_address & 0x3;
  }
  
  

  inline unsigned int read_modify_write_adapter::byte_enable2mask(unsigned int _byte_enable) {
    switch (_byte_enable) {
    case 0x1:
      return 0x000000FF;
    case 0x2:
      return 0x0000FF00;
    case 0x3:
      return 0x0000FFFF;
    case 0x4:
      return 0x00FF0000;
    case 0x7:
      return 0x00FFFFFF;
    case 0x8:
      return 0xFF000000;
    case 0xC:
      return 0xFFFF0000;
    case 0xF:
      return 0xFFFFFFFF;
    default:
      report_unhandled_be(_byte_enable);
    }
    return 0xFFFFFFFF;
  }
  
  
  
  inline void read_modify_write_adapter::report_unhandled_be(unsigned int _byte_enable)
  {
    msg_gen.str("");
      msg_gen << "unsupported byte enable: ";
      msg_gen << " be = 0x" << _byte_enable;
      GR_REPORT_ERROR("read_modify_write_adapter", msg_gen.str().c_str());
  }  


} // end namespace gs:reg
} // end namespace gs:reg


#endif /*READ_MODIFY_WRITE_ADAPTER_H_*/
