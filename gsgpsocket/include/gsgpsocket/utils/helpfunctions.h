// LICENSETEXT
// 
//   Copyright (C) 2008 : GreenSocs Ltd
//       http://www.greensocs.com/ , email: info@greensocs.com
// 
//   Developed by :
// 
//   Christian Schroeder
//     Technical University of Braunschweig, Dept. E.I.S.
//     http://www.eis.cs.tu-bs.de
// 
// 
// The contents of this file are subject to the licensing terms specified
// in the file LICENSE. Please consult this file for restrictions and
// limitations that may apply.
// 
// ENDLICENSETEXT

#ifndef __HELPFUNCTIONS_H__
#define __HELPFUNCTIONS_H__

#include <tlm.h>
#include <iostream>

namespace gs {

  struct tlm_command_writer {
    static const char* to_string(const tlm::tlm_command& cmd) {
      switch (cmd) {
        case tlm::TLM_READ_COMMAND:
          return "TLM_READ_COMMAND";
        case tlm::TLM_WRITE_COMMAND:
          return "TLM_WRITE_COMMAND";
        case tlm::TLM_IGNORE_COMMAND:
          return "TLM_IGNORE_COMMAND";
        default:
          assert(false);
          exit(1);
      }
    }
  };
  
  struct tlm_phase_writer {
    static const std::string to_string(const tlm::tlm_phase& ph) {
      std::stringstream ss;
      ss << ph;
      return ss.str();
    }
  };
  
}

#endif
