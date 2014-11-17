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

#include "TestSetup.h"

int main(int argc, char *argv[])
{
  TestSetup ts;
  int testNum;

  // run specified test
  if(argc > 1)
  {
    testNum = atoi(argv[1]);
    std::cout << "Running TestCase " << testNum << std::endl;
    ts.run(testNum);
  }
  else // run all tests
  {
    std::cout << "Running all tests" << std::endl;
    ts.runAll();
  }

  std::cout << "All tests done" << std::endl;
}