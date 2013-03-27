//   GreenReg framework
//
// LICENSETEXT
//
//   Copyright (C) 2008 : GreenSocs Ltd
//      http://www.greensocs.com/ , email: info@greensocs.com
//
//   Developed by :
//   
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

//
// based on I_register.h
//

namespace gs {
namespace reg {
  
  ////////////////////////////////////////////
  /// gr_reg_rule_container_e
  /// enumeration of notification rule containers for add_rule capability
  /// PRE_READ - rules will execute prior to a read request to register
  /// POST_READ - rules will execute after a read request from register
  /// PRE_WRITE - rules will execute prior to a write request to register
  /// POST_WRITE - rules will execute after a write request to register
  /// USR_IN_WRITE - rules will execute after a model side write to the input buffer
  /// USR_OUT_WRITE - rules will execute after a model side write to the output buffer
  /// @author 
  /// @since 
  ////////////////////////////////////////////
  enum gr_reg_rule_container_e
  {
    PRE_READ = 0,
    POST_READ,
    PRE_WRITE,
    POST_WRITE,
    USR_IN_WRITE,
    USR_OUT_WRITE
  };
  
  ////////////////////////////////////////////
  /// gr_reg_rule_type_e
  /// enumeration of notification rule type for add_rule_capability
  ///	NOTIFY - (READ, WRITE) simple notification of stimulus (no checks)
  ///	WRITE_PATTERN_EQUAL - (WRITE) checks for a specified pattern, active on every write
  ///	READ_PATTERN_EQUAL - (READ) checks for a specified pattern, active on every read
  ///	PATTERN_STATE_CHANGE - (WRITE) checks for a specified pattern state change
  ///	BIT_STATE - (WRITE) checks for a specified state on a specific bit, active on every write
  ///	BIT_STATE_CHANGE - (WRITE) checks for a specified state change on a specific bit
  /// @author 
  /// @since 
  ////////////////////////////////////////////
  enum gr_reg_rule_type_e
  {
    NOTIFY = 0,
    NOTIFY_ON_CHANGE,
    WRITE_PATTERN_EQUAL,
    READ_PATTERN_EQUAL,
    PATTERN_STATE_CHANGE,
    BIT_STATE,
    BIT_STATE_CHANGE
  };

} // end namespace gs::reg
} // end namespace gs::reg
