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


#include "greenreg/internal/registers/bit_range/bit_range_data.h"

#include "greenreg/internal/registers/register_data/I_register_data.h"
#include "greenreg/internal/registers/bit_range/bit_range.h"

#include "greenreg/sysc_ext/utils/gr_report.h"
#include <sstream>

namespace gs {
namespace reg {

bit_range_data::bit_range_data()
: m_register_data( 0),
m_parent( 0),
m_write_rules( 0)
{
}

void bit_range_data::post_init( I_register_data & _register_data, bit_range & _parent)
{
  m_register_data = & _register_data;
  m_parent = & _parent;
}

bit_range_data::~bit_range_data()
{
  if( m_write_rules != NULL)
  {
    delete( m_write_rules);
    m_write_rules = NULL;
  }
}

uint_gr_t bit_range_data::get()
{
  // read
  uint_gr_t reg = m_register_data->get();

  // modify
  reg = (reg >> m_parent->m_bit_range_start) & ~( ~0x0 << ((m_parent->m_bit_range_end + 1) - m_parent->m_bit_range_start));

  // return
  return( reg);  
}

void bit_range_data::set( const uint_gr_t & _data)
{
  // Do NOT check write mask for this internal access!
  /*uint_gr_t write_mask = m_register_data->get_write_mask();
  if ( ((~write_mask) & m_parent->m_bit_range_mask) != 0x0) {
    GR_REPORT_FORCE_WARNING("write_protected/bit_range_access", "User write to write protected bit_range detected! Will be ignored for protected bits.");
  }*/
  // read
  uint_gr_t reg = m_register_data->get();
  // modify
  // clear bits to be modified
  reg &= ~(m_parent->m_bit_range_mask);
  // merge new value
  reg |= (_data << m_parent->m_bit_range_start) & m_parent->m_bit_range_mask;

  // write
  m_register_data->set( reg, false);
}

} // end namespace gs:reg
} // end namespace gs:reg
