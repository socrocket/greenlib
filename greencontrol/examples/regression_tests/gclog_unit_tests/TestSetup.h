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

#ifndef __TESTSETUP_H__
#define __TESTSETUP_H__

#define GCLOG_RETURN

#include "greencontrol/config.h"
#include "greencontrol/log.h"

using namespace gs::ctr;
using namespace gs::log;

class TestCase;

// This class manages the single testcases.
class TestSetup
{
public:
  TestSetup(void);
  virtual ~TestSetup(void);

  void run(unsigned int test);
  void runAll();

protected:
  std::vector<TestCase*> testVec;
};

#endif
