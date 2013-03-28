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


#include "greenreg/internal/registers/bit/bit_accessor.h"

#include "greenreg/internal/registers/register/I_register.h"
#include "greenreg/internal/registers/bit/bit.h"
#include "greenreg/internal/registers/bit/splitio_bit.h"
#include "greenreg/internal/registers/bit/sharedio_bit.h"

namespace gs {
namespace reg {

bit_accessor::bit_accessor( I_register & _register)
: m_register( & _register)
{
}

bit_accessor::~bit_accessor()
{
	unsigned int size = m_bits.size();
	while( size != 0)
	{
		std::map< gr_uint_t, bit *>::iterator mit = m_bits.begin();
		delete( mit->second);
		m_bits.erase( mit);
		size--;
	}
}

bit & bit_accessor::operator [] ( gr_uint_t _bit_id)
{
	if( m_bits.find( _bit_id) == m_bits.end())
	{
		bit * b = NULL;

		if( (m_register->get_register_type() & SPLIT_IO) > 0)
		{
			b = new splitio_bit( _bit_id, *m_register);
		} else {
			b = new sharedio_bit( _bit_id, *m_register);
		}

		m_bits[ _bit_id] = b;

		return( *b);
	}

	return( *(m_bits[ _bit_id]));
}

} // end namespace gs:reg
} // end namespace gs:reg
