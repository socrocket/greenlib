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


#ifndef _I_GR_ANALYZER_USER_CALLBACK_H_
#define _I_GR_ANALYZER_USER_CALLBACK_H_

namespace gs {
namespace reg {

template< class DATA_TYPE>
class gr_analyzer_composite;

////////////////////////////////////////////
/// I_gr_analyzer_user_callback
/// An interface defined to solve a recursive
/// include problem.
/// @author 
/// @since 
////////////////////////////////////////////
template< class DATA_TYPE>
class I_gr_analyzer_user_callback
{
public:
	////////////////////////////////////////////
	/// on_calc_power
	/// internal callback from the power system 
	/// for the "local" power (sub-modules not
	/// included).  
	///
	/// WARNING: exists for polling architectures only!
	/// override only if you are not using power
	/// callback methodology in your platform!
	///
	/// @param _power	local power to be calculated
	/// @return N/A
	////////////////////////////////////////////
	virtual void on_calc_local_data( DATA_TYPE & _data) = 0;
	
	////////////////////////////////////////////
	/// get_gr_analyzer_composite
	/// interface method to return the true "this"
	/// instance of an object, was required
	/// to eliminate name mangaling due to recursive
	/// header includes (actually that is why this class exists..)
	///
	/// @return self gr_defined as analyzer_composite<DATA_TYPE>
	////////////////////////////////////////////
	virtual gr_analyzer_composite< DATA_TYPE> * get_gr_analyzer_composite() = 0;
};

} // end namespace gs:reg
} // end namespace gs:reg

#endif /*_I_GR_ANALYZER_USER_CALLBACK_H_*/
