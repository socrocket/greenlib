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


#ifndef I_DEVICE_H_
#define I_DEVICE_H_

// std
#include <iostream>

// greenreg
#include "greenreg/framework/core/gr_module.h"

namespace gs {
namespace reg {

class subdevice_container;
class gr_attribute_container;
class gr_register_container;
class gr_register_container_reference_container;
class gr_component_container;

////////////////////////////////////////////
/// I_device
/// Interface / top level class definition for the device
/// framework appart from some required higher level definitions
/// found in gr_module.
/// @author 
/// @since 
////////////////////////////////////////////
class I_device : public gr_module
{
public:

	////////////////////////////////////////////
	/// Constructor
	///
	/// @param _name ? sc_core::sc_module_name based name of the module, passed up the hierarchy for safe keeping.
	///
	/// @see gr_module,
	////////////////////////////////////////////
	I_device( sc_core::sc_module_name _name);

	////////////////////////////////////////////
	/// Destructor
	////////////////////////////////////////////
	virtual ~I_device();

	////////////////////////////////////////////
	/// get_device_attribute_container
	/// method to explicitly retrieve the attribute container, use "a" instead.
	///
	/// @return reference to gr_attribute_container for this device hierarchy
	///
	/// @see gr_attribute_container
	////////////////////////////////////////////
	virtual gr_attribute_container & get_device_attribute_container() = 0;

	////////////////////////////////////////////
	/// get_device_register_container
	/// method to explicitly retrieve the register container, use "r" instead.
	///
	/// @return reference to gr_register_container for this device hierarchy
	///
	/// @see gr_register_container
	////////////////////////////////////////////
	virtual gr_register_container & get_device_register_container() = 0;

	////////////////////////////////////////////
	/// get_device_register_container_reference_container
	/// method to explicitly retrieve the register container reference container, use "rc" instead.
	///
	/// @return reference to gr_register_container_reference_container for this device hierarchy
	///
	/// @see gr_register_container_reference_container
	////////////////////////////////////////////
	virtual gr_register_container_reference_container & get_device_register_container_reference_container() = 0;

public:

	///! reference object to internally declared subdevice_container
	subdevice_container & sd;

	///! reference object to internally declared attribute_container
	gr_attribute_container & a;

	///! reference object to internally declared component_container
	gr_component_container & c;
};

} // end namespace gs:reg
} // end namespace gs:reg

#endif /*I_DEVICE_H_*/
