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


#ifndef _GR_REGISTER_SPLITIO_H_
#define _GR_REGISTER_SPLITIO_H_

#include "greenreg/gr_externs.h"

#include "greenreg/internal/registers/register/register.h"
#include "greenreg/internal/registers/register_data/primary_register_data.h"
#include "greenreg/internal/registers/register_data/standard_register_data.h"

namespace gs {
namespace reg {

////////////////////////////////////////////
/// gr_register_splitio
/// implementation of a splitio register (seperate data buffer for input & output as seen from the bus)
/// @author 
/// @since 
////////////////////////////////////////////
class gr_register_splitio : public _register
{
protected:

	friend class gr_register_container;

	////////////////////////////////////////////
	/// Constructor
	///
	/// @param _container ? register_container that will bind and take ownership of this register; absolutely required
	/// @param _name ? string name reference of register for external access
	/// @param _description ? description of register
	/// @param _offset ? offset of register relative to device base
	/// @param _type ? type of register, defined as a combination of register_type, register_io, register_buffer, & register_data (orientation)
	/// @param _data ? default data, easiet to set as hex value.
	/// @param _input_mask ? write mask for data coming in from the bus.
	/// @param _width ? should always be equal to the register container addressing mode.
	/// @param _lock_mask ? intended as an override write mask, not implemented.
	///
	/// @see gr_register
	////////////////////////////////////////////
	gr_register_splitio( register_container & _container, std::string _name, std::string _description,
		uint_gr_t _offset, uint_gr_t _data, uint_gr_t _input_mask,
		unsigned int _type, uint_gr_t _width = 32, uint_gr_t _lock_mask = 0x0);

public:

	////////////////////////////////////////////
	/// Destructor
	////////////////////////////////////////////
	virtual ~gr_register_splitio();

	////////////////////////////////////////////
	/// get_primary_register_data
	/// returns the greenreg data class which wrapps
	/// the primary bitfield.
	///
	/// @return primary_register_data *
	////////////////////////////////////////////
	virtual primary_register_data * get_primary_register_data();

//	virtual void gr_dump( gr_dump_format_e _format, ::std::ostream & _stream, unsigned int _tab_level = 0);

	////////////////////////////////////////////
	/// get_user_ibuf_write_rules
	/// used when adding notification rules for stimulus to the models write (as apposed to bus) of the input buffer.
	///
	/// @return data's user side input buffer write rules container
	///
	/// @see gr_notification_rule_container
	////////////////////////////////////////////
	inline virtual gr_notification_rule_container & get_user_ibuf_write_rules()
	{ return( m_i_data.get_write_rules()); }

	////////////////////////////////////////////
	/// get_user_obuf_write_rules
	/// used when adding notification rules for stimulus to the models write (as apposed to bus) of the output buffer.
	///
	/// @param name ? description
	/// @return data's user side output buffer write rules container
	///
	/// @see gr_notification_rule_container
	////////////////////////////////////////////
	inline virtual gr_notification_rule_container & get_user_obuf_write_rules()
	{ return( m_o_data.get_write_rules()); }

private:
	///! same line short description
	standard_register_data m_i_data;

	///! same line short description
	primary_register_data m_o_data;
};

} // end namespace gs:reg
} // end namespace gs:reg

#endif /*_GR_REGISTER_SPLITIO_H_*/
