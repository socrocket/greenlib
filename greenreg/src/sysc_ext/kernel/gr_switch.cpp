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
// 2008-11-25 Christian Schroeder: gs_params<sc_attribute<T> >
// 2009-01-21 Christian Schroeder: changed namespace
// 2009-01-29 Christian Schroeder: renamings
// 2009-01-29 FURTHER CHANGES SEE SVN LOG!
//


//#define EXCLUDE_FROM_BUILD
#ifndef EXCLUDE_FROM_BUILD
#undef EXCLUDE_FROM_BUILD

#include "greenreg/sysc_ext/kernel/gr_switch.h"

#include "greenreg/utils/stl_ext/string_utils.h"

#include "greenreg/sysc_ext/utils/gr_report.h"

using namespace gs::reg;


gr_switch::gr_switch( std::string _name, const std::string _description, gr_attribute_container & _switch_container, unsigned int _default_value)
: gr_attribute< int>( _name, _description, _switch_container, _default_value),
option_list()
{
}

gr_switch::gr_switch( std::string _name, const std::string _description, gr_attribute_container & _switch_container, gs::reg_utils::option_list_t & _options, unsigned int _default_value)
: gr_attribute< int>( _name, _description, _switch_container, _default_value),
option_list( _options)
{
	std::vector< std::string> keys = list_keys();
	std::vector< std::string>::iterator vit;
	for( vit = keys.begin(); vit != keys.end(); vit++)
	{
		m_events.add_entry( this->lookup_key( *vit), new sc_core::sc_event);
	}
	m_events.lock_table();
}

gr_switch::~gr_switch()
{
	std::vector< int> keys = m_events.list_keys();
	std::vector< int>::iterator vit;
	for( vit = keys.begin(); vit != keys.end(); vit++)
	{
		sc_core::sc_event * event = m_events.lookup_key( *vit);
		delete( event);
	}
}

bool gr_switch::set_value( const int& _value)
{
	if( lookup_val( _value) != get_error_key())
	{
		m_attrib = _value;

		if( m_has_event)
		{
			// ok, so we WILL fire the standard change event
			m_change_event.notify();

			// but what is important is the switch event
			get_switch_event( _value).notify();
		}

		return( true);
	} else {
		// we still have to notify of error!!!
		if( m_has_event)
		{
			// ok, so we WILL NOT fire the standard change event
			// m_change_event.notify();

			// but we will fire the switches error event...
			get_switch_event( get_error_key()).notify();
		}
	}
	return( false);
}

bool gr_switch::add_entry( std::string _key, unsigned int _value, bool _override)
{
	if( option_list::add_entry( _key, _value, _override))
	{
		m_events.add_entry( this->lookup_key( _key), new sc_core::sc_event);
		return true;
	}
	return false;
}

sc_core::sc_event & gr_switch::get_switch_event( std::string _key)
{
	return( get_switch_event( lookup_key( _key)));
}

sc_core::sc_event & gr_switch::get_switch_event( int _key)
{
	return( *(m_events.lookup_key( _key)));
}

void gr_switch::gr_dump( gr_dump_format_e _format, std::ostream & _stream, unsigned int _tab_level)
{
	_stream << gs::reg_utils::string::tabs( _tab_level).c_str() << "# " << m_description << "\n"
			<< gs::reg_utils::string::tabs( _tab_level).c_str() << "# options: ";

	std::vector< std::string> keys = list_keys();
	std::vector< std::string>::iterator vit;
	for( vit = keys.begin(); vit != keys.end(); vit++)
	{
		if( vit != keys.begin())
		{
			_stream << ", ";
		}
		_stream << *vit;
	}

	_stream << "\n"
			<< gs::reg_utils::string::tabs( _tab_level).c_str() << m_attrib.name()
			<< " = " << lookup_val( m_attrib.getValue().value).c_str() << "\n\n";
}

bool gr_switch::parse_config( std::string _str)
{
	if( this->lookup_key( _str) != this->get_error_value())
	{
		m_attrib = this->lookup_key( _str);
		return true;
	} else {
		std::stringstream ss;
		ss << "Invalid setting for attribute: " << name();
		GR_FORCE_WARNING( ss.str().c_str());
	}
	return false;
}

#endif // EXCLUDE_FROM_BUILD
#undef EXCLUDE_FROM_BUILD
