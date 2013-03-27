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
// 2008-12-03 Christian Schroeder: comment
// 2008-12-11 Christian Schroeder: changed GR_SENSITIVE macro to take a reference instead of a pointer
// 2009-01-20 Christian Schroeder: renamed DR_METHOD->GR_FUNCTION (and GR_SENSITIVE)
// 2009-01-21 Christian Schroeder: changed namespace
// 2009-01-29 Christian Schroeder: renamings
// 2009-01-29 FURTHER CHANGES SEE SVN LOG!
//


#ifndef _GR_MODULE_H_
#define _GR_MODULE_H_

#include "greenreg/framework/core/I_gr_dump.h"
#include "greenreg/framework/core/gr_common.h"
#include <boost/config.hpp>
#include <systemc>

#include "greenreg/sysc_ext/kernel/gr_sc_sensitive.h"
#include "greenreg/sysc_ext/kernel/gr_event.h"

namespace gs {
namespace reg {

/// GR_FUNCTION without params, needs empty param callback signature
#define GR_FUNCTION(class_name, method_name)                                          \
  {                                                                                   \
    this->m_current_callback = new gs::reg_utils::void_cpp_callback_0< class_name>(); \
    this->m_callbacks.push_back( this->m_current_callback );                          \
    dynamic_cast< gs::reg_utils::void_cpp_callback_0< class_name> *>(this->m_current_callback)->bind( this, &class_name::method_name);  \
  }                                      

/// GR_FUNCTION with params, needs callback signature with params
#define GR_FUNCTION_PARAMS(class_name, method_name)                                   \
  {                                                                                   \
    this->m_current_callback = new gs::reg_utils::void_cpp_callback_2< class_name, gs::reg::transaction_type*, const sc_core::sc_time>();   \
    this->m_callbacks.push_back( this->m_current_callback );                          \
    dynamic_cast< gs::reg_utils::void_cpp_callback_2< class_name, gs::reg::transaction_type*, const sc_core::sc_time> *>  \
      (this->m_current_callback)->bind( this, &class_name::method_name);              \
  }

/// GR_METHOD is a GR_FUNCTION without params but being a SC_METHOD
#define GR_METHOD(class_name, method_name)                                          \
  {                                                                                   \
    SC_METHOD(method_name);                                                           \
    this->m_current_callback = new gs::reg_utils::void_cpp_callback_0< class_name>(); \
    this->m_callbacks.push_back( this->m_current_callback );                          \
    dynamic_cast< gs::reg_utils::void_cpp_callback_0< class_name> *>(this->m_current_callback)->bind( this, &class_name::method_name);  \
  }                                      

/// Adds sensitivity to GR_FUNCTION or GR_FUNCTION_PARAMS without a delay
#define GR_SENSITIVE(event)                      \
  {                                              \
    gs::reg::gr_event &ev = event;               \
    ev.disable_timing();                         \
    sensitive << ev;                             \
    gr_sensitive << ev;                          \
  }

/// Adds sensitivity to GR_FUNCTION or GR_FUNCTION_PARAMS with a delay
#define GR_DELAYED_SENSITIVE(event, delay)       \
  {                                              \
    gs::reg::gr_event &ev = event;               \
    ev.disable_timing();                         \
    sensitive << ev;                             \
    gr_sensitive << ev;                          \
    ev.set_delay(delay);                         \
  }


////////////////////////////////////////////
/// gr_module
/// base object for most gr_ element types
/// mirrored as much of the API to match 
/// sc_object as possible.
///
/// @author 
/// @since 
////////////////////////////////////////////
class gr_module : public sc_core::sc_module, public I_gr_dump
{
public:
  SC_HAS_PROCESS( gr_module);
  ////////////////////////////////////////////
  /// constructor
  /// forces user to enter name
  ////////////////////////////////////////////
  // no default constructor on purpose
  gr_module( sc_core::sc_module_name _name);
  virtual ~gr_module();

  ////////////////////////////////////////////
  /// kind
  /// forces user to implement kind (TOUGH, DEAL WITH IT!)
  ///
  /// @see sc_object
  ////////////////////////////////////////////
  virtual const char * kind() const = 0;

  ////////////////////////////////////////////
  /// @see I_gr_dump
  /// DEPRICATED
  ////////////////////////////////////////////
//  void gr_dump( gr_dump_format_e _format, std::ostream & _stream);

protected:
  friend class gr_sc_sensitive;
  
  gr_sc_sensitive gr_sensitive;
//  gr_event test_event;
//  void test() { printf( "hello world"); }

  gs::reg_utils::callback_base * m_current_callback;
  
  typedef std::vector< gs::reg_utils::callback_base * > callback_storage_type;
  callback_storage_type m_callbacks;
  
  ////////////////////////////////////////////
  /// gr_dump_self
  /// forced for all descendents of gr_object
  /// to implement.  Same as gr_dump, except
  /// that gr_object will call this method.
  /// DEPRICATED
  ///@see gr_dump
  ////////////////////////////////////////////
//  virtual void gr_dump_self( gr_dump_format_e _format, std::ostream & _stream) = 0;
};


} // end namespace gs:reg
} // end namespace gs:reg

#endif /*_GR_MODULE_H_*/
