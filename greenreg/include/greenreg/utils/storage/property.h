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


/// NOTE: This class is currently not valid and in an undertermined state of use
/// PLEASE DISREGARD, comments have been stripped.

#define EXCLUDE_FROM_BUILD
#ifndef EXCLUDE_FROM_BUILD
#undef EXCLUDE_FROM_BUILD

#ifndef PROPERTY_H_
#define PROPERTY_H_

#include <string>

#include "I_property.h"
#include "greenreg/utils/stl_ext/string_utils.h"

namespace gs {
namespace reg_utils {

template< class TYPE>
class property : public I_property
{
public:
	property( std::string _name, TYPE _data, std::string _description)
	: m_name( _name), m_prop( _data), m_description( _description)
	{
	}
	
	virtual ~property()
	{}

	virtual std::string get_property_as_string() const
	{ return( gs::reg_utils::string::to_string( m_prop)); }
	
	virtual void set_property_as_string( std::string & _prop)
	{ gs::reg_utils::string::from_string( _prop, m_prop); }
	
	virtual std::string get_property_name() const
	{ return( m_name); }
	
	virtual std::string get_property_implementation_type() const
	{ return( std::string( "property"));}
	
	virtual std::string get_property_description() const
	{ return( m_description); }
	
	operator TYPE& () 
	{ return m_prop; }
	
	property & operator = ( const TYPE & _type)
	{ m_prop = _type; return *this; }
	
protected:

	std::string m_name;
	TYPE m_prop;
	std::string m_description;
};

} // end namespace gs:reg_utils
} // end namespace gs:reg_utils

#endif /*PROPERTY_H_*/

#ifndef COMPOSITE_MAP_H_
#define COMPOSITE_MAP_H_

