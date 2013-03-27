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


#ifndef STANDARD_REGISTER_DATA_H_
#define STANDARD_REGISTER_DATA_H_

#include "greenreg/gr_externs.h"

#include "greenreg/internal/registers/register_data/I_register_data.h"

namespace gs {
namespace reg {

class I_register;

////////////////////////////////////////////
/// standard_register_data
/// register data that is not bound to the block.
/// this acts as a stand in for output buffer
/// @author 
/// @since 
////////////////////////////////////////////
class standard_register_data : public I_register_data
{
public:

	////////////////////////////////////////////
	/// Constructor
	/// Width is unimportant here, as it is controlled by the actual register
	///
	/// @param _register ? reference to register that will own this data
	/// @param _data ? default data
	/// @param _write_mask ? write mask for this data buffer
	/// @param _lock_mask ? lock mask for this data buffer
	///
	/// @see I_register
	////////////////////////////////////////////
	standard_register_data( I_register & _register, std::string _name, uint_gr_t _data, uint_gr_t _write_mask, uint_gr_t _lock_mask);

	////////////////////////////////////////////
	/// Destructor
	////////////////////////////////////////////
	virtual ~standard_register_data();

	/// @see I_register_data
	inline virtual operator uint_gr_t ()
	{ return( m_data); }

	/// @see I_register_data
	inline virtual I_register_data& operator = ( const uint_gr_t & _val)
	{
		set( _val);
		return( *this);
	}

	/// @see I_register_data
	inline virtual uint_gr_t get()
	{ return( m_data); }

	/// @see I_register_data
	virtual void set( uint_gr_t _data, bool _warn_write_mask = true);

	/// @see I_register_data
	inline virtual void put( uint_gr_t _data)
	{ set( _data);}

	/// @see I_register_data
	inline virtual void set_write_mask( uint_gr_t _mask)
	{ m_write_mask = _mask; }

	/// @see I_register_data
	inline virtual uint_gr_t & get_write_mask()
	{ return( m_write_mask); }

	/// @see I_register_data
	inline virtual void set_lock_mask( uint_gr_t _mask)
	{ m_lock_mask = _mask; }

	/// @see I_register_data
	inline virtual uint_gr_t & get_lock_mask()
	{ return( m_lock_mask); }

protected:

	/// storage data for this buffer, which is only used for splitio registers on the input buffer (for now)
	uint_gr_t m_data;
};

} // end namespace gs:reg
} // end namespace gs:reg

#endif /*STANDARD_REGISTER_DATA_H_*/
