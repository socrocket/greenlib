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
// 2008-11-17 Christian Schroeder: gs_params
// 2008-11-25 Christian Schroeder: gs_params<sc_attribute<T> >
// 2009-01-20 Christian Schroeder: add callback/event switch for notification rules
// 2009-01-21 Christian Schroeder: changed namespace
// 2009-01-29 Christian Schroeder: renamings
// 2009-01-29 FURTHER CHANGES SEE SVN LOG!
//


//#define EXCLUDE_FROM_BUILD
#ifndef EXCLUDE_FROM_BUILD
#undef EXCLUDE_FROM_BUILD

#include "greenreg/sysc_ext/kernel/gr_attribute.h"

//using namespace gs::reg;

namespace gs {
namespace reg {

template<>
bool gr_attribute< gr_uint_t>::set_value( const gr_uint_t& _value)
{
  gr_uint_t old_value = m_attrib.getValue().value;
  m_attrib.setValue(_value);

  if( m_write_rules != NULL)
    m_write_rules->process_active_rules( old_value, _value);

  if( m_has_event)
    m_change_event.notify();

  return(true);
}

template<>
inline const gr_uint_t& gr_attribute< gr_uint_t>::get_value()
{
  if( m_read_rules != NULL)
  {
    m_read_rules->process_active_rules( m_attrib.getValue().value, m_attrib.getValue().value);
  }

  return( m_attrib.getValue().value);
}

template<>
const inline gr_uint_t& gr_attribute< gr_uint_t>::get_value() const
{
  if( m_read_rules != NULL)
  {
    m_read_rules->process_active_rules( m_attrib.getValue().value, m_attrib.getValue().value);
  }

  return( m_attrib.getValue().value);
}

template<>
bool gr_attribute<int>::parse_config( std::string _str)
{
  std::stringstream ss;
  ss << _str;
  int i;
  ss >> i;
  set_value( i);
  return true;
}

template<>
bool gr_attribute<unsigned int>::parse_config( std::string _str)
{
  std::stringstream ss;
  ss << _str;
  unsigned int ui;
  ss >> ui;
  set_value( ui);
  return true;
}

template<>
bool gr_attribute<double>::parse_config( std::string _str)
{
  std::stringstream ss;
  ss << _str;
  double d;
  ss >> d;
  set_value( d);
  return true;
}

template<>
bool gr_attribute<std::string>::parse_config( std::string _str)
{
  set_value( _str);
  return true;
}

} // end namespace gs:reg
} // end namespace gs:reg


#endif // EXCLUDE_FROM_BUILD
#undef EXCLUDE_FROM_BUILD
