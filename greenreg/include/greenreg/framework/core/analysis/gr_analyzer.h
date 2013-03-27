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


#ifndef _GR_ANALYZER_H_
#define _GR_ANALYZER_H_

#include "I_gr_analyzer.h"
#include "I_gr_analyzer_user_callback.h"
#include "gr_analyzer_composite.h"

namespace gs {
namespace reg {
	
////////////////////////////////////////////
/// gr_analyzer
/// user level interface to the greenreg analysis suite
/// @author 
/// @since 
////////////////////////////////////////////
template< class DATA_TYPE>
class gr_analyzer : public I_gr_analyzer< DATA_TYPE>, public I_gr_analyzer_user_callback< DATA_TYPE>
{
public:

	////////////////////////////////////////////
	/// constructor
	/// definition for parameters
	///
	/// @param _name	name of module
	/// @param _power_parent	reference to parent (or NULL if top)
	/// @return N/A
	////////////////////////////////////////////
	gr_analyzer( sc_core::sc_module_name _name, I_gr_analyzer_user_callback< DATA_TYPE> * _data_parent);
	
	/// short description
	virtual ~gr_analyzer();
	
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

	/// @see I_gr_analyzer
	virtual void recalc_data();
	
	/// @see I_gr_analyzer_user_callback
	virtual void on_calc_local_data( DATA_TYPE & _data);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///		PUSH METHODOLOGY
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/// @see I_gr_analyzer
	virtual sc_core::sc_event & get_on_cache_change_event();

	/// @see I_gr_analyzer
	virtual void enable_on_cache_change_event();

	/// @see I_gr_analyzer
	virtual void disable_on_cache_change_event();

	/// @see I_gr_analyzer
	virtual void recalc_data_cache( bool _recurse_to_top);
	
	/// @see I_gr_analyzer_user_callback
	virtual gr_analyzer_composite< DATA_TYPE> * get_gr_analyzer_composite()
	{ return & m_analyzer_composite; }
	
private:

	friend class gr_analyzer_composite< DATA_TYPE>;
	
	gr_analyzer_composite< DATA_TYPE> m_analyzer_composite;
};

template< class DATA_TYPE>
gr_analyzer< DATA_TYPE>::gr_analyzer( sc_core::sc_module_name _name, I_gr_analyzer_user_callback< DATA_TYPE> * _data_parent)
: m_analyzer_composite( _name, _data_parent)
{
	m_analyzer_composite.post_constructor_initialize( this);
}

template< class DATA_TYPE>
gr_analyzer< DATA_TYPE>::~gr_analyzer()
{
}

template< class DATA_TYPE>
void gr_analyzer< DATA_TYPE>::set_local_data( const DATA_TYPE & _data)
{
	m_analyzer_composite.set_local_data( _data);
}

template< class DATA_TYPE>
void gr_analyzer< DATA_TYPE>::get_local_data( DATA_TYPE & _data) const
{
	m_analyzer_composite.get_local_data( _data);
}

template< class DATA_TYPE>
void gr_analyzer< DATA_TYPE>::get_cache_data( DATA_TYPE & _data) const
{
	m_analyzer_composite.get_cache_data( _data);
}

template< class DATA_TYPE>
void gr_analyzer< DATA_TYPE>::get_local_plus_cache_data( DATA_TYPE & _data) const
{
	m_analyzer_composite.get_local_plus_cache_data( _data);
}

template< class DATA_TYPE>
void gr_analyzer< DATA_TYPE>::recalc_data()
{
	m_analyzer_composite.recalc_data();
}

template< class DATA_TYPE>
void gr_analyzer< DATA_TYPE>::on_calc_local_data( DATA_TYPE & _data)
{
	// in case a top level polling architecture
	// is implemented (to enable support of both types)
	// this methods default implementation is to 
	// return the local power (assuming it's change was
	// driven by an outside source through the set_power
	// method.
	m_analyzer_composite.get_local_data( _data);
}

template< class DATA_TYPE>
sc_core::sc_event & gr_analyzer< DATA_TYPE>::get_on_cache_change_event()
{
	return( m_analyzer_composite.get_on_cache_change_event());
}

template< class DATA_TYPE>
void gr_analyzer< DATA_TYPE>::enable_on_cache_change_event()
{
	m_analyzer_composite.enable_on_cache_change_event();
}

template< class DATA_TYPE>
void gr_analyzer< DATA_TYPE>::disable_on_cache_change_event()
{
	m_analyzer_composite.disable_on_cache_change_event();
}

template< class DATA_TYPE>
void gr_analyzer< DATA_TYPE>::recalc_data_cache( bool _recurse_to_top)
{
	m_analyzer_composite.recalc_data_cache( _recurse_to_top); 
}

typedef gr_analyzer< gr_power_t> gr_power_analyzer_t;
typedef gr_analyzer< gr_energy_t> gr_energy_analyzer_t;
typedef gr_analyzer< gr_emi_t> gr_emi_analyzer_t;

} // end namespace gs:reg
} // end namespace gs:reg

#endif /*_GR_ANALYZER_H_*/
