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

#ifndef __I_CALLBACK_EVENT_SWITCH_H__
#define __I_CALLBACK_EVENT_SWITCH_H__

#include <sstream>
#include <boost/config.hpp>
#include <systemc>

#include "greenreg/sysc_ext/utils/gr_report.h"

namespace gs {
namespace reg {
	
  /// Interface class for elements that provide a callback/event switch for their notification rule (containers)
  /**
   * By default the switch state is not to use callbacks
   * since this is the default for the gr_events as well.
   */
  class I_callback_event_switch
  {
  protected:
    /// If switch is switched to use callbacks
    bool m_use_callbacks;
    
  public:
    I_callback_event_switch()
    : m_use_callbacks(false)
    {  }
    
    virtual ~I_callback_event_switch() 
    { } 
    
    /// Set the switch to use callbacks, not events
    virtual void switch_to_callback_mode() {
      m_use_callbacks = true;
    }
    
    /// Set the switch to use events, not callbacks
    virtual void switch_to_event_mode() {
      m_use_callbacks = false;
    }
    
    /// Returns if the switch is switched to use callbacks
    bool get_callbacks_enabled() {
      return m_use_callbacks;
    }
    
  };
  
} // end namespace gs::reg
} // end namespace gs::reg

#endif // __I_CALLBACK_EVENT_SWITCH_H__
