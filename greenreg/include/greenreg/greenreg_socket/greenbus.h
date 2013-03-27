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


//
//
// WARNING: this is an old file! 
//
//    Use the greenreg_socket instead!
//
//


#ifndef GREENBUS_H_
#define GREENBUS_H_

#warning greenbus.h is an old file. Use the greenreg_socket instead!

#include "greenreg/greenreg.h"
#include "transactor_if.h"

// old greenbus
#include "gstlm/tlm.h"
#include "protocol/generic.h"
#include "userAPI/basicPorts.h"
#include "gstlm/tlm_slave_if.h"

// new greenbus
//#include "greenbus/core/tlm.h"
//#include "greenbus/transport/generic.h"
//#include "greenbus/api/basicPorts.h"

namespace GREENBUS
{

	class slave_base
	{
	public:
		slave_base(	gs::reg::I_register_container_bus_access * _registers,
					gr_uint_t _base_addr,
					gr_uint_t _decode_size
					)
		: m_registers( _registers),
		m_decode_size( _decode_size),
		m_base_addr( _base_addr)
		{
		}

		virtual ~slave_base()
		{}

		inline gr_uint_t get_decode_size() { return m_decode_size; }
		
	protected:
		bool write_to_module( unsigned int _data, unsigned int _address, unsigned int _byte_enable)
		{
			return( m_registers->bus_write( _data, _address, _byte_enable));
		}

		unsigned int read_from_module( unsigned int& _data, unsigned int _address, unsigned int _byte_enable)
		{
			return( m_registers->bus_read( _data, _address, _byte_enable));
		}

		gs::reg::I_register_container_bus_access * m_registers;
		gr_uint_t m_decode_size;

	private:
		gr_uint_t m_base_addr;
	};

	class generic_slave 
	: public sc_core::sc_module
	, public slave_base
	, public ::tlm::tlm_b_if< ::tlm::GenericTransaction_P>
	, public ::tlm::tlm_slave_if< ::tlm::MAddr>
	{
	public:

		SC_HAS_PROCESS( generic_slave);

		generic_slave( sc_core::sc_module_name _name, gs::reg::I_register_container_bus_access & _reg_bind, gr_uint_t _base_address, gr_uint_t _decode_size) :
			sc_core::sc_module( _name),
			slave_base( &_reg_bind, _base_address, _decode_size),
			m_slave_port( "slave_port"),
			m_base( _base_address),
			m_high( _base_address + _decode_size)
		{
			m_slave_port.bind_b_if( *this);
		}

		virtual ~generic_slave()
		{}

		/// this method does not care about time
		void b_transact( ::tlm::GenericTransaction_P _transaction)
		{
			stimulate_model( _transaction);
		}

		// tlm_slave_if implementation
		virtual void setAddress(::tlm::MAddr  base, ::tlm::MAddr  high)
		{
			  m_base =base;
			  m_high =high;
		}
		
		virtual void getAddress(::tlm::MAddr& base, ::tlm::MAddr& high)
		{
			  base =m_base;
			  high =m_high;
		}
		
		/// this method is timing agnostic
		int stimulate_model( ::tlm::GenericTargetPort::accessHandle _transaction)
		{
			if( _transaction->get_mCmd() == ::tlm::Generic_MCMD_RD)
			{
				unsigned int address = static_cast<unsigned int>(_transaction->get_mAddr()) - m_base;

				// re-creates byte enables.  Assumes addresses are correctly aligned
				unsigned int length = _transaction->get_mBurstLength();
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

				
				m_registers->bus_read( m_bus_read_data, address, mask);
				_transaction->set_sData(
					::tlm::MasterDataType(
						reinterpret_cast<unsigned char *>(&m_bus_read_data),
						_transaction->get_mBurstLength()));

			} else if( _transaction->get_mCmd() == ::tlm::Generic_MCMD_WR)
			{
				const ::tlm::MasterDataType& bus_data(_transaction->get_mData());
				::tlm::MasterDataType& unlocked_bus_data(const_cast< ::tlm::MasterDataType& >(bus_data));
				unsigned int data = *(reinterpret_cast<const unsigned int* >(unlocked_bus_data.get()));
				unsigned int address = static_cast<unsigned int>(_transaction->get_mAddr()) - m_base;

				// re-creates byte enables.  Assumes addresses are correctly aligned
				unsigned int length = _transaction->get_mBurstLength();
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

				
				m_registers->bus_write( data, address, mask);
			} else
			{
			}

			return( 1);
		}

	protected:
		::tlm::GenericTargetPort m_slave_port;
		::tlm::MAddr	m_base;
		::tlm::MAddr	m_high;
		
	private:
		unsigned int m_bus_read_data;
	};

	class generic_master : public sc_core::sc_module, public tlm_components::transactor_if
	{
	public:

		SC_HAS_PROCESS( generic_master);

		generic_master( sc_core::sc_module_name _name) :
		sc_core::sc_module( _name),
		m_master_port( "master_port")
		{
		    transaction = m_master_port.create_transaction();
		}

		virtual ~generic_master()
		{}

		// user methods - using default implementation in transactor_if
//		virtual unsigned int read( unsigned int _address, unsigned int _width);
//		virtual void write(unsigned int _address, unsigned int _data, unsigned int _width);

        virtual void random_read()
        {}

        virtual void random_write()
        {}

		::tlm::GenericMasterPort m_master_port;

	protected:
		::tlm::GenericMasterPort::accessHandle transaction;
		
		// transactor methods
		virtual void _read(unsigned _address, unsigned _length, unsigned int* _db, bool _bytes_enabled, unsigned int* _be)
		{
//		    ::tlm::GenericMasterPort::accessHandle transaction = m_master_port.create_transaction();

			// fix because greenbus does not overwrite data block cleanly (even though the back end does)
			*_db = 0;
			
		    transaction->set_mCmd( ::tlm::Generic_MCMD_RD);
		    transaction->set_mAddr( _address);
		    transaction->set_mBurstLength( _length);
		    transaction->set_mData(
		    	::tlm::MasterDataType( reinterpret_cast< unsigned char *>( _db), _length));

		    m_master_port.Transact( transaction);
		}

		virtual void _write(unsigned _address, unsigned _length, unsigned int* _db, bool _bytes_enabled, unsigned int* _be)
		{
//		    ::tlm::GenericMasterPort::accessHandle transaction = m_master_port.create_transaction();

		    transaction->set_mCmd( ::tlm::Generic_MCMD_WR);
		    transaction->set_mAddr( _address);
		    transaction->set_mBurstLength( _length);
		    transaction->set_mData(
		    	::tlm::MasterDataType( reinterpret_cast< unsigned char *>( _db), _length));


		    m_master_port.Transact( transaction);
		}

	private:
	};
}

#endif /*GREENBUS_H_*/
