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


/// NOTE: This is a really old class, documentation is a bit out of wack, no time to change now, not being used now.

//#define EXCLUDE_FROM_BUILD
#ifndef EXCLUDE_FROM_BUILD
#undef EXCLUDE_FROM_BUILD

#ifndef _EXT_MAP_H_
#define _EXT_MAP_H_

#include <map>
#include <string>

namespace gs {
namespace reg_utils {

////////////////////////////////////////////
/// ext_map
/// simple extention to standard stl map that
/// provides an easy to use reverse-lookup
/// capability.  Modified from the dual map
/// implementation to conserve memory by taking
/// a hit on performance.
///
/// @author 
/// @since 
////////////////////////////////////////////
template< class KEY, class VALUE>
class ext_map : public std::map< KEY, VALUE>
{
protected:
	typename std::map< KEY, VALUE>::const_iterator cmit;
	VALUE m_empty_value;
public:
	ext_map( const VALUE & _empty_value) : m_empty_value( _empty_value) {}

//	There is some issue with these methods which do not work,
//	obviously something related to templates, but at this time
//	everything looks good and i do not have time to deal with some
//	(probably) compiler level issue...

	/**
	Simple implementation of the get value method returns
	the value for the key,val pair.  If it does not exist
	it simply returns ""
	*/
	VALUE get_value( KEY _key);

	/**
	The following get_value methods provide for checking of a key
	before retrieving the value, as well as provide support for
	various types of values
	*/
	bool get_value( KEY _key, VALUE & _value);
};


template< class KEY, class VALUE>
VALUE ext_map< KEY, VALUE>::get_value( KEY _key)
{
	cmit = find( _key);
	if( cmit != this->end())
		return( cmit->second);
	return m_empty_value;
}

template< class KEY, class VALUE>
bool ext_map< KEY, VALUE>::get_value( KEY _key, VALUE & _value)
{
	cmit = find( _key);
	if( cmit != this->end())
	{
		_value = cmit->second;
		return true;
	}
	return false;
}

typedef ext_map< std::string, std::string> STR_STR_MAP_t;

} // end namespace gs::reg_utils
} // end namespace gs::reg_utils

#endif // _EXT_MAP_H

#endif // EXCLUDE_FROM_BUILD 
#undef EXCLUDE_FROM_BUILD
