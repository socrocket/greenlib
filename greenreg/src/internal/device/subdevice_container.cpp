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


#include <vector>
#include <string>

#include "greenreg/internal/device/subdevice_container.h"

using namespace std;
using namespace gs::reg_utils;

namespace gs {
namespace reg {

subdevice_container::subdevice_container()
: access_data_container< subdevice>( false)
{
}

subdevice_container::~subdevice_container()
{
}

void subdevice_container::gr_dump( gr_dump_format_e _format, std::ostream & _stream, unsigned int _tab_level)
{
	subdevice * object = NULL;
	
	// CODE TEMPLATE BELOW
	// dump_for_container
	std::vector< unsigned int> objects;
	std::vector< unsigned int>::const_iterator cvit;
	
	objects = get_all_data_ids();

	if( objects.size() > 0)
	{
	_stream << "\n" << gs::reg_utils::string::tabs( _tab_level).c_str() << "############\n"
			<< gs::reg_utils::string::tabs( _tab_level).c_str() << "# subdevices\n"
			<< gs::reg_utils::string::tabs( _tab_level).c_str() << "############\n\n";
	}

	for( cvit = objects.begin(); cvit != objects.end(); cvit++)
	{
//		_stream << dump_name_type.c_str();
		object = this->get_data( *cvit);
		if( object != NULL)
			object->gr_dump( _format, _stream, _tab_level);
	}
	// END CODE TEMPLATE
}

} // end namespace gs:reg
} // end namespace gs:reg
