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

#ifndef _REGISTER_CONTAINER_H_
#define _REGISTER_CONTAINER_H_

#include "greenreg/utils/storage/addressable_owning_container.h"

#include <boost/config.hpp>
#include <systemc>

#include "greenreg/utils/storage/data_store_array.h"

#include "greenreg/gr_externs.h"
#include "greenreg/framework/core/I_gr_dump.h"
#include "greenreg/internal/registers/register/I_register.h"
#include "I_register_container_bus_access.h"

namespace gs {
namespace reg {

////////////////////////////////////////////
/// register_container
/// Specialized container for storage and access to registers.
/// Responsible for declaration of the memory serving as the register
/// data bank and stimulus hooks which are bound to a locally held PVTarget_port(),
/// @see gs::reg_utils::addressable_owning_container, I_register, I_gr_dump
/// @author 
/// @since 
////////////////////////////////////////////
class register_container : public sc_core::sc_module, public ::gs::reg_utils::addressable_owning_container< uint_gr_t, I_register>, public I_register_container_bus_access, public I_gr_dump
{
public:

	////////////////////////////////////////////
	/// Constructor
	///
	/// @param _name ? name of register container block
	/// @param _address_mode ? addressing mode of the register container, ALIGNED_ADDRESS for strict byte alignment, INDEXED_ADDRESS for adhoch alignment
	/// @param _dword_size ? ALIGNED_ADDRESS - dword size of register block (i.e. # of 32bit segments), INDEXED_ADDRESS - number of registers (regardless of size)
	////////////////////////////////////////////
	register_container( sc_core::sc_module_name _name, register_container_addressing_mode_e _address_mode, unsigned int _dword_size);

	////////////////////////////////////////////
	/// Destructor
	////////////////////////////////////////////
	virtual ~register_container();

	///@see I_dump
	virtual void gr_dump( gr_dump_format_e _format, ::std::ostream & _stream, unsigned int _tab_level = 0);

	///@see I_register_container_bus_access
	virtual bool bus_read( unsigned int& _data, unsigned int _address, unsigned int _offset, transaction_type* _transaction = NULL, bool _delayed = true);

	///@see I_register_container_bus_access
	virtual bool bus_write( unsigned int _data, unsigned int _address, unsigned int _offset, transaction_type* _transaction = NULL, bool _delayed = true);

	////////////////////////////////////////////
	/// get_addressing_mode
	/// returns the current addressing mode of the register container
	///
	/// @return addressing mode of the register container
	////////////////////////////////////////////
	register_container_addressing_mode_e get_addressing_mode();

	////////////////////////////////////////////
	/// get_active_register
	/// returns the current register to allow simple user manipulation
	///
	/// @return active register pointer (may be null if no register is active)
	////////////////////////////////////////////
	inline I_register * get_active_register() { return m_active_register; }
	
protected:

	friend class primary_register_data;

	////////////////////////////////////////////
	/// get_next_register_index
	/// used when creating a register to verify
	/// it is within the pre-allocated range
	///
	/// @return new register index if valid, -1 if invalid or not in index mode
	////////////////////////////////////////////
	int get_next_register_index();

	gs::reg_utils::data_store_array m_register_input_store;
	
	/// addressing mode of the register container
	register_container_addressing_mode_e m_addressing_mode;

	/// index counter for adding the next register
	int m_next_register_index;
	
	/// active register in transaction
	I_register * m_active_register;
};

} // end namespace gs:reg
} // end namespace gs:reg

#endif // _REGISTER_CONTAINER_H_

#endif // EXCLUDE_FROM_BUILD
#undef EXCLUDE_FROM_BUILD
