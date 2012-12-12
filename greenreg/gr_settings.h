//   GreenReg framework
//
// LICENSETEXT
//
//   Copyright (C) 2009 : GreenSocs Ltd
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
// User and System 
// Settings for GreenReg
//

#ifndef __GR_SETTINGS_H__
#define __GR_SETTINGS_H__


// /////////////////    USER MODEL SETTINGS    ///////////////////// //
// /////////////////    USER MODEL SETTINGS    ///////////////////// //
// This is an example for some settings the user can apply within his/her model
// (This means, do NOT enable the setting here in this file!!)

/*
// Configure warnings about accesses to write protected register bits:
sc_core::sc_report_handler::set_actions("/GreenSocs/GreenReg/write_protected/unequal_current", sc_core::SC_DISPLAY);
sc_core::sc_report_handler::set_actions("/GreenSocs/GreenReg/write_protected/unequal_zero", sc_core::SC_DO_NOTHING);
sc_core::sc_report_handler::set_actions("/GreenSocs/GreenReg/write_protected/bit_range_access", sc_core::SC_DISPLAY);
sc_core::sc_report_handler::set_actions("/GreenSocs/GreenReg/write_protected/bit_access", sc_core::SC_DISPLAY);


*/

// /////////////////    USER SETTINGS    ///////////////////// //
// /////////////////    USER SETTINGS    ///////////////////// //
// User settings that can be modified by users when compiling the GreenReg lib


//#define GR_NOTIF_RULE_DBG // @see I_notification_rule.h


// /////////////////    SYSTEM SETTINGS    ///////////////////// //
// /////////////////    SYSTEM SETTINGS    ///////////////////// //
// Do not modify these settings without knowing exactly what you are doing!
// These settings may influence the GreenReg behavior thus differ from
// the manual.

// if the register-wide events are enabled or not 
// if the gr_events notify their parent events (if not having registered callbacks) or not
#define GR_DEFAULT_EVENT_BEHAVIOR true

#endif // __GR_SETTINGS_H__
