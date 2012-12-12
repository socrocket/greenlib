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


#ifndef REGISTER_CONSTANTS_H_
#define REGISTER_CONSTANTS_H_

namespace gs {
namespace reg {

////////////////////////////////////////////
/// register_container_addressing_mode_e
/// Defines the addressing mode of a register container.
/// ALIGNED_ADDRESS - strict alignment forcing 32 bit registers to 32 bit boundaries, 16 to 16, 8 to 8
/// INDEXED_ADDRESS - Allows non-aligned offsets to have any size register (32 bit max for now)
////////////////////////////////////////////
enum register_container_addressing_mode_e
{
	ALIGNED_ADDRESS = 0x1,
	INDEXED_ADDRESS = 0x2
};

////////////////////////////////////////////
/// register_type_e
/// Defines the register containment type at a given offset.
/// STANDARD_REG - single register entity exists at offset
/// MULTI_MODE_REG - multiple register entities exist at offset (useful for flash devices)
////////////////////////////////////////////
enum register_type_e
{
	STANDARD_REG = 0x1, // default
	MULTI_MODE_REG = 0x2
};

////////////////////////////////////////////
/// register_io_e
/// Defines the register interaction with the bus.
/// SINGLE_IO - the data is shared for both read and write transaction requests from the bus.
/// SPLIT_IO - the data for read and write are stored in seperate buffers, modification to one
///            will not proliferate to the other.
////////////////////////////////////////////
enum register_io_e
{
	SINGLE_IO = 0x100, // default
	SPLIT_IO = 0x200
};

////////////////////////////////////////////
/// register_buffer_e
/// Describes the registers write buffer (as seen from bus) (not supported at this time)
/// SINGLE_BUFFER - standard single buffer for writing data
/// DOUBLE_BUFFER - basically a que for data, often used in for video frame pointers.
////////////////////////////////////////////
enum register_buffer_e
{
	SINGLE_BUFFER = 0x10000, // default
	DOUBLE_BUFFER = 0x20000
};

////////////////////////////////////////////
/// register_data_e
/// Describes the registers focused optimization (not supported at this time)
/// FULL_WIDTH - register is accessed primarily through full width API.
/// BIT_ONLY - register is accessed primarily at the bit level.
/// BIT_RANGE - register is accessed primarily through defined bit ranges.
////////////////////////////////////////////
enum register_data_e
{
	FULL_WIDTH = 0x1000000, // default
	BIT_ONLY = 0x2000000,
	BIT_RANGE = 0x4000000
};

////////////////////////////////////////////
/// data_direction_e
/// Describes a register data buffer's direction as seen from the bus.
/// Ideally the typical user should NEVER have to deal with this enumeration.
/// DATA_IN - describes a data buffer that receives data from the bus
/// DATA_OUT - describes a data buffer that is read from the bus
////////////////////////////////////////////
enum data_direction_e
{
	DATA_IN = 0x001,
	DATA_OUT = 0x002
};

} // end namespace gs:reg
} // end namespace gs:reg

#endif /*REGISTER_CONSTANTS_H_*/
