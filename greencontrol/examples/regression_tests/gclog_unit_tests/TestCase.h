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

#ifndef __TESTCASE_H__
#define __TESTCASE_H__

// This class is to be implemented by each test.
class TestCase
{
public:
  TestCase(void) {}
  virtual ~TestCase(void) {}

  // provide a transaction for testing
  ControlTransactionHandle getCT()
  {
    ControlTransactionHandle cth(new ControlTransaction());
    cth->set_mID((cport_address_type)this);
    return cth;
  }

  virtual std::string identify() = 0;

  virtual void setup() = 0;
  virtual void destroy() = 0;

  virtual void run() = 0;
};

#endif
