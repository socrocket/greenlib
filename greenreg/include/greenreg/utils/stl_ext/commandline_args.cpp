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


#define EXCLUDE_FROM_BUILD
#ifndef EXCLUDE_FROM_BUILD
#undef EXCLUDE_FROM_BUILD

#include "commandline_args.h"

#include <vector>

#include "greenreg/utils/stl_ext/string_utils.h"

namespace gs {
namespace reg_utils {

commandline_args::commandline_args()
{
	args.define_error_value( "DRF VALUE ERROR");
	args.define_error_value( "DRF KEY ERROR");
}

commandline_args::~commandline_args()
{
}

void commandline_args::parse_args( int argc, char** argv)
{
	std::string key = "";
	std::string value = "";

	for( int i = 1; i < argc; i++)
	{
		key = argv[i];

		// strip commas and whitespace
		std::string::size_type location = key.find( " ", 0);
		
		key = gs::reg_utils::string::trim_all( key, " ,\t");
		key = gs::reg_utils::string::trim_leading( key, "-");

		// determine if this is a key value pair
		key = gs::reg_utils::string::to_lower( key);
		if( gs::reg_utils::string::does_pattern_match( key, "*=*", "*"))
		{
			std::vector< std::string> key_val_pair;
			gs::reg_utils::string::parse( key, key_val_pair, "=");
			key = key_val_pair[0];
			value = key_val_pair[1];

		// if not set value to true
		} else {
			value = "true";
		}

		// assign map entry
		args.add_entry( key, value);
	}
}

} // end namespace gs:reg_utils
} // end namespace gs:reg_utils

#endif
