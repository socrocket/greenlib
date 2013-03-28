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

#include "greenreg/utils/storage/composite_container.h"

#include "greenreg/utils/storage/dynamic_classtype_enumeration.h"

using namespace gs::reg_utils;

dynamic_classtype_enumeration< composite_container> gs::reg_utils::g_composite_containers_enum;

composite_container::composite_container()
{
}

composite_container::~composite_container() 
{
	std::map< std::string, composite_container *>::iterator mit;
	for( unsigned int i = number_of_containers(); i > 0; i--)
	{
		mit = m_containers.begin();
		delete( mit->second);
		m_containers.erase(mit);
	}
}

bool composite_container::has_containers() 
{ 
	return (m_containers.size() > 0 ? true : false); 
}

unsigned int composite_container::number_of_containers() 
{ 
	return m_containers.size(); 
}

std::vector< std::string> composite_container::get_container_names()
{
	std::vector< std::string> retvec;
	std::map< std::string, composite_container *>::iterator mit;
	for( mit = m_containers.begin(); mit != m_containers.end(); mit++)
	{
		retvec.push_back( mit->first);
	}
	return( retvec);
}

composite_container * composite_container::get_container( std::string _name)
{ 
	return( m_containers.find( _name) != m_containers.end() ? m_containers.find( _name)->second : NULL); 
}

#endif // EXCLUDE_FROM_BUILD 
#undef EXCLUDE_FROM_BUILD
