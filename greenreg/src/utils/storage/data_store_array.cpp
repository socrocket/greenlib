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


#include "greenreg/utils/storage/data_store_array.h"

namespace gs {
namespace reg_utils {

data_store_array::data_store_array( std::string _name, unsigned int _array_dword_size)
{
	m_array = new unsigned int[ _array_dword_size];
	m_array_size = _array_dword_size * 4;
	m_name = _name;
}

data_store_array::~data_store_array()
{
	delete[] m_array;
	m_array = 0;
}

unsigned int data_store_array::read( unsigned int _byte_offset, unsigned int _byte_size)
{
	unsigned char * byte_ptr = 0;
	unsigned int ret_val = 0;
	unsigned char * ret_ptr = reinterpret_cast< unsigned char *>( &ret_val);
	unsigned int ret_mask = 0;
	
	// calculate the dword and byte offsets
	unsigned int dword_offset = (_byte_offset == 0 ? 0 : _byte_offset / 4);
	unsigned int byte_offset = _byte_offset % 4;
	
	// adjust the byte pointer to the exact offset in the array
	byte_ptr = reinterpret_cast< unsigned char *>( &(m_array[dword_offset]));
	byte_ptr += byte_offset;

	// now read the correct number of bytes
	for( unsigned int counter = 0; counter < _byte_size; counter++)
	{
		ret_ptr[counter] = byte_ptr[counter];
		
		// bit-shift the mask left by 8
		ret_mask = ret_mask << 8;
		
		// add 0xff
		ret_mask += 0xff;
	}
	
	// finally we can return the data...
	return( ret_val & ret_mask);
}

void data_store_array::write( unsigned int _byte_offset, unsigned int _byte_size, unsigned int _data)
{
	unsigned char * byte_ptr = 0;
	unsigned char * data_ptr = reinterpret_cast< unsigned char *>( &_data);
	
	// calculate the dword and byte offsets
	unsigned int dword_offset = (_byte_offset == 0 ? 0 : _byte_offset / 4);
	unsigned int byte_offset = _byte_offset % 4;
	
	// adjust the byte pointer to the exact offset in the array
	byte_ptr = reinterpret_cast< unsigned char *>( &(m_array[dword_offset]));
	byte_ptr += byte_offset;

	// now write the correct number of bytes
	for( unsigned int counter = 0; counter < _byte_size; counter++)
	{
		byte_ptr[counter] = data_ptr[counter];
	}
}

unsigned long data_store_array::size()
{
	return( m_array_size);
}

} // end namespace gs:reg_utils
} // end namespace gs:reg_utils
