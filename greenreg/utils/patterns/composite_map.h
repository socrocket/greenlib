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


/// NOTE: This class will not work with newer compilers due to recursive unknown type resolution.
/// please use composite_reference_container instead.  Otherwise you are on your own for now.

#define EXCLUDE_FROM_BUILD
#ifndef EXCLUDE_FROM_BUILD
#undef EXCLUDE_FROM_BUILD

#ifndef COMPOSITE_MAP_H_
#define COMPOSITE_MAP_H_

#include <string>

namespace gs {
namespace reg_utils {

template< class KEY, class TYPE>
class composite_map_entry;

template< class KEY, class TYPE>
class composite_map;

template< class KEY, class TYPE>
class composite_map_entry : public TYPE
{
public:
	const KEY & key() const
	{ return( m_key != NULL ? *m_key : ""); }
	
protected:
	composite_map_entry( KEY * _key = NULL)
	: m_owner( false)
	{ m_key = _key; }
	
	~composite_map_entry()
	{
		if( m_owner)
			delete( m_key);
	}

	friend class composite_map< KEY, TYPE>;

	void key( KEY * _key)
	{ m_key = _key; }

	KEY * m_key;
	bool m_owner;
};

template< class KEY, class TYPE>
class composite_map : public composite_map_entry< KEY, TYPE>
{
public:

	typedef composite_map< KEY, TYPE> composite_map_t;
	typedef std::map< KEY, composite_map_t * > composite_map_container_t;
	
	composite_map( KEY & _key, composite_map_t * _parent) : m_composite_map_parent(NULL), m_composite_map_children(0)
	{ 
		if( _parent != NULL)
			set_parent( _key, _parent);
		else 
		{
			KEY * str = new KEY( _key);
			key( str);
			m_owner = true;
		}
	}
	
	virtual ~composite_map()  
	{ 
		delete m_composite_map_children; 
	}

	inline operator TYPE & () 
	{ return( *this); }
	
	inline composite_map_t& operator = ( const KEY& _value) 
	{ *(dynamic_cast< TYPE *>(this)) = _value; return( *this); }
	
	composite_map_t & operator[] ( KEY _key)
	{
		assert( m_composite_map_children != NULL);
		return( *(m_composite_map_children[ _key])); 
	}
	
	void set_parent(  KEY & _key, composite_map_t * _parent)
	{ 
		if( m_composite_map_parent == NULL)
		{
			m_composite_map_parent = _parent;
			
			if( _parent != NULL)
			{
				_parent->add_child( _key, this);
				if( m_owner)
				{
					delete( m_key);
					m_owner = false;
				}
				key( &_key);
			}
				
		} else {
			cout << "cannot reset parent of composit\n"; 
		}
	}
	
	const composite_map_t * get_parent() const
	{ return( m_composite_map_parent); }
	
private:

	void add_child( KEY _key, composite_map_t * _child)
	{ 
		if( m_composite_map_children == NULL)
			m_composite_map_children = new composite_map_container_t;
		(*m_composite_map_children)[_key] = _child;
	}
	
	composite_map_t * m_composite_map_parent;
	composite_map_container_t * m_composite_map_children;
};

} // end namespace gs:reg_utils
} // end namespace gs:reg_utils

#endif /*COMPOSITE_MAP_H_*/

#endif // EXCLUDE_FROM_BUILD 
#undef EXCLUDE_FROM_BUILD

