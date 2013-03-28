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
// 2008-12-10 Christian Schroeder, Puneet Arora: init of gs_param_drf moved
// 2009-01-21 Christian Schroeder: changed namespace
// 2009-01-29 Christian Schroeder: renamings
// 2009-01-29 FURTHER CHANGES SEE SVN LOG!
//


#include "greenreg/internal/registers/bit_range/bit_range.h"

#include <sstream>
#include <string>
#include <stdarg.h>

#include "greenreg/internal/registers/register/I_register.h"
#include "greenreg/internal/registers/bit_range/bit_range_data.h"

#include "greenreg/sysc_ext/kernel/I_notification_rule.h"
#include "greenreg/sysc_ext/kernel/gr_notification_rules.h"
#include "greenreg/sysc_ext/kernel/gr_notification_rule_container.h"

namespace gs {
namespace reg {

bit_range::bit_range( std::string & _name, gr_uint_t _start_bit, gr_uint_t _end_bit, I_register & _register, bit_range_data & _i, bit_range_data & _o)
: gs::gs_param_greenreg<gr_uint_t>(_register.getName() +"."+_name, true), // gs parameter adapter; init_param() needs to be called at lowest constructor  
m_name( _name),
m_register( &_register),
m_bit_range_start( _start_bit),
m_bit_range_end( _end_bit),
o( _o),
i( _i),
b( _register.b, _start_bit),
m_bit_range_mask( ~0x0),
m_pre_write_rules( 0),
m_post_write_rules( 0),
m_pre_read_rules( 0),
m_post_read_rules( 0)
{
	// pre-calc the bit range mask
	gr_uint_t a = (~0x0 << m_bit_range_start);
	gr_uint_t b = ~(~0x0 << (m_bit_range_end + 1));
	if( b == 0)
		b = ~0;
	m_bit_range_mask = a & b;
}

bit_range::~bit_range()
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

gr_event & bit_range::add_rule(
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
      assert (m_register != NULL && "The parent register of a bit range should never be NULL.");
      if (m_register->get_events_enabled())  
        rules->enable_events();
      else
        rules->disable_events();
		} else {
			return_event = & rules->get_rule_event("dummy_event");
		}
	} else {
		return_event = & i.get_write_rules().get_rule_event("dummy_event");
	}

	return( *return_event);
}

void bit_range::disable_events() {
  I_event_switch::disable_events(); // update the state bool
  // switch all owned notification rule container
  get_pre_write_rules().disable_events();
  get_post_write_rules().disable_events();
  get_pre_read_rules().disable_events();
  get_post_read_rules().disable_events();
  get_user_ibuf_write_rules().disable_events();
  get_user_obuf_write_rules().disable_events();
}

void bit_range::enable_events() {
  I_event_switch::enable_events(); // update the state bool
  // switch all owned notification rule container
  get_pre_write_rules().enable_events();
  get_post_write_rules().enable_events();
  get_pre_read_rules().enable_events();
  get_post_read_rules().enable_events();
  get_user_ibuf_write_rules().enable_events();
  get_user_obuf_write_rules().enable_events();
}

} // end namespace gs:reg
} // end namespace gs:reg
