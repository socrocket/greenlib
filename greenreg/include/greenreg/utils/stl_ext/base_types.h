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


#ifndef BASE_TYPES_H_
#define BASE_TYPES_H_

#include "greenreg/utils/storage/option_structs.h"
#include "greenreg/utils/storage/option_list.h"
#include "greenreg/utils/storage/lookup_table.h"

namespace gs {
namespace reg_utils {

////////////////////////////////////////////
/// gr_options_attrib_type
/// attribute types for gr_attribute
/// @author 
/// @since 
////////////////////////////////////////////
enum base_type_e
{
	UNKNOWN = 0,
	INT,
	UINT,
	DOUBLE,
	STRING
};
class options_base_type_t : public gs::reg_utils::option_list_t
{
public:
	options_base_type_t()
	{
		error_value = 0;
		_( "UNKNOWN", UNKNOWN);
		_( "INT", INT);
		_( "UINT", UINT);
		_( "DOUBLE", DOUBLE);
		_( "STRING", STRING);
	}
};

extern options_base_type_t options_base_type;

extern std::string & get_type_string( base_type_e _type);

extern base_type_e determine_type( int & _t);
extern base_type_e determine_type( unsigned int & _t);
extern base_type_e determine_type( double & _t);
extern base_type_e determine_type( std::string & _t);

template< typename T>
struct determine
{
	static base_type_e type()
	{ return( UNKNOWN); }

	static base_type_e type( T & _t)
	{ return( UNKNOWN); }
};

template<>
struct determine< int>
{
	static base_type_e type()
	{ return( INT); }

	static base_type_e type( int & _t)
	{ return( INT); }
};

template<>
struct determine< unsigned int>
{
	static base_type_e type()
	{ return( UINT); }

	static base_type_e type( unsigned int & _t)
	{ return( UINT); }
};

template<>
struct determine< double>
{
	static base_type_e type()
	{ return( DOUBLE); }

	static base_type_e type( double & _t)
	{ return( DOUBLE); }
};

template<>
struct determine< std::string>
{
	static base_type_e type()
	{ return( STRING); }

	static base_type_e type( std::string & _t)
	{ return( STRING); }
};

} // end namespace gs::reg_utils
} // end namespace gs::reg_utils

#endif /*BASE_TYPES_H_*/
