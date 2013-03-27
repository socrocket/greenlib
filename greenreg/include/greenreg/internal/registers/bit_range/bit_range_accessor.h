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
// 2009-01-27  forward event switch
// 2009-01-29 Christian Schroeder: renamings
// 2009-01-29 FURTHER CHANGES SEE SVN LOG!
//


#ifndef BIT_RANGE_ACCESSOR_H_
#define BIT_RANGE_ACCESSOR_H_

#include <map>
#include <string>

#include "greenreg/gr_externs.h"
#include "greenreg/framework/core/gr_common.h"

namespace gs {
namespace reg {

class I_register;
class bit_range;

////////////////////////////////////////////
/// bit_range_accessor
/// This could be considered the user accessability class
/// to bit ranges.  It serves at the sole point of creation
/// and storage of bit ranges, as well as providing both
/// user model and DRF internal specific accessibility features
/// @author 
/// @since 
////////////////////////////////////////////
class bit_range_accessor
{
public:
	////////////////////////////////////////////
	/// Constructor
	///
	/// @param _register ? reference to parent regsiter
	///
	/// @see I_register
	////////////////////////////////////////////
	bit_range_accessor( I_register & _register);

	////////////////////////////////////////////
	/// Destructor
	////////////////////////////////////////////
	virtual ~bit_range_accessor();

	////////////////////////////////////////////
	/// operator []
	/// accessability operator for bit ranges
	///
	/// @param _bit_range ? name of bit range to access
	/// @return reference to bit range if valid, error message and mock bit range otherwise
	///
	/// @see bit_range
	////////////////////////////////////////////
	bit_range & operator [] ( std::string _bit_range);

	////////////////////////////////////////////
	/// create
	/// method for creating a bit range
	///
	/// @param _bit_range_name ? unique name of bit range (duplicates ignored)
	/// @param _start_bit ? bit the range starts on (0 being first bit in register)
	/// @param _stop_bit ? bit the range ends on (must be > start bit, and less than reg width)
	/// @return true if successful
	////////////////////////////////////////////
	bool create( std::string _bit_range_name, gr_uint_t _start_bit, gr_uint_t _stop_bit);

private:

	friend class standard_register_data;
	friend class primary_register_data;
  friend class I_register;

	////////////////////////////////////////////
	/// process_tlm_pre_read_rules
	/// executes stored pre read notification rules
	/// for this bit range, activated by the *_register_data classes.
	/// method adheres to a pre-defined function pointer definition.
	///
	/// @param _old_value ? value before the action
	/// @param _new_value ? value after the action (if post, otherwise same)
  /// @param _transaction ? transaction initiating this call
  /// @param _delayed ? if the notifications shall be delayed or called directly
	////////////////////////////////////////////
	void process_tlm_pre_read_rules( uint_gr_t _old_value, uint_gr_t _new_value, transaction_type* _transaction = NULL, bool _delayed = false);

	////////////////////////////////////////////
	/// process_tlm_post_read_rules
	/// executes stored post read notification rules
	/// for this bit range, activated by the *_register_data classes.
	/// method adheres to a pre-defined function pointer definition.
	///
	/// @param _old_value ? value before the action
	/// @param _new_value ? value after the action (if post, otherwise same)
  /// @param _transaction ? transaction initiating this call
  /// @param _delayed ? if the notifications shall be delayed or called directly
	////////////////////////////////////////////
	void process_tlm_post_read_rules( uint_gr_t _old_value, uint_gr_t _new_value, transaction_type* _transaction = NULL, bool _delayed = false);

	////////////////////////////////////////////
	/// process_tlm_pre_write_rules
	/// executes stored pre write notification rules
	/// for this bit range, activated by the *_register_data classes.
	/// method adheres to a pre-defined function pointer definition.
	///
	/// @param _old_value ? value before the action
	/// @param _new_value ? value after the action (if post, otherwise same)
  /// @param _transaction ? transaction initiating this call
  /// @param _delayed ? if the notifications shall be delayed or called directly
	////////////////////////////////////////////
	void process_tlm_pre_write_rules( uint_gr_t _old_value, uint_gr_t _new_value, transaction_type* _transaction = NULL, bool _delayed = false);

	////////////////////////////////////////////
	/// process_tlm_post_write_rules
	/// executes stored post write notification rules
	/// for this bit range, activated by the *_register_data classes.
	/// method adheres to a pre-defined function pointer definition.
	///
	/// @param _old_value ? value before the action
	/// @param _new_value ? value after the action (if post, otherwise same)
  /// @param _transaction ? transaction initiating this call
  /// @param _delayed ? if the notifications shall be delayed or called directly
	////////////////////////////////////////////
	void process_tlm_post_write_rules( uint_gr_t _old_value, uint_gr_t _new_value, transaction_type* _transaction = NULL, bool _delayed = false);

	////////////////////////////////////////////
	/// process_ibuf_write_rules
	/// executes stored ibuf (post) write notification rules
	/// for this bit range, activated by the *_register_data classes.
	/// method adheres to a pre-defined function pointer definition.
	///
	/// @param _old_value ? value before the action
	/// @param _new_value ? value after the action (if post, otherwise same)
	////////////////////////////////////////////
	void process_ibuf_write_rules( uint_gr_t _old_value, uint_gr_t _new_value);

	////////////////////////////////////////////
	/// process_obuf_write_rules
	/// executes stored obuf (post) write notification rules
	/// for this bit range, activated by the *_register_data classes.
	/// method adheres to a pre-defined function pointer definition.
	///
	/// @param _old_value ? value before the action
	/// @param _new_value ? value after the action (if post, otherwise same)
	////////////////////////////////////////////
	void process_obuf_write_rules( uint_gr_t _old_value, uint_gr_t _new_value);

  /// @see I_event_switch
  void disable_events();
  
  /// @see I_event_switch
  void enable_events();
  
	/// reference to owning register
	I_register * m_register;

	/// map of bit ranges held by this container
	std::map< std::string, bit_range * > m_bits;
};

} // end namespace gs:reg
} // end namespace gs:reg

#endif /*BIT_RANGE_ACCESSOR_H_*/
