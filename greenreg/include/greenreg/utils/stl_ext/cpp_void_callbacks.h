//   GreenReg framework
//
// LICENSETEXT
//
//   Copyright (C) 2008 : GreenSocs Ltd
//      http://www.greensocs.com/ , email: info@greensocs.com
//
//   Developed by :
//   
//   Robert Guenzel <guenzel@eis.cs.tu-bs.de>,
//   Christian Schroeder <schroeder@eis.cs.tu-bs.de>,
//     Technical University of Braunschweig, Dept. E.I.S.
//     http://www.eis.cs.tu-bs.de
//
//
//   This program is free software.
// 
//   If you have no applicable agreement with GreenSocs Ltd, this software
//   is licensed to you, and you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
// 
//   If you have a applicable agreement with GreenSocs Ltd, the terms of that
//   agreement prevail.
// 
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
// 
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
//   02110-1301  USA 
// 
// ENDLICENSETEXT

// This file is based on the drf file cpp_callbacks.h


#ifndef __CPP_VOID_CALLBACKS_H__
#define __CPP_VOID_CALLBACKS_H__

#include <cassert>

#include "cpp_callbacks.h"


namespace gs {
namespace reg_utils {
    

/*class void_callback
: public callback_base
{
public:
  virtual ~void_callback() {};
  inline virtual void execute() {}
};*/
typedef callback_0<void> void_callback;

////////////////////////////////////////////
/// void_cpp_callback
/// implementation for 0 param 0 return
/// standard cpp callback.
///
/// @author 
/// @since 
////////////////////////////////////////////
template< class C>
class void_cpp_callback_0
: public callback_0<void>
{
public:
  define_callback( cpp_callback_t, C, void);
  
  void_cpp_callback_0() : m_callback_class_instance(0) {
    callback_0<void>::is_void = true;
    callback_0<void>::number_params = 0;
  }
  
  ////////////////////////////////////////////
  /// bind
  /// Binds a class instance and method to this
  /// callback
  ///
  /// @param _callback_class_instance ? ditto
  /// @param _callback - handle to VFPT callback
  /// @return void
  ///
  /// @see reference
  ////////////////////////////////////////////
  inline void bind( C * _callback_class_instance, cpp_callback_t _callback)
  {
    m_callback_class_instance = _callback_class_instance;
    m_callback = _callback;
  }
  
  ////////////////////////////////////////////
  /// execute
  /// Asserts class callback instance then
  /// executes callback.
  ///
  /// @see cpp_callback
  ////////////////////////////////////////////
  inline virtual void execute()
  {
    assert( m_callback_class_instance != 0);
    (m_callback_class_instance->*m_callback)();
  }
  
protected:
  /// reference to class instance to execute callback on
  C * m_callback_class_instance;
  
  /// method in class type to call on execute
  cpp_callback_t m_callback;
};
  

////////////////////////////////////////////
/// cpp_callback_1
/// implementation for 1 param standard cpp
/// callback.
/// @author 
/// @since 
////////////////////////////////////////////
template< class C, typename P1>
class void_cpp_callback_1 :
public callback_1< void, P1>
{
public:
  define_callback_1( cpp_callback_t, C, void, P1);
  
  void_cpp_callback_1() : m_callback_class_instance(0) {
    callback_1< void, P1>::is_void = true;
    callback_1< void, P1>::number_params = 1;
  }
  
  ////////////////////////////////////////////
  /// bind
  /// Binds a class instance and method to this
  /// callback
  ///
  /// @param _callback_class_instance ? ditto
  /// @param _callback - handle to VFPT callback
  /// @return void
  ///
  /// @see reference
  ////////////////////////////////////////////
  inline void bind( C * _callback_class_instance, cpp_callback_t _callback)
  {
    m_callback_class_instance = _callback_class_instance;
    m_callback = _callback;
  }
  
  ////////////////////////////////////////////
  /// execute
  /// Asserts class callback instance then
  /// executes callback.
  ///
  /// @param _p1 ? parameter 1 of type P1
  ////////////////////////////////////////////
  inline virtual void execute(P1 & _p1)
  {
    assert( m_callback_class_instance != 0);
    return( (m_callback_class_instance->*m_callback)( _p1));
  }
  
protected:
  /// reference to class instance to execute callback on
  C * m_callback_class_instance;
  
  /// method in class type to call on execute
  cpp_callback_t m_callback;
};

////////////////////////////////////////////
/// cpp_callback_2
/// implementation for 2 param standard cpp
/// callback.
/// @author 
/// @since 
////////////////////////////////////////////
template< class C, typename P1, typename P2>
class void_cpp_callback_2 :
public callback_2< void, P1, P2>
{
public:
  define_callback_2( cpp_callback_t, C, void, P1, P2);
  
  void_cpp_callback_2() : m_callback_class_instance(0) {
    callback_2< void, P1, P2>::is_void = true;
    callback_2< void, P1, P2>::number_params = 2;
  }
  
  ////////////////////////////////////////////
  /// bind
  /// Binds a class instance and method to this
  /// callback
  ///
  /// @param _callback_class_instance ? ditto
  /// @param _callback - handle to VFPT callback
  /// @return void
  ///
  /// @see reference
  ////////////////////////////////////////////
  inline void bind( C * _callback_class_instance, cpp_callback_t _callback)
  {
    m_callback_class_instance = _callback_class_instance;
    m_callback = _callback;
  }
  
  ////////////////////////////////////////////
  /// execute
  /// Asserts class callback instance then
  /// executes callback.
  ///
  /// @param _p1 ? parameter 1 of type P1
  /// @param _p2 ? parameter 2 of type P2
  ///
  /// @see cpp_callback_2
  ////////////////////////////////////////////
  inline virtual void execute( P1 & _p1, P2 & _p2)
  {
    assert( m_callback_class_instance != 0);
    return( (m_callback_class_instance->*m_callback)( _p1, _p2));
  }
  
protected:
  /// reference to class instance to execute callback on
  C * m_callback_class_instance;
  
  /// method in class type to call on execute
  cpp_callback_t m_callback;
};

////////////////////////////////////////////
/// cpp_callback_3
/// implementation for 3 param standard cpp
/// callback.
/// @author 
/// @since 
////////////////////////////////////////////
template< class C, typename P1, typename P2, typename P3>
class void_cpp_callback_3 :
public callback_3< void, P1, P2, P3>
{
public:
  define_callback_3( cpp_callback_t, C, void, P1, P2, P3);
  
  void_cpp_callback_3() : m_callback_class_instance(0) {
    callback_3< void, P1, P2, P3>::is_void = true;
    callback_3< void, P1, P2, P3>::number_params = 3;
  }
  
  ////////////////////////////////////////////
  /// bind
  /// Binds a class instance and method to this
  /// callback
  ///
  /// @param _callback_class_instance ? ditto
  /// @param _callback - handle to VFPT callback
  /// @return void
  ///
  /// @see reference
  ////////////////////////////////////////////
  inline void bind( C * _callback_class_instance, cpp_callback_t _callback)
  {
    m_callback_class_instance = _callback_class_instance;
    m_callback = _callback;
  }
  
  ////////////////////////////////////////////
  /// execute
  /// Asserts class callback instance then
  /// executes callback.
  ///
  /// @param _p1 ? parameter 1 of type P1
  /// @param _p2 ? parameter 2 of type P2
  /// @param _p3 ? parameter 3 of type P3
  ///
  /// @see cpp_callback_3
  ////////////////////////////////////////////
  inline virtual void execute( P1 & _p1, P2 & _p2, P3 & _p3)
  {
    assert( m_callback_class_instance != 0);
    return( (m_callback_class_instance->*m_callback)( _p1, _p2, _p3));
  }
  
protected:
  /// reference to class instance to execute callback on
  C * m_callback_class_instance;
  
  /// method in class type to call on execute
  cpp_callback_t m_callback;
};

  

} // end namespace gs::reg_utils
} // end namespace gs::reg_utils

#endif //__CPP_VOID_CALLBACKS_H__
