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

#ifndef ACCESS_DATA_CONTAINER_H_
#define ACCESS_DATA_CONTAINER_H_

#include "greenreg/utils/storage/data_container.h"

namespace gs {
namespace reg_utils {

////////////////////////////////////////////
/// access_data_container
/// a simple convenience layer (operator[]) for data_container.
/// Now it may seem that data_container should have these
/// operators, but their are particular use cases where
/// these operators are overloaded to do other behaviour
/// (such as composite_reference_container), to the extent
/// that it was just made since to provide a simple separation.
///
/// @author 
/// @since 
////////////////////////////////////////////
template< class DATA_TYPE>
class access_data_container : public data_container< DATA_TYPE>
{
public:
	////////////////////////////////////////////
	/// @see data_container
	////////////////////////////////////////////
	explicit access_data_container( bool _owns_objects)
	: data_container< DATA_TYPE>( _owns_objects)
	{;}

	virtual ~access_data_container()
	{}

	////////////////////////////////////////////
	/// operator[]( unsigned int)
	/// wraps a call to get_data( _id)
	///
	/// @param _id ? id to lookup: MUST BE VALID
	/// @return reference to data type
	///
	/// @see data_container::get_data( unsigned int)
	////////////////////////////////////////////
	inline DATA_TYPE & operator [] ( unsigned int _id)
	{
		return(* data_container< DATA_TYPE>::get_data( _id));
	}

	////////////////////////////////////////////
	/// operator[]( string)
	/// wraps a call to get_data( _key)
	///
	/// @param _key ? key to lookup: MUST BE VALID
	/// @return reference to data type
	///
	/// @see data_container::get_data( string)
	////////////////////////////////////////////
	inline DATA_TYPE & operator[] ( std::string _key)
	{
		return(* data_container< DATA_TYPE>::get_data(_key));
	}
};

} // end namespace gs:reg_utils
} // end namespace gs:reg_utils

#endif /*ACCESS_DATA_CONTAINER_H_*/

#endif // EXCLUDE_FROM_BUILD
#undef EXCLUDE_FROM_BUILD

