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


//#define EXCLUDE_FROM_BUILD
#ifndef EXCLUDE_FROM_BUILD
#undef EXCLUDE_FROM_BUILD

#ifndef _OPTION_STRUCTS_H_
#define _OPTION_STRUCTS_H_

#include <vector>
#include <string>

namespace gs {
namespace reg_utils {
	
////////////////////////////////////////////
/// option_t
/// defines an option entry for static options
/// @author 
/// @since 
////////////////////////////////////////////
/*
typedef struct option_ts
{
	/// string name
	std::string m_name;

	/// value
	unsigned int m_value;
} option_t;
*/

typedef std::pair< std::string, unsigned int> option_t;

////////////////////////////////////////////
/// pragma warning disable
/// WARNING: nonstandard extension used : zero-sized array in struct/union
/// RELATED TO: m_options
/// REASON: enable non-deterministic array lengths for static instances of option_list_t
////////////////////////////////////////////
#ifdef _WINDOWS
#pragma warning(disable:4200)
#endif // _WINDOWS

/*
struct option_list_base_t
{
	/// number of options in array
	unsigned int number_options;

	/// value returned when an entry is not found
	unsigned int error_value;
};

////////////////////////////////////////////
/// option_list_t
/// defines struct for static options
/// @author 
/// @since 
////////////////////////////////////////////
template< unsigned int COUNT>
struct option_list_t : option_list_base_t
{
	/// array of options, though there is a compiler warning
	option_t m_options [COUNT]; // staticd
};
*/

////////////////////////////////////////////
/// option_list_t
/// the current SAD implementation for a
/// static options list since one cannot use
/// free-arrays (from C)... I'm looking for
/// alternatives, but for now there are way
/// to many other capabilities to spend any
/// more time here
/// @author 
/// @since 
////////////////////////////////////////////
struct option_list_t
{
public:

	/// defines the default error value in the entries list 
	unsigned int error_value;
	
	/// the vector of option data, you could just use this directly
	std::vector< std::pair<std::string, unsigned int> > m_options;
	
	/// this is a very simple method to add a key, value pair to the vector of options
	/// id's are based on vector element (i.e. index)
	void _( std::string _str, unsigned int _val)
	{
		std::pair< std::string, unsigned int> p;
		p.first = _str;
		p.second = _val;
		m_options.push_back( p);
	}
	
	
};


////////////////////////////////////////////
/// re-enable pragma warning
////////////////////////////////////////////
#ifdef _WINDOWS
#pragma warning(default:4200)
#endif // _WINDOWS

} // end namespace gs::reg_utils
} // end namespace gs::reg_utils

#endif // _OPTION_STRUCTS_H_

#endif // EXCLUDE_FROM_BUILD
#undef EXCLUDE_FROM_BUILD
