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


#ifndef SUBDEVICE_H_
#define SUBDEVICE_H_

// std
#include <iostream>

// GREENREG_UTILS
#include "greenreg/utils/storage/access_data_container_ref.h"
#include "greenreg/utils/storage/addressable_container_ref.h"

// greenreg
#include "greenreg/gr_externs.h"
#include "greenreg/framework/core/gr_common.h"
#include "greenreg/framework/core/I_gr_dump.h"
#include "greenreg/internal/device/I_device.h"
#include "greenreg/sysc_ext/kernel/gr_attribute_type_container.h"
#include "greenreg/sysc_ext/kernel/gr_switch_container.h"
#include "greenreg/framework/registers/gr_register_container.h"
#include "greenreg/framework/registers/gr_register_container_reference_container.h"

namespace gs {
namespace reg {

////////////////////////////////////////////
/// subdevice
/// top+N level declaration of sub-blocks for a DRF based device.
/// May not be created stand alone, must be associated with a parent or "owner"
/// @author 
/// @since 
////////////////////////////////////////////
class subdevice : public I_device
{
public:

	////////////////////////////////////////////
	/// Constructor
	/// Forces subdevice to know it's owning parent
	/// upon instantiation.
	///
	/// @param _name ? sc module name ( at user level )
	/// @param _parent ? parent is the next level up in logic ownership, and may either be a device or subdevice
	///
	/// @see I_device
	////////////////////////////////////////////
	subdevice( sc_core::sc_module_name _name, I_device & _parent);

	////////////////////////////////////////////
	/// Destructor
	////////////////////////////////////////////
	virtual ~subdevice();

	/// @see I_gr_dump
	virtual void gr_dump( gr_dump_format_e _format, std::ostream & _stream, unsigned int _tab_level = 0);

	/// @see I_device
	virtual gr_attribute_container & get_device_attribute_container()
	{ return *static_cast< gr_attribute_container *>( da.get_container()); }

	/// @see I_device
	virtual gr_register_container & get_device_register_container()
	{ return *static_cast< gr_register_container *>( r.get_container()); }

	/// @see I_device
	virtual gr_register_container_reference_container & get_device_register_container_reference_container()
	{ return *static_cast< gr_register_container_reference_container *>( rc.get_container()); }

public:

	///! reference to parent
	I_device & m_parent;

	///! bounded class reference to device attributes (should this be a, and subdevice attributes be la (local attribs))?
	gs::reg_utils::access_data_container_ref< gr_attrib_base> da;

	///! bounded class reference to device registers
	gs::reg_utils::addressable_container_ref<  uint_gr_t, I_register> r;

	///! bounded class reference to device attributes (should this be a, and subdevice attributes be la (local attribs))?
	gs::reg_utils::access_data_container_ref< gr_register_container> rc;
};

} // end namespace gs:reg
} // end namespace gs:reg

#endif /*SUBDEVICE_H_*/
