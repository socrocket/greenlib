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

#ifndef __DEBUGTEXTFILE_TEST_H__
#define __DEBUGTEXTFILE_TEST_H__

#include "greencontrol/log.h"
#include "TestCase.h"

// GCLOG_RETURN should be enabled
class DebugTextfile_Test : public TestCase
{
public:
  DebugTextfile_Test() {}
  virtual ~DebugTextfile_Test(void) {}

  std::string identify()
  {
    return "DebugTextfile_Test";
  }

  void setup()
  {
    pLog = new GC_Logger();
  }

  void destroy()
  {
    delete pLog;
  }

  // This is for testing the DebugTextfile class
  void run()
  {
    // Create DebugTextfile
    DebugTextfile textOut("DebugTextfile_Test.txt", false);
    ControlTransactionHandle ct, ct2;
    LogConfig cfg;

    ct = getCT();
    ct2 = getCT();
    ct->resetChangeFlag();

    // Test 1: The following outputs should be printed in the DebugTextfile_Test.txt
    textOut.write("This should be the first line in DebugTextfile_Test.txt after the header.");
    textOut.writePure("This should be the second line, but without the \"User output:\" prefix.");

    // Test 2: An unmodified transaction should be printed, with all debug info.
    textOut.writePure("\nTest 2\nAfter this, there should be a single transaction.");
    textOut.processTransaction(ct);

    // Test 3: Two transactions should be printed. One was modified, so it should act as a returning transaction.
    textOut.writePure("\nTest 3\nNow the same transaction should be printed, followed by another, which acts as a returning CT");
    ct2->set_mService(AV_SERVICE);
    ct2->set_mSpecifier("This transaction should be modified.");
    ct2->set_mMetaData("This way it should act as a returning transaction");
    ct2->set_mAnyPointer(this);

    textOut.processTransaction(ct);
    textOut.processTransaction(ct2);

    // Test 4: A LogConfig gets modified and activated. This should be visible in the output.
    textOut.writePure("\nTest 4\nAnother LogConfig was activated. This should be visible in the following output.");
    cfg.reset(false);
    cfg.setAnyPointer(true);
    ct->activateConfig(&cfg);

    textOut.processTransaction(ct);
    ct->deactivateConfig();
  }

protected:
  GC_Logger* pLog;
};

#endif
