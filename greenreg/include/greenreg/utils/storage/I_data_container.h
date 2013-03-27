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

#ifndef I_DATA_CONTAINER_H_
#define I_DATA_CONTAINER_H_

#include <string>
#include <vector>
#include <map>

namespace gs {
namespace reg_utils {

////////////////////////////////////////////
/// I_data_container
/// generic data container interface
/// @author 
/// @since 
////////////////////////////////////////////
template< typename DATA_TYPE>
class I_data_container
{
public:
  virtual ~I_data_container() {}

	////////////////////////////////////////////
	/// has_data_elements
	/// returns if container has data elements
	///
	/// @return true if yes, false if no
	////////////////////////////////////////////
	virtual bool has_data_elements() = 0;

	////////////////////////////////////////////
	/// number_of_data_elements
	/// returns number of data elements contained
	///
	/// @return number of data elements contained
	////////////////////////////////////////////
	virtual unsigned int number_of_data_elements() = 0;

	////////////////////////////////////////////
	/// get_data
	///
	/// @param _key ? data key
	/// @return data reference pointer or NULL if not valid
	////////////////////////////////////////////
	inline virtual DATA_TYPE * get_data( std::string _key) = 0;

	////////////////////////////////////////////
	/// get_data
	///
	/// @param _id ? data id
	/// @return data reference pointer or NULL if not valid
	////////////////////////////////////////////
	inline virtual DATA_TYPE * get_data( unsigned int _id) = 0;

	////////////////////////////////////////////
	/// get_data_key
	///
	/// @param _id ? data id
	/// @return data key or "" if not valid
	////////////////////////////////////////////
	inline virtual std::string get_data_key( unsigned int _id) = 0;

	////////////////////////////////////////////
	/// get_data_id
	///
	/// @param _key ? data key
	/// @return data id or 0 if not valid
	////////////////////////////////////////////
	inline virtual unsigned int get_data_id( std::string _key) = 0;

	////////////////////////////////////////////
	/// get_all_data_ids
	///
	/// @return returns a vector with all data ids in container
	////////////////////////////////////////////
	inline virtual std::vector< unsigned int> get_all_data_ids() = 0;

	////////////////////////////////////////////
	/// add_key_data_pair
	/// adds key and data pair to container
	///
	/// @param _key ? unique data key
	/// @param _data - data object by reference
	/// @return unique data id
	////////////////////////////////////////////
	inline virtual unsigned int add_key_data_pair( std::string _key, DATA_TYPE & _data) = 0;
};

} // end namespace gs:reg_utils
} // end namespace gs:reg_utils

#endif /*I_DATA_CONTAINER_H_*/

#endif // EXCLUDE_FROM_BUILD 
#undef EXCLUDE_FROM_BUILD
