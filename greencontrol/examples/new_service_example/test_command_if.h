//   GreenControl framework
//
// LICENSETEXT
//
//   Copyright (C) 2010 : GreenSocs Ltd
// 	 http://www.greensocs.com/ , email: info@greensocs.com
//
//   Developed by :
//   
//   Christian Schroeder <schroeder@eis.cs.tu-bs.de>,
//     Technical University of Braunschweig, Dept. E.I.S.
//     http://www.eis.cs.tu-bs.de
//
//
// The contents of this file are subject to the licensing terms specified
// in the file LICENSE. Please consult this file for restrictions and
// limitations that may apply.
// 
// ENDLICENSETEXT


#ifndef __test_command_if_h__
#define __test_command_if_h__

#include "greencontrol/core/command_if.h"

namespace gs {
namespace test {
    
/// command_if implementation for the Phugin and the APIs
/**
 * This is the GreenControl Test_Plugin.
 * The plugin provides the service TODO: unnamed (enumeration entry OTHER_SERVICE).
 */
class test_command_if
  : public gs::ctr::command_if
{
public:
  
  /// Returns the name of the plugin.
  virtual const std::string getName() = 0;

  /// Returns the name of the specified command.
  const std::string getCommandName(unsigned int cmd)
  {
    // Make sure the API(s) either contain the same data or access this one!
    switch (cmd) {
      case CMD_TEST_NONE:
        return std::string("CMD_TEST_NONE");
      case CMD_TEST_1:
        return std::string("CMD_TEST_1");
      case CMD_TEST_2:
        return std::string("CMD_TEST_2");
      default:
        return std::string("unknown");
    }
  }

  /// Return a description of the specified command.
  const std::string getCommandDescription(unsigned int cmd)
  {
    // Make sure the API(s) either contain the same data or access this one!
    switch (cmd) {
      case CMD_TEST_NONE:
        return std::string("No command.");
      case CMD_TEST_1:
        return std::string("Test command number 1");
      case CMD_TEST_2:
        return std::string("Test command number 2");
      default:
        return std::string("unknown");
    }
  }

};

} // end namespace
} // end namespace

#endif
