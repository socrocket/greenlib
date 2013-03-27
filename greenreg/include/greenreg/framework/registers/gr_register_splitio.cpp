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
// 2008-11-27 Christian Schroeder: added gs_param_gr
// 2009-01-21 Christian Schroeder: changed namespace
// 2009-01-29 Christian Schroeder: renamings
// 2009-01-29 FURTHER CHANGES SEE SVN LOG!
//


#include "greenreg/framework/registers/gr_register_splitio.h"

#include "greenreg/utils/stl_ext/string_utils.h"

namespace gs {
namespace reg {

gr_register_splitio::gr_register_splitio( register_container & _container, std::string _name, std::string _description,
	uint_gr_t _offset, uint_gr_t _data, uint_gr_t _input_mask,
	unsigned int _type, uint_gr_t _width, uint_gr_t _lock_mask)
: _register( _container, _name, _description, _offset, _type, _width, m_i_data, m_o_data),
m_i_data( *this, _name, _data, _input_mask, _lock_mask),
m_o_data( _name.c_str(),
 	*this, _container,
 	_offset, _data, _input_mask, _lock_mask)
{
  add_param_attribute(gs::cnf::param_attributes::gr_register); // set attribute of gs param
  add_param_attribute(gs::cnf::param_attributes::gr_splitio_register); // set attribute of gs param
  init_param(); // calls init function of base gs_param
}

gr_register_splitio::~gr_register_splitio()
{
}

primary_register_data * gr_register_splitio::get_primary_register_data()
{
	return( & m_o_data);
}

} // end namespace gs:reg
} // end namespace gs:reg
