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

#ifndef _GR_OBJECT_H_
#define _GR_OBJECT_H_

#include "greenreg/framework/core/I_gr_dump.h"
#include "greenreg/framework/core/gr_common.h"
#include <boost/config.hpp>
#include <systemc>

namespace gs {
namespace reg {

////////////////////////////////////////////
/// gr_object
/// base object for most gr_ element types
/// mirrored as much of the API to match 
/// sc_object as possible.
///
/// @author 
/// @since 
////////////////////////////////////////////
  class gr_object : public sc_core::sc_object, public I_gr_dump
{
public:

	////////////////////////////////////////////
	/// constructor
	/// forces user to enter name
	////////////////////////////////////////////
	// no default constructor on purpose
	gr_object( std::string _name);
	virtual ~gr_object();


	////////////////////////////////////////////
	/// kind
	/// forces user to implement
	///
	/// @see sc_object
	////////////////////////////////////////////
	// all dr classes must implement their "kind"
	virtual const char * kind() const = 0;

	////////////////////////////////////////////
	/// @see I_gr_dump
	////////////////////////////////////////////
//	void gr_dump( gr_dump_format_e _format, std::ostream & _stream);

protected:

	////////////////////////////////////////////
	/// gr_dump_self
	/// forced for all descendents of gr_object
	/// to implement.  Same as gr_dump, except
	/// that gr_object will call this method.
	///
	///@see gr_dump
	////////////////////////////////////////////
//	virtual void gr_dump_self( gr_dump_format_e _format, std::ostream & _stream) = 0;
};

} // end namespace gs::reg
} // end namespace gs::reg

#endif // _GR_OBJECT_H_

#endif // EXCLUDE_FROM_BUILD 
#undef EXCLUDE_FROM_BUILD
