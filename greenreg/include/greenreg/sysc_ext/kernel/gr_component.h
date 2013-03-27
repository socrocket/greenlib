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


#ifndef _GR_COMPONENT_H_
#define _GR_COMPONENT_H_

#include <boost/config.hpp>
#include <systemc>

#include "I_gr_component.h"

namespace gs {
namespace reg {

////////////////////////////////////////////
/// gr_component
/// user class for building components, declares
/// process of template type.  One should use this
/// because it automatically supports attributes
/// and switches throught the control and config API's
/// by inheriting from I_gr_component.
/// @author 
/// @since 
////////////////////////////////////////////
template< class T>
class gr_component : public I_gr_component
{
public:
	SC_HAS_PROCESS(T);

	////////////////////////////////////////////
	/// Constructor
	///
	/// @param _name ? sc_name of component
	/// @param _component_container ? container to register with
	///
	/// @see I_gr_component
	////////////////////////////////////////////
	gr_component( sc_core::sc_module_name _name, gr_component_container & _component_container)
	: I_gr_component( _name)
	{
			_component_container.add_key_data_pair( (const char *)(_name), *this);
	}

	////////////////////////////////////////////
	/// Destructor
	////////////////////////////////////////////
	virtual ~gr_component()
	{;}
};

} // end namespace gs:reg
} // end namespace gs:reg

#endif /*_GR_COMPONENT_H_*/
