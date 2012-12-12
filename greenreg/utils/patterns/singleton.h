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

#ifndef SINGLETON_H_
#define SINGLETON_H_

namespace gs {
namespace reg_utils {

////////////////////////////////////////////
/// singleton
/// defines a class as a singleton through
/// template definition.  This is useful so
/// that your class does not require itself
/// to have knowledge in specific cases where
/// it is a singleton, and can therefor also
/// be used in other places.
///
/// for example, if I wished to have a singleton
/// of a specific datastructure, I could do:
/// singleton< datastructure> st;... refer
/// to examples for more details.
///
/// @author 
/// @since 
////////////////////////////////////////////
template< class T>
class singleton
{
public:

	singleton<T>()
	{
	}
	
	virtual ~singleton<T>()
	{
	}

	////////////////////////////////////////////
	/// get
	/// The only real way to declare a templated
	/// singleton with newer compilers.  There was 
	/// a much more elegant and understandable way
	/// ( from a scoping perspective ) before C++03 standards...
	///
	/// @return reference to a local scoped static which
	/// exists as long as singleton<T>() exists.
	////////////////////////////////////////////
	static T & get()
	{
		static T local_scope_static;
		return local_scope_static;
	}
	
protected:
};

} // end namespace gs:reg_utils
} // end namespace gs:reg_utils

#endif /*SINGLETON_H_*/

#endif // EXCLUDE_FROM_BUILD 
#undef EXCLUDE_FROM_BUILD
