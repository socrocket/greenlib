//   GreenReg framework
//
// LICENSETEXT
//
//   Copyright (C) 2008-2009 : GreenSocs Ltd
//    http://www.greensocs.com/ , email: info@greensocs.com
//
//   Developed by :
//   
//
//
//   This program is free software.
// 
//   If you have no applicable agreement with GreenSocs Ltd, this software
//   is licensed to you, and you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
// 
//   If you have a applicable agreement with GreenSocs Ltd, the terms of that
//   agreement prevail.
// 
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
// 
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
//   02110-1301  USA 
// 
// ENDLICENSETEXT

#ifndef _GR_GSGPSOCKET_H_
#define _GR_GSGPSOCKET_H_

#include "greenreg/greenreg.h"
#include "transactor_if.h"

#include "gsgpsocket/transport/GSGPSlaveSocket.h"
#include "gsgpsocket/transport/GSGPMasterBlockingSocket.h"


// greensocket

namespace gs {
namespace gp {

  class generic_slave_base
  {
  public:
    generic_slave_base(gs::reg::I_register_container_bus_access * _registers,
                       gr_uint_t _base_addr,
                       gr_uint_t _decode_size )
    : m_registers( _registers)
    , m_decode_size( _decode_size)
    , m_base_addr( _base_addr)
    , m_delay_enabled(true) // enabled by default
    {
    }

    virtual ~generic_slave_base()
    {}

    inline gr_uint_t get_decode_size() { return m_decode_size; }
    
    /// Disables the delay for all notification rule callbacks caused by this socket
    void disable_delay() { m_delay_enabled = false; }
    /// Enables the delay for all notification rule callbacks caused by this socket
    void enable_delay() { m_delay_enabled = true; }
    /// Returns if the delay is enabled
    bool delay_enabled() {return m_delay_enabled; }
    
  protected:
    bool write_to_module( unsigned int _data, unsigned int _address, unsigned int _byte_enable, gs::reg::transaction_type* _transaction, bool _delayed) {
      return( m_registers->bus_write( _data, _address, _byte_enable, _transaction, _delayed));
    }

    unsigned int read_from_module( unsigned int& _data, unsigned int _address, unsigned int _byte_enable, gs::reg::transaction_type* _transaction, bool _delayed) {
      return( m_registers->bus_read( _data, _address, _byte_enable, _transaction, _delayed));
    }

    gs::reg::I_register_container_bus_access * m_registers;
    gr_uint_t m_decode_size;

  protected:
    gr_uint_t m_base_addr;
    
    /// If this socket let the caused notification rule callbacks be delayed
    bool m_delay_enabled;
    
  };

  class generic_slave 
  //: public sc_core::sc_module
  : public gs::gp::GenericSlavePort<32>
  , public generic_slave_base
  , public gs::tlm_b_if<gs::gp::GenericSlaveAccessHandle>
  , public gs::payload_event_queue_output_if<gs::gp::slave_atom>
  {
  public:

    typedef gs::gp::GenericSlavePort<32>::accessHandle accessHandle_slave;
    typedef gs::gp::GenericSlavePort<32>::phase phase_slave;
    typedef gs::gp::GenericSlavePort<32>::virtual_base_type virtual_base_type;

    SC_HAS_PROCESS( generic_slave);

    generic_slave( sc_core::sc_module_name _name, gs::reg::I_register_container_bus_access & _reg_bind, gr_uint_t _base_address, gr_uint_t _decode_size)
    //: sc_core::sc_module( _name)
    : virtual_base_type(_name/*"slave_port"*/, GSGP_SLAVE_SOCKET_TYPE_NAME)
    , gs::gp::GenericSlavePort<32>( _name/*"slave_port"*/)
    , generic_slave_base( &_reg_bind, _base_address, _decode_size)
    , m_base( _base_address)
    , m_high( _base_address + _decode_size)
    {
      gs::gp::GenericSlavePort<32>::bind_b_if( *this);
      gs::gp::GenericSlavePort<32>::peq.out_port(*this);
      
      gs::gp::GenericSlavePort<32>::base_addr = _base_address;
      gs::gp::GenericSlavePort<32>::high_addr = _base_address + _decode_size;
    }

    generic_slave( sc_core::sc_module_name _name, gs::reg::I_register_container_bus_access & _reg_bind)
    //: sc_core::sc_module( _name)
    : virtual_base_type(_name/*"slave_port"*/, GSGP_SLAVE_SOCKET_TYPE_NAME)
    , gs::gp::GenericSlavePort<32>( _name/*"slave_port"*/)
    , generic_slave_base( &_reg_bind, base_addr, high_addr - base_addr)
    , m_base( base_addr)
    , m_high( high_addr)
    {
      gs::gp::GenericSlavePort<32>::bind_b_if( *this);
      gs::gp::GenericSlavePort<32>::peq.out_port(*this);
      
//       gs::gp::GenericSlavePort<32>::base_addr = _base_address;
//       gs::gp::GenericSlavePort<32>::high_addr = _base_address + _decode_size;
      if (high_addr == 0) {
        std::stringstream ss;
        ss << "check parameter value for " << base_addr.getName() << "=" << base_addr << " and "
           << high_addr.getName() << "=" << high_addr << ". They should be initialized already.";
        GR_FATAL(ss.str().c_str());
      }
    }

    virtual ~generic_slave()
    {}

    /// this method does not care about time
    void b_transact(gs::gp::GenericSlaveAccessHandle _transaction) {
      stimulate_model( _transaction);
    }

    // tlm_slave_if implementation
    virtual void setAddress(sc_dt::uint64 base, sc_dt::uint64 high) {
      m_base =base;
      m_high =high;

      gs::gp::GenericSlavePort<32>::base_addr = base;
      gs::gp::GenericSlavePort<32>::high_addr = high;        
    }
    
    virtual void getAddress(sc_dt::uint64& base, sc_dt::uint64& high) {
      base =m_base;
      high =m_high;
    }

    void notify(gs::gp::slave_atom& tc) {
    }    
    
    /// this method is timing agnostic
    int stimulate_model(accessHandle_slave _transaction) {
      if( _transaction->getMCmd() == gs::Generic_MCMD_RD) {
        unsigned int address = static_cast<unsigned int>(_transaction->getMAddr()) - m_base;

        // re-creates byte enables.  Assumes addresses are correctly aligned
        unsigned int length = _transaction->getMBurstLength();
        unsigned int mask;
        switch(length) {
          case 1:
            mask = 0x1;
            break;
          case 2:
            mask = 0x3;
            break;
          case 3:
            mask = 0x7;
            break;
          case 4:
          default:
            mask = 0xF;
        }

        m_registers->bus_read(m_bus_read_data, address, mask, &_transaction,
                              m_delay_enabled);
        gs::GSDataType::dtype tmp((unsigned char *)(&m_bus_read_data),
                                  _transaction->getMBurstLength());
        gs::MData mdata(tmp);
        _transaction->setSData(mdata);
      } else if( _transaction->getMCmd() == gs::Generic_MCMD_WR) {
        unsigned int data;
        unsigned int address = static_cast<unsigned int>(_transaction->getMAddr()) - m_base;        
        unsigned int length = _transaction->getMBurstLength();
        
        memcpy(&data, &(_transaction->getMData()[0]), length);
        
        unsigned int mask; 
        switch(length)
        {
          case 1:
            mask = 0x1;
            break;
          case 2:
            mask = 0x3;
            break;
          case 3:
            mask = 0x7;
            break;
          case 4:
          default:
            mask = 0xF;
        }
        
        m_registers->bus_write( data, address, mask, &_transaction, m_delay_enabled);
      } else {
      }

      return( 1);
    }

  protected:
    sc_dt::uint64  m_base;
    sc_dt::uint64  m_high;
    
  private:
    unsigned int m_bus_read_data;
  };

  
  
  class generic_master
  //: public sc_core::sc_module
  : public tlm_components::transactor_if
  , public gs::payload_event_queue_output_if<gs::gp::master_atom>
  , public gs::gp::GenericMasterBlockingPort<32>
  {
  public:

    typedef gs::gp::GenericMasterBlockingPort<32>::accessHandle transactionHandle_master;
    typedef gs::gp::GenericMasterBlockingPort<32>::phase phase_master;
    typedef gs::gp::GenericMasterBlockingPort<32>::virtual_base_type virtual_base_type;

    SC_HAS_PROCESS( generic_master);

    generic_master( sc_core::sc_module_name _name)
    //: sc_core::sc_module( _name)
    : virtual_base_type( _name/*"master_port"*/, GSGP_MASTER_SOCKET_TYPE_NAME)
    , gs::gp::GenericMasterBlockingPort<32>( _name/*"master_port"*/)
    {
      gs::gp::GenericMasterBlockingPort<32>::out_port(*this);
    }

    virtual ~generic_master()
    {}

    // user methods - using default implementation in transactor_if
//    virtual unsigned int read( unsigned int _address, unsigned int _width);
//    virtual void write(unsigned int _address, unsigned int _data, unsigned int _width);

    virtual void random_read()
    {}

    virtual void random_write()
    {}

    virtual void notify(gs::gp::master_atom &tc)
    {}

  protected:
     transactionHandle_master transaction;
    
    // transactor methods
    virtual void _read(unsigned _address, unsigned _length, unsigned int* _db, bool _bytes_enabled, unsigned int* _be) {
      transaction = gs::gp::GenericMasterBlockingPort<32>::create_transaction();
      // fix because greenbus does not overwrite data block cleanly (even though the back end does)
      *_db = 0;

      transaction->setMCmd(gs::Generic_MCMD_RD);
      transaction->setMAddr( _address);
      transaction->setMBurstLength( _length);
      gs::MData mdata(gs::GSDataType::dtype((unsigned char *)_db, _length));
      transaction->setMData(mdata);

      gs::gp::GenericMasterBlockingPort<32>::Transact( transaction);
      
      gs::gp::GenericMasterBlockingPort<32>::release_transaction(transaction);
    }

    virtual void _write(unsigned _address, unsigned _length, unsigned int* _db, bool _bytes_enabled, unsigned int* _be) {
      transaction = gs::gp::GenericMasterBlockingPort<32>::create_transaction();
      
      transaction->setMCmd(gs::Generic_MCMD_WR);
      transaction->setMAddr( _address);
      transaction->setMBurstLength( _length);
      gs::MData mdata(gs::GSDataType::dtype((unsigned char *)_db, _length));
      transaction->setMData(mdata);
//        transaction->set_mData(::tlm::MasterDataType( reinterpret_cast< unsigned char *>( _db), _length));

      gs::gp::GenericMasterBlockingPort<32>::Transact( transaction);
      
      gs::gp::GenericMasterBlockingPort<32>::release_transaction(transaction);
    }

  private:
  };

} // end namespace gs::gp
} // end namespace gs::gp

#endif /*_GSGPSOCKET_H_*/
