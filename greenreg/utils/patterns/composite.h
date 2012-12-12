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

#ifndef COMPOSITE_H_
#define COMPOSITE_H_

#include <set>
#include <iostream>

namespace gs {
namespace reg_utils {

////////////////////////////////////////////
/// composite
/// templated class that defines a simplified
/// composite pattern for use as data
/// storage or complex containment.
///
/// @author 
/// @since 
////////////////////////////////////////////
template< class C>
class composite : public C
{
public:

	/// type definition for the composites storage container
	typedef std::set< composite< C> * > composite_container_t;

	////////////////////////////////////////////
	/// constructor
	/// defined due to custom parameters
	///
	/// @param _parent ? parent of "this" instance in the tree
	/// @return N/A
	////////////////////////////////////////////
	composite( composite< C> * _parent = NULL) : m_composite_parent(_parent)//, m_composite_children(0)
	{
		if( _parent != NULL)
			_parent->add_child( this);
	}

	///
	virtual ~composite()
	{
//		if( m_composite_children != NULL)
//		{
			typename composite_container_t::iterator ccit;
//			typename std::set< composite< C> * >::iterator ccit;
			for( unsigned int count = 0; count <= m_composite_children.size(); count++)
			{
				ccit = m_composite_children.begin();
				delete( *ccit);
			}
//			delete( m_composite_children);
//		}

		if( m_composite_parent != NULL)
		{
			m_composite_parent->m_composite_children.erase( this);
		}
	}

	////////////////////////////////////////////
	/// set_parent
	/// allows user to set parent after creation
	/// but only if parent has not already been set
	///
	/// @param _parent ? parent of "this" instance in the tree
	/// @return N/A
	////////////////////////////////////////////
	void set_parent( composite< C> * _parent)
	{ m_composite_parent == NULL ? m_composite_parent = _parent : std::cout << "cannot reset parent of composite\n"; }

	////////////////////////////////////////////
	/// get_parent
	/// allows non-inherited class to get
	/// a const pointer to the parent composite
	///
	/// @return const composite reference to the parent
	////////////////////////////////////////////
	const composite<C> * get_parent() const
	{ return( m_composite_parent); }

	////////////////////////////////////////////
	/// get_children
	/// returns container (set) of composites
	///
	/// @return pointer to the composite_container_t for children
	///
	/// @see typedef std::set< composite< C> * > composite_container_t;
	////////////////////////////////////////////
	composite_container_t & get_children()
	{ return( m_composite_children); }

	////////////////////////////////////////////
	/// add_child
	/// adds a child to the tree
	///
	/// @param _child ? child to add, if container is not created it will do so, set used internally, no duplicates
	/// @return N/A
	////////////////////////////////////////////
	void add_child( composite< C> * _child)
	{
//		if( m_composite_children == NULL)
//			m_composite_children = new composite_container_t;
		m_composite_children.insert( _child);
	}

public:

	/// only one parent
	composite< C> * m_composite_parent;

	/// ensure not more than one entry for each child
	composite_container_t m_composite_children;
};

} // end namespace gs:reg_utils
} // end namespace gs:reg_utils

#endif /*COMPOSITE_H_*/

#endif // EXCLUDE_FROM_BUILD
#undef EXCLUDE_FROM_BUILD

