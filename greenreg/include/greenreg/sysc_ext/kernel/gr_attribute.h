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

#ifndef _GR_ATTRIBUTE_H_
#define _GR_ATTRIBUTE_H_

#include <string>
#include <vector>

#include <boost/config.hpp>
#include <systemc>

#include "greenreg/utils/storage/option_list.h"
#include "greenreg/utils/stl_ext/string_utils.h"

#include "greenreg/framework/core/gr_common.h"
#include "gr_attribute_base.h"
#include "greenreg/sysc_ext/utils/gr_report.h"

#include "I_event_switch.h"

#include "greencontrol/config.h"

namespace gs {
namespace reg {

class gr_attribute_container;

////////////////////////////////////////////
/// gr_attribute
/// a wrapper class for sc_attribute (CHANGED to gs_param) which
/// adds several key funtionalities missing
/// in systemc implementation:
/// * operator overloading for data (remove .value)
/// * forced type specification for non try-catch style casting
/// * support for gr_dump
/// * event notification on content change (disabled by default)
///
/// note: type is forced in template declaration
///
/// @author 
/// @since 
////////////////////////////////////////////
template< class T>
class gr_attribute 
: public gr_attrib_base
, public I_event_switch
{
public:

  ////////////////////////////////////////////
  /// constructor
  /// forces name specification
  ///
  /// @param _name ? name of attribute
  ///
  /// @see gr_attrib_base
  ////////////////////////////////////////////
  gr_attribute( std::string _name, const std::string _description, gr_attribute_container & _a, T _t)
  : gr_attrib_base( _name, _description, _a)
  , m_attrib( _name, _name, _t) // sc_attrib name, param name, value
  {
    m_type = gs::reg_utils::determine<T>::type();
    set_value( _t);
  }

  virtual ~gr_attribute()
  {
  }

  ////////////////////////////////////////////
  /// default operator
  /// return value from attribute reducing
  /// confusion to user.
  ///
  /// note: optimzes out in release build
  ///
  /// @return reference to value of attribute
  ////////////////////////////////////////////
  inline virtual operator const T ()
  { return( get_value() ); }

  ////////////////////////////////////////////
  /// assignment operator
  /// sets sc_attribute value and returns self
  ///
  /// @param _value - value to set
  /// @return reference of this
  ////////////////////////////////////////////
  inline virtual gr_attribute<T>& operator = ( const T& _value)
  {
    set_value( _value);
    return(*this);
  }

  ////////////////////////////////////////////
  /// set_value
  /// performs the actual set value and event
  /// notification of change if enabled.
  ///
  /// @param _value = value to set
  /// @return if value was in valid "range" (not applicable in gr_attribute)
  /// @see gr_switch
  ////////////////////////////////////////////
  virtual inline bool set_value( const T& _value)
  {
    m_attrib.setValue(_value);
    if( m_has_event) m_change_event.notify();
    return(true);
  }

  ////////////////////////////////////////////
  /// get_value
  ///
  /// @return const reference to attribute value
  ////////////////////////////////////////////
  inline const T& get_value()
  { return( m_attrib.getValue().value); }

  const inline T& get_value() const
  { return( m_attrib.getValue().value); }

  ////////////////////////////////////////////
  /// get_sc_attribute
  ///
  /// @return reference to contained sc_attribute
  ////////////////////////////////////////////
  /*sc_core::sc_attribute<T> & get_sc_attribute()
  { return m_attrib.getValue(); }*/

  const sc_core::sc_attribute<T> & get_sc_attribute() const
  { return m_attrib.getValue(); }

  /// @see gr_attrib_base
  /// @see I_gr_dump
  virtual void gr_dump( gr_dump_format_e _format, std::ostream & _stream, unsigned int _tab_level = 0)
  {
    _stream << gs::reg_utils::string::tabs( _tab_level).c_str() << "# " << m_description << "\n"
        << gs::reg_utils::string::tabs( _tab_level).c_str() << m_attrib.name() << " = " << m_attrib.getString() << "\n\n";
//        << gs::reg_utils::string::tabs( _tab_level).c_str() << m_attrib.name() << " = " << std::hex << m_attrib.getString() << "\n\n";
  }

  /// @see sc_object
  virtual const char * kind() const
  {
    std::stringstream ss;
    ss << "gr_attribute<" << gs::reg_utils::get_type_string( m_type) << ">";
    return( ss.str().c_str()) ;
  }

  virtual bool parse_config( std::string _str)
  {
    std::stringstream ss;

    ss << "Trying to call parse_config on type: "
      << typeid( *this).name()
      << " which does not of template specialization implemented, do not know how to handle, ignoring!!!";
    GR_FORCE_WARNING( ss.str().c_str());

    return false;
  }

  /// @see I_event_switch
  virtual void disable_events() {
    I_event_switch::disable_events(); // updated the state bool
    // switch all owned notification rule container
    get_pre_read_rules().disable_events();
    get_post_write_rules().disable_events();
  }
  
  /// @see I_event_switch
  virtual void enable_events() {
    I_event_switch::enable_events(); // updated the state bool
    // switch all owned notification rule container
    get_pre_read_rules().enable_events();
    get_post_write_rules().enable_events();
  }
  
protected:

  /// contained attribute
  //sc_attribute<T> m_attrib;
  gs::gs_param< sc_core::sc_attribute<T> > m_attrib;
};

template<>
bool gr_attribute< gr_uint_t>::set_value( const gr_uint_t& _value);

template<>
bool gr_attribute<int>::parse_config( std::string _str);

template<>
bool gr_attribute<unsigned int>::parse_config( std::string _str);

template<>
bool gr_attribute<double>::parse_config( std::string _str);

template<>
bool gr_attribute<std::string>::parse_config( std::string _str);

} // end namespace gs::reg
} // end namespace gs::reg

#endif // _GR_ATTRIBUTE_H_
#endif // EXCLUDE_FROM_BUILD
#undef EXCLUDE_FROM_BUILD
