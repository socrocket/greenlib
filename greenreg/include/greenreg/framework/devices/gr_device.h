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


#ifndef _GR_DEVICE_H_
#define _GR_DEVICE_H_

#include <boost/config.hpp>
#include <systemc>

#include "greenreg/framework/devices/gr_device_container.h"
#include "greenreg/sysc_ext/kernel/gr_attribute_type_container.h"
#include "greenreg/sysc_ext/kernel/gr_switch_container.h"

namespace gs {
namespace reg {

////////////////////////////////////////////
/// gr_device
/// top level device for a module
/// @author 
/// @since 
////////////////////////////////////////////
//template< class USER_MODULE>
class gr_device : public device
{
public:

// allow gr_device to define it's own processes
//	SC_HAS_PROCESS( USER_MODULE);

	////////////////////////////////////////////
	/// gr_device
	/// explicit constructor for a device
	///
	/// @param _name ? name of module
	/// @param _address_mode ? addressing mode of the register container, ALIGNED_ADDRESS for strict byte alignment, INDEXED_ADDRESS for adhoch alignment
	/// @param _dword_size ? ALIGNED_ADDRESS - dword size of register block (i.e. # of 32bit segments), INDEXED_ADDRESS - number of registers (regardless of size)
	/// @param _parent ? pointer to parent device so that this device can add itself to the parent container, set to NULL for default configuration
	///
	////////////////////////////////////////////
	explicit gr_device( sc_core::sc_module_name _name, register_container_addressing_mode_e _address_mode, unsigned int _dword_size, device * _parent)
	: device( _name, _address_mode, _dword_size, _parent)
	{
	}

	virtual ~gr_device()
	{
	}

	////////////////////////////////////////////
	/// kind
	/// implements kind for the user...
	///
	/// @return string container full templated device name
	///
	/// @see sc_object
	////////////////////////////////////////////
	virtual const char * kind() const
	{
		std::stringstream ss;
		ss << "gr_device<" << basename() << ">";
		return( ss.str().c_str()) ;
	}

// data members
public:

protected:
};

} // end namespace gs:reg
} // end namespace gs:reg

#endif /*_GR_DEVICE_H_*/
