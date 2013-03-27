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


#ifndef SUBDEVICE_CONTAINER_H_
#define SUBDEVICE_CONTAINER_H_

// std
#include <iostream>

// GREENREG_UTILS
#include "greenreg/utils/storage/access_data_container.h"

// greenreg
#include "greenreg/framework/core/gr_common.h"
#include "I_device_container.h"
#include "subdevice.h"

namespace gs {
namespace reg {

////////////////////////////////////////////
/// subdevice_container
/// A glorified access_data_contianer of type subdevice which implementes I_dump
/// due to inheritance from I_device_container.
/// @author 
/// @since 
////////////////////////////////////////////
class subdevice_container : public I_device_container, public ::gs::reg_utils::access_data_container< subdevice>
{
public:

	///! same line short description
	subdevice_container();

	///! same line short description
	virtual ~subdevice_container();

	///@see I_dump
	virtual void gr_dump( gr_dump_format_e _format, ::std::ostream & _stream, unsigned int _tab_level = 0);
};

} // end namespace gs:reg
} // end namespace gs:reg

#endif /*SUBDEVICE_CONTAINER_H_*/
