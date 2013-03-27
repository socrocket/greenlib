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
// 2008-12-17 Puneet Arora: defined init_param() which explicitly calls the gs_param_drf<T> base class version
// 2009-01-21 Christian Schroeder: changed namespace
// 2009-01-29 Christian Schroeder: renamings
// 2009-01-29 FURTHER CHANGES SEE SVN LOG!
//


//#define EXCLUDE_FROM_BUILD
#ifndef EXCLUDE_FROM_BUILD
#undef EXCLUDE_FROM_BUILD

#ifndef _REGISTER_H_
#define _REGISTER_H_

#include <string>

#include "greenreg/sysc_ext/utils/gr_report.h"
#include "greenreg/gr_externs.h"
#include "greenreg/internal/registers/register/I_register.h"
#include "greenreg/sysc_ext/kernel/gr_notification_rule_container.h"

namespace gs {
namespace reg {

class register_container;

////////////////////////////////////////////
/// class_name
/// long_description
/// @author 
/// @since 
////////////////////////////////////////////
class _register : public I_register
{
public:

	////////////////////////////////////////////
	/// method_name
	/// long_description
	///
	/// @param name ? description
	/// @return value
	///
	/// @see reference
	////////////////////////////////////////////
	_register( register_container & _container, std::string _name, std::string _description,
		uint_gr_t _offset, unsigned int _type, uint_gr_t _width,
		I_register_data & _i, I_register_data & _o);

	////////////////////////////////////////////
	/// Destructor
	////////////////////////////////////////////
	virtual ~_register();

	/// @see I_register
	inline const uint_gr_t & get_width()
	{ return( m_register_width); }

	/// @see I_register
	inline operator uint_gr_t ()
	{ return( i.get()); }

	/// @see I_register
	inline I_register& operator = ( const uint_gr_t & _data)
	{ o.set( _data); return( *this); }

	/// @see I_register
	inline virtual uint_gr_t get()
	{ return( i.get()); }

	/// @see I_register
	inline virtual void set( uint_gr_t & _data)
	{ o.set( _data); }

	/// @see I_register
	inline virtual void put( uint_gr_t & _data)
	{ o.put( _data); }

	/// @see I_register
	inline virtual void set_write_mask( uint_gr_t _mask)
	{ i.set_write_mask( _mask); }

	/// @see I_register
	inline virtual const uint_gr_t & get_write_mask()
	{ return( i.get_write_mask()); }

	/// @see I_register
	inline virtual void set_lock_mask( uint_gr_t _mask)
	{ i.set_lock_mask( _mask); }

	/// @see I_register
	inline virtual const uint_gr_t & get_lock_mask()
	{ return( i.get_lock_mask()); }

	/// @see I_register
	inline virtual const uint_gr_t get_offset()
	{ return( m_register_offset); }

	/// @see I_register
	inline virtual bool bit_get( unsigned int _bit)
	{ return( bool( get() >> _bit)); }

	/// @see I_register
	inline virtual void bit_set( unsigned int _bit, bool & _data)
	{
		uint_gr_t val = get();
		set_bit( val, _bit, _data);
		set( val);
	}

	/// @see I_register
	inline virtual void bit_put( unsigned int _bit, bool & _data)
	{
		bit_set( _bit, _data);
	}

	/// @see I_register
	inline virtual const bool bit_is_writable( unsigned int _bit)
	{
		return( get_write_mask() >> _bit);
	}

	/// @see I_register
	inline virtual void bit_set_writable( unsigned int _bit, bool _writeable)
	{
		uint_gr_t val = get_write_mask();
		set_bit( val, _bit, _writeable);
		set_write_mask( val);
	}

	/// @see I_register
	inline virtual const bool bit_is_locked( unsigned int _bit)
	{
		return( get_lock_mask() >> _bit);
	}

	/// @see I_register
	inline virtual void bit_lock( unsigned int _bit)
	{
		uint_gr_t val = get_write_mask();
		set_bit( val, _bit, true);
		set_write_mask( val);
	}

	/// @see I_register
	inline virtual void bit_unlock( unsigned int _bit)
	{
		uint_gr_t val = get_lock_mask();
		set_bit( val, _bit, false);
		set_lock_mask( val);
	}

	/// @see I_register
	inline virtual gr_notification_rule_container & get_pre_write_rules()
	{
		if( m_pre_write_rules == NULL)
			m_pre_write_rules = new gr_notification_rule_container();
		return( *m_pre_write_rules);
	}

	/// @see I_register
	inline virtual gr_notification_rule_container & get_post_write_rules()
	{
		if( m_post_write_rules == NULL)
			m_post_write_rules = new gr_notification_rule_container();
		return( *m_post_write_rules);
	}

	/// @see I_register
	inline virtual gr_notification_rule_container & get_pre_read_rules()
	{
		if( m_pre_read_rules == NULL)
			m_pre_read_rules = new gr_notification_rule_container();
		return( *m_pre_read_rules);
	}

	/// @see I_register
	inline virtual gr_notification_rule_container & get_post_read_rules()
	{
		if( m_post_read_rules == NULL)
			m_post_read_rules = new gr_notification_rule_container();
		return( *m_post_read_rules);
	}

	/// @see I_register
	inline virtual gr_notification_rule_container & get_user_ibuf_write_rules()
	{
		return( i.get_write_rules());
	}

	/// @see I_register
	inline virtual gr_notification_rule_container & get_user_obuf_write_rules()
	{
		return( o.get_write_rules());
	}

	/// @see I_register
	virtual gr_event & add_rule(
		gr_reg_rule_container_e _container,
		std::string _name,
		gr_reg_rule_type_e _rule_type,
		...);

	/// @see I_gr_dump
	void gr_dump( gr_dump_format_e _format, ::std::ostream & _stream, unsigned int _tab_level);

protected:

  virtual void init_param()
  {
    gs::gs_param_greenreg<gr_uint_t>::init_param();
    // *** Initialization of bits read from database
    size_t found;
    std::string index;
    std::vector<std::string> members = m_api->getParamList(m_par_name, true); // getParamList with childs of childs
    for (std::vector<std::string>::iterator iter = members.begin(); iter != members.end(); iter++) {
      if (iter->length() > m_par_name.length()) {
        //so when constructing MyMod.default_registers.reg0 , get in here for MyMod.default_registers.reg0.1 , MyMod.default_registers.reg01.1 etc
        //std::cout << (*iter).c_str() << "\n";
        found = (*iter).find(m_par_name);
        assert(found != std::string::npos && found == 0);
        index = (*iter).substr(m_par_name.length()+1);
        //for MyMod.default_registers.reg0.1 in db, index will be 1
        //for MyMod.default_registers.reg01.1 in db, index will be .1
        found = index.find(SC_NAME_DELIMITER);
        if (found == std::string::npos) {
          //so get in here for MyMod.default_registers.reg0.*
          if (index.find_last_not_of("1234567890") == std::string::npos) { //confirming that * contains integers only
            unsigned int i = atoi(index.c_str());
            if (i < m_register_width) {
              bool val(false);
              gs::gs_param<bool>::static_deserialize(val, m_api->getValue(*iter));
              bit_set(i, val);
            }
            else {
              std::stringstream ss;
              ss << "Could not assign the value, as the bit index "<<i<<" exceeds register width ("<<m_register_width<<")";
              GR_FORCE_WARNING(ss.str().c_str());
            }
          }
          else {
            std::stringstream ss;
            ss << "Could not assign the value, as bit index '"<<index<<"' is invalid (has non-numeric characters)";
            GR_FORCE_WARNING(ss.str().c_str());
          }
        }//endif (found == std::string::npos)
      }//endif (iter->length() > m_par_name.length())
    }
  }

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
    if( (m_register_type & SPLIT_IO) > 0) {
      rule_names.push_back(std::string("gs_param_usr_out_cb_") + name());
      add_rule( gs::reg::USR_OUT_WRITE, rule_names.back(), gs::reg::NOTIFY);
    }
    // Post write bus access notifications
    // TODO: REMOVED! not needed because USR_IN_WRITE is fired as well on bus access!
    //rule_names.push_back(std::string("gs_param_post_wr_cb_") + name());
    //add_rule( gs::reg::POST_WRITE,  rule_names.back(), gs::reg::NOTIFY);
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
  
	/// name of register for external access and reporting
	std::string m_name;

	/// description of register for configuration
	std::string m_description;

	/// register offset
	uint_gr_t m_register_offset;

	/// register width
	uint_gr_t m_register_width;

	/// place holder for PRE_WRITE rule container
	gr_notification_rule_container * m_pre_write_rules;

	/// place holder for POST_WRITE rule container
	gr_notification_rule_container * m_post_write_rules;

	/// place holder for PRE_READ rule container
	gr_notification_rule_container * m_pre_read_rules;

	/// place holder for POST_READ rule container
	gr_notification_rule_container * m_post_read_rules;
};

} // end namespace gs:reg
} // end namespace gs:reg

#endif // _REGISTER_H_

#endif // EXCLUDE_FROM_BUILD
#undef EXCLUDE_FROM_BUILD
