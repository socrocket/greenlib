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

#ifndef I_REGISTER_CONTAINER_BUS_ACCESS_H_
#define I_REGISTER_CONTAINER_BUS_ACCESS_H_

#include "greenreg/gr_externs.h"

namespace gs {
namespace reg {

////////////////////////////////////////////
/// I_register_container_bus_access
/// Interface definition for bus side stimulas of
/// registers, allowing multiple ports or other drivers
/// to bind to the same block of registers.
/// At this time the method options are limited and
/// very coarse, but they will be expanded over time.
///
/// @author 
/// @since 
////////////////////////////////////////////
class I_register_container_bus_access
{
public:
  virtual ~I_register_container_bus_access() {}
  
	////////////////////////////////////////////
	/// bus_read
	/// simulates a bus read, this is the final API
	/// used in the transaction which communicates
	/// with the register block.
	///
	/// @param _data ? data buffer to fill
	/// @param _address ? address (not absolute, offset in 0x4e)
	/// @param _byte_enable ? 0xF if all bytes enabled, first bit defines sub-offset for INDEXED mode
  /// @param _transaction ? transaction initiating this call (default NULL)
  /// @param _delayed ? if the notifications shall be delayed or called directly (default true)
	/// @return true if data was read
	////////////////////////////////////////////
	virtual bool bus_read( unsigned int& _data, unsigned int _address, unsigned int _byte_enable, transaction_type* _transaction = NULL, bool _delayed = true) = 0;

	////////////////////////////////////////////
	/// bus_write
	/// bus_read
	/// simulates a bus read, this is the final API
	/// used in the transaction which communicates
	/// with the register block.
	///
	/// @param _data ? data
	/// @param _address ? address (not absolute, offset in 0x4)
	/// @param _byte_enable ? 0xF if all bytes enabled, first bit defines sub-offset for INDEXED mode
  /// @param _transaction ? transaction initiating this call (default NULL)
  /// @param _delayed ? if the notifications shall be delayed or called directly (default true)
	/// @return true if data was written
	////////////////////////////////////////////
	virtual bool bus_write( unsigned int _data, unsigned int _address, unsigned int _byte_enable, transaction_type* _transaction = NULL, bool _delayed = true) = 0;
};

} // end namespace gs:reg
} // end namespace gs:reg

#endif /*I_REGISTER_CONTAINER_BUS_ACCESS_H_*/

#endif // EXCLUDE_FROM_BUILD
#undef EXCLUDE_FROM_BUILD


