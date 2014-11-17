//   GreenControl framework
//
// LICENSETEXT
//
//   Copyright (C) 2009 : GreenSocs Ltd
// 	 http://www.greensocs.com/ , email: info@greensocs.com
//
//   Developed by :
//   
//   Michael Ruetz <m.ruetz@tu-bs.de>,
//   Christian Schroeder <schroeder@eis.cs.tu-bs.de>
//     Technical University of Braunschweig, Dept. E.I.S.
//     http://www.eis.cs.tu-bs.de
//
//
// The contents of this file are subject to the licensing terms specified
// in the file LICENSE. Please consult this file for restrictions and
// limitations that may apply.
// 
// ENDLICENSETEXT

// doxygen comments

#ifndef __COMMAND_IF_DUMMY_H__
#define __COMMAND_IF_DUMMY_H__

#include "greencontrol/core/command_if.h"

// This dummy implements the command_if methods for testing.
class Command_if_dummy : public command_if
{
public:
  Command_if_dummy() {}
  virtual ~Command_if_dummy(void) {}

  // //////////////// command_if methods ////////////////////////////////////

  const std::string getName()
  {
    return std::string("Command_if_dummy instance name");
  }

  const std::string getCommandName(unsigned int cmd)
  {
    switch (cmd) {
      case 0:
        return std::string("CMD_0");
      case 1:
        return std::string("CMD_1");
      default:
        return std::string("unknown");
    }
  }

  const std::string getCommandDescription(unsigned int cmd)
  {
    switch (cmd) {
      case 0:
        return std::string("Doesn't do anything.");
      case 1:
        return std::string("Doesn't do anything either.");
      default:
        return std::string("unknown");
    }
  }
};

#endif
