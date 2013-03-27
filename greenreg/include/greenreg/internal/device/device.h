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


#ifndef DEVICE_H_
#define DEVICE_H_

// std
#include <iostream>

// greenreg
#include "greenreg/framework/core/gr_common.h"
#include "greenreg/framework/core/I_gr_dump.h"
#include "greenreg/internal/device/I_device.h"
#include "greenreg/framework/registers/gr_register_container.h"
#include "greenreg/framework/registers/gr_register_container_reference_container.h"

namespace gs {
namespace reg {

class gr_attribute_container;
class gr_switch_container;
class device_container;

////////////////////////////////////////////
/// device
/// top level declaration for a DRF based device.
/// @author 
/// @since 
////////////////////////////////////////////
class device : public I_device
{
public:

	////////////////////////////////////////////
	/// Constructor
	///
	/// @param _name ? sc module name ( at user level )
	/// @param _dword_size ? dword size of register block (i.e. # of 32bit segments)
	/// @param _parent ? pointer to parent device so that this device can add itself to the parent container, set to NULL for default configuration
	///
	/// @see gr_register_container
	////////////////////////////////////////////
	device( sc_core::sc_module_name _name, register_container_addressing_mode_e _address_mode, unsigned int _dword_size, device * _parent);

	////////////////////////////////////////////
	/// Destructor
	////////////////////////////////////////////
	virtual ~device();

	/// @see I_gr_dump
	virtual void gr_dump( gr_dump_format_e _format, std::ostream & _stream, unsigned int _tab_level = 0);

	/// @see I_device
	virtual gr_attribute_container & get_device_attribute_container()
	{ return a; }

	/// @see I_device
	virtual gr_register_container & get_device_register_container()
	{ return r; }

	/// @see I_device
	virtual gr_register_container_reference_container & get_device_register_container_reference_container()
	{ return rc; }

public:

	/// reference container for register containers (in the case one has multiple containers)
	gr_register_container_reference_container rc;

	/// primary register container block for this device hierarchy
	gr_register_container r;

	/// used to navigate to parent device in the cases where this hierarchy is needed
	device * m_parent;

	/// allows device hierarchy in conjunction with subdevice hierarchy for combining register block level IP
	device_container & d;
};

} // end namespace gs:reg
} // end namespace gs:reg

#endif /*DEVICE_H_*/
