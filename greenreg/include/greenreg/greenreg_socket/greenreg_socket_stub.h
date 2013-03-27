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


#ifndef __GREENREG_SOCKET_STUB_H__
#define __GREENREG_SOCKET_STUB_H__

#include <sstream>

#include <boost/config.hpp>
#include <systemc>

#include "greenreg/greenreg_socket.h"
#include "gr_gsgpsocket.h"
#include "greenreg/greenreg.h"

namespace gs
{
	namespace reg
	{

		typedef gs::reg::I_register_container_bus_access memory_space_type;

		template< typename PROTOCOL>
		class greenreg_socket_stub;

		/// greenreg_socket_stub: A greenreg port class
		template<>
		class greenreg_socket_stub<gs::gp::generic_slave>
		: public gs::gp::generic_slave
		, public I_register_container_bus_access
		{
		public:

			greenreg_socket_stub(sc_core::sc_module_name _name, std::string _bus, gr_uint_t _start_address, gr_uint_t _end_address, bool _real_stub_device = false, bool _print = true)
			: gs::gp::generic_slave::virtual_base_type( _name, GSGP_SLAVE_SOCKET_TYPE_NAME)
			, gs::gp::generic_slave( _name, *this, _start_address, _end_address - _start_address)
			, m_start_address( _start_address)
			, m_real_stub_device( _real_stub_device)
			, m_print( _print)
			, m_bus( _bus)
			{
			}

			gs::gp::GenericSlavePort<32>& operator()()
			{
				return *this;
			}

			gs::gp::GenericSlavePort<32>& get_bus_port()
			{
				return *this;
			}

			inline virtual bool bus_write( unsigned int _data, unsigned int _address, unsigned int _byte_enable, gs::reg::transaction_type* _transaction = NULL, bool _delayed = true)
			{
				bool retval = false;
				std::stringstream ss;

				if( m_print)
				{
					ss << std::hex;

					if( m_real_stub_device)
					{
						ss << this->basename() << ":\t\t";
						retval = true;
					}
					else
					{
						ss << "ERROR: invalid access: ";
					}

					ss << "BUS: " << m_bus << ", WR (ignored), A: 0x" << (m_start_address + _address) << " D: 0x" << _data << "\r\n";
					std::cout << ss.str().c_str();
				}

				return( retval);
			}

			inline virtual bool bus_read( unsigned int & _data, unsigned int _address, unsigned int _byte_enable, gs::reg::transaction_type* _transaction = NULL, bool _delayed = true)
			{
				// force the return data to 0
				_data = 0xffffffff;
				bool retval = false;
				std::stringstream ss;

				if( m_print)
				{
					ss << std::hex;

					if( m_real_stub_device)
					{
						ss << this->basename() << ":\t\t";
						retval = true;
					}
					else
					{
						ss << "ERROR: invalid access: ";
					}

					ss << "BUS: " << m_bus << ", RD (forced), A: 0x" << (m_start_address + _address) << " D: 0x" << _data << "\r\n";
					std::cout << ss.str().c_str();
				}

				return( retval);
			}

		protected:
			unsigned int m_start_address;
			bool m_real_stub_device;
			bool m_print;
			std::string m_bus;
		};

		template<>
		class greenreg_socket_stub<gs::gp::generic_master>: public gs::gp::generic_master
		{
		public:

			greenreg_socket_stub(sc_core::sc_module_name _name):
			gs::gp::generic_master::virtual_base_type(_name, GSGP_MASTER_SOCKET_TYPE_NAME),
			gs::gp::generic_master( _name)
			{
			}

			gs::gp::GenericMasterBlockingPort<32>& operator()()
			{
				return *this;
			}

			//! get_bus_port {enables port binding to bus}
			gs::gp::GenericMasterBlockingPort<32>& get_bus_port()
			{
				return *this;
			}

			//! operator ->() {enables high level API invokeaction}
			sc_core::sc_export<tlm_components::transactor_if>& operator->()
			{
				return gs::gp::generic_master::m_in_port;
			}
		};

	} // end namespace gs:reg
} // end namespace gs:reg

#endif /*__GREENREG_SOCKET_STUB_H__*/
