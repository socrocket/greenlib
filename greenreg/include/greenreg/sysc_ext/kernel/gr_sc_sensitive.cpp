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
// 2008-12-03 Christian Schroeder: 
//             renamed function gr_event::add_static gr_event::add_callback
// 2009-01-21 Christian Schroeder: changed namespace
// 2009-01-29 Christian Schroeder: renamings
// 2009-01-29 FURTHER CHANGES SEE SVN LOG!
//


#include "greenreg/sysc_ext/kernel/gr_sc_sensitive.h"

#include "greenreg/framework/core/gr_module.h"
#include "greenreg/sysc_ext/kernel/gr_event.h"

namespace gs {
namespace reg {

gr_sc_sensitive::gr_sc_sensitive( gr_module * _gr_module)
: m_gr_module( _gr_module)
{
}

gr_sc_sensitive::~gr_sc_sensitive()
{
}

gr_sc_sensitive& gr_sc_sensitive::operator () ( gr_event& _event)
{
  switch (m_gr_module->m_current_callback->number_params) {
    case 0:
      _event.add_callback( *(m_gr_module->m_current_callback));
      break;
    case 2:
      _event.add_payload_callback( *(m_gr_module->m_current_callback));
      break;
    default:
      assert(false && "Called with wrong callback type");
  }
  return( *this);
}

gr_sc_sensitive& gr_sc_sensitive::operator << ( gr_event& _event)
{
  operator()( _event);
  return( *this);
}

} // end namespace gs:reg
} // end namespace gs:reg
