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


#ifndef _CPP_CALLBACKS_H_
#define _CPP_CALLBACKS_H_

#include <cassert>


namespace gs {
namespace reg_utils {

  
////////////////////////////////////////////
/// macro for defining a 0 parameter callback
////////////////////////////////////////////
#define define_callback( TYPE_NAME, CALLBACK_CLASS, RETURN_TYPE) typedef RETURN_TYPE(CALLBACK_CLASS::*TYPE_NAME)()

////////////////////////////////////////////
/// macro for defining a 1 parameter callback
////////////////////////////////////////////
#define define_callback_1( TYPE_NAME, CALLBACK_CLASS, RETURN_TYPE, PARAM_1) typedef RETURN_TYPE(CALLBACK_CLASS::*TYPE_NAME)(PARAM_1 &)

////////////////////////////////////////////
/// macro for defining a 2 parameter callback
////////////////////////////////////////////
#define define_callback_2( TYPE_NAME, CALLBACK_CLASS, RETURN_TYPE, PARAM_1, PARAM_2) typedef RETURN_TYPE(CALLBACK_CLASS::*TYPE_NAME)(PARAM_1 &, PARAM_2 &)

////////////////////////////////////////////
/// macro for defining a 3 parameter callback
////////////////////////////////////////////
#define define_callback_3( TYPE_NAME, CALLBACK_CLASS, RETURN_TYPE, PARAM_1, PARAM_2, PARAM_3) typedef RETURN_TYPE(CALLBACK_CLASS::*TYPE_NAME)(PARAM_1 &, PARAM_2 &, PARAM_3 &)



/// Base class for all callback classes (nearly useless)
class callback_base
{
public:
  virtual ~callback_base() {};
  bool is_void;
  unsigned int number_params;
};


////////////////////////////////////////////
/// callback
/// handle to 0 param callback defined as a templated interface
/// without the I_ definition.  This allows the callbacks
/// to be contained externally.
/// @author 
/// @since 
////////////////////////////////////////////
template< typename R>
class callback_0
: public callback_base
{
public:
  
  ////////////////////////////////////////////
  /// execute
  /// virtual method place holder for execute
  ///
  /// @return value of type R
  ///
  /// @see callback
  /// @see templates
  ////////////////////////////////////////////
  virtual R execute() = 0;
};

  
////////////////////////////////////////////
/// callback_1
/// handle to 1 param callback defined as a templated interface
/// without the I_ definition.  This allows the callbacks
/// to be contained externally.
/// @author 
/// @since 
////////////////////////////////////////////
template< typename R, typename P1>
class callback_1
: public callback_base
{
public:
  
  ////////////////////////////////////////////
  /// execute
  /// virtual method place holder for execute
  ///
  /// @param _p1 ? parameter 1 of type P1
  /// @return value of type R
  ///
  /// @see callback_1
  ////////////////////////////////////////////
  virtual R execute(P1 & _p1) = 0; // {R t; return(t);}
};

  
////////////////////////////////////////////
/// callback_2
/// handle to 2 param callback defined as a templated interface
/// without the I_ definition.  This allows the callbacks
/// to be contained externally.
/// @author 
/// @since 
////////////////////////////////////////////
template< typename R, typename P1, typename P2>
class callback_2
: public callback_base
{
public:
  
  ////////////////////////////////////////////
  /// execute
  /// virtual method place holder for execute
  ///
  /// @param _p1 ? parameter 1 of type P1
  /// @param _p2 ? parameter 2 of type P2
  /// @return value of type R
  ///
  /// @see callback_2
  ////////////////////////////////////////////
  virtual R execute(P1 & _p1, P2 & _p2) = 0; //{R t; return(t);}
};

  
////////////////////////////////////////////
/// callback_3
/// handle to 3 param callback defined as a templated interface
/// without the I_ definition.  This allows the callbacks
/// to be contained externally.
/// @author 
/// @since 
////////////////////////////////////////////
template< typename R, typename P1, typename P2, typename P3>
class callback_3
: public callback_base
{
public:
  
  ////////////////////////////////////////////
  /// execute
  /// virtual method place holder for execute
  ///
  /// @param _p1 ? parameter 1 of type P1
  /// @param _p2 ? parameter 2 of type P2
  /// @param _p3 ? parameter 3 of type P3
  /// @return value of type R
  ///
  /// @see callback_3
  ////////////////////////////////////////////
  virtual R execute(P1 & _p1, P2 & _p2, P3 & _p3) = 0; // {R t; return(t);}
};

  


} // end namespace gs::reg_utils
} // end namespace gs::reg_utils

#endif //_CPP_CALLBACKS_H_
