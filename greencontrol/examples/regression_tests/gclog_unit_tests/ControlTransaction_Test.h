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

#ifndef __CONTROLTRANSACTION_TEST_H__
#define __CONTROLTRANSACTION_TEST_H__

#include "greencontrol/log.h"
#include "TestCase.h"
#include "greencontrol/gcnf/plugin/string_vector.h"
#include "greencontrol/gcnf/plugin/log_vector.h"
#include "Command_if_dummy.h"

// GCLOG_RETURN should be enabled
class ControlTransaction_Test : public TestCase
{
public:
  ControlTransaction_Test() {}
  virtual ~ControlTransaction_Test(void) {}

  std::string identify()
  {
    return "ControlTransaction_Test";
  }

  void setup()
  {
    pLog = new GC_Logger();
    pTextOut = new DebugTextfile("ControlTransaction_Test.txt", false);

    pLog->setFilter(pTextOut);
  }

  void destroy()
  {
    delete pLog;
    delete pTextOut;
  }

  // This is for testing the ControlTransaction, LogConfig, log_if, command_if, string_vector and log_vector classes
  void run()
  {
    ControlTransaction c1, c2;
    ControlTransactionHandle ct, ct2;
    LogConfig cfg, cfg2, cfg3;
    std::stringstream ss;

    // Test 1: The LogConfig class gets tested.
    pTextOut->writePure("Test 1\nThe LogConfig class should be printed three times.");
    pTextOut->writePure("The first is set to true, the second to false and the third one is mixed.\n");
    pTextOut->writePure(cfg.toString());
    cfg.reset(false);
    pTextOut->writePure(cfg.toString());
    cfg.setAnyPointer(true);
    cfg.setCmd(true);
    cfg.setLogPointer(true);
    pTextOut->writePure(cfg.toString());

    // Test 2: The different to*String() methods of the CT class are being tested. The LogConfig from the previous test gets activated.
    pTextOut->writePure("\nTest 2\nThe four different toString() methods of the CT are being tested. (toString, toDetailedString, toConfigString, toDebugString)");
    pTextOut->writePure("The LogConfig from the previous test is activated. The output should change accordingly.\n");
    c1.activateConfig(&cfg);
    pTextOut->writePure(c1.toString());
    pTextOut->writePure(c1.toDetailedString());
    pTextOut->writePure(c1.toConfigString());
    pTextOut->writePure(c1.toDebugString());
    c1.deactivateConfig();

    // Test 3: CT hasChanged flag, copy operator, LogConfig/stack
    pTextOut->writePure("\nTest 3\nThe automatic setting of the hasChanged flag is being tested.\n");
    // The hasChanged flag is being tested
    c1.set_mMetaData("Transaction 1");
    c2.set_mMetaData("Transaction 2");
    c1.resetChangeFlag();
    pTextOut->writePure(c1.toDebugString());
    pTextOut->writePure(c2.toDebugString());
    // The copy operator of the CT gets tested.
    pTextOut->writePure("\nThe copy operator of the CT gets tested.\n");
    c2 = c1;
    pTextOut->writePure(c1.toDebugString());
    pTextOut->writePure(c2.toDebugString());
    // The behaviour of the copy operator gets tested, while a LogConfig is activated. This LogConfig object must not be copied.
    pTextOut->writePure("\nThe copy operator of the CT gets tested again, this time with an activated LogConfig. The LogConfig must not be copied\n");
    c2.set_mMetaData("Transaction 2");
    c1.activateConfig(&cfg);
    c2 = c1;
    pTextOut->writePure(c1.toDebugString());
    pTextOut->writePure(c2.toDebugString());
    c1.deactivateConfig();

    // Test 4: The behaviour of the integrated stack gets tested
    pTextOut->writePure("\nTest 4\nThe behaviour of the integrated stack is being tested.\n");
    ct = getCT();
    ct2 = getCT();
    ct->set_mMetaData("Transaction 1");
    ct2->set_mMetaData("Transaction 2");
    c1.activateConfig(&cfg);
    // try to activate a NULL pointer
    std::cout << "\nA warning about activating a NULL pointer should follow:\n";
    ct->activateConfig(NULL);
    // try to activate a LogConfig, that is already in use
    std::cout << "\nA warning about activating an already active LogConfig should follow:\n";
    ct->activateConfig(&cfg);
    c1.deactivateConfig();
    // the normal behaviour of the stack gets tested
    pTextOut->writePure(ct->toDebugString());
    ct->activateConfig(&cfg);
    ct->activateConfig(&cfg2);
    pTextOut->writePure(ct->toDebugString());
    ct->deactivateConfig();
    pTextOut->writePure(ct->toDebugString());
    ct->deactivateConfig();
    // try to deactivate the default LogConfig of the CT
    std::cout << "\nA warning about deactivating the default LogConfig should follow:\n";
    ct->deactivateConfig();
    pTextOut->writePure(ct->toDebugString());

    // Test 5: The copy operator of the LogConfig class gets tested.
    pTextOut->writePure("\nTest 5\nThe copy operator of the LogConfig class is being tested.\n");
    // Two CTs/stacks are getting prepared
    ct = getCT();
    ct2 = getCT();
    ct->set_mMetaData("Transaction 1");
    ct2->set_mMetaData("Transaction 2");
    ct2->resetChangeFlag();
    cfg.reset(false);
    cfg.setSpecifier(true);
    ct->activateConfig(&cfg);
    cfg2.reset(true);
    cfg3.setSpecifier(false);
    ct2->activateConfig(&cfg3);
    ct2->activateConfig(&cfg2);
    pTextOut->writePure(ct->toDebugString());
    pTextOut->writePure(ct2->toDebugString());
    // Copy the LogConfig, the single parts are being copied. The stack must not change.
    pTextOut->writePure("\nThe LogConfigs are being copied, the stack must not change.\n");
    cfg = cfg2;
    pTextOut->writePure(ct->toDebugString());
    pTextOut->writePure(ct2->toDebugString());
    // The default LogConfig gets used.
    pTextOut->writePure("\nThe default LogConfig of Transaction 1 gets modified.\n");
    ct->deactivateConfig();
    ct->get_mLogConfig()->setSpecifier(false);
    pTextOut->writePure(ct->toDebugString());
    *(ct->get_mLogConfig()) = cfg2;
    pTextOut->writePure(ct->toDebugString());
    ct2->deactivateConfig();
    ct2->deactivateConfig();
    
    // Test 6: The command_if interface gets tested.
    pTextOut->writePure("\nTest 6\nThe command_if interface gets tested.");
    pTextOut->writePure("The first time no command_if object is set, which results in warnings. The second time the command_if is set.\n");
    Command_if_dummy cmdIf;
    ct = getCT();
    // There is no command_if set, so a warning should be printed.
    pTextOut->write(ct->getSenderName());
    pTextOut->write(ct->getCommandName());
    pTextOut->write(ct->getCommandDescription());
    pTextOut->writePure(ct->toDebugString());
    // now a command_if gets set
    ct->set_mCmdIf(&cmdIf);
    pTextOut->write(ct->getSenderName());
    pTextOut->write(ct->getCommandName());
    pTextOut->write(ct->getCommandDescription());
    pTextOut->writePure(ct->toDebugString());

    // Test 7: The log_if interface gets tested. Therefore the string_vector and log_vector classes are being used.
    pTextOut->writePure("\nTest 7\nThe log_if interface gets tested. Therefore the string_vector and log_vector classes are being used.\n");
    gs::cnf::string_vector stringVec;
    std::vector<std::string> vec;
    vec.push_back("Added to vec");
    // the copy operator gets tested
    stringVec = vec;
    pTextOut->write(stringVec.toString());
    stringVec.push_back("Added to stringVec");
    ct->set_mLogPointer(&stringVec);
    pTextOut->writePure(ct->toDebugString());

    // the log_vector is being used
    gs::cnf::log_vector<int> logVec;
    std::vector<int> intVec;
    intVec.push_back(42);
    // test the copy operator
    logVec = intVec;
    pTextOut->write(logVec.toString());
    logVec.push_back(-5);
    logVec.push_back(85);
    ct->set_mLogPointer(&logVec);
    pTextOut->writePure(ct->toDebugString());

    // test/demonstrate the log_vector with different datatypes
    gs::cnf::log_vector<void*> addrVec;
    addrVec.push_back(this);
    addrVec.push_back(&logVec);
    addrVec.push_back(pTextOut);
    pTextOut->write(addrVec.toString());

    gs::cnf::log_vector<std::string> logStrVec;
    logStrVec.push_back("Test String");
    logStrVec.push_back(stringVec[0]);
    logStrVec.push_back(stringVec.toString());
    pTextOut->write(logStrVec.toString());
  }

protected:
  GC_Logger* pLog;
  DebugTextfile* pTextOut;
};

#endif
