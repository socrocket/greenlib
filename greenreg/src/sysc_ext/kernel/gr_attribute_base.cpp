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

#include <sstream>
#include <stdarg.h>

#include "greenreg/sysc_ext/kernel/gr_attribute_base.h"
#include "greenreg/sysc_ext/kernel/gr_attribute_type_container.h"
#include "greenreg/sysc_ext/kernel/gr_attribute.h"

#include "greenreg/sysc_ext/utils/gr_report.h"

#include "greenreg/sysc_ext/kernel/I_notification_rule.h"
#include "greenreg/sysc_ext/kernel/gr_notification_rules.h"
#include "greenreg/sysc_ext/kernel/gr_notification_rule_container.h"

using namespace gs::reg;

gr_attrib_base::gr_attrib_base( std::string _name, const std::string _description, gr_attribute_container & _a)
: gr_object( _name), m_has_event( false), m_description( _description), m_read_rules( NULL), m_write_rules( NULL)
{
	_a.add_key_data_pair( _name.c_str(), *this);
}

gr_attrib_base::~gr_attrib_base()
{
	if( m_read_rules == NULL)
	{
		delete( m_read_rules);
	}
	if( m_write_rules == NULL)
	{
		delete( m_write_rules);
	}
}

gr_attrib_base& gr_attrib_base::operator = ( const double& _value)
{
	if( m_type == gs::reg_utils::DOUBLE)
	{
		GR_FORCE_WARNING( "THIS CODE SHOULD NEVER BE REACHED, OPERATOR OVERLOADING WILL TAKE CARE OF ASSIGNMENT\n");
		static_cast< gr_attribute< double> *>( this)->set_value( _value);
	} else {

		switch( m_type)
		{
		case gs::reg_utils::INT:
			static_cast< gr_attribute< int> *>( this)->set_value( (int) _value);
			GR_FORCE_WARNING( "Converting double to gr_attribute< int>; this should not occur, please check your assignment!");
		break;

		case gs::reg_utils::UINT:
			static_cast< gr_attribute< unsigned int> *>( this)->set_value( (unsigned int) _value);
			GR_FORCE_WARNING( "Converting double to gr_attribute< unsigned int>; this should not occur, please check your assignment!");
		break;

		case gs::reg_utils::STRING:
			GR_ERROR( "Request to assign double to gr_attribute<std::string>\nThis cannot be done, please check code\n");
		break;

		default:
			GR_ERROR( "Request to assign double to gr_attribute<UNKNOWN>\nThis cannot be done, please check code\n");
		break;
		}
	}
	return( *this);
}

gr_attrib_base& gr_attrib_base::operator = ( const int& _value)
{
	if( m_type == gs::reg_utils::INT)
	{
		GR_FORCE_WARNING( "THIS CODE SHOULD NEVER BE REACHED, OPERATOR OVERLOADING WILL TAKE CARE OF ASSIGNMENT\n");
		static_cast< gr_attribute< int> *>( this)->set_value( _value);
	} else {

		switch( m_type)
		{
		case gs::reg_utils::DOUBLE:
			static_cast< gr_attribute< double> *>( this)->set_value( (double) _value);
			GR_FORCE_WARNING( "Converting int to gr_attribute< double>; this should not occur, please check your assignment!");
		break;

		case gs::reg_utils::UINT:
			static_cast< gr_attribute< unsigned int> *>( this)->set_value( (unsigned int) _value);
			GR_FORCE_WARNING( "Converting int to gr_attribute< unsigned int>; this should not occur, please check your assignment!");
		break;

		case gs::reg_utils::STRING:
			GR_ERROR( "Request to assign int to gr_attribute<std::string>\nThis cannot be done, please check code\n");
		break;

		default:
			GR_ERROR( "Request to assign int to gr_attribute<UNKNOWN>\nThis cannot be done, please check code\n");
		break;
		}
	}
	return( *this);
}

gr_attrib_base& gr_attrib_base::operator = ( const unsigned int& _value)
{
	if( m_type == gs::reg_utils::UINT)
	{
		GR_FORCE_WARNING( "THIS CODE SHOULD NEVER BE REACHED, OPERATOR OVERLOADING WILL TAKE CARE OF ASSIGNMENT\n");
		static_cast< gr_attribute< unsigned int> *>( this)->set_value( _value);
	} else {

		switch( m_type)
		{
		case gs::reg_utils::INT:
			static_cast< gr_attribute< int> *>( this)->set_value( (int) _value);
			GR_FORCE_WARNING( "Converting unsigned int to gr_attribute< int>; this should not occur, please check your assignment!");
		break;

		case gs::reg_utils::DOUBLE:
			static_cast< gr_attribute< double> *>( this)->set_value( (double) _value);
			GR_FORCE_WARNING( "Converting unsigned int to gr_attribute< double>; this should not occur, please check your assignment!");
		break;

		case gs::reg_utils::STRING:
			GR_ERROR( "Request to assign unsigned int to gr_attribute<std::string>\nThis cannot be done, please check code\n");
		break;

		default:
			GR_ERROR( "Request to assign unsigned int to gr_attribute<UNKNOWN>\nThis cannot be done, please check code\n");
		break;
		}
	}
	return( *this);
}

gr_notification_rule_container & gr_attrib_base::get_pre_read_rules()
{
	if( m_read_rules == NULL)
	{
		m_read_rules = new gr_notification_rule_container();
	}
	return( *m_read_rules);
}

gr_notification_rule_container & gr_attrib_base::get_post_write_rules()
{
	if( m_write_rules == NULL)
	{
		m_write_rules = new gr_notification_rule_container();
	}
	return( *m_write_rules);
}

gr_event & gr_attrib_base::add_rule( gr_attrib_rule_container_e _container, std::string _name, gr_reg_rule_type_e _rule_type, ...)
{
	I_notification_rule * rule = 0;
	gr_notification_rule_container * rules = 0;

	gr_event * return_event = 0;

	switch( _container)
	{
		case gs::reg::ATTRIB_PRE_READ:
			rules = & get_pre_read_rules();
		break;
		case gs::reg::ATTRIB_POST_WRITE:
			rules = & get_post_write_rules();
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
		} else {
			return_event = & rules->get_rule_event("dummy_event");
		}
	} else {
		return_event = & get_post_write_rules().get_rule_event("dummy_event");
	}

	return( *return_event);
}

#endif // EXCLUDE_FROM_BUILD
#undef EXCLUDE_FROM_BUILD

