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
// 2008-11-27 Christian Schroeder: added gs_param_drf
// 2009-01-20 Christian Schroeder: add callback/event switch for notification rules
// 2009-01-21 Christian Schroeder: changed namespace
// 2009-01-29 Christian Schroeder: renamings
// 2009-01-29 FURTHER CHANGES SEE SVN LOG!
//


//#define EXCLUDE_FROM_BUILD
#ifndef EXCLUDE_FROM_BUILD
#undef EXCLUDE_FROM_BUILD

#include "greenreg/internal/registers/register/I_register.h"
#include "greenreg/internal/registers/register_data/standard_register_data.h"

#include "greenreg/utils/greenreg_utils.h"

namespace gs {
namespace reg {

I_register::I_register(unsigned int _register_type, I_register_data & _i, I_register_data & _o, std::string &_name, std::string _param_name_prefix)
: gs::gs_param_greenreg<gr_uint_t   >(_param_name_prefix + "." + _name, true) // gs parameter adapter; init_param() needs to be called at lowest constructor  
, o( _o)
, i( _i)
, b( *this)
, br( *this)
, m_register_type( _register_type)
{
}

I_register::~I_register()
{
}

bool I_register::parse_config( std::string _str)
{
	i.set( gs::reg_utils::string::hex_string_to_unsigned_long( _str));
	o.set( gs::reg_utils::string::hex_string_to_unsigned_long( _str));
	return true;
}

void I_register::disable_events() {
  I_event_switch::disable_events(); // update the state bool
  // switch all owned notification rule container
  get_pre_write_rules().disable_events();
  get_post_write_rules().disable_events();
  get_pre_read_rules().disable_events();
  get_post_read_rules().disable_events();
  get_user_ibuf_write_rules().disable_events();
  get_user_obuf_write_rules().disable_events();
  br.disable_events(); // disable events in bit ranges
}
  
void I_register::enable_events() {
  I_event_switch::enable_events(); // update the state bool
  // switch all owned notification rule container
  get_pre_write_rules().enable_events();
  get_post_write_rules().enable_events();
  get_pre_read_rules().enable_events();
  get_post_read_rules().enable_events();
  get_user_ibuf_write_rules().enable_events();
  get_user_obuf_write_rules().enable_events();
  br.enable_events(); // disable events in bit ranges
}
  
} // end namespace gs::reg
} // end namespace gs::reg

#endif // EXCLUDE_FROM_BUILD
#undef EXCLUDE_FROM_BUILD
