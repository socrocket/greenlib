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


#ifndef _GR_NOTIFICATION_RULES_H_
#define _GR_NOTIFICATION_RULES_H_

#include <boost/config.hpp>
#include <systemc>

#include "greenreg/sysc_ext/kernel/I_notification_rule.h"
#include "greenreg/gr_externs.h"

#include "greenreg/utils/stl_ext/string_utils.h"


namespace gs {
namespace reg {
	
class gr_reg_rule_notify : public I_notification_rule
{
public:
	gr_reg_rule_notify()
	{;}
	
	virtual ~gr_reg_rule_notify()
	{;}
	
	virtual void process_rule( uint_gr_t _old_value, uint_gr_t & _new_value, transaction_type* _transaction, bool _delayed)
	{
    GR_NOTIF_RULE_DUMP("   process_rule makes notify");
		//m_default_event.notify();
    if (_delayed)
      m_default_event.notify_with_delay(_transaction);
    else
      m_default_event.notify(_transaction);
	}
	
protected:
};

class gr_reg_rule_notify_on_change : public I_notification_rule
{
public:
	gr_reg_rule_notify_on_change()
	{;}
	
	virtual ~gr_reg_rule_notify_on_change()
	{;}
	
	virtual void process_rule( uint_gr_t _old_value, uint_gr_t & _new_value, transaction_type* _transaction, bool _delayed)
	{
		if( _old_value != _new_value) {
      GR_NOTIF_RULE_DUMP("   process_rule makes notify");
      //m_default_event.notify();
      if (_delayed)
        m_default_event.notify_with_delay(_transaction);
      else
        m_default_event.notify(_transaction);
    } 
    else
      GR_NOTIF_RULE_DUMP("   process_rule not matches, NOT make notify");

	}
	
protected:
};

class gr_reg_rule_write_pattern_equal : public I_notification_rule
{
public:
	gr_reg_rule_write_pattern_equal( std::string _check_pattern)
	: m_check_pattern( _check_pattern)
	{
	}

	virtual ~gr_reg_rule_write_pattern_equal()
	{;}
	
	virtual void process_rule( uint_gr_t _old_value, uint_gr_t & _new_value, transaction_type* _transaction, bool _delayed)
	{
		if( gs::reg_utils::string::string_match( m_check_pattern, gs::reg_utils::string::unsigned_long_to_hex_string( _new_value)))
		{
      GR_NOTIF_RULE_DUMP("   process_rule makes notify");
      //m_default_event.notify();
      if (_delayed)
        m_default_event.notify_with_delay(_transaction);
      else
        m_default_event.notify(_transaction);
		}
    else
      GR_NOTIF_RULE_DUMP("   process_rule not matches, NOT make notify");
	}
	
protected:

	std::string m_check_pattern;
};

class gr_reg_rule_read_pattern_equal : public I_notification_rule
{
public:
	gr_reg_rule_read_pattern_equal( std::string _check_pattern)
	: m_check_pattern( _check_pattern)
	{
	}

	virtual ~gr_reg_rule_read_pattern_equal()
	{;}
	
	virtual void process_rule( uint_gr_t _old_value, uint_gr_t & _new_value, transaction_type* _transaction, bool _delayed)
	{
		if( gs::reg_utils::string::string_match( m_check_pattern, gs::reg_utils::string::unsigned_long_to_hex_string( _old_value)))
		{
      GR_NOTIF_RULE_DUMP("   process_rule makes notify");
      //m_default_event.notify();
      if (_delayed)
        m_default_event.notify_with_delay(_transaction);
      else
        m_default_event.notify(_transaction);
		}
    else
      GR_NOTIF_RULE_DUMP("   process_rule not matches, NOT make notify");
	}
	
protected:

	std::string m_check_pattern;
};

class gr_reg_rule_pattern_state_change : public I_notification_rule
{
public:
	gr_reg_rule_pattern_state_change( std::string _old_pattern, std::string _new_pattern)
	: m_old_pattern( _old_pattern),
	m_new_pattern( _new_pattern)
	{
	}
	
	virtual ~gr_reg_rule_pattern_state_change()
	{;}
	
	virtual void process_rule( uint_gr_t _old_value, uint_gr_t & _new_value, transaction_type* _transaction, bool _delayed)
	{
		if( gs::reg_utils::string::string_match( m_old_pattern, gs::reg_utils::string::unsigned_long_to_hex_string( _old_value)) &&
			gs::reg_utils::string::string_match( m_new_pattern, gs::reg_utils::string::unsigned_long_to_hex_string( _new_value)))
		{
      GR_NOTIF_RULE_DUMP("   process_rule makes notify");
      //m_default_event.notify();
      if (_delayed)
        m_default_event.notify_with_delay(_transaction);
      else
        m_default_event.notify(_transaction);
		}
    else
      GR_NOTIF_RULE_DUMP("   process_rule not matches, NOT make notify");
	}
	
protected:

	std::string m_old_pattern;
	std::string m_new_pattern;
};

class gr_reg_rule_bit_state : public I_notification_rule
{
public:
	gr_reg_rule_bit_state( uint_gr_t _bit, bool _state)
	: m_bit( _bit),
	m_state( _state)
	{
	}
	
	virtual ~gr_reg_rule_bit_state()
	{;}
	
	virtual void process_rule( uint_gr_t _old_value, uint_gr_t & _new_value, transaction_type* _transaction, bool _delayed)
	{
		if( ((_new_value >> m_bit) & 1) == m_state)
		{
      GR_NOTIF_RULE_DUMP("   process_rule makes notify");
      //m_default_event.notify();
      if (_delayed)
        m_default_event.notify_with_delay(_transaction);
      else
        m_default_event.notify(_transaction);
		}
    else
      GR_NOTIF_RULE_DUMP("   process_rule not matches, NOT make notify");
	}
	
	protected:
	uint_gr_t m_bit;
	bool m_state;
};

class gr_reg_rule_bit_state_change : public I_notification_rule
{
public:
	gr_reg_rule_bit_state_change( uint_gr_t _bit, bool _start_state, bool _end_state)
	: m_bit( _bit),
	m_start_state( _start_state),
	m_end_state( _end_state)
	{
	}
	
	virtual ~gr_reg_rule_bit_state_change()
	{;}
	
	virtual void process_rule( uint_gr_t _old_value, uint_gr_t & _new_value, transaction_type* _transaction, bool _delayed)
	{
		if( ( ( ( _old_value >> m_bit) & 1) == m_start_state) &&
			( ( ( _new_value >> m_bit) & 1) == m_end_state))
		{
      GR_NOTIF_RULE_DUMP("   process_rule makes notify");
      //m_default_event.notify();
      if (_delayed)
        m_default_event.notify_with_delay(_transaction);
      else
        m_default_event.notify(_transaction);
		}
    else
      GR_NOTIF_RULE_DUMP("   process_rule not matches, NOT make notify");
	}

protected:
	uint_gr_t m_bit;
	bool m_start_state;
	bool m_end_state;
};

} // end namespace gs:reg
} // end namespace gs:reg

#endif /*_GR_NOTIFICATION_RULES_H_*/
