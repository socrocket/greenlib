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


//#define EXCLUDE_FROM_BUILD
#ifndef EXCLUDE_FROM_BUILD
#undef EXCLUDE_FROM_BUILD

#ifndef _GR_ATTRIBUTE_CONTAINER_H_
#define _GR_ATTRIBUTE_CONTAINER_H_

#include "greenreg/utils/storage/access_data_container.h"
#include "gr_attribute_base.h"
#include "greenreg/sysc_ext/kernel/gr_attribute.h"
#include "gr_switch.h"
#include "greenreg/framework/core/I_gr_dump.h"
#include "greenreg/sysc_ext/utils/gr_report.h"

namespace gs {
namespace reg {
	
////////////////////////////////////////////
/// gr_attribute_container
/// basic container for gr_attributes
/// @author 
/// @since 
////////////////////////////////////////////
class gr_attribute_container : public ::gs::reg_utils::access_data_container<gr_attrib_base>, public I_gr_dump
{
public:
	////////////////////////////////////////////
	/// constructor
	/// forces name to "attributes"
	////////////////////////////////////////////
	gr_attribute_container();
	
	////////////////////////////////////////////
	/// attrib
	/// templated method to return the correct gr_attribute type.
	/// performs some level of type checking to warn user of 
	/// improper usage, but does not prevent improper usage.
	///
	/// @param _name ? name of attribute
	/// @return attribute of type T !WARNING! invalid lookup will return bad reference
	////////////////////////////////////////////
	template< typename T>
	gr_attribute<T> & attrib( std::string _name)
	{
		gr_attrib_base * attrib_base = &((*this)[_name]);
		
		if( attrib_base->get_type() != gs::reg_utils::determine<T>::type())
		{
			GR_FORCE_WARNING( "\nYou are inproperly using gr_attribute_container::attrib<T>( std::string _name) to convert return type to non-aligned type, CRASH is HIGHLY possible!\n\n");
		}
		
		return( *(static_cast< gr_attribute< T> * >( attrib_base) ));
	}
	
	////////////////////////////////////////////
	/// _switch
	/// name chosen because of conflict with C++ keyword switch.
	/// performs type conversion to a switch (i.e. gr_attribute< int>)
	///
	/// @param _name ? name of switch to grab
	/// @return switch reference !WARNING! invalid lookup will return bad reference
	////////////////////////////////////////////
	gr_switch & _switch( std::string _name)
	{
		gr_attrib_base * attrib_base = &((*this)[_name]);
		
		if( attrib_base->get_type() != gs::reg_utils::INT)
		{
			GR_FORCE_WARNING( "\nYou are inproperly using gr_attribute_container::switch( std::string _name) to convert return type to non-aligned type, CRASH is HIGHLY possible!\n\n");
		}
		
		return( *(static_cast< gr_switch * >( attrib_base) ));
	}
	
	///@see I_gr_dump
	virtual void gr_dump( gr_dump_format_e _format, std::ostream & _stream, unsigned int _tab_level = 0);

};

} // end namespace gs::reg
} // end namespace gs::reg

#endif // _GR_ATTRIBUTE_CONTAINER_H_

#endif // EXCLUDE_FROM_BUILD 
#undef EXCLUDE_FROM_BUILD
