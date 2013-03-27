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


#ifndef _I_GR_ANALYZER_H_
#define _I_GR_ANALYZER_H_

#include <boost/config.hpp>
#include <systemc>

namespace gs {
namespace reg {

/// forward class declaration for gr_analyzer_composite
template< class DATA_TYPE>
class gr_analyzer_composite;

////////////////////////////////////////////
/// I_gr_analyzer
/// top level class with method
/// definitions for user and internal side
/// of the analyasis API
/// @author 
/// @since 
////////////////////////////////////////////
template< class DATA_TYPE>
class I_gr_analyzer
{
public:

	////////////////////////////////////////////
	/// set_local_data
	/// pushes a new power state to the "local"
	/// module.  This will push the power state
	/// change all the way up the hierarchy.
	///
	/// @param _data	calculated local data
	/// @return N/A
	////////////////////////////////////////////
	virtual void set_local_data( const DATA_TYPE & _data) = 0;
	
	////////////////////////////////////////////
	/// get_local_data
	/// returns the current local analysis data
	///
	/// @param _data	user supplied variable to be filled in
	/// @return N/A
	////////////////////////////////////////////
	virtual void get_local_data( DATA_TYPE & _data) const = 0;
	
	////////////////////////////////////////////
	/// get_cache_data
	/// returns the current cache analysis data
	///
	/// @param _data	user supplied variable to be filled in
	/// @return N/A
	////////////////////////////////////////////
	virtual void get_cache_data( DATA_TYPE & _data) const = 0;
	
	////////////////////////////////////////////
	/// get_local_plus_cache_data
	/// returns the current local + cache analysis data
	///
	/// @param _data	user supplied variable to be filled in
	/// @return N/A
	////////////////////////////////////////////
	virtual void get_local_plus_cache_data( DATA_TYPE & _data) const = 0;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///		PULL METHODOLOGY
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////
	/// recalc_data
	/// forces a recalc on local data & recursively
	/// downward.  At which point the cache is
	/// rebuilt from bottom (leafs) up.
	///
	/// @param _power	local power to be calculated
	/// @return N/A
	////////////////////////////////////////////
	virtual void recalc_data() = 0;
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///		PUSH METHODOLOGY
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////
	/// get_cache_change_event()
	/// event fires for "this" item when ever it
	/// or it's cache is updated
	///
	/// @return sc_core::sc_event to notify of change
	////////////////////////////////////////////
	virtual sc_core::sc_event & get_on_cache_change_event() = 0;

	////////////////////////////////////////////
	/// enable_on_cache_change_event()
	/// enables the internal cache change event
	////////////////////////////////////////////
	virtual void enable_on_cache_change_event() = 0;
	
	////////////////////////////////////////////
	/// disable_on_cache_change_event()
	/// disables the internal cache change event
	/// (default)
	////////////////////////////////////////////
	virtual void disable_on_cache_change_event() = 0;

	////////////////////////////////////////////
	/// recalc_power_cache
	/// recalculates the power cache of "this"
	/// objects PARENT, and optionally upward
	/// along the tree leg.  Ideally the user
	/// should never need to call this.
	///
	/// @param _recurse_to_top	true - recurses the cache recalc up the tree leg
	////////////////////////////////////////////
	virtual void recalc_data_cache( bool _recurse_to_top) = 0;
	
};

} // end namespace gs:reg
} // end namespace gs:reg

#endif /*_I_GR_ANALYZER_H_*/
