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


#ifndef BIT_H_
#define BIT_H_

#include "greenreg/gr_externs.h"

#include "greenreg/internal/registers/register/I_register.h"
#include "greenreg/internal/registers/bit/bit_data.h"

namespace gs {
namespace reg {

////////////////////////////////////////////
/// bit
/// definition of a bit and entry point to
/// backend accessability.
/// @author 
/// @since 
////////////////////////////////////////////
class bit
{
public:

	////////////////////////////////////////////
	/// Constructor
	/// The ability to configure both the input and output buffer via
	/// external references enables features such as split i/o, or double buffer,
	/// or any other type of specialized buffering system to be bound to
	/// the bit, and is an important key to DRF's flexability.
	///
	/// @param _id ? essentially, the bit number
	/// @param _register ? parent register (owner)
	/// @param _i ? register data interface for the input buffer
	/// @param _o ? register data interface for the output buffer
	///
	/// @see I_register, bit_data
	////////////////////////////////////////////
	bit( uint_gr_t & _id, I_register & _register, bit_data & _i, bit_data & _o)
	: m_id( _id),
	m_register( &_register),
	o( _o),
	i( _i)
	{;}

	////////////////////////////////////////////
	/// Destructor
	////////////////////////////////////////////
	virtual ~bit()
	{;}

	////////////////////////////////////////////
	/// operator () for type uint_gr_t
	/// provides access casting and typically should bind
	/// to the input buffer (write from the bus, read from the model)
	///
	/// @return copy of data in input buffer
	////////////////////////////////////////////
	inline operator bool ()
	{ return i.get(); }

	////////////////////////////////////////////
	/// operator = (uint_gr_t)
	/// provides assignment casting and typically should bind
	/// to the output buffer (write from model, read from bus)
	///
	/// @param _val ? the value to be assigned to the register
	/// @return refernce to self
	////////////////////////////////////////////
	inline bit & operator = ( const bool & _val)
	{
		o.set( _val);
		return( *this);
	}

	////////////////////////////////////////////
	/// get
	///
	/// @return data in input buffer (same as operator () )
	////////////////////////////////////////////
	inline virtual bool get()
	{ return i.get(); }

	////////////////////////////////////////////
	/// set
	///
	/// @param _data ? set data in write buffer (same as operator = )
	////////////////////////////////////////////
	inline virtual void set( const bool & _data)
	{ o.set( _data); }

	////////////////////////////////////////////
	/// put
	/// another common name for set in TLM terminology
	///
	/// @param _data ? set data in write buffer (same as operator = )
	///
	/// @see set
	////////////////////////////////////////////
	inline virtual void put( const bool & _data)
	{ o.put( _data); }

	////////////////////////////////////////////
	/// is_writable
	/// check to see if bit is writable
	///
	/// @return true if writable
	////////////////////////////////////////////
	inline const bool is_writable()
	{ return i.is_writable(); }

	////////////////////////////////////////////
	/// set_writable
	///
	/// @param _writeable ? true for writable
	////////////////////////////////////////////
	inline void set_writable( bool _writeable)
	{ i.set_writable( _writeable); }

	////////////////////////////////////////////
	/// is_locked
	/// intended for a locking mask, not used at this time
	///
	/// @return true if locked
	////////////////////////////////////////////
	inline const bool is_locked() const
	{ return i.is_locked(); }

	////////////////////////////////////////////
	/// lock
	/// intended for a locking mask, not used at this time
	////////////////////////////////////////////
	inline void lock()
	{ i.lock(); }

	////////////////////////////////////////////
	/// bit_unlock
	/// intended for a locking mask, not used at this time
	////////////////////////////////////////////
	inline void unlock()
	{ i.unlock(); }

	/// which bit this is (it's id)
	uint_gr_t m_id;

	/// reference to the parent register (owner)
	I_register * m_register;

	/// reference to bit data representing the output buffer
	bit_data & o;

	/// reference to bit data representing the input buffer
	bit_data & i;
};

} // end namespace gs:reg
} // end namespace gs:reg

#endif /*BIT_H_*/
