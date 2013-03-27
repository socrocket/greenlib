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


#ifndef _GR_ANALYSIS_T_H_
#define _GR_ANALYSIS_T_H_

namespace gs {
namespace reg {

////////////////////////////////////////////
/// class_name
/// long_description
/// @author name
/// @since date
////////////////////////////////////////////
enum gr_analysis_e
{
	UNDEFINED = 0,
	POWER = 1,
	ENERGY = 2,
	EMI = 3
};

////////////////////////////////////////////
/// gr_analysis_t
/// an analysis type with methods that
/// are executed from the internal framework
/// for the most part one would wrap a base
/// type and leverege all the operator
/// overloads.  Secondary template type exists
/// to use as an index so that user module
/// can descend and implement multiple instances
/// of analisys API for the same base data type
/// without clobbering instances.
////////////////////////////////////////////
template< class T, gr_analysis_e TYPE_ID = gr_analysis_e(0) >
class gr_analysis_t
{
public:
	////////////////////////////////////////////
	/// gr_analysis_t
	/// constructor, trys to be nice and set
	/// default value to 0.
	////////////////////////////////////////////
	gr_analysis_t()
	{
		// try initializing the data to zero if it is a base type
		try {
			m_analysis_data = 0;
		} catch (...) {
			// nothing to do
		}
	}

	////////////////////////////////////////////
	/// gr_analysis_t
	/// constructor that takes an initialization value
	///
	/// @param _init_value ? initialization value
	////////////////////////////////////////////
	gr_analysis_t( T& _init_value) : m_analysis_data( _init_value)
	{}

	////////////////////////////////////////////
	/// ~gr_analysis_t
	/// destructor
	////////////////////////////////////////////
	virtual ~gr_analysis_t()
	{}

	////////////////////////////////////////////
	/// operator T& ()
	/// @return contained analysis data
	////////////////////////////////////////////
	inline operator T& ()
	{ return( m_analysis_data); }

	////////////////////////////////////////////
	/// operator =
	/// assignment operator just sets data to passed value
	///
	/// @param _value ? value to set internal analysis data to
	/// @return gr_analysis_t< T, TYPE_ID> &
	////////////////////////////////////////////
	inline gr_analysis_t<T, TYPE_ID> & operator = ( const T& _value)
	{ m_analysis_data = _value; return ( *this);}

	////////////////////////////////////////////
	/// set
	/// set the analysis data to the passed data
	///
	/// @param _data ? data to set internal analysis data to
	////////////////////////////////////////////
	void set( T _data)
	{ m_analysis_data = _data; }

	////////////////////////////////////////////
	/// get
	/// @return copy of the internal data
	////////////////////////////////////////////
	T get() const
	{ return m_analysis_data; }

	////////////////////////////////////////////
	/// increment
	/// increments the internal value by the passed in data
	///
	/// @param _inc ? value to increase by
	/// @return updated analysis data
	////////////////////////////////////////////
	T increment( T _inc)
	{ m_analysis_data += _inc; return m_analysis_data; }

	////////////////////////////////////////////
	/// decrement
	/// decrements the internal value by the passed in data
	///
	/// @param _dec ? value to decrement by
	/// @return updated analysis data
	////////////////////////////////////////////
	T decrement( T _dec)
	{ m_analysis_data -= _dec; return m_analysis_data; }

protected:
	/// internal data
	T m_analysis_data;
};

/// some standard analysis type definitions
typedef gr_analysis_t<double, POWER> gr_power_t;
typedef gr_analysis_t<double, ENERGY> gr_energy_t;
typedef gr_analysis_t<double, EMI> gr_emi_t;

} // end namespace gs::reg
} // end namespace gs::reg

#endif /*_GR_ANALYSIS_T_H_*/
