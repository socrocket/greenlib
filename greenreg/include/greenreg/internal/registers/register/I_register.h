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
// 2009-01-20 Christian Schroeder: add callback/event switch for notification rules
// 2009-01-21 Christian Schroeder: changed namespace
// 2009-01-29 Christian Schroeder: renamings
// 2009-01-29 FURTHER CHANGES SEE SVN LOG!
//


//#define EXCLUDE_FROM_BUILD
#ifndef EXCLUDE_FROM_BUILD
#undef EXCLUDE_FROM_BUILD

#ifndef _I_REGISTER_H_
#define _I_REGISTER_H_

#include "greenreg/gr_externs.h"
#include "greenreg/framework/core/I_gr_dump.h"
#include "greenreg/internal/registers/bit/bit_accessor.h"
#include "greenreg/internal/registers/bit_range/bit_range_accessor.h"
#include "greenreg/internal/registers/register_data/primary_register_data.h"

#include "greenreg/sysc_ext/kernel/gr_notification_rule_container.h"

#include "greenreg/framework/core/I_gr_config.h"

#include "greenreg/sysc_ext/kernel/gr_event.h"

#include "greenreg/internal/registers/register/I_register_types.h"
#include "greenreg/gs/gs_param_greenreg.h"

#include "greenreg/sysc_ext/kernel/I_event_switch.h"


namespace gs {
namespace reg {

class I_register_data;

////////////////////////////////////////////
/// I_register
/// Interface definition of a register, serves also
/// as the declaration time configuration of data
/// storage, and access.
/// @author 
/// @since 
////////////////////////////////////////////
class I_register 
: public I_gr_dump, public I_gr_config
, public gs::gs_param_greenreg<gr_uint_t>
, public I_event_switch
{
public:

	////////////////////////////////////////////
	/// Constructor
	/// The ability to configure both the input and output buffer via
	/// external references enables features such as split i/o, or double buffer,
	/// or any other type of specialised buffering system to be bound to
	/// the register, and is an important key to DRF's flexability.
	///
	/// @param _register_type ? type of register, defined as a combination of register_type, register_io, register_buffer, & register_data (orientation)
	/// @param _i ? register data interface for the input buffer
	/// @param _o ? register data interface for the output buffer
  /// @param _name ? name of the register, given to the gs_param adapter
  /// @param _param_name_prefix ? prefix string for the gs_param's name, given to the gs_param adapter
	///
	/// @see I_register_data, register_type_e, register_io_e, register_buffer_e, register_data_e
	////////////////////////////////////////////
	I_register( unsigned int _register_type, I_register_data & _i, I_register_data & _o, std::string &_name, std::string _param_name_prefix);

	////////////////////////////////////////////
	/// Destructor
	////////////////////////////////////////////
	virtual ~I_register();

	////////////////////////////////////////////
	/// get_register_type
	///
	/// @return the register type as defined by a combination of register_type, register_io, register_buffer, & register_data (orientation)
	///
	/// @see register_type_e, register_io_e, register_buffer_e, register_data_e
	////////////////////////////////////////////
	inline unsigned int & get_register_type()
	{ return( m_register_type); }

	////////////////////////////////////////////
	/// get_width
	///
	/// @return width of register
	////////////////////////////////////////////
	virtual const uint_gr_t & get_width() = 0;

	////////////////////////////////////////////
	/// operator () for type uint_gr_t
	/// provides access casting and typically should bind
	/// to the input buffer (write from the bus, read from the model)
	///
	/// @return copy of data in input buffer
	////////////////////////////////////////////
	virtual operator uint_gr_t () = 0;

	////////////////////////////////////////////
	/// operator = (uint_gr_t)
	/// provides assignment casting and typically should bind
	/// to the output buffer (write from model, read from bus)
	///
	/// @param _val ? the value to be assigned to the register
	/// @return refernce to self
	////////////////////////////////////////////
	virtual I_register& operator = ( const uint_gr_t & _val) = 0;

	////////////////////////////////////////////
	/// get
	///
	/// @return data in input buffer (same as operator () )
	////////////////////////////////////////////
	virtual uint_gr_t get() = 0;

	////////////////////////////////////////////
	/// set
	///
	/// @param _data ? set data in write buffer (same as operator = )
	////////////////////////////////////////////
	virtual void set( uint_gr_t & _data) = 0;

	////////////////////////////////////////////
	/// put
	/// another common name for set in TLM terminology
	///
	/// @param _data ? set data in write buffer (same as operator = )
	///
	/// @see set
	////////////////////////////////////////////
	virtual void put( uint_gr_t & _data) = 0;

	////////////////////////////////////////////
	/// set_write_mask
	///
	/// @param _mask ? new write mask
	////////////////////////////////////////////
	virtual void set_write_mask( uint_gr_t _mask) = 0;

	////////////////////////////////////////////
	/// get_write_mask
	///
	/// @return write mask (1's are writable)
	////////////////////////////////////////////
	virtual const uint_gr_t & get_write_mask() = 0;

	////////////////////////////////////////////
	/// set_lock_mask
	/// intended for a locking mask, not used at this time
	///
	/// @param _mask ? new lock mask
	////////////////////////////////////////////
	virtual void set_lock_mask( uint_gr_t _mask) = 0;

	////////////////////////////////////////////
	/// get_lock_mask
	/// intended for a locking mask, not used at this time
	///
	/// @return lock mask (1's are accessible)
	////////////////////////////////////////////
	virtual const uint_gr_t & get_lock_mask() = 0;

	////////////////////////////////////////////
	/// get_offset
	/// returns the offset of this register assuming it has been set
	///
	/// @return offset or address of the register
	////////////////////////////////////////////
	virtual const uint_gr_t get_offset() = 0;
	
	////////////////////////////////////////////
	/// bit_get
	///
	/// @param _bit ? bit to get value for
	/// @return bit value
	////////////////////////////////////////////
	virtual bool bit_get( unsigned int _bit) = 0;

	////////////////////////////////////////////
	/// bit_set
	///
	/// @param _bit ? bit to set value for
	/// @param _data ? new value
	////////////////////////////////////////////
	virtual void bit_set( unsigned int _bit, bool & _data) = 0;

	////////////////////////////////////////////
	/// bit_put
	/// Same as set in most cases, uses different naming convention
	///
	/// @param _bit ? bit to set value for
	/// @param _data ? new value
	////////////////////////////////////////////
	virtual void bit_put( unsigned int _bit, bool & _data) = 0;

	////////////////////////////////////////////
	/// bit_is_writable
	/// check to see if bit is writable
	///
	/// @param _bit ? bit to check
	/// @return true if writable
	////////////////////////////////////////////
	virtual const bool bit_is_writable( unsigned int _bit) = 0;

	////////////////////////////////////////////
	/// bit_set_writable
	///
	/// @param _bit ? bit to set writable
	/// @param _writeable ? true for writable
	////////////////////////////////////////////
	virtual void bit_set_writable( unsigned int _bit, bool _writeable) = 0;

	////////////////////////////////////////////
	/// bit_is_locked
	/// intended for a locking mask, not used at this time
	///
	/// @param _bit ? bit to check if locked
	/// @return true if locked
	////////////////////////////////////////////
	virtual const bool bit_is_locked( unsigned int _bit) = 0;

	////////////////////////////////////////////
	/// bit_lock
	/// intended for a locking mask, not used at this time
	///
	/// @param _bit ? bit to lock
	////////////////////////////////////////////
	virtual void bit_lock( unsigned int _bit) = 0;

	////////////////////////////////////////////
	/// bit_unlock
	/// intended for a locking mask, not used at this time
	///
	/// @param _bit ? bit to unlock
	////////////////////////////////////////////
	virtual void bit_unlock( unsigned int _bit) = 0;

	////////////////////////////////////////////
	/// get_pre_write_rules
	///
	/// @return gr_notification_rule_container for PRE_WRITE rules
	///
	/// @see gr_notification_rule_container
	////////////////////////////////////////////
	virtual gr_notification_rule_container & get_pre_write_rules() = 0;

	////////////////////////////////////////////
	/// get_post_write_rules
	///
	/// @return gr_notification_rule_container for POST_WRITE rules
	///
	/// @see gr_notification_rule_container
	////////////////////////////////////////////
	virtual gr_notification_rule_container & get_post_write_rules() = 0;

	////////////////////////////////////////////
	/// get_pre_read_rules
	///
	/// @return gr_notification_rule_container for PRE_READ rules
	///
	/// @see gr_notification_rule_container
	////////////////////////////////////////////
	virtual gr_notification_rule_container & get_pre_read_rules() = 0;

	////////////////////////////////////////////
	/// get_post_read_rules
	///
	/// @return gr_notification_rule_container for POST_READ rules
	///
	/// @see gr_notification_rule_container
	////////////////////////////////////////////
	virtual gr_notification_rule_container & get_post_read_rules() = 0;

	////////////////////////////////////////////
	/// get_ibuf_write_rules
	/// access to input buffer write rules container
	///
	/// @return gr_notification_rule_container for USR_IN_WRITE rules
	///
	/// @see gr_notification_rule_container
	////////////////////////////////////////////
	virtual gr_notification_rule_container & get_user_ibuf_write_rules() = 0;

	////////////////////////////////////////////
	/// get_obuf_write_rules
	/// access to output buffer write rules container
	///
	/// @return gr_notification_rule_container for USR_OUT_WRITE rules
	///
	/// @see gr_notification_rule_container
	////////////////////////////////////////////
	virtual gr_notification_rule_container & get_user_obuf_write_rules() = 0;

	////////////////////////////////////////////
	/// add_rule
	/// adds a rule based on content provided, each
	/// rule has a dynamic set of parameters
	///
	/// @param _container ? enumeration id of which container to add the rule to
	/// @param _name ? unique string name identifying the rule
	/// @param _rule_type ? enumeration id of which rule to add
	/// @param ... ? param list based on rule requirements
	/// @return gr_event of rule or dummy if rule already existed
	////////////////////////////////////////////
	virtual gr_event & add_rule(
		gr_reg_rule_container_e _container,
		std::string _name,
		gr_reg_rule_type_e _rule_type,
		...) = 0;

	////////////////////////////////////////////
	/// get_primary_register_data
	/// returns the greenreg data class which wrapps
	/// the backend data.
	///
	/// @return primary_register_data *
	////////////////////////////////////////////
	virtual primary_register_data * get_primary_register_data() = 0;

	/// @see I_gr_config
	virtual bool parse_config( std::string _str);

  /// @see I_event_switch
  virtual void disable_events();
  
  /// @see I_event_switch
  virtual void enable_events();
  
	/// reference to register data interface representing the output buffer
	I_register_data & o;

	/// reference to register data interface representing the input buffer
	I_register_data & i;

	/// bit accessor provides dynamic creation of bits when necessary
	bit_accessor b;

	/// bit range accessor provides storage of bit ranges and bit range notification rules
	bit_range_accessor br;

protected:

	////////////////////////////////////////////
	/// set_bit
	/// sets a particular bit to specified data without
	/// using the bit accessor, allowing key functionality
	/// without memory overhead if needed in minimalist use models
	///
	/// @param _reg ? register offset within device
	/// @param _bit ? bit within register
	/// @param _data ? new data to write (true or false)
	////////////////////////////////////////////
	inline void set_bit( gr_uint_t & _reg, gr_uint_t & _bit, const bool & _data)
	{
                //TODO PA: _data is replaced with constants because when _data is false,
                //(~_data) becomes 0xffffffff whereas it is required to be 0x1
		if( _data)
		{
			//_reg |= _data << _bit;
			_reg |= (0x1) << _bit;
		} else {
			//_reg &= ~( (~_data) << _bit);
			_reg &= ~( (0x1) << _bit);
		}
	}

	/// storage for reference of the register type (combination of enums, see above)
	unsigned int m_register_type;
};

} // end namespace gs:reg
} // end namespace gs:reg

#endif // _I_REGISTER_H_

#endif // EXCLUDE_FROM_BUILD
#undef EXCLUDE_FROM_BUILD
