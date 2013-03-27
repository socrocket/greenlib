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


#ifndef _GR_DUMPER_H_
#define _GR_DUMPER_H_

#include <string>

#include "greenreg/framework/core/gr_common.h"
#include "greenreg/framework/core/I_gr_dump.h"

namespace gs {
namespace reg {

////////////////////////////////////////////
/// gr_dumper
/// basically an implementation mechanism
/// for calling dump from anything that
/// implements the I_gr_dump interface
/// and the ability to dump out the platform
/// @author 
/// @since 
////////////////////////////////////////////
class gr_dumper
{
public:
	gr_dumper();
	virtual ~gr_dumper();
	
	////////////////////////////////////////////
	/// dump
	/// dumps _gr_object and anything below
	///
	/// @param _file_name ? file name to overwrite
	/// @param _format ? format to output (HARD CODED TO GR_DUMP NOW)
	/// @param _gr_object ? object to start dump from
	/// @return bool : success or failure
	////////////////////////////////////////////
	static bool dump( std::string _file_name, gr_dump_format_e _format, I_gr_dump & _gr_object);
	
	////////////////////////////////////////////
	/// dump_platform
	/// dumps the entire platform
	///
	/// @param _path ? directory to dump out platform to
	/// @param _format ? format to output (HARD CODED TO GR_DUMP NOW)
	/// @return bool : success or failure
	////////////////////////////////////////////
	static bool dump_platform( std::string _path, gr_dump_format_e _format);
};

} // end namespace gs:reg
} // end namespace gs:reg

#endif /*_GR_DUMPER_H_*/
