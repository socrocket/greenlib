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

//
// ChangeLog GreenSocs
// 2009-01-29 Christian Schroeder: renamings
// 2009-01-29 FURTHER CHANGES SEE SVN LOG!
//


#ifndef _GREENREG_UTILS_H_
#define _GREENREG_UTILS_H_


// patterns
#include "greenreg/utils/patterns/cache_item.h" ///! implementation of cache design pattern
#include "greenreg/utils/patterns/singleton.h" ///! imlementation of singleton design pattern
#include "greenreg/utils/patterns/composite.h" ///! implementation of composite (set based) design pattern
#include "greenreg/utils/patterns/composite_map.h" ///! UNDER DEVELOPMENT, MAY BE REMOVED

// stl_ext
#include "greenreg/utils/stl_ext/cpp_callbacks.h" ///! implementation of callbacks ~93% faster than boost
#include "greenreg/utils/stl_ext/cpp_return_callbacks.h"
#include "greenreg/utils/stl_ext/cpp_void_callbacks.h"
#include "greenreg/utils/stl_ext/string_utils.h" ///! a plethora of utilities for std::string
#include "greenreg/utils/stl_ext/file_stream_tokenizer.h" ///! a very old, very powerful stream tokenizer
#include "greenreg/utils/stl_ext/base_types.h" ///! determine base type through overridden static functions

// others
#include "greenreg/utils/gr_cbpeq.h" ///! Payload event queue for delaying callbacks

// storage
#include "greenreg/utils/storage/lookup_table.h" ///! a lockable table of key_value pairs
#include "greenreg/utils/storage/option_structs.h" ///! structs for easily defining options lists
#include "greenreg/utils/storage/option_list.h" ///! special implementation of lookup_table to support option structs or dynamic use cases
#include "greenreg/utils/storage/I_data_container.h" ///! base interface for defining a data_container
#include "greenreg/utils/storage/data_container.h" ///! general implementation for the I_data_container interface
#include "greenreg/utils/storage/access_data_container_ref.h" ///! don't use access_data_container *, use this class instead!!! overloads operator[] access even from reference
#include "greenreg/utils/storage/access_data_container.h" ///! addes operator[] access to data_container
#include "greenreg/utils/storage/composite_container.h" ///! implementation of a composite container, no data (why would you want that?!?!)
#include "greenreg/utils/storage/composite_reference_container.h" ///! decends from data_container and composite_container to provide a composite (data) reference container (that's why)
#include "greenreg/utils/storage/dynamic_classtype_enumeration.h" ///! a crude attempt at providing an enumeration equvilant of class types
#include "greenreg/utils/storage/ext_map.h" ///! IGNORE, MAY BE REMOVED. bi-directional map, implementation was wacked during port.
#include "greenreg/utils/storage/properties_container.h" ///! a container for string based properties (actually, just a defined composite_reference_container)

// file_formats
//#include "simple_file_format.h" ///! enable simple mechanism for implementing file formats
//#include "properties_file_format.h" ///! the basic properties format for DRF, only load is used by DRF, write exists for purpose of demonstration

#endif /*_GREENREG_UTILS_H_*/
