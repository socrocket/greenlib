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

#ifndef __GC_LOGGER_TEST_H__
#define __GC_LOGGER_TEST_H__

#include "greencontrol/log.h"
#include "TestCase.h"

// GCLOG_RETURN should be enabled
class GC_Logger_Test : public TestCase
{
public:
  GC_Logger_Test() {}
  virtual ~GC_Logger_Test(void) {}

  std::string identify()
  {
    return "GC_Logger_Test";
  }

  void setup()
  {
    pLog = new GC_Logger();
  }

  void destroy()
  {
    delete pLog;
  }

  // This is for testing the GC_Logger class
  void run()
  {
    DebugTextfile textOut("GC_Logger_Test.txt", false);
    ControlTransactionHandle ct, invCT;

    ct = getCT();
    invCT = getCT();
    invCT->set_mMetaData("This Transaction should not appear in any log!");

    // Test 1: Nothing should happen if there aren't any filters set.
    textOut.writePure("\nTest 1\nThere should be no output until test 2.");
    invCT->set_mSpecifier("Test 1 REQUEST");
    pLog->log(invCT);
    invCT->set_mSpecifier("Test 1 RESPONSE");
    pLog->logReturn(invCT);

    // Test 2: Only the first transaction should get logged, since the second has not changed.
    textOut.writePure("\nTest 2\nOnly the REQUEST should be printed.");
    pLog->setFilter(&textOut);
    ct->set_mSpecifier("Test 2 REQUEST");
    ct->set_mMetaData("This transaction should be the first in the log and not be followed by a RESPONSE");
    pLog->log(ct);
    pLog->logReturn(ct);
    ct->set_mMetaData("");

    // Test 3: Both transactions should get logged.
    textOut.writePure("\nTest 3\nThere should be a REQUEST followed by a RESPONSE.");
    ct->set_mSpecifier("Test 3 REQUEST");
    pLog->log(ct);
    ct->set_mSpecifier("Test 3 RESPONSE");
    pLog->logReturn(ct);

    // Test 4: Nothing should get logged, since the filters are deactivated.
    textOut.writePure("\nTest 4\nThe filters are deactivated now, so nothing should be printed.");
    pLog->setFilter(NULL);
    invCT->set_mSpecifier("Test 4 REQUEST");
    pLog->log(invCT);
    invCT->set_mSpecifier("Test 4 RESPONSE");
    pLog->logReturn(invCT);

    // Test 5: Only the RESPONSE should get logged, because only the returnFilter is set.
    textOut.writePure("\nTest 5\nOnly the RESPONSE should get logged.\n");
    ct->set_mSpecifier("Test 5 REQUEST");
    ct->set_mMetaData("This transaction should not get logged");
    pLog->setReturnFilter(&textOut);
    pLog->log(ct);
    ct->set_mSpecifier("Test 5 RESPONSE");
    ct->set_mMetaData("This RESPONSE should not be preceeded by a REQUEST. It should appear after Test 3.");
    pLog->logReturn(ct);

    // Test 6: Only the REQUEST should get logged, since the returnFilter is not set.
    textOut.writePure("\nTest 6\nOnly the REQUEST should get logged.");
    pLog->setFilter(&textOut);
    pLog->setReturnFilter(NULL);
    ct->set_mSpecifier("Test 6 REQUEST");
    ct->set_mMetaData("This transaction should not be followed by any other");
    pLog->log(ct);
    ct->set_mSpecifier("Test 6 RESPONSE");
    pLog->logReturn(ct);
  }

protected:
  GC_Logger *pLog;
};

#endif
