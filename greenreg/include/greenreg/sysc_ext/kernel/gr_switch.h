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

#ifndef _GR_SWITCH_H_
#define _GR_SWITCH_H_

#include "greenreg/utils/storage/option_list.h"

#include "greenreg/sysc_ext/kernel/gr_attribute.h"

namespace gs {
namespace reg {

class gr_switch_container;

////////////////////////////////////////////
/// gr_switch
/// a functional "switch" that exposes itself
/// and it's options to the outside world.
///
/// @author 
/// @since 
////////////////////////////////////////////
class gr_switch : public gr_attribute< int>, public gs::reg_utils::option_list
{
public:

	////////////////////////////////////////////
	/// constructor
	/// registers with switch container
	/// * dynamic option list
	///
	/// warning: if you do not provide any options
	///			 then you will be unable to set
	///			 the value. (use gr_attribute instead)
	///		 because... what use is a switch without options?
	///
	/// @param _name - of switch
	/// @param _switch_container ? forced registration
	////////////////////////////////////////////
	gr_switch( std::string _name, const std::string _description, gr_attribute_container & _switch_container, unsigned int _default_value);

	////////////////////////////////////////////
	/// constructor
	/// registers with switch container
	/// static option list
	///
	/// @param _name - of switch
	/// @param _switch_contianer - forced registration
	////////////////////////////////////////////
	gr_switch( std::string _name, const std::string _description, gr_attribute_container & _switch_container, gs::reg_utils::option_list_t & _options, unsigned int _default_value);
	
	virtual ~gr_switch();

	////////////////////////////////////////////
	/// set_value
	/// performs the actual set value and event
	/// notification of change.
	///
	/// * event notification is forced
	///
	/// Method replace gr_attribute implementation
	/// to validate type option.
	///
	/// @param _value = value to set
	/// @return if value was in valid "range" (not applicable in gr_attribute)
	/// @see gr_switch
	////////////////////////////////////////////
	virtual bool set_value( const int& _value);
	
	/// @see gr_attribute & gr_attribute_base
	inline virtual gr_switch & operator = ( const int& _value)
	{
		set_value( _value);
		return( *this);
	}

	/// @see sc_object
	virtual const char * kind() const
	{ 
		std::stringstream ss;
		ss << "gr_switch<" << gs::reg_utils::get_type_string( m_type) << ">";
		return( ss.str().c_str()) ;
	}
	
	/// @see gs::reg_utils::option_list
	virtual bool add_entry( std::string _key, unsigned int _value, bool _override = false);

	////////////////////////////////////////////
	/// get_switch_event
	/// returns the event associated with the key
	/// the key better exist...
	///
	/// @param _key ? string key
	/// @return event associated with _key
	////////////////////////////////////////////
	sc_core::sc_event & get_switch_event( std::string _key);
	
	////////////////////////////////////////////
	/// get_switch_event
	/// returns the event associated with the key
	/// the key better exist...
	///
	/// @param _key ? int key
	/// @return event associated with _key
	////////////////////////////////////////////
	sc_core::sc_event & get_switch_event( int _key);

	/// @see I_gr_dump
	virtual void gr_dump( gr_dump_format_e _format, std::ostream & _stream, unsigned int _tab_level = 0);
	
	/// @see I_gr_config
	virtual bool parse_config( std::string _str);

protected:

	/// container for events
	gs::reg_utils::lookup_table< int, sc_core::sc_event *> m_events;

};

} // end namespace gs::reg
} // end namespace gs::reg

#endif // _GR_SWITCH_H_

#endif // EXCLUDE_FROM_BUILD 
#undef EXCLUDE_FROM_BUILD
