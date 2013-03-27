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


#ifndef _GR_ANALYZER_COMPOSITE_H_
#define _GR_ANALYZER_COMPOSITE_H_

#include <boost/config.hpp>
#include <systemc>

#include "greenreg/utils/patterns/composite.h"

#include "greenreg/framework/core/analysis/gr_analysis_t.h"
#include "greenreg/framework/core/analysis/I_gr_analyzer.h"
#include "greenreg/framework/core/analysis/I_gr_analyzer_user_callback.h"

namespace gs {
namespace reg {

////////////////////////////////////////////
/// gr_analyzer_composite
/// composite container that hangs on the
/// side of the classes using the analysis API
/// and basically maintains a secondary hierarchy
/// allowing:
/// 1) separation from SystemC containment contraints
/// 2)
/// @author 
/// @since 
////////////////////////////////////////////
template< class DATA_TYPE>
class gr_analyzer_composite : public sc_core::sc_module, public gs::reg_utils::composite< I_gr_analyzer< DATA_TYPE> >
{
public:

	SC_HAS_PROCESS( gr_analyzer_composite);

	////////////////////////////////////////////
	/// gr_analyzer_composite
	/// paren is the real parent( i.e. the parent in the
	/// exoskeleton tree parallel to the user classes).
	/// BINDING OF PARENT AT DECLARATION!!!
	///
	/// @param _name ? name of this analyzer module
	/// @param _parent ? the real exoskeleton parent
	////////////////////////////////////////////
	gr_analyzer_composite( sc_core::sc_module_name _name, I_gr_analyzer_user_callback< DATA_TYPE> * _parent);

	////////////////////////////////////////////
	/// post_constructor_initialize
	/// alows late binding (forced on user in user api)
	/// to bind owner (the real module this is associated with).
	/// BINDING OF OWNER MADE IN OWNERS CONSTRUCTOR!!!
	///
	/// @param _owner ? my creator, and destroyer...
	////////////////////////////////////////////
	virtual void post_constructor_initialize( I_gr_analyzer_user_callback< DATA_TYPE> * _owner);

	~gr_analyzer_composite();

	/// @see I_gr_analyzer
	virtual void set_local_data( const DATA_TYPE & _data);

	/// @see I_gr_analyzer
	virtual void get_local_data( DATA_TYPE & _data) const;

	/// @see I_gr_analyzer
	virtual void get_cache_data( DATA_TYPE & _data) const;

	/// @see I_gr_analyzer
	virtual void get_local_plus_cache_data( DATA_TYPE & _data) const;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///		PULL METHODOLOGY
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////
	/// update_monitors
	/// long description
	///
	/// @param name ? description
	/// @return value
	///
	/// @see reference
	////////////////////////////////////////////
	void update_monitors();

	/// @see I_gr_analyzer
	virtual void recalc_data();

	/// @see I_gr_analyzer_user_callback
//	virtual void on_calc_local_data( DATA_TYPE & _data);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///		PUSH METHODOLOGY
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/// @see I_gr_analyzer
	virtual sc_core::sc_event & get_on_cache_change_event();
	virtual void enable_on_cache_change_event();
	virtual void disable_on_cache_change_event();

	/// @see I_gr_analyzer
	virtual void recalc_data_cache( bool _recurse_to_top);

protected:

	/// the owner (not composite parent) of this sc_core::sc_module
	I_gr_analyzer_user_callback< DATA_TYPE> * m_owner;

	/// local analysis data
	DATA_TYPE m_local_data;

	///	local cache total
	DATA_TYPE m_cache_total;

	/// event fired when a cache change occurs at "this" level
	sc_core::sc_event m_on_cache_change_event;

	/// determines if the event will fire when a cache change occurs at "this" level
	bool m_enable_on_cache_change_event;
};

template< class DATA_TYPE>
gr_analyzer_composite< DATA_TYPE>::gr_analyzer_composite( sc_core::sc_module_name _name, I_gr_analyzer_user_callback< DATA_TYPE> * _parent)
: m_enable_on_cache_change_event( false)
{
	if( _parent != NULL)
	{
		_parent->get_gr_analyzer_composite()->add_child( this);
		this->set_parent( _parent->get_gr_analyzer_composite());
	}
}

template< class DATA_TYPE>
void gr_analyzer_composite< DATA_TYPE>::post_constructor_initialize( I_gr_analyzer_user_callback< DATA_TYPE> * _owner)
{
	// set the owning container( hierarchy)
	m_owner = _owner;
}

template< class DATA_TYPE>
gr_analyzer_composite< DATA_TYPE>::~gr_analyzer_composite()
{
}

template< class DATA_TYPE>
void gr_analyzer_composite< DATA_TYPE>::update_monitors()
{
	// nothing to do yet...
}

template< class DATA_TYPE>
void gr_analyzer_composite< DATA_TYPE>::set_local_data( const DATA_TYPE & _data)
{
	// obviously we have to set the local power
	m_local_data = _data;

	// if we have a parent
	if( gs::reg_utils::composite< I_gr_analyzer< DATA_TYPE> >::m_composite_parent != NULL)
	{
		// recalculate parent cache recursively to top
		gs::reg_utils::composite< I_gr_analyzer< DATA_TYPE> >::m_composite_parent->recalc_data_cache( true);
	}

	// update monitor for local power
}

template< class DATA_TYPE>
void gr_analyzer_composite< DATA_TYPE>::get_local_data( DATA_TYPE & _data) const
{
	_data = m_local_data;
}

template< class DATA_TYPE>
void gr_analyzer_composite< DATA_TYPE>::get_cache_data( DATA_TYPE & _data) const
{
	_data = m_cache_total;
}

template< class DATA_TYPE>
void gr_analyzer_composite< DATA_TYPE>::get_local_plus_cache_data( DATA_TYPE & _data) const
{
	_data.set( m_local_data.get() + m_cache_total.get());
}

template< class DATA_TYPE>
void gr_analyzer_composite< DATA_TYPE>::recalc_data()
{
	update_monitors();
}

/*
template< class DATA_TYPE>
void on_calc_local_data( DATA_TYPE & _data)
{
}
*/

template< class DATA_TYPE>
sc_core::sc_event & gr_analyzer_composite< DATA_TYPE>::get_on_cache_change_event()
{
	return( m_on_cache_change_event);
}

template< class DATA_TYPE>
void gr_analyzer_composite< DATA_TYPE>::enable_on_cache_change_event()
{
	m_enable_on_cache_change_event = true;
}

template< class DATA_TYPE>
void gr_analyzer_composite< DATA_TYPE>::disable_on_cache_change_event()
{
	m_enable_on_cache_change_event = false;
}

template< class DATA_TYPE>
void gr_analyzer_composite< DATA_TYPE>::recalc_data_cache( bool _recurse_to_top)
{
	// if a child changed our cached calculation for sibling
	// power is invalid, we must recalculate (non-probing)
	typename gs::reg_utils::composite< I_gr_analyzer< DATA_TYPE> >::composite_container_t::iterator sit; // must use typename for template declaration of link types
	typename gs::reg_utils::composite< I_gr_analyzer< DATA_TYPE> >::composite_container_t * cache; // not sure how to resolve this warning...
	cache = &(gs::reg_utils::composite< I_gr_analyzer< DATA_TYPE> >::get_children());

	m_cache_total.set(0);
	DATA_TYPE temp_data;// = ZERO_POWER;

	// simply add up the items in the cache
	// (remember, these are already calculated)
	for( sit = cache->begin(); sit != cache->end(); sit++)
	{
		(*sit)->get_local_plus_cache_data( temp_data);
		m_cache_total += temp_data;
	}

	// update monitor
	// we want to update as we go UP the tree leg!!!
	if( m_enable_on_cache_change_event)
		get_on_cache_change_event().notify();

	// in most cases a child update will require everything
	// above the current level to be re-calculated (non-probing recalc)
	if( _recurse_to_top && (gs::reg_utils::composite< I_gr_analyzer< DATA_TYPE> >::m_composite_parent != NULL))
	{
		gs::reg_utils::composite< I_gr_analyzer< DATA_TYPE> >::m_composite_parent->recalc_data_cache( _recurse_to_top);
	}
}

} // end namespace gs:reg
} // end namespace gs:reg

#endif /*_GR_ANALYZER_COMPOSITE_H_*/
