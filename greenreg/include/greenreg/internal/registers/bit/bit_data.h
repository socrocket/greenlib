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


#ifndef BIT_DATA_H_
#define BIT_DATA_H_

#include "greenreg/gr_externs.h"

namespace gs {
namespace reg {

class bit;
class I_register_data;

////////////////////////////////////////////
/// bit_data
/// despite the name, this class does not store
/// the actual bit data, instead it references
/// the owning register for data.  Most importantly this
/// protects the integrity of the data by removing
/// unnecessary synchronization while providing the
/// facade that this class stores real data.
/// @author 
/// @since 
////////////////////////////////////////////
class bit_data
{
public:

	////////////////////////////////////////////
	/// Constructor
	////////////////////////////////////////////
	bit_data();

	////////////////////////////////////////////
	/// post_init
	/// A secondary initialization procedure executed
	/// from the creating code block which passes
	/// in the register data for accessability and the
	/// parent bit range for traversal.  This cannot
	/// be done at construction time because the bit
	/// actually requires the data buffers to be passed
	/// in as parameters.  Thus creating a bit
	/// becomes a multi-step process.
	///
	/// @param _register_data ? register data to reference
	/// @param _parent ? parent object for shared resources
	///
	/// @see bit_range_accessor::create
	////////////////////////////////////////////
	void post_init( I_register_data & _register_data, bit & _parent);

	////////////////////////////////////////////
	/// Destructor
	////////////////////////////////////////////
	virtual ~bit_data();

	////////////////////////////////////////////
	/// get
	///
	/// @return data in buffer
	////////////////////////////////////////////
	bool get();

	////////////////////////////////////////////
	/// set
	///
	/// @param _data ? new data to set in buffer
	////////////////////////////////////////////
	void set( const bool & _data);

	////////////////////////////////////////////
	/// put
	/// another common name for set in TLM terminology
	///
	/// @param _data ? new data to set in buffer
	///
	/// @see set
	////////////////////////////////////////////
	void put( const bool & _data);

	////////////////////////////////////////////
	/// is_writable
	/// check to see if bit is writable
	///
	/// @return true if writable
	////////////////////////////////////////////
	const bool is_writable();

	////////////////////////////////////////////
	/// set_writable
	///
	/// @param _writeable ? true for writable
	////////////////////////////////////////////
	void set_writable( bool _writeable);

	////////////////////////////////////////////
	/// is_locked
	/// intended for a locking mask, not used at this time
	///
	/// @return true if locked
	////////////////////////////////////////////
	const bool is_locked() const;

	////////////////////////////////////////////
	/// lock
	/// intended for a locking mask, not used at this time
	////////////////////////////////////////////
	void lock();

	////////////////////////////////////////////
	/// bit_unlock
	/// intended for a locking mask, not used at this time
	////////////////////////////////////////////
	void unlock();

	////////////////////////////////////////////
	/// operator () for type bool
	/// provides access casting for reading
	/// from "THIS" buffer (i.e. does not care if input or output)
	///
	/// @return copy of data in buffer
	////////////////////////////////////////////
	inline operator bool ()
	{ return get(); }

	////////////////////////////////////////////
	/// operator = (bool &)
	/// provides assignment casting for writing
	/// to "THIS" buffer (i.e. does not care if input or output)
	///
	/// @param _val ? the value to be assigned to the buffer
	/// @return refernce to self
	////////////////////////////////////////////
	inline bit_data & operator = ( bool & _data)
	{
		if( is_writable())
			set( _data);
		return( *this);
	}

	////////////////////////////////////////////
	/// operator = (bool)
	/// provides assignment casting for writing
	/// to "THIS" buffer (i.e. does not care if input or output)
	///
	/// @param _val ? the value to be assigned to the buffer
	/// @return refernce to self
	////////////////////////////////////////////
	inline bit_data & operator = ( bool _data)
	{
		if( is_writable())
			set( _data);
		return( *this);
	}

protected:

	////////////////////////////////////////////
	/// bit_set
	///
	/// @param _reg ? represents the register data to manipulate
	/// @param _bit ? bit to set value for
	/// @param _data ? new value
	////////////////////////////////////////////
	inline void bit_set( gr_uint_t & _reg, gr_uint_t & _bit, const bool & _data)
	{
                //TODO PA: _data is replaced with constants because when _data is false,
                //(~_data) becomes 0xffffffff whereas it is required to be 0x1.
		if( _data)
		{
			//_reg |= _data << _bit;
			_reg |= (0x1) << _bit;
		} else {
			//_reg &= ~( (~_data) << _bit);
			_reg &= ~( (0x1) << _bit);
		}
	}

	/// reference to register data
	I_register_data * m_register_data;

	/// parent bit (owner)
	bit * m_parent;
};

} // end namespace gs:reg
} // end namespace gs:reg

#endif /*BIT_DATA_H_*/
