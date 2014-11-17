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

#include "DebugTextfile_Test.h"
#include "GC_Logger_Test.h"
#include "ControlTransaction_Test.h"
#include "FilterBase_Test.h"
#include "CSV_File_Test.h"

// Create tests in constructor
TestSetup::TestSetup(void)
{
  testVec.push_back(new DebugTextfile_Test());
  testVec.push_back(new GC_Logger_Test());
  testVec.push_back(new ControlTransaction_Test());
  testVec.push_back(new FilterBase_Test());
  testVec.push_back(new CSV_File_Test());
}

TestSetup::~TestSetup(void)
{
  for(unsigned int i=0;i<testVec.size();i++)
    delete testVec[i];
}

void TestSetup::run(unsigned int test)
{
  if(test < testVec.size())
  {
    std::cout << "Running test " << test << " (" << testVec[test]->identify() << ")" << std::endl;
    testVec[test]->setup();
    testVec[test]->run();
    testVec[test]->destroy();
    std::cout << "Done" << std::endl;
  }
  else
    std::cout << "WARNING: Could not run TestCase " << test << std::endl;
}

void TestSetup::runAll()
{
  for(unsigned int i=0;i<testVec.size();i++)
    run(i);
}