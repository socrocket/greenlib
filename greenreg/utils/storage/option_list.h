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

#ifndef _OPTION_LIST_H_
#define _OPTION_LIST_H_

/// standard includes
#include <vector>
#include <string>

/// generic includes
#include "option_structs.h"
#include "lookup_table.h"

namespace gs {
namespace reg_utils {
	
////////////////////////////////////////////
/// option_list
/// defines a functional accessible set of
/// options from either a static struct
/// reference or dynamically
///
/// dynamic and static definitions are not
/// allowed to co-exist in same class instance
/// when using constructor for static 
/// definition the inherited lookup_table will be locked
///
/// @author 
/// @since 
////////////////////////////////////////////
class option_list : public lookup_table<std::string, unsigned int>
{
public:

	////////////////////////////////////////////
	/// Constructor
	/// assumes option list will be stored
	/// dynamically, sets internal variables
	/// as such
	////////////////////////////////////////////
	option_list();

	////////////////////////////////////////////
	/// Constructor
	/// takes a defined option list structure
	/// and fills out the lookup_table. This
	/// constructor will also lock the lookup
	/// table to additional changes cannot
	/// be made.
	////////////////////////////////////////////
	option_list( option_list_t & _options);

/* REMOVED FOR NOW
	////////////////////////////////////////////
	/// lookup
	/// if using static option list method will
	/// use m_options, otherwise calls superclass
	/// @see lookup_table::lookup
	////////////////////////////////////////////
	unsigned int lookup( std::string _key);

	////////////////////////////////////////////
	/// lookup
	/// if using static option list method will
	/// use m_options, otherwise calls superclass
	/// @see lookup_table::lookup
	////////////////////////////////////////////
	std::string lookup( unsigned int _value);

	////////////////////////////////////////////
	/// list_keys
	/// if using static option list method will
	/// use m_options, otherwise calls superclass
	/// @see lookup_table::list_keys
	////////////////////////////////////////////
	std::vector< std::string> list_keys();
*/

protected:

	/// reference to static options list
	std::vector< option_t> m_options;
};

} // end namespace gs::reg_utils
} // end namespace gs::reg_utils


#endif // _OPTION_LIST_H_

#endif // EXCLUDE_FROM_BUILD
#undef EXCLUDE_FROM_BUILD
