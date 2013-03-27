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
// 2009-01-21 Christian Schroeder: changed namespace
// 2009-01-27  added event switch
// 2009-01-29 Christian Schroeder: renamings
// 2009-01-29 FURTHER CHANGES SEE SVN LOG!
//


#ifndef BIT_RANGE_H_
#define BIT_RANGE_H_

#include <string>

#include "greenreg/gr_externs.h"
#include "greenreg/internal/registers/register/I_register.h"
#include "bit_range_data.h"
#include "bit_range_bit_accessor.h"
#include "greenreg/sysc_ext/kernel/gr_notification_rule_container.h"

#include "greenreg/sysc_ext/kernel/gr_event.h"

namespace gs {
namespace reg {

////////////////////////////////////////////
/// bit_range
/// The bit range class holds all of the notification
/// rules and controls access to the buffers as expected
/// by the owning register by mimicing the bit_range_data
/// accessability to register data (via input and output buffers)
///
/// The event switch for the managed notification rules is
/// managed by the parent register
////////////////////////////////////////////
class bit_range
: public gs::gs_param_greenreg<gr_uint_t>
, protected I_event_switch
{
public:

	////////////////////////////////////////////
	/// Constructor
	///
	/// @param _name ? unique name of bit range (duplicates ignored)
	/// @param _start_bit ? bit the range starts on (0 being first bit in register)
	/// @param _end_bit ? bit the range ends on (must be > start bit, and less than reg width)
	/// @param _register ? owning register (parent)
	/// @param _i ? input buffer reference
	/// @param _o ? output buffer reference (could be the same as input)
	///
	/// @see I_regsiter, bit_range_data
	////////////////////////////////////////////
	bit_range( std::string & _name, gr_uint_t _start_bit, gr_uint_t _end_bit, I_register & _register, bit_range_data & _i, bit_range_data & _o);

	////////////////////////////////////////////
	/// Destructor
	////////////////////////////////////////////
	virtual ~bit_range();

	////////////////////////////////////////////
	/// operator () for type uint_gr_t
	/// provides access casting for the input buffer
	///
	/// @return copy of data in input buffer
	////////////////////////////////////////////
	inline operator uint_gr_t ()
	{ return i.get(); }

	////////////////////////////////////////////
	/// operator = (uint_gr_t)
	/// provides assignment casting for the output buffer
	///
	/// @param _val ? the value to be assigned to the output buffer
	/// @return refernce to self
	////////////////////////////////////////////
	inline bit_range & operator = ( const uint_gr_t & _val)
	{
		o.set( _val);
		return( *this);
	}

	////////////////////////////////////////////
	/// get
	///
	/// @return data in the input buffer
	////////////////////////////////////////////
	inline virtual uint_gr_t get()
	{ return i.get(); }

	////////////////////////////////////////////
	/// set
	///
	/// @param _data ? new data to set in the output buffer
	////////////////////////////////////////////
	inline virtual void set( const uint_gr_t & _data)
	{ o.set( _data); }

	////////////////////////////////////////////
	/// put
	/// another common name for set in TLM terminology
	///
	/// @param _data ? new data to set in buffer
	///
	/// @see set
	////////////////////////////////////////////
	inline virtual void put( const uint_gr_t & _data)
	{ o.put( _data); }

	////////////////////////////////////////////
	/// get_pre_write_rules
	///
	/// @return gr_notification_rule_container for PRE_WRITE rules
	///
	/// @see gr_notification_rule_container
	////////////////////////////////////////////
	inline virtual gr_notification_rule_container & get_pre_write_rules()
	{
		if( m_pre_write_rules == NULL)
			m_pre_write_rules = new gr_notification_rule_container();
		return( *m_pre_write_rules);
	}

	////////////////////////////////////////////
	/// get_post_write_rules
	///
	/// @return gr_notification_rule_container for POST_WRITE rules
	///
	/// @see gr_notification_rule_container
	////////////////////////////////////////////
	inline virtual gr_notification_rule_container & get_post_write_rules()
	{
		if( m_post_write_rules == NULL)
			m_post_write_rules = new gr_notification_rule_container();
		return( *m_post_write_rules);
	}

	////////////////////////////////////////////
	/// get_pre_read_rules
	///
	/// @return gr_notification_rule_container for PRE_READ rules
	///
	/// @see gr_notification_rule_container
	////////////////////////////////////////////
	inline virtual gr_notification_rule_container & get_pre_read_rules()
	{
		if( m_pre_read_rules == NULL)
			m_pre_read_rules = new gr_notification_rule_container();
		return( *m_pre_read_rules);
	}

	////////////////////////////////////////////
	/// get_post_read_rules
	///
	/// @return gr_notification_rule_container for POST_READ rules
	///
	/// @see gr_notification_rule_container
	////////////////////////////////////////////
	inline virtual gr_notification_rule_container & get_post_read_rules()
	{
		if( m_post_read_rules == NULL)
			m_post_read_rules = new gr_notification_rule_container();
		return( *m_post_read_rules);
	}

	////////////////////////////////////////////
	/// get_ibuf_write_rules
	/// access to input buffer write rules container
	///
	/// @return gr_notification_rule_container for USR_IN_WRITE rules
	///
	/// @see gr_notification_rule_container
	////////////////////////////////////////////
	inline virtual gr_notification_rule_container & get_user_ibuf_write_rules()
	{
		return( i.get_write_rules());
	}

	////////////////////////////////////////////
	/// get_obuf_write_rules
	/// access to output buffer write rules container
	///
	/// @return gr_notification_rule_container for USR_OUT_WRITE rules
	///
	/// @see gr_notification_rule_container
	////////////////////////////////////////////
	inline virtual gr_notification_rule_container & get_user_obuf_write_rules()
	{
		return( o.get_write_rules());
	}

	////////////////////////////////////////////
	/// add_rule
	/// adds a rule based on content provided, each
	/// rule has a dynamic set of parameters
	///
	/// @param _container ? enumeration id of which container to add the rule to
	/// @param _name ? unique string name identifying the rule
	/// @param _rule_type ? enumeration id of which rule to add
	/// @param ... ? param list based on rule requirements
	/// @return sc_core::sc_event of rule or dummy if rule already existed
	////////////////////////////////////////////
	virtual gr_event & add_rule(
		gr_reg_rule_container_e _container,
		std::string _name,
		gr_reg_rule_type_e _rule_type,
		...);

	/// unique name for this bit range
	std::string m_name;

	/// reference to owning register
	I_register * m_register;

	/// which bit in the register the range starts on
	gr_uint_t m_bit_range_start;

	/// which bit in the register the range ends on
	gr_uint_t m_bit_range_end;

	/// the output data control/buffer for this range
	bit_range_data & o;

	/// the input data control/buffer for this range
	bit_range_data & i;

	/// accessability to bits in the bit range for completeness
	bit_range_bit_accessor b;

protected:
  
  /// @see I_event_switch, accessed by bit_range_accessor accessed by register
  virtual void disable_events();
  
  /// @see I_event_switch, accessed by bit_range_accessor accessed by register
  virtual void enable_events();
  
  /// Function setting the greenreg value, being called by gs parameter
  inline void set_greenreg_value(const uint_gr_t &val)
  {
    uint_gr_t i = val;
    set(i);
  }
  
  /// Function getting the greenreg value, being called by gs parameter
  inline uint_gr_t get_greenreg_value()
  {
    return get();
  }
  
  inline std::vector<std::string> add_post_read_param_rules() 
  {
    std::vector<std::string> rule_names;
    // Post write direct access notifications (only call on writes to IN buffer, because this is the one the param get returns!)
    rule_names.push_back(std::string("gs_param_usr_in_cb_") + name());
    add_rule( gs::reg::USR_IN_WRITE,  rule_names.back(), gs::reg::NOTIFY);
    // TODO: For a SPLIT_IO this way you get a callback if the local set (OUT) is written to, but the parameter value still returns the not changed IN value
    if( (m_register->get_register_type() & SPLIT_IO) > 0) {
     rule_names.push_back(std::string("gs_param_usr_out_cb_") + name());
     add_rule( gs::reg::USR_OUT_WRITE, rule_names.back(), gs::reg::NOTIFY);
    }
    // Post write bus access notifications
    // TODO: REMOVED! not needed because USR_IN_WRITE is fired as well on bus access!
    rule_names.push_back(std::string("gs_param_post_wr_cb_") + name());
    add_rule( gs::reg::POST_WRITE,  rule_names.back(), gs::reg::NOTIFY);
    return rule_names;
  }
  
  inline std::vector<gr_notification_rule_container*> get_param_rules() 
  {
    std::vector<gr_notification_rule_container*> vec;
    vec.push_back(&get_post_write_rules());
    vec.push_back(&get_user_ibuf_write_rules());
    vec.push_back(&get_user_obuf_write_rules());
    return vec;
  }
  
protected:
	friend class bit_range_data;
	friend class bit_range_accessor;

	/// bit range write mask
	gr_uint_t m_bit_range_mask;

	/// placeholder for pre_write rule container if needed
	gr_notification_rule_container * m_pre_write_rules;

	/// placeholder for post_write rule container if needed
	gr_notification_rule_container * m_post_write_rules;

	/// placeholder for pre_read rule container if needed
	gr_notification_rule_container * m_pre_read_rules;

	/// placeholder for post_read rule container if needed
	gr_notification_rule_container * m_post_read_rules;
};

} // end namespace gs:reg
} // end namespace gs:reg

#endif /*BIT_RANGE_H_*/
