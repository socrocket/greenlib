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
// 2009-01-10 Puneet Arora: fixed the functions using left shift operator
// to not use it when shift_count is equal to width of register
// 2009-01-21 Christian Schroeder: changed namespace
// 2009-01-29 Christian Schroeder: renamings
// 2009-01-29 FURTHER CHANGES SEE SVN LOG!
//

#include "greenreg/internal/registers/bit_range/bit_range_accessor.h"

#include <sstream>

#include "greenreg/sysc_ext/utils/gr_report.h"

#include "greenreg/internal/registers/register/I_register.h"
#include "greenreg/internal/registers/bit_range/bit_range.h"
#include "greenreg/internal/registers/bit_range/sharedio_bit_range.h"
#include "greenreg/internal/registers/bit_range/splitio_bit_range.h"

namespace gs {
namespace reg {

bit_range_accessor::bit_range_accessor( I_register & _register)
: m_register( &_register)
{
}

bit_range_accessor::~bit_range_accessor()
{
	std::map< std::string, bit_range *>::iterator mit = m_bits.begin();
	
	while( mit != m_bits.end())
	{
		delete( mit->second);
		m_bits.erase( mit);
		mit = m_bits.begin();
	}
}

bit_range & bit_range_accessor::operator [] ( std::string _bit_range_name)
{
	if( m_bits.find( _bit_range_name) != m_bits.end())
		return *(m_bits[ _bit_range_name]);
	std::stringstream ss;
	ss << "Invalid bit range name: '" << _bit_range_name << "', please check code!\n";
	GR_FATAL    ( ss.str().c_str());
	return *(m_bits[ _bit_range_name]);
}
	
bool bit_range_accessor::create( std::string _bit_range_name, gr_uint_t _start_bit, gr_uint_t _end_bit)
{
	// Check for code errors
	std::stringstream ss;
	if( (m_bits.find( _bit_range_name) != m_bits.end()))
        {
		ss << "Bit range named '" << _bit_range_name << "' already exists.\n";
		GR_FATAL    ( ss.str().c_str());
		return( false); // avoid compiler warn
        }
	if( ( _end_bit >= m_register->get_width()))
        {
		ss << "The end bit for bit range named '" << _bit_range_name << "' pass the end of register width (" << _end_bit << " >= " << m_register->get_width() << ").\n";
		GR_FATAL    ( ss.str().c_str());
		return( false); // avoid compiler warn
        }

	if( m_register->get_register_type() == gs::reg::SPLIT_IO)
	{
		m_bits[ _bit_range_name] = new splitio_bit_range( _bit_range_name, *m_register, _start_bit, _end_bit);
	} else { // it must be shared
		m_bits[ _bit_range_name] = new sharedio_bit_range( _bit_range_name, *m_register, _start_bit, _end_bit);
	}
	return( true);
}
	
void bit_range_accessor::process_tlm_pre_read_rules( uint_gr_t _old_value, uint_gr_t _new_value, transaction_type* _transaction, bool _delayed)
{
	std::map< std::string, bit_range * >::iterator mit;

	for( mit = m_bits.begin(); mit != m_bits.end(); mit++)
	{
		bit_range * br = mit->second;

                unsigned int shift_count = ((br->m_bit_range_end + 1) - br->m_bit_range_start);
                gr_uint_t temp_old_value = _old_value;
                gr_uint_t temp_new_value = _new_value;
                //assuming, if shift_count == register_width, then 
                //(bit_range_start == 0) and following processing is actually no-op
                if (m_register->get_width() != shift_count) {
                  temp_old_value = (_old_value >> br->m_bit_range_start) & ~( ~0x0 << shift_count);
                  temp_new_value = (_new_value >> br->m_bit_range_start) & ~( ~0x0 << shift_count);
                }

		if( br->m_pre_read_rules != NULL)
		{
			br->m_pre_read_rules->process_active_rules( temp_old_value, temp_new_value, _transaction, _delayed);
		}
	}
}

void bit_range_accessor::process_tlm_post_read_rules( uint_gr_t _old_value, uint_gr_t _new_value, transaction_type* _transaction, bool _delayed)
{
	std::map< std::string, bit_range * >::iterator mit;

	for( mit = m_bits.begin(); mit != m_bits.end(); mit++)
	{
		bit_range * br = mit->second;

                unsigned int shift_count = ((br->m_bit_range_end + 1) - br->m_bit_range_start);
                gr_uint_t temp_old_value = _old_value;
                gr_uint_t temp_new_value = _new_value;
                //assuming, if shift_count == register_width, then 
                //(bit_range_start == 0) and following processing is actually no-op
                if (m_register->get_width() != shift_count) {
                  temp_old_value = (_old_value >> br->m_bit_range_start) & ~( ~0x0 << shift_count);
                  temp_new_value = (_new_value >> br->m_bit_range_start) & ~( ~0x0 << shift_count);
                }

		if( br->m_post_read_rules != NULL)
		{
			br->m_post_read_rules->process_active_rules( temp_old_value, temp_new_value, _transaction, _delayed);
		}
	}
}

void bit_range_accessor::process_tlm_pre_write_rules( uint_gr_t _old_value, uint_gr_t _new_value, transaction_type* _transaction, bool _delayed)
{
	std::map< std::string, bit_range * >::iterator mit;

	for( mit = m_bits.begin(); mit != m_bits.end(); mit++)
	{
		bit_range * br = mit->second;

                unsigned int shift_count = ((br->m_bit_range_end + 1) - br->m_bit_range_start);
                gr_uint_t temp_old_value = _old_value;
                gr_uint_t temp_new_value = _new_value;
                //assuming, if shift_count == register_width, then 
                //(bit_range_start == 0) and following processing is actually no-op
                if (m_register->get_width() != shift_count) {
                  temp_old_value = (_old_value >> br->m_bit_range_start) & ~( ~0x0 << shift_count);
                  temp_new_value = (_new_value >> br->m_bit_range_start) & ~( ~0x0 << shift_count);
                }

		if( br->m_pre_write_rules != NULL)
		{
			br->m_pre_write_rules->process_active_rules( temp_old_value, temp_new_value, _transaction, _delayed);
		}
	}
}

void bit_range_accessor::process_tlm_post_write_rules( uint_gr_t _old_value, uint_gr_t _new_value, transaction_type* _transaction, bool _delayed)
{
	std::map< std::string, bit_range * >::iterator mit;

	for( mit = m_bits.begin(); mit != m_bits.end(); mit++)
	{
		bit_range * br = mit->second;

        unsigned int shift_count = ((br->m_bit_range_end + 1) - br->m_bit_range_start);
        gr_uint_t temp_old_value = _old_value;
        gr_uint_t temp_new_value = _new_value;
        //assuming, if shift_count == register_width, then 
        //(bit_range_start == 0) and following processing is actually no-op
        if (m_register->get_width() != shift_count) {
          temp_old_value = (_old_value >> br->m_bit_range_start) & ~( ~0x0 << shift_count);
          temp_new_value = (_new_value >> br->m_bit_range_start) & ~( ~0x0 << shift_count);
        }

		if( br->m_post_write_rules != NULL)
		{
			br->m_post_write_rules->process_active_rules( temp_old_value, temp_new_value, _transaction, _delayed);
		}
	}
}

void bit_range_accessor::process_ibuf_write_rules( uint_gr_t _old_value, uint_gr_t _new_value)
{
	std::map< std::string, bit_range * >::iterator mit;

	for( mit = m_bits.begin(); mit != m_bits.end(); mit++)
	{
		bit_range * br = mit->second;

        unsigned int shift_count = ((br->m_bit_range_end + 1) - br->m_bit_range_start);
        gr_uint_t temp_old_value = _old_value;
        gr_uint_t temp_new_value = _new_value;
        //assuming, if shift_count == register_width, then 
        //(bit_range_start == 0) and following processing is actually no-op
        if (m_register->get_width() != shift_count) {
          temp_old_value = (_old_value >> br->m_bit_range_start) & ~( ~0x0 << shift_count);
          temp_new_value = (_new_value >> br->m_bit_range_start) & ~( ~0x0 << shift_count);
        }

		if( br->i.m_write_rules != NULL)
		{
			br->i.m_write_rules->process_active_rules( temp_old_value, temp_new_value);
		}
	}
}

void bit_range_accessor::process_obuf_write_rules( uint_gr_t _old_value, uint_gr_t _new_value)
{
	std::map< std::string, bit_range * >::iterator mit;

	for( mit = m_bits.begin(); mit != m_bits.end(); mit++)
	{
		bit_range * br = mit->second;
		
                unsigned int shift_count = ((br->m_bit_range_end + 1) - br->m_bit_range_start);
                gr_uint_t temp_old_value = _old_value;
                gr_uint_t temp_new_value = _new_value;
                //assuming, if shift_count == register_width, then 
                //(bit_range_start == 0) and following processing is actually no-op
                if (m_register->get_width() != shift_count) {
                  temp_old_value = (_old_value >> br->m_bit_range_start) & ~( ~0x0 << shift_count);
                  temp_new_value = (_new_value >> br->m_bit_range_start) & ~( ~0x0 << shift_count);
                }
		
		if( br->o.m_write_rules != NULL)
		{
			br->o.m_write_rules->process_active_rules( temp_old_value, temp_new_value);
		}
	}
}
	
void bit_range_accessor::disable_events() {
  std::map< std::string, bit_range * >::iterator mit;
  for( mit = m_bits.begin(); mit != m_bits.end(); mit++) {
	  bit_range * br = mit->second;
    br->disable_events();
  }
}

void bit_range_accessor::enable_events() {
  std::map< std::string, bit_range * >::iterator mit;
  for( mit = m_bits.begin(); mit != m_bits.end(); mit++) {
	  bit_range * br = mit->second;
    br->enable_events();
  }
}

} // end namespace gs:reg
} // end namespace gs:reg
