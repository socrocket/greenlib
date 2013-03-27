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


#include "greenreg/internal/registers/bit/bit_data.h"

#include "greenreg/internal/registers/register_data/I_register_data.h"
#include "greenreg/internal/registers/bit/bit.h"

namespace gs {
namespace reg {

bit_data::bit_data()
: m_register_data( 0),
m_parent( 0)
{
}

void bit_data::post_init( I_register_data & _register_data, bit & _parent)
{
	m_register_data = &_register_data,
	m_parent = &_parent;
}

bit_data::~bit_data()
{
}

bool bit_data::get()
{ 
	return( bool( 0x1 & ( m_register_data->get() >> m_parent->m_id))); 
}

void bit_data::set( const bool & _data)
{ 
  // Do NOT check write mask for this internal access!
  /*uint_gr_t write_mask = m_register_data->get_write_mask();
  if (  ((write_mask >> (m_parent->m_id)) & 0x01) == 1) {
    // calculate and write data
    uint_gr_t val = m_register_data->get();
    bit_set( val, m_parent->m_id, _data);
    m_register_data->set( val, false);
  } else {
    GR_REPORT_FORCE_WARNING("write_protected/bit_access", "User write to write protected bit detected! Will be ignored.");
  }*/
  uint_gr_t val = m_register_data->get();
  bit_set( val, m_parent->m_id, _data);
  m_register_data->set( val, false);
}

void bit_data::put( const bool & _data)
{ set( _data); }

const bool bit_data::is_writable()
{ 
	return( (bool)(m_register_data->m_write_mask >> m_parent->m_id));
}

void bit_data::set_writable( bool _writeable)
{ 
	bit_set( m_register_data->m_write_mask, m_parent->m_id, _writeable);
}

const bool bit_data::is_locked() const
{ 
	return( (bool)(m_register_data->m_lock_mask >> m_parent->m_id));
}

void bit_data::lock()
{ 
	bit_set( m_register_data->m_lock_mask, m_parent->m_id, 1);
}

void bit_data::unlock()
{ 
	bit_set( m_register_data->m_lock_mask, m_parent->m_id, 0);
}

} // end namespace gs:reg
} // end namespace gs:reg
