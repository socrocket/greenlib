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

#ifndef ACCESS_DATA_CONTAINER_REF_H_
#define ACCESS_DATA_CONTAINER_REF_H_

#include "greenreg/utils/storage/access_data_container.h"

namespace gs {
namespace reg_utils {

////////////////////////////////////////////
/// access_data_container_ref
/// provides a reference to an access
/// data container for the SOLE purpose of
/// enabling "user candy" through the overloaded
/// operator [] for access purposes.  This
/// prevents the user from having to access
/// these operators as (*my_container_ref)["abc"]
/// and instead enables the use of my_container_ref["abc"]
///
/// @author 
/// @since 
////////////////////////////////////////////
template< class DATA_TYPE>
class access_data_container_ref
{
public:
	access_data_container_ref() {}
	
	////////////////////////////////////////////
	/// operator[] (unsigned int)
	/// encapsulates the dereference of id based
	/// access to operator[] for the referenced
	/// access_data_container.
	///
	/// the id MUST be valid or an exception will occur
	/// or the program may crash.  These are convienience
	/// API's, use the get methods of access_data_container
	/// for validated access.
	///
	/// @param _id ? id to lookup
	/// @return reference to data type
	///
	/// @see access_data_container::operator[]( unsigned int)
	////////////////////////////////////////////
	inline DATA_TYPE & operator[] ( unsigned int _id)
	{ 
		DATA_TYPE * temp = m_container->get_data( _id);
		if( temp == NULL)
			std::cout << "\naccess_data_container::operator[] of id " << _id << " is invalid, application will crash.\n";
		return(*temp); 
	}
	
	////////////////////////////////////////////
	/// operator[] (string)
	/// encapsulates the dereference of key based
	/// access to operator[] for the referenced
	/// access_data_container.
	///
	/// the key MUST be valid or an exception will occur
	/// or the program may crash.  These are convienience
	/// API's, use the get methods of access_data_container
	/// for validated access.
	///
	/// @param _key ? key string to lookup
	/// @return reference to data type
	///
	/// @see access_data_container::operator[](string)
	////////////////////////////////////////////
	inline DATA_TYPE & operator[] ( std::string _key)
	{ 
		DATA_TYPE * temp = m_container->get_data( _key);
		if( temp == NULL)
			std::cout << "\naccess_data_container::operator[] of key " << _key << " is invalid, application will crash.\n";
		return(*temp); 
	}
	
	////////////////////////////////////////////
	/// operator->()
	/// provides pointer access to the referenced
	/// access_data_container for method calling purposes.
	///
	/// @return pointer to contained access_data_container
	///
	/// @see access_data_container
	////////////////////////////////////////////
	inline access_data_container< DATA_TYPE> * operator->() const
	{ 
		return m_container; 
	}
	
	////////////////////////////////////////////
	/// operator = 
	/// nice way of setting the contained
	/// reference to access_data_container.
	///
	/// this method supports pass by reference assignment
	///
	/// @param _value ? access_data_container to reference
	/// @return *this
	////////////////////////////////////////////
	inline access_data_container_ref< DATA_TYPE> & operator = ( access_data_container< DATA_TYPE> & _value) 
	{ m_container = & _value; return( *this); }
	
	////////////////////////////////////////////
	/// operator = 
	/// nice way of setting the contained
	/// reference to access_data_container.
	///
	/// this method supports pointer assignment
	///
	/// @param _value ? access_data_container to reference
	/// @return *this
	////////////////////////////////////////////
	inline access_data_container_ref< DATA_TYPE> & operator = ( access_data_container< DATA_TYPE> * _value) 
	{ m_container = _value; }
	
	////////////////////////////////////////////
	/// get_container
	/// an alternative to operator->()
	///
	/// @see access_data_container_ref::operator->()
	////////////////////////////////////////////
	inline access_data_container< DATA_TYPE> * get_container()
	{ return( m_container); }
	
private:

	/// container that is referenced for operations
	access_data_container< DATA_TYPE> * m_container;
};

} // end namespace gs:reg_utils
} // end namespace gs:reg_utils

#endif /*ACCESS_DATA_CONTAINER_REF_H_*/

#endif // EXCLUDE_FROM_BUILD 
#undef EXCLUDE_FROM_BUILD

