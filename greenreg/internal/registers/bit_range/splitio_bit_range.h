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


#ifndef SPLITIO_BIT_RANGE_H_
#define SPLITIO_BIT_RANGE_H_

#include <string>

#include "bit_range.h"
#include "bit_range_data.h"

namespace gs {
namespace reg {

////////////////////////////////////////////
/// splitio_bit_range
/// implementation of bit range which supports
/// seperate input and output buffers
/// @author 
/// @since 
////////////////////////////////////////////
class splitio_bit_range : public bit_range
{
public:

	////////////////////////////////////////////
	/// Constructor
	///
	/// @param _name ? unique name of bit range (duplicates ignored)
	/// @param _register ? owning register (parent)
	/// @param _start_bit ? bit the range starts on (0 being first bit in register)
	/// @param _stop_bit ? bit the range ends on (must be > start bit, and less than reg width)
	///
	/// @see I_regsiter
	////////////////////////////////////////////
	splitio_bit_range( std::string & _name, I_register & _register, gr_uint_t _start_bit, gr_uint_t _stop_bit);

	////////////////////////////////////////////
	/// Destructor
	////////////////////////////////////////////
	virtual ~splitio_bit_range();

private:

	/// bit range data for input buffer
	bit_range_data in_bit_range_data;

	/// bit range data for output buffer
	bit_range_data out_bit_range_data;
};

} // end namespace gs:reg
} // end namespace gs:reg

#endif /*SPLITIO_BIT_RANGE_H_*/
