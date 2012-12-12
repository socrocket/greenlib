//   GreenControl framework
//
// LICENSETEXT
//
//   Copyright (C) 20010 : GreenSocs Ltd
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

/*
 * This example shows how to create a new GreenControl Service
 */

#include <systemc>

// GreenConfig
#include "greencontrol/core.h" 

// Example
#include "test_api.h"
#include "test_plugin.h"
#include "ServiceUserModule.h"

/// Testbench for the GreenControl example showing how to develop a new GreenControl Service (Plugin) and User APIs
int sc_main(int argc, char *argv[]) {
  
  // GreenControl Core instance
  gs::ctr::GC_Core      core;

  // Test Plugin
  gs::test::Test_Plugin testPlugin;
  
  ServiceUserModule  modA ("ServiceUserModuleA");
  ServiceUserModule  modB ("ServiceUserModuleB");
  
  sc_core::sc_start();
  
  return EXIT_SUCCESS; 
}
