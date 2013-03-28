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

#include "greenreg/utils/storage/dynamic_classtype_enumeration.h"

using namespace gs::reg_utils;

I_dynamic_classtype_enumeration::I_dynamic_classtype_enumeration() : m_id_counter(0) 
{
}

I_dynamic_classtype_enumeration::~I_dynamic_classtype_enumeration()
{
}

bool I_dynamic_classtype_enumeration::has_type( std::string _type) 
{
	return( m_typename_to_ids.find( _type) != m_typename_to_ids.end() ? true : false); 
}

unsigned int I_dynamic_classtype_enumeration::get_type_id( std::string _type)
{ 
	return( m_typename_to_ids.find( _type) != m_typename_to_ids.end() ? m_typename_to_ids[ _type] : 0); 
} 

std::string I_dynamic_classtype_enumeration::get_type( unsigned int _id)
{ 
	return( m_ids_to_typename.find( _id) != m_ids_to_typename.end() ? m_ids_to_typename[ _id].m_type : ""); 
} 

std::string I_dynamic_classtype_enumeration::get_printable_type( unsigned int _id)
{ 
	return( m_ids_to_typename.find( _id) != m_ids_to_typename.end() ? m_ids_to_typename[ _id].m_printable_name : ""); 
} 

unsigned int I_dynamic_classtype_enumeration::create_enumeration_entry( I_dynamic_enum_type * _enum_type)
{
	std::map< std::string, unsigned int>::iterator mit;

	#ifdef _WINDOWS
		mit = m_typename_to_ids.find( _enum_type->get_rtti_type_id());
	#else
		mit = m_typename_to_ids.find( _enum_type->get_rtti_printable_type_id());
	#endif

	if( mit == m_typename_to_ids.end()) {
		m_id_counter++;
		_enum_type->m_type_id = m_id_counter;
		typename_t names;
		
		#ifdef _WINDOWS
			names.m_type = _enum_type->get_rtti_type_id();
		#else
			names.m_type = _enum_type->get_rtti_printable_type_id();
		#endif // _WINDOWS
		
		names.m_printable_name = _enum_type->get_rtti_printable_type_id();
		m_typename_to_ids[ names.m_type] = m_id_counter;
		m_ids_to_typename[ m_id_counter] = names;
	} else {
		_enum_type->m_type_id = mit->second;
	}

	return( _enum_type->m_type_id);
}

#endif // EXCLUDE_FROM_BUILD 
#undef EXCLUDE_FROM_BUILD
