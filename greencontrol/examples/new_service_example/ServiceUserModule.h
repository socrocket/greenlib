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


#ifndef __ServiceUserModule_h__
#define __ServiceUserModule_h__

#include <systemc>
#include <iostream>

#include "test_api.h"


/// Module which uses the new demonstration Test_Api
class ServiceUserModule
: public sc_core::sc_module
{
  
public:

  SC_HAS_PROCESS(ServiceUserModule);
	
  /// Constructor
  ServiceUserModule(sc_core::sc_module_name name)
  : sc_core::sc_module(name)
  , m_Test_Api("Test_Api_ServiceUserModule")
  { 
    SC_THREAD(main_action);
  }
  
  /// Main action to make tests with parameters.
  void main_action();

  /// Call User API
  void call_User_Api(const char*);

  /// Test API
  gs::test::Test_Api m_Test_Api;
};


#endif

