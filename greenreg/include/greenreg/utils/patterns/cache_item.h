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

#ifndef CACHE_ITEM_H_
#define CACHE_ITEM_H_

#include <iostream>

namespace gs {
namespace reg_utils {

////////////////////////////////////////////
/// I_cache_item
/// class your data class to be cached
/// MUST descend from.  the on_recache_item
/// callback is critical to implement
/// proper cache support;
///
/// @author 
/// @since 
////////////////////////////////////////////
class I_cache_item
{
public:
  virtual ~I_cache_item() {}
  
	////////////////////////////////////////////
	/// on_recache_item
	/// called when the cache is dirty and
	/// needs to be re-calculated.  Is not
	/// recalculated at the time the cache
	/// goes dirty, but when the item is
	/// read.
	////////////////////////////////////////////
	virtual void on_recache_item() = 0;
};

////////////////////////////////////////////
/// cache_item
/// declares an entity or target class
/// as an elment that is cached.
///
/// the class to be cached MUST have default
/// constructor otherwise use another system.
///
/// @author 
/// @since 
////////////////////////////////////////////
template< class TARGET>
class cache_item
{
public:
	////////////////////////////////////////////
	/// constructor
	////////////////////////////////////////////
	cache_item() : dirty( true)
	{
		try {
			TARGET * test_target;
			I_cache_item * test_cache_item;
			test_cache_item = static_cast< I_cache_item *>( test_target);
		} catch( ...) {
			std::cout << "\nERROR: cache_item requires that TARGET descend from I_cache_item, exiting.\n";
			exit(1);
		}
	}

	////////////////////////////////////////////
	/// flush
	/// basically forces the dirty flag to true
	/// so that any other call to data will force
	/// a data refresh.
	////////////////////////////////////////////
	void flush()
	{ dirty = true; }

	////////////////////////////////////////////
	/// is_dirty
	///
	/// @return true if cache is bad, false otherwise
	////////////////////////////////////////////
	bool is_dirty()
	{ return dirty; }

	////////////////////////////////////////////
	/// operator TARGET & ()
	/// default operator for TARGET & ensures
	/// that if the cache is dirty it is re-cached
	///
	/// @return reference to data
	////////////////////////////////////////////
	inline operator TARGET& ()
	{
		if( dirty)
		{
			m_cached_data.on_recache_item();
			dirty = false;
		}

		return( m_cached_data);
	}

	////////////////////////////////////////////
	/// method name
	/// long description
	///
	/// @param name ? description
	/// @return value
	///
	/// @see reference
	////////////////////////////////////////////
	inline cache_item< TARGET> & operator = ( const TARGET & _data)
	{
		m_cached_data = _data;
		dirty = false;
		return( *this);
	}

	////////////////////////////////////////////
	/// operator->()
	/// provides access to member functions of
	/// m_cached_data (type TARGET), BUT before
	/// doing so ensures the cache is up to date
	///
	/// @return pointer access to TARGET for method calls
	////////////////////////////////////////////
	inline TARGET * operator->()
	{
		if( dirty)
		{
			m_cached_data.on_recache_item();
			dirty = false;
		}

		return( &m_cached_data);
	}

	////////////////////////////////////////////
	/// get_data_before_dirty_check
	/// essentially returns a reference to the
	/// m_cached_data WITHOUT checking if the
	/// cache is dirty or not, does not force a
	/// recache on the item.
	///
	/// @return reference to m_cached_data
	////////////////////////////////////////////
	TARGET & get_data_before_dirty_check()
	{
		return( m_cached_data);
	}

private:

	/// boolean to determine if the cache is dirty or not
	bool dirty;

	/// cached data
	TARGET m_cached_data;
};

} // end namespace gs:reg_utils
} // end namespace gs:reg_utils

#endif /*CACHE_ITEM_H_*/

#endif // EXCLUDE_FROM_BUILD
#undef EXCLUDE_FROM_BUILD

