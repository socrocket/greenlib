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
//
// 2008-12-01 Christian Schroeder
// 2009-01-21 Christian Schroeder: changed namespace
// 2009-01-29 Christian Schroeder: renamings
// 2009-01-29 FURTHER CHANGES SEE SVN LOG!
//



#ifndef _GR_REGISTER_CONTAINER_H_
#define _GR_REGISTER_CONTAINER_H_

#include "greenreg/gr_externs.h"

#include "greenreg/internal/registers/register/register_container.h"

namespace gs {
namespace reg {

class device;

////////////////////////////////////////////
/// gr_register_container
/// class outlining the construction and
/// containment of registers
/// @author 
/// @since 
////////////////////////////////////////////
class gr_register_container : public register_container
{
public:

	////////////////////////////////////////////
	/// constructor
	///
	/// @param _name ? name of register container block
	/// @param _address_mode ? addressing mode of the register container, ALIGNED_ADDRESS for strict byte alignment, INDEXED_ADDRESS for adhoch alignment
	/// @param _dword_size ? ALIGNED_ADDRESS - dword size of register block (i.e. # of 32bit segments), INDEXED_ADDRESS - number of registers (regardless of size)
	/// @param _owner ? device this register container belongs to
	///
	/// @see register_container::register_container
	/// @see register_container_addressing_mode_e
	////////////////////////////////////////////
	gr_register_container( sc_core::sc_module_name _name, register_container_addressing_mode_e _address_mode, unsigned int _dword_size, device & _owner);

	////////////////////////////////////////////
	/// destructor
	///
	/// @see register_container::~register_container
	////////////////////////////////////////////
	virtual ~gr_register_container();

	////////////////////////////////////////////
	/// create_register
	///
	/// @param _name ? string name reference of register for external access
	/// @param _description ? description of register
	/// @param _offset ? offset of register relative to device base
	/// @param _type ? type of register, defined as a combination of register_type, register_io, register_buffer, & register_data (orientation)
	/// @param _data ? default data, easiet to set as hex value.
	/// @param _input_mask ? write mask for data coming in from the bus.
	/// @param _width ? should always be equal to the register container addressing mode.
	/// @param _lock_mask ? intended as an override write mask, not implemented.
	/// @return void - to be modified in the future
	///
	/// @see register_type_e, register_io_e, register_buffer_e, register_data_e
	////////////////////////////////////////////
	void create_register( std::string _name, std::string _description,
		uint_gr_t _offset, unsigned int _type,
		uint_gr_t _data = 0x0, uint_gr_t _input_mask = ~0x0,
		uint_gr_t _width = 32, uint_gr_t _lock_mask = 0x0);

	////////////////////////////////////////////
	/// create_register_block
	/// The string name for registers created with the block command
	/// are simply a concatination of the modules name and the register offset.
	///
	/// @param _description ? description of register
	/// @param _offset_start ? starting offset of registers relative to device base
	/// @param _offset_end ? ending offset of registers relative to device base
	/// @param _type ? type of register, defined as a combination of register_type, register_io, register_buffer, & register_data (orientation)
	/// @param _data ? default data, easiet to set as hex value.
	/// @param _input_mask ? write mask for data coming in from the bus.
	/// @param _width ? should always be equal to the register container addressing mode.
	/// @param _lock_mask ? intended as an override write mask, not implemented.
	/// @return void - to be modified in the future
	///
	/// @see register_type_e, register_io_e, register_buffer_e, register_data_e
	////////////////////////////////////////////
	void create_register_block( std::string _description,
		uint_gr_t _offset_start, uint_gr_t _offset_end, unsigned int _type,
		uint_gr_t _data = 0x0, uint_gr_t _input_mask = 0xFFFFFFFF,
		uint_gr_t _width = 32, uint_gr_t _lock_mask = 0x0);

};

} // end namespace gs:reg
} // end namespace gs:reg

#endif /*_GR_REGISTER_CONTAINER_H_*/
