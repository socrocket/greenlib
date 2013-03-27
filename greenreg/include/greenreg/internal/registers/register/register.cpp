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
// 2008-11-27 Christian Schroeder: added gs_param_drf
// 2009-01-21 Christian Schroeder: changed namespace
// 2009-01-29 Christian Schroeder: renamings
// 2009-01-29 FURTHER CHANGES SEE SVN LOG!
//


#include "greenreg/internal/registers/register/register.h"

#include <sstream>
#include <string>
#include <stdarg.h>

#include "greenreg/utils/stl_ext/string_utils.h"

#include "greenreg/gr_externs.h"
#include "greenreg/sysc_ext/utils/gr_report.h"

#include "greenreg/internal/registers/register_constants.h"
#include "greenreg/internal/registers/register/register_container.h"

#include "greenreg/sysc_ext/kernel/I_notification_rule.h"
#include "greenreg/sysc_ext/kernel/gr_notification_rules.h"
#include "greenreg/sysc_ext/kernel/gr_notification_rule_container.h"

using namespace gs::reg;

_register::_register( register_container & _container, std::string _name, std::string _description,
                     uint_gr_t _offset, unsigned int _type, uint_gr_t _width,
                     I_register_data & _i, I_register_data & _o)
: I_register( _type, _i, _o, _name, std::string(_container.name()) ),
m_name( _name),
m_description( _description),
m_register_offset( _offset),
m_register_width( _width),
m_pre_write_rules( NULL),
m_post_write_rules( NULL),
m_pre_read_rules( NULL),
m_post_read_rules( NULL)
{
	std::stringstream ss;

	if( _container.get_addressing_mode() == ALIGNED_ADDRESS)
	{
		switch( _width)
		{
			case 8:
				if( ! _container.add( _offset, this))
				{
					ss	<< "Attempt to add multiple registers at offset 0x"
						<< std::hex << _offset << " not allowed!\n";
					GR_FATAL( ss.str().c_str()); // Fatal because error would produce problems during destruction
				}
			break;
			case 16:
				if( ((_offset) % 2 > 0) ||
					(! _container.add( _offset, this)))
				{
					ss	<< "Attempt to add multiple registers or miss-alignment at offset 0x"
						<< std::hex << _offset << " not allowed!\n";
					GR_FATAL( ss.str().c_str()); // Fatal because error would produce problems during destruction
				}
			break;
			case 32:
				if( ((_offset) % 4 > 0) ||
					(! _container.add( _offset, this)))
				{
					ss	<< "Attempt to add multiple registers or miss-alignment at offset 0x"
            << std::hex << _offset << " not allowed!\n";
					GR_FATAL( ss.str().c_str()); // Fatal because error would produce problems during destruction
				}
			break;
			default:
				ss	<< "Attempt to create register at 0x"
          << std::hex << _offset << " with an invalid width of 0x" << _width << " not allowed!\n";
				GR_FATAL( ss.str().c_str()); // Fatal because error would produce problems during destruction
			break;
		}
	} else if( _container.get_addressing_mode() == INDEXED_ADDRESS)
	{
		// MARCUS: Check for non-overlapping registers when using indexed address mode
		if( ! _container.add( _offset, this))
		{
			ss	<< "Attempt to add multiple registers at offset 0x"
				<< std::hex << _offset << " not allowed!\n";
			GR_FATAL( ss.str().c_str());
		}
	}
}

_register::~_register()
{
	if( m_pre_write_rules != NULL)
	{
		delete( m_pre_write_rules);
		m_pre_write_rules = NULL;
	}
	if( m_post_write_rules != NULL)
	{
		delete( m_post_write_rules);
		m_post_write_rules = NULL;
	}
	if( m_pre_read_rules != NULL)
	{
		delete( m_pre_read_rules);
		m_pre_read_rules = NULL;
	}
	if( m_post_read_rules != NULL)
	{
		delete( m_post_read_rules);
		m_post_read_rules = NULL;
	}
}

gr_event & _register::add_rule(
	gr_reg_rule_container_e _container,
	std::string _name,
	gr_reg_rule_type_e _rule_type,
	...)
{
	I_notification_rule * rule = 0;
	gr_notification_rule_container * rules = 0;

	gr_event * return_event = 0;

	switch( _container)
	{
		case gs::reg::PRE_READ:
			rules = & get_pre_read_rules();
		break;
		case gs::reg::POST_READ:
			rules = & get_post_read_rules();
		break;
		case gs::reg::PRE_WRITE:
			rules = & get_pre_write_rules();
		break;
		case gs::reg::POST_WRITE:
			rules = & get_post_write_rules();
		break;
		case gs::reg::USR_IN_WRITE:
			rules = & get_user_ibuf_write_rules();
		break;
		case gs::reg::USR_OUT_WRITE:
			rules = & get_user_obuf_write_rules();
		break;

		default:
		break;
	}

	if( rules != 0)
	{
		va_list list;
		va_start( list, _rule_type);

		switch( _rule_type)
		{
			case gs::reg::NOTIFY:
				rule = new gr_reg_rule_notify();
			break;
			case gs::reg::NOTIFY_ON_CHANGE:
				rule = new gr_reg_rule_notify_on_change();
			break;
			case gs::reg::WRITE_PATTERN_EQUAL:
				rule = new gr_reg_rule_write_pattern_equal( va_arg( list, char *));
			break;
			case gs::reg::READ_PATTERN_EQUAL:
				rule = new gr_reg_rule_write_pattern_equal( va_arg( list, char *));
			break;
			case gs::reg::PATTERN_STATE_CHANGE:
				rule = new gr_reg_rule_pattern_state_change( va_arg( list, char *), va_arg( list, char *));
			break;
			case gs::reg::BIT_STATE:
				rule = new gr_reg_rule_bit_state( va_arg( list, unsigned int), (bool)va_arg( list, int));
			break;
			case gs::reg::BIT_STATE_CHANGE:
				rule = new gr_reg_rule_bit_state_change( va_arg( list, unsigned int), (bool)va_arg( list, int), (bool)va_arg( list, int));
			break;
			default:
			break;
		}

		va_end( list);

		if( rule != 0)
		{
			rules->add_notification_rule( _name, rule);
			return_event = & rule->default_event();
      // set switch as it belongs
      if (get_events_enabled())  rules->enable_events();
      else rules->disable_events();
		} else {
			return_event = & rules->get_rule_event("dummy_event");
		}
	} else {
		return_event = & i.get_write_rules().get_rule_event("dummy_event");
	}

	return( *return_event);
}

void _register::gr_dump( gr_dump_format_e _format, ::std::ostream & _stream, unsigned int _tab_level)
{
	_stream << "\n"
			<< gs::reg_utils::string::tabs( _tab_level).c_str() << "# non modifiable parameters \n"
			<< gs::reg_utils::string::tabs( _tab_level).c_str() << "# r[" << std::hex << m_register_offset << "].width = " << std::dec << m_register_width << "\n"
			<< gs::reg_utils::string::tabs( _tab_level).c_str() << "# the following can be expanded in true detail later...\n"
			<< gs::reg_utils::string::tabs( _tab_level).c_str() << "# r[" << std::hex << m_register_offset << "].type = 0x" << m_register_type << "\n";
	if( (m_register_type & SINGLE_IO) > 0) {
		_stream	<< gs::reg_utils::string::tabs( _tab_level).c_str() << "r[" << std::hex << m_register_offset << "] = 0x" << i.get() << "\n"
				<< gs::reg_utils::string::tabs( _tab_level).c_str() << "r[" << std::hex << m_register_offset << "].write_mask = 0x" << get_write_mask() << "\n"
				<< gs::reg_utils::string::tabs( _tab_level).c_str() << "r[" << std::hex << m_register_offset << "].lock_mask = 0x" << get_lock_mask() << "\n";
	} else {
		_stream	<< gs::reg_utils::string::tabs( _tab_level).c_str() << "r[" << std::hex << m_register_offset << "].i = 0x" << i.get() << "\n"
				<< gs::reg_utils::string::tabs( _tab_level).c_str() << "r[" << std::hex << m_register_offset << "].i.write_mask = 0x" << get_write_mask() << "\n"
				<< gs::reg_utils::string::tabs( _tab_level).c_str() << "r[" << std::hex << m_register_offset << "].i.lock_mask = 0x" << get_lock_mask() << "\n"
				<< gs::reg_utils::string::tabs( _tab_level).c_str() << "r[" << std::hex << m_register_offset << "].o = 0x" << o.get() << "\n";
	}
}

