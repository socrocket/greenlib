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

//
// ChangeLog GreenSocs
//
// 2009-01-20 Christian Schroeder: minor changes
// 2009-01-21 Christian Schroeder: changed namespace
// 2009-01-29 Christian Schroeder: renamings
// 2009-01-29 FURTHER CHANGES SEE SVN LOG!
//


#ifndef _GREENREG_SOCKET_MONITOR_H_
#define _GREENREG_SOCKET_MONITOR_H_

#include <iostream>

#include "I_register_container_bus_access.h"

namespace gs {
namespace reg {

class greenreg_socket_monitor 
: public gs::reg::I_register_container_bus_access
{
public:
	greenreg_socket_monitor( unsigned int _base_address,
                           gs::reg::I_register_container_bus_access & _target)
	: m_target( &_target)
	, m_base_address( _base_address)
	{;}
	
	virtual ~greenreg_socket_monitor()
	{;}
	
	virtual bool bus_read( unsigned int& _data, unsigned int _address, unsigned int _byte_enable)
	{
		std::cout << std::hex << "RD A: 0x" << _address + m_base_address << std::dec << "\n";
		bool retval = m_target->bus_read( _data, _address, _byte_enable);
		std::cout << std::hex << "RD D: 0x" << _data << std::dec << "\n";
		return( retval);
	}
	
	virtual bool bus_write( unsigned int _data, unsigned int _address, unsigned int _byte_enable)
	{
		std::cout << std::hex << "WR A: 0x" << _address  + m_base_address << " D: 0x" << _data << std::dec << "\n";
		return( m_target->bus_write( _data, _address, _byte_enable));
	}
	
	gs::reg::I_register_container_bus_access * m_target;
	unsigned int m_base_address;
};
	

} // end namespace gs::reg
} // end namespace gs::reg

#endif /*_GREENREG_SOCKET_MONITOR_H_*/
