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


#include "ServiceUserModule.h"

using namespace std;
using namespace sc_core;

void ServiceUserModule::main_action() {

  cout << endl << endl;
  TEST_DUMP_N(name(), "Test User API Tests");
  std::cout << std::endl;

  for (unsigned int t = 1; t <= 5; t++) {
    std::stringstream ss_send;
    ss_send << "bla_bla_" << t;
    call_User_Api(ss_send.str().c_str());
    ss_send.clear();
    std::cout << std::endl;
  }

  std::cout << std::endl;
}

void ServiceUserModule::call_User_Api(const char* sent) {
  unsigned int ret;
  TEST_DUMP_N(name(), "call API with: "<<sent);
  ret = m_Test_Api.get_random(sent);
  TEST_DUMP_N(name(), "got from API:  "<<ret);
}

