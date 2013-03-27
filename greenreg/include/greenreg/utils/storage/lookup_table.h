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

#ifndef _LOOKUP_TABLE_H_
#define _LOOKUP_TABLE_H_

#include <vector>
#include <map>
#include <string>

namespace gs {
namespace reg_utils {

////////////////////////////////////////////
/// lookup table
/// a class that provides a key / value
/// mapping for the purpose of long
/// term lookups.
///
/// The class has the ability
/// to "lock" itself so that no additions
/// to the table can be made.
///
/// The class does not have the ability to
/// modify it's entries, this is by design.
///
/// @author 
/// @since 
////////////////////////////////////////////
template< typename KEY_T, typename VAL_T>
class lookup_table
{
public:
	lookup_table();
	virtual ~lookup_table();

	////////////////////////////////////////////
	/// define_error_value
	/// defines the default error value to
	/// return when doing key lookups
	///
	/// @param _error_value ? default error value
	////////////////////////////////////////////
	void define_error_value( VAL_T _error_value);

	////////////////////////////////////////////
	/// define_error_key
	/// defines the default error key to
	/// return when doing value lookups
	///
	/// @param _error_key ? default error key
	////////////////////////////////////////////
	void define_error_key( KEY_T _error_key);

	////////////////////////////////////////////
	/// get_error_value
	///
	/// @return default error value
	////////////////////////////////////////////
	const VAL_T & get_error_value() const;

	////////////////////////////////////////////
	/// get_error_key
	///
	/// @return default error key
	////////////////////////////////////////////
	const KEY_T & get_error_key() const;

	////////////////////////////////////////////
	/// add_entry
	/// adds the key value pair as an entry
	/// into the internal map
	///
	/// @param _key ? item to search(fast) on
	/// @param _value - value stored at _key location
	/// @param _override - forces the entry to be made
	///			even if it already exists.
	///
	/// @return - true if successful, false if entry
	/// 		already exists.
	////////////////////////////////////////////
	virtual bool add_entry( KEY_T _key, VAL_T _value, bool _override = false);

	////////////////////////////////////////////
	/// lookup
	///
	/// @param _key ? item to lookup
	/// @return stored data or default error value
	////////////////////////////////////////////
	VAL_T lookup_key( KEY_T _key) const;

	////////////////////////////////////////////
	/// lookup
	///
	/// @param _value - backward lookup on data
	/// @return key or default error key
	///
	/// @see reference
	////////////////////////////////////////////
	KEY_T lookup_val( VAL_T _value) const;

	////////////////////////////////////////////
	/// list_keys
	///
	/// @return vector of all keys held
	////////////////////////////////////////////
	std::vector< KEY_T> list_keys() const;

	////////////////////////////////////////////
	/// number_entries
	///
	/// @return number of key value pairs
	////////////////////////////////////////////
	unsigned int number_entries() const;

	////////////////////////////////////////////
	/// lock_table
	/// locks the table so that no additions or
	/// changes to default errors can be made
	////////////////////////////////////////////
	void lock_table();

/// rarely do I use private, but in this case
/// I am particular about not wanting changes to
/// happen if the table has been locked
private:

	/// internal storage map
	std::map< KEY_T, VAL_T> m_map;

	/// default error value
	VAL_T m_error_value;

	/// default error key
	KEY_T m_error_key;

	/// boolean variable determining if map is locked or not
	bool m_table_locked;
};

template< typename KEY_T, typename VAL_T>
lookup_table< KEY_T, VAL_T>::lookup_table()
{
	m_table_locked = false;
}

template< typename KEY_T, typename VAL_T>
lookup_table< KEY_T, VAL_T>::~lookup_table()
{
	m_map.clear();
}

template< typename KEY_T, typename VAL_T>
void lookup_table< KEY_T, VAL_T>::define_error_value( VAL_T _error_value)
{
	m_error_value = _error_value;
}

template< typename KEY_T, typename VAL_T>
void lookup_table< KEY_T, VAL_T>::define_error_key( KEY_T _error_key)
{
	m_error_key = _error_key;
}

template< typename KEY_T, typename VAL_T>
const VAL_T & lookup_table< KEY_T, VAL_T>::get_error_value() const
{
	return( m_error_value);
}

template< typename KEY_T, typename VAL_T>
const KEY_T & lookup_table< KEY_T, VAL_T>::get_error_key() const
{
	return( m_error_key);
}

template< typename KEY_T, typename VAL_T>
bool lookup_table< KEY_T, VAL_T>::add_entry( KEY_T _key, VAL_T _value, bool _override)
{
	bool retval = false;

	if( !m_table_locked)
	{
		typename std::map< KEY_T, VAL_T>::iterator mit;
		mit = m_map.find( _key);
		if( mit == m_map.end() || _override)
		{
			m_map[ _key] = _value;
			retval = true;
		}
	}

	return retval;
}

template< typename KEY_T, typename VAL_T>
VAL_T lookup_table< KEY_T, VAL_T>::lookup_key( KEY_T _key) const
{
	VAL_T * retval = const_cast< VAL_T *>(& m_error_value);
	typename std::map< KEY_T, VAL_T>::const_iterator cmit;
	cmit = m_map.find( _key);
	if( cmit != m_map.end())
	{
		retval = const_cast< VAL_T *>(&(cmit->second));
	}
	return( *retval);
}

template< typename KEY_T, typename VAL_T>
KEY_T lookup_table< KEY_T, VAL_T>::lookup_val( VAL_T _value) const
{
	KEY_T * retval = const_cast< KEY_T *>(& m_error_key);
	typename std::map< KEY_T, VAL_T>::const_iterator cmit;

	cmit = m_map.begin();
	while( cmit != m_map.end())
	{
		cmit = m_map.find( cmit->first);
		if( cmit->second == _value)
		{
			retval = const_cast< KEY_T *>(&(cmit->first));
			cmit = m_map.end();
		} else {
			cmit++;
		}
	}

	return( *retval);
}

template< typename KEY_T, typename VAL_T>
std::vector< KEY_T> lookup_table< KEY_T, VAL_T>::list_keys() const
{
	std::vector< KEY_T> keys;
	typename std::map< KEY_T, VAL_T>::const_iterator cmit;

	for( cmit = m_map.begin(); cmit != m_map.end(); cmit++)
	{
		keys.push_back( cmit->first);
	}

	return( keys);
}

template< typename KEY_T, typename VAL_T>
unsigned int lookup_table< KEY_T, VAL_T>::number_entries() const
{
	return( m_map.size());
}

template< typename KEY_T, typename VAL_T>
void lookup_table< KEY_T, VAL_T>::lock_table()
{
	m_table_locked = true;
}

typedef lookup_table< std::string, unsigned int> compile_test_t;
static compile_test_t g_compile_test;

} // end namespace gs::reg
} // end namespace gs::reg

#endif // _LOOKUP_TABLE_H_

#endif // EXCLUDE_FROM_BUILD
#undef EXCLUDE_FROM_BUILD

