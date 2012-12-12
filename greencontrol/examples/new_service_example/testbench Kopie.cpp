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

#define ENABLE_TEST_LOG
#define GCLOG_RETURN

#include <systemc>

// GreenConfig
#include "greencontrol/core.h" 

// Example
#include "test_api.h"
#include "test_plugin.h"
#include "ServiceUserModule.h"

#ifdef ENABLE_TEST_LOG
  // Logger
  #include "greencontrol/log.h"
#endif

/// Testbench for the GreenControl analysis (GreenAV) example
int sc_main(int argc, char *argv[]) {
  
  // GreenControl Core instance
  gs::ctr::GC_Core      core;

  // Test Plugin
  gs::test::Test_Plugin testPlugin;
  
  ServiceUserModule  modA ("ServiceUserModuleA");
  ServiceUserModule  modB ("ServiceUserModuleB");
  
#ifdef ENABLE_TEST_LOG
  gs::log::GC_Logger* gclog;
  gs::log::Console* consoleOut;
  gclog = new gs::log::GC_Logger();
  consoleOut = new gs::log::Console();
  gclog->setFilter(consoleOut);
#endif
  
  sc_core::sc_start();
  
  return EXIT_SUCCESS; 
}
