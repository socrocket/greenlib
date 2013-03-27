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


#ifndef I_REGISTER_DATA_H_
#define I_REGISTER_DATA_H_

#include "greenreg/gr_externs.h"
#include "greenreg/framework/core/gr_common.h"

#include "greenreg/internal/registers/register_constants.h"

#include "greenreg/sysc_ext/kernel/gr_notification_rule_container.h"

#include "greenreg/framework/core/I_gr_config.h"

#include "greenreg/utils/greenreg_utils.h"

namespace gs {
namespace reg {

class I_register;

////////////////////////////////////////////
/// I_register_data
/// Interface definition for register data, this allows the data
/// to exist as different implementations, and also as either the same
/// or seperate entities as seen from the register.
/// @author 
/// @since 
////////////////////////////////////////////
class I_register_data : public I_gr_config
{
public:

	////////////////////////////////////////////
	/// Constructor
	///
	/// @param _register ? reference to register that will own this data
	/// @param _write_mask ? write mask for this data buffer
	/// @param _lock_mask ? lock mask for this data buffer
	///
	/// @see I_register
	////////////////////////////////////////////
	I_register_data( I_register & _register, uint_gr_t _write_mask, uint_gr_t _lock_mask)
	: m_parent_register( & _register),
	m_write_mask( _write_mask),
	m_lock_mask( _lock_mask),
	m_write_rules( NULL)
	{;}

	////////////////////////////////////////////
	/// Destructor
	////////////////////////////////////////////
	virtual ~I_register_data()
	{;}

	////////////////////////////////////////////
	/// get_register
	///
	/// @return reference to the parent register (owner)
	///
	/// @see I_register
	////////////////////////////////////////////
	inline I_register * get_register()
	{ return( m_parent_register); }

	////////////////////////////////////////////
	/// operator () for type uint_gr_t
	/// provides access casting for reading
	/// from "THIS" buffer (i.e. does not care if input or output)
	///
	/// @return copy of data in buffer
	////////////////////////////////////////////
	virtual operator uint_gr_t () = 0;

	////////////////////////////////////////////
	/// operator = (uint_gr_t)
	/// provides assignment casting for writing
	/// to "THIS" buffer (i.e. does not care if input or output)
	///
	/// @param _val ? the value to be assigned to the buffer
	/// @return refernce to self
	////////////////////////////////////////////
	virtual I_register_data& operator = ( const uint_gr_t & _val) = 0;

	////////////////////////////////////////////
	/// get
	///
	/// @return data in buffer
	////////////////////////////////////////////
	virtual uint_gr_t get() = 0;

	////////////////////////////////////////////
	/// set
	///
	/// @param _data ? new data to set in buffer
  /// @param _warn_write_mask ? if to warn for violated write mask (e.g. for bit accesses that already checked), default: true, the mask is applied anyway!
	////////////////////////////////////////////
	virtual void set( uint_gr_t _data, bool _warn_write_mask = true) = 0;

	////////////////////////////////////////////
	/// put
	/// another common name for set in TLM terminology
	///
	/// @param _data ? new data to set in buffer
	///
	/// @see set
	////////////////////////////////////////////
	virtual void put( uint_gr_t _data) = 0;

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
	virtual uint_gr_t & get_write_mask() = 0;

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
	virtual uint_gr_t & get_lock_mask() = 0;

	////////////////////////////////////////////
	/// get_write_rules
	/// returns the write rules for this data container (which could be input, output, or other)
	/// the rule container is not created until needed.
	///
	/// @return gr_notification_rule_container for USR_*BUF_WRITE rules
	///
	/// @see gr_notification_rule_container
	////////////////////////////////////////////
	inline gr_notification_rule_container & get_write_rules()
	{
		if( m_write_rules == NULL)
			m_write_rules = new gr_notification_rule_container();
		return( *m_write_rules);
	}

	/// @see I_gr_config
	virtual bool parse_config( std::string _str)
	{
		set( gs::reg_utils::string::hex_string_to_unsigned_long( _str));
		return( true);
	}

protected:
	friend class bit_data;

	/// reference to the parent register
	I_register * m_parent_register;

	/// storage of the write mask
	uint_gr_t m_write_mask;

	/// storage of the lock mask
	uint_gr_t m_lock_mask;

	/// reference to the dynamically created container for write rules (user side)
	gr_notification_rule_container * m_write_rules;
};

} // end namespace gs:reg
} // end namespace gs:reg

#endif /*I_REGISTER_DATA_H_*/
