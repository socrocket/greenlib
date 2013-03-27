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


#include "greenreg/internal/registers/register_data/standard_register_data.h"
#include "greenreg/internal/registers/bit_range/bit_range_accessor.h"
#include "greenreg/internal/registers/register/I_register.h"

namespace gs {
namespace reg {

standard_register_data::standard_register_data(  I_register & _register, std::string _name, uint_gr_t _data, uint_gr_t _write_mask, uint_gr_t _lock_mask)
: I_register_data( _register, _write_mask, _lock_mask)
, m_data( _data)
{
}

standard_register_data::~standard_register_data()
{
}

void standard_register_data::set( uint_gr_t _data, bool _warn_write_mask)
{
  uint_gr_t old_data = m_data;
  /*uint_gr_t masked_value = (_data & ~m_write_mask);
  if (_warn_write_mask) {
    if (masked_value != (m_data & ~m_write_mask)) { // warn if write different than current value to write protected area
      GR_REPORT_FORCE_WARNING("write_protected/unequal_current", "User write to write protected pri_reg_dat area/bits detected (written with bits different from current ones)! Will be ignored for the protected bits.");
    }
    if (masked_value != 0) { // warn if write to write protected area with values different from zero
      GR_REPORT_FORCE_WARNING("write_protected/unequal_zero", "User write to write protected pri_reg_dat area/bits detected (written with bits different from zero)! Will be ignored for the protected bits.");
    }
  }
  // applying write-mask here (for direct user access (and not only bus access)), also see standard_register_data.cpp
  // original: m_data = _data & m_write_mask; // TODO: why not m_data = ( _data & m_write_mask ) | ( old_data & ~m_write_mask ); ???
  _data = ( _data & m_write_mask ) | ( old_data & ~m_write_mask );*/
  m_data = _data;
	if( m_write_rules != NULL)
		m_write_rules->process_active_rules( old_data, _data);
	m_parent_register->br.process_ibuf_write_rules( old_data, _data);
}

} // end namespace gs:reg
} // end namespace gs:reg
