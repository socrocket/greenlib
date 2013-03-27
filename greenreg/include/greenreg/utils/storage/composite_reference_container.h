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

#ifndef _COMPOSITE_REFERENCE_CONTAINER_H_
#define _COMPOSITE_REFERENCE_CONTAINER_H_

#include <typeinfo>

#include "composite_container.h"
#include "greenreg/utils/storage/data_container.h"

namespace gs {
namespace reg_utils {

////////////////////////////////////////////
/// composite_reference_container
/// combines composite_container and
/// data_container while also registering
/// itself (as templated definition of DATA_TYPE)
/// with the global g_composite_containers_enum
/// and implementes rtti access type methods
///
/// This is the end of classes in this templated
/// hierarchy, the user will most likely never see
/// this particular code as it will be hidden with
/// explicit type definitions.
///
/// @author 
/// @since 
///
/// @see data_container
/// @see composite_container
////////////////////////////////////////////
template< typename DATA_TYPE>
class composite_reference_container : public data_container< DATA_TYPE>, public composite_container
{
public:

	composite_reference_container( bool _owns_objects) : data_container< DATA_TYPE>( _owns_objects), composite_container(), m_owns_data( _owns_objects)
	{
		// _register dynamic enum type
		//   composite_container is the template type of the dynamic enumeration storage template
		//   composite_reference_container is the leaf type ( explicit inherited template type of the non templated base( composite_container)
		//   the global definition for the composite containers enum is passed along
		//   RTTI will then be used to implement all the magic.
		_register< composite_container, composite_reference_container< DATA_TYPE> >( g_composite_containers_enum);
	}

	composite_reference_container() : data_container< DATA_TYPE>( true), composite_container(), m_owns_data( true)
	{
		// _register dynamic enum type
		//   composite_container is the template type of the dynamic enumeration storage template
		//   composite_reference_container is the leaf type ( explicit inherited template type of the non templated base( composite_container)
		//   the global definition for the composite containers enum is passed along
		//   RTTI will then be used to implement all the magic.
		_register< composite_container, composite_reference_container< DATA_TYPE> >( g_composite_containers_enum);
	}

	composite_reference_container< DATA_TYPE> & operator[]( std::string _name)
	{
		composite_container * cc = composite_container::get_container( _name);

		// at all costs, do not throw an error here
		if( cc == NULL)
			cc = this;

		return( *(static_cast< composite_reference_container< DATA_TYPE> *>(cc)));
	}

	composite_reference_container< DATA_TYPE> * get_container( std::string _name)
	{
		composite_container * cc = composite_container::get_container( _name);
		return( static_cast< composite_reference_container< DATA_TYPE> * >( cc));
	}

	/// @see composite_container
	composite_reference_container< DATA_TYPE> * add_container( std::string _name)
	{
		if( m_containers.find( _name) == m_containers.end())
		{
			m_containers[ _name] = new composite_reference_container< DATA_TYPE>( m_owns_data);
		}
		return( static_cast< composite_reference_container *>(m_containers[ _name]));
	}

	/// @see composite_container
	bool del_container( std::string _name)
	{
		if( m_containers.find( _name) != m_containers.end())
		{
			composite_reference_container< DATA_TYPE> * temp = static_cast< composite_reference_container *>(m_containers[ _name]);
			m_containers.erase( m_containers.find( _name));
			delete( temp);
			return( true);
		}
		return( false);
	}

	DATA_TYPE & operator()( std::string _name)
	{
		DATA_TYPE * temp = data_container< DATA_TYPE>::get_data( _name);

		// at all costs, do not throw an error here
		if( temp == NULL)
			temp = & m_data_type_default;

		return( *temp);
	}

	DATA_TYPE & operator()( unsigned int _id)
	{
		DATA_TYPE * temp = data_container< DATA_TYPE>::get_data( _id);

		// at all costs, do not throw an error here
		if( temp == NULL)
			temp = & m_data_type_default;

		return( *temp);
	}

protected:

	// @see I_dynamic_enum_type
	virtual std::string get_rtti_printable_type_id() { return( typeid( *this).name()); }

#ifdef _WINDOWS
	// @see I_dynamic_enum_type
	virtual std::string get_rtti_type_id() { return( typeid( *this).raw_name()); }
#endif // WINDOWS

	DATA_TYPE m_data_type_default;

	bool m_owns_data;
};

} // end namespace gs::reg_utils
} // end namespace gs::reg_utils

#endif // _COMPOSITE_REFERENCE_CONTAINER_H_

#endif // EXCLUDE_FROM_BUILD
#undef EXCLUDE_FROM_BUILD
