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


#include "greenreg/internal/device/subdevice.h"

// std
#include <iostream>

// greenreg
#include "greenreg/framework/core/gr_common.h"
#include "greenreg/framework/core/I_gr_dump.h"
#include "greenreg/internal/device/I_device.h"
#include "greenreg/sysc_ext/kernel/gr_attribute_type_container.h"
#include "greenreg/framework/registers/gr_register_container_reference_container.h"
#include "greenreg/sysc_ext/kernel/gr_switch_container.h"
#include "greenreg/internal/device/subdevice_container.h"

using namespace std;
using namespace gs::reg_utils;

namespace gs {
namespace reg {

subdevice::subdevice( sc_core::sc_module_name _name, I_device & _parent)
: I_device( _name), m_parent( _parent)
{
	da = _parent.get_device_attribute_container();
	r = _parent.get_device_register_container();
	rc = _parent.get_device_register_container_reference_container();
	m_parent.sd.add_key_data_pair( (const char *)_name, *this);
}

subdevice::~subdevice()
{
}

void subdevice::gr_dump( gr_dump_format_e _format, std::ostream & _stream, unsigned int _tab_level)
{
	_stream << gs::reg_utils::string::tabs( _tab_level).c_str() << basename() << "\n"
			<< gs::reg_utils::string::tabs( _tab_level).c_str() << "{\n";
	_tab_level++;
	a.gr_dump( _format, _stream, _tab_level);
	sd.gr_dump( _format, _stream, _tab_level);
	_tab_level--;
	_stream << gs::reg_utils::string::tabs( _tab_level).c_str() << "}\n\n";
}

} // end namespace gs:reg
} // end namespace gs:reg
