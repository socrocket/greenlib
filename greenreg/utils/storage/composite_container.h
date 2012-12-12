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

#ifndef _COMPOSITE_CONTAINER_H_
#define _COMPOSITE_CONTAINER_H_

#include <vector>

#include "dynamic_classtype_enumeration.h"

namespace gs {
namespace reg_utils {
	
////////////////////////////////////////////
/// composite_container
/// container of containers enabling recursive
/// tree structure of containment.
/// 
/// is a dynamic_enum_type so that a static
/// enumeration for composite_container(s)
/// can exist.
///
/// @author 
/// @since 
////////////////////////////////////////////
class composite_container : public dynamic_enum_type
{
public:
	composite_container();
	virtual ~composite_container();

	////////////////////////////////////////////
	/// has_containers
	///
	/// @return >= 1 container ? true : false
	////////////////////////////////////////////
	bool has_containers();

	////////////////////////////////////////////
	/// number_of_containers
	///
	/// @return size of containers
	////////////////////////////////////////////
	unsigned int number_of_containers();

	////////////////////////////////////////////
	/// get_contianer_names
	/// walks through container map and returns
	/// a vector of container names
	///
	/// @return vector of container names
	////////////////////////////////////////////
	std::vector< std::string> get_container_names();

	////////////////////////////////////////////
	/// get_container
	/// returns a container reference by name
	///
	/// @param _name ? name of container
	/// @return container reference or NULL if not found
	////////////////////////////////////////////
	composite_container * get_container( std::string _name);
	
	////////////////////////////////////////////
	/// add_contianer
	/// adds a container to the internal map
	///
	/// @param _name ? name of container
	/// @param _contianer ? pointer to container
	////////////////////////////////////////////
	/// prefer this explicitly retun the type of
	/// container, but not will to create this as 
	/// a template right now
//	virtual void add_container( std::string _name) = 0;

	////////////////////////////////////////////
	/// del_container
	/// attempts to remove container
	///
	/// @param _name - name of container
	/// @return true on success, false if fail
	////////////////////////////////////////////
	virtual bool del_container( std::string _name) = 0;

protected:

	/// map of container name to conainter reference pointer
	std::map< std::string, composite_container *> m_containers;
};

/// global declaration for composite container enumeration
extern dynamic_classtype_enumeration< composite_container> g_composite_containers_enum;

} // end namespace gs::reg_utils
} // end namespace gs::reg_utils

#endif // _COMPOSITE_CONTAINER_H_

#endif // EXCLUDE_FROM_BUILD 
#undef EXCLUDE_FROM_BUILD
