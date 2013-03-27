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

#ifndef _ADDRESSABLE_CONTAINER_H_
#define _ADDRESSABLE_CONTAINER_H_

#include <map>
#include <cassert>

#include "I_addressable_container.h"

namespace gs {
namespace reg_utils {
    
template< typename KEY, typename VALUE>
class addressable_container : public I_addressable_container< KEY, VALUE>
{
public:
	addressable_container() : m_contents() 
		{}

	virtual ~addressable_container()
	{
		// by default do nothing
	}
	
	virtual inline bool is_valid_key( KEY _key)
	{
		return( (m_contents.find( _key) != m_contents.end()) );
	}
		
	virtual inline VALUE & operator [] (KEY _key) 
	{ 
		assert( m_contents.find( _key) != m_contents.end());
		return( *(m_contents[ _key])); 
	}

	virtual inline bool add( KEY _key, VALUE * _value)
	{
		if( m_contents.find( _key) == m_contents.end())
		{
			m_contents[ _key] = _value;
			return true;
		}
		return false;
	}

protected:

	std::map< KEY, VALUE *> m_contents;
};

} // end namespace gs:reg_utils
} // end namespace gs:reg_utils


#endif // _ADDRESSABLE_CONTAINER_H_

#endif // EXCLUDE_FROM_BUILD 
#undef EXCLUDE_FROM_BUILD
