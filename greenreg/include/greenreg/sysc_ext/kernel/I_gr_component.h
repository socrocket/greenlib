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


#ifndef I_GR_COMPONENT_H_
#define I_GR_COMPONENT_H_

#include "greenreg/framework/core/gr_module.h"
#include "greenreg/sysc_ext/kernel/gr_attribute_type_container.h"
#include "greenreg/sysc_ext/kernel/gr_switch_container.h"

namespace gs {
namespace reg {

////////////////////////////////////////////
/// I_gr_component
/// Interface or rather, top level definition of a component.
/// Components are functional building blocks for a model or device
/// that have all of the DRF support for config and control built in,
/// as more components are created externally, they will be brought in
/// and commbined in greenreg.  For the first set of example components,
/// @see gr_file_in, gr_file_out
/// @author 
/// @since 
////////////////////////////////////////////
class I_gr_component : public gr_module
{
public:

	////////////////////////////////////////////
	/// Constructor
	///
	/// @param _name ? sc module name of component
	////////////////////////////////////////////
	I_gr_component( sc_core::sc_module_name _name);

	////////////////////////////////////////////
	/// Destructor
	////////////////////////////////////////////
	virtual ~I_gr_component();

	/// @see I_gr_dump
	virtual void gr_dump( gr_dump_format_e _format, std::ostream & _stream, unsigned int _tab_level);

	/// attribute container for component
	gr_attribute_container a;

	/// switch container for component (should be deprecated)
	gr_switch_container s;

protected:

	/// stored string name of component
	std::string m_name;
};

} // end namespace gs:reg
} // end namespace gs:reg

#endif /*I_GR_COMPONENT_H_*/
