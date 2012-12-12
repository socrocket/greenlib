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

#ifndef _DATA_CONTAINER_H_
#define _DATA_CONTAINER_H_

#include <string>
#include <vector>
#include <map>

#include "I_data_container.h"

namespace gs {
namespace reg_utils {

////////////////////////////////////////////
/// data_container
/// implementation of I_data_container
/// @author 
/// @since 
////////////////////////////////////////////
template< typename DATA_TYPE>
class data_container : public I_data_container< DATA_TYPE>
{
public:

	////////////////////////////////////////////
	/// constructor
	/// explicit because user MUST define if the
	/// container _owns_objects or not.
	///
	/// @param _owns_objects ? determines if the container will delete objects at time of destruction
	////////////////////////////////////////////
	explicit data_container( bool _owns_objects) : m_owns_objects( _owns_objects), m_id_counter(0)
	{
	}

	virtual ~data_container()
	{
		// only delete objects if we own them
		if( m_owns_objects)
		{
			typename std::map< unsigned int, detail_pair_t>::iterator mit;
			for( mit = m_idmap.begin(); mit != m_idmap.end(); mit++)
			{
				delete( mit->second.second);
				mit->second.second = NULL;
			}
			m_keymap.clear();
			m_idmap.clear();
		}
	}

	/// @see I_data_container::has_data_elements
	bool has_data_elements() {return( m_idmap.size() == 0 ? false : true);}

	/// @see I_data_container::number_of_data_elements
	unsigned int number_of_data_elements() {return( m_idmap.size());}

	/// @see I_data_container::get_data
	DATA_TYPE * get_data( std::string _key)
	{ return( m_keymap.find( _key) != m_keymap.end() ? m_idmap[ m_keymap[ _key]].second : NULL); }

	void set_data( std::string _key, DATA_TYPE & _data)
	{
		if( m_keymap.find( _key) != m_keymap.end())
		{
			m_idmap[ m_keymap[ _key]].second = & _data;
		}
	}

	/// @see I_data_container::get_data
	DATA_TYPE * get_data( unsigned int _id)
	{ return( m_idmap.find( _id) != m_idmap.end() ? m_idmap[ _id].second : NULL); }

	/// @see I_data_container::get_data_key
	std::string get_data_key( unsigned int _id)
	{ return( m_idmap.find( _id) != m_idmap.end() ? *(m_idmap[ _id].first) : ""); }

	/// @see I_data_container::get_data_id
	unsigned int get_data_id( std::string _key)
	{ return( m_keymap.find( _key) != m_keymap.end() ? m_keymap[ _key] : 99999); }

	/// @see I_data_container::get_all_data_ids
	std::vector< unsigned int> get_all_data_ids()
	{
		std::vector< unsigned int> retvec;
		typename std::map< unsigned int, detail_pair_t>::iterator mit;

		for( mit = m_idmap.begin(); mit != m_idmap.end(); mit++)
		{
			retvec.push_back( mit->first);
		}

		return( retvec);
	}

	/// @see I_data_container::get_all_data_keys
	std::vector< std::string> get_all_data_keys()
	{
		std::vector< std::string> retvec;
		typename std::map< std::string, unsigned int>::iterator mit;

		for( mit = m_keymap.begin(); mit != m_keymap.end(); mit++)
		{
			retvec.push_back( mit->first);
		}

		return( retvec);
	}

	/// @see I_data_container::add_key_data_pair
	unsigned int add_key_data_pair( std::string _key, DATA_TYPE & _data)
	{
		if( m_keymap.find( _key) == m_keymap.end())
		{
			m_id_counter++;

			m_keymap[ _key] = m_id_counter;
			std::map< std::string, unsigned int>::iterator mit;

			mit = m_keymap.find( _key);

			detail_pair_t details;
			details.first = &(mit->first);
			details.second = & _data;
			m_idmap[ m_id_counter] = details;

			return m_id_counter;
		}
		return 0;
	}

protected:

	/// determines if the map will delete the object it containes or not
	bool m_owns_objects;

	/// active id, used to calculate id for next element to be added
	unsigned int m_id_counter;

	/// type for paired data storage
	typedef std::pair< const std::string* , DATA_TYPE *> detail_pair_t;

	/// string to id map
	std::map< std::string, unsigned int> m_keymap;

	// id to paired data map
	std::map< unsigned int, detail_pair_t> m_idmap;
};

} // end namespace gs::reg_utils
} // end namespace gs::reg_utils

#endif // _DATA_CONTAINER_H_

#endif // EXCLUDE_FROM_BUILD
#undef EXCLUDE_FROM_BUILD
