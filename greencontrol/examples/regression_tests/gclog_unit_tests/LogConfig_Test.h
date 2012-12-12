// LICENSETEXT
//
//   Copyright (C) 2007 : GreenSocs Ltd
//       http://www.greensocs.com/ , email: info@greensocs.com
//
//   Developed by :
//   
//   MichaelRuetz <m.ruetz@tu-bs.de>,
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

#pragma once

#include "greencontrol/log.h"
#include "TestCase.h"

// GCLOG_RETURN should be enabled
class LogConfig_Test : public TestCase
{
public:
  LogConfig_Test(TestSetup* pTS)
  {
    this->pTS = pTS;
  }

  virtual ~LogConfig_Test(void) {}

  std::string identify()
  {
    return "LogConfig_Test";
  }

  void setup()
  {
  }

  void destroy()
  {
  }

  void run()
  {

  }

protected:
};
