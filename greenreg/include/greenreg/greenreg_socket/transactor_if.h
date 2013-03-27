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
// 2009-01-29 Christian Schroeder: renamings
// 2009-01-29 FURTHER CHANGES SEE SVN LOG!


#ifndef  _TRANSACTOR_IF_H_
#define _TRANSACTOR_IF_H_

#include <boost/config.hpp>
#include <systemc>

namespace tlm_components
{

    /*! \brief transactor_if: Transactor interface
    This class provides interface definition for the bus transactor
    \sa sc_interface
    */
    class transactor_if : public virtual sc_core::sc_interface
    {
    public:

	transactor_if()
	{
		m_in_port( *this);		
	}

	////////////////////////////////////////////
	/// blocking read
	/// Reads a single word, double, or byte of data from the bus.
	/// Must be called from an SC_THREAD.
	///
	/// @param _address ? absolute address to access for transaction
	/// @param _width ? with of block to transact in bytes (valid values are 1, 2 & 4)
	/// @return value read
	////////////////////////////////////////////
	virtual unsigned int read( unsigned int _address, unsigned int _width)
	{
		assert( _width > 0);
		assert( _width < 5);

		unsigned int data;
		unsigned int be = 0xf;

		if( _width == 1) be = 0x1;
		if( _width == 2) be = 0x3;
		if( _width == 3) be = 0x7;
		if( _width == 4) be = 0xf;

		_read( _address, _width, &data, false, &be);
		return data;
	}

	////////////////////////////////////////////
	/// blocking write
	/// Writes a single word, double, or byte of data from the bus.
	/// Must be called from an SC_THREAD.
	///
	/// @param _address ? absolute address to access for transaction
	/// @param _data ? data to transfer
	/// @param _width ? with of block to transact in bytes (valid values are 1, 2 & 4)
	////////////////////////////////////////////
	virtual void write(unsigned int _address, unsigned int _data, unsigned int _width)
	{
		assert( _width > 0);
		assert( _width < 5);

		unsigned int be = 0xf;

		if( _width == 1) be = 0x1;
		if( _width == 2) be = 0x3;
		if( _width == 3) be = 0x7;
		if( _width == 4) be = 0xf;

		_write( _address, _width, &_data, false, &be);
		return;
	}

	////////////////////////////////////////////
	/// non-blocking read
	/// Reads a single word, double, or byte of data from the bus.
	///
	/// @param _address ? absolute address to access for transaction
	/// @param _width ? with of block to transact in bytes (valid values are 1, 2 & 4)
	/// @return transaction_id that will be passed back in callback
	////////////////////////////////////////////
	//virtual int read_nb( unsigned int _address, unsigned int _width) = 0;

	////////////////////////////////////////////
	/// non-blocking write
	/// Writes a single word, double, or byte of data from the bus.
	///
	/// @param _address ? absolute address to access for transaction
	/// @param _data ? data to transfer
	/// @param _width ? with of block to transact in bytes (valid values are 1, 2 & 4)
	/// @return transaction_id that will be passed back in callback
	////////////////////////////////////////////
	//virtual int write_nb(unsigned int _address, unsigned int _data, unsigned int _width) = 0;

	////////////////////////////////////////////
	/// blocking manual read
	/// Allows user flexability over byte enables
	/// at a more detailed level.
	/// Must be called from an SC_THREAD.
	///
	/// @param _address ? word aligned address to access for transaction
	/// @param _length ? number of words to read
	/// @param _db ? data buffer to store transacted data in
	/// @param _bytes_enabled ? true if all bytes are enabled, false if using _be
	/// @param _be ? bit representation of bytes enabled for transaction
	////////////////////////////////////////////
	virtual void _read(unsigned _address, unsigned _length, unsigned int* _db, bool _bytes_enabled, unsigned int* _be) = 0;

	////////////////////////////////////////////
	/// blocking manual write
	/// Allows user flexability over byte enables
	/// at a more detailed level.
	/// Must be called from an SC_THREAD
	///
	/// @param _address ? word aligned address to access for transaction
	/// @param _length ? number of words to read
	/// @param _db ? data buffer to be transacted out
	/// @param _bytes_enabled ? true if all bytes are enabled, false if using _be
	/// @param _be ? bit representation of bytes enabled for transaction
	////////////////////////////////////////////
	virtual void _write(unsigned address, unsigned length, unsigned int* db, bool bytes_enabled, unsigned int* be) = 0;

	////////////////////////////////////////////
	/// non-blocking manual read
	/// Allows user flexability over byte enables
	/// at a more detailed level.
	///
	/// @param _address ? word aligned address to access for transaction
	/// @param _length ? number of words to read
	/// @param _db ? data buffer to store transacted data in
	/// @param _bytes_enabled ? true if all bytes are enabled, false if using _be
	/// @param _be ? bit representation of bytes enabled for transaction
	////////////////////////////////////////////
	//virtual int _read_nb(unsigned address, unsigned length, unsigned int* db, bool bytes_enabled, unsigned int* be) = 0;

	////////////////////////////////////////////
	/// non-blocking manual write
	/// Allows user flexability over byte enables
	/// at a more detailed level.
	///
	/// @param _address ? word aligned address to access for transaction
	/// @param _length ? number of words to read
	/// @param _db ? data buffer to be transacted out
	/// @param _bytes_enabled ? true if all bytes are enabled, false if using _be
	/// @param _be ? bit representation of bytes enabled for transaction
	////////////////////////////////////////////
	//virtual int _write_nb(unsigned address, unsigned length, unsigned int* db, bool bytes_enabled, unsigned int* be) = 0;

        //!Random Read , Initiate a read transaction to the TLM bus
        virtual void random_read()  = 0;

        //!Random Write, Initiate a write transaction to the TLM bus
        virtual void random_write()  = 0;

        sc_core::sc_export<tlm_components::transactor_if> m_in_port;
    };

} //namespace

#endif // _TRANSACTOR_IF_H_
