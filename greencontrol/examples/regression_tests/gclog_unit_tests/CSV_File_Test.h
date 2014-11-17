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

#ifndef __CSV_FILE_TEST_H__
#define __CSV_FILE_TEST_H__

#include "greencontrol/log.h"
#include "TestCase.h"

// GCLOG_RETURN should be enabled
class CSV_File_Test : public TestCase
{
public:
  CSV_File_Test() {}
  virtual ~CSV_File_Test(void) {}

  std::string identify()
  {
    return "CSV_File_Test";
  }

  void setup()
  {
    pLog = new GC_Logger();
    pCSVOut = new CSV_File("CSV_File_Test.csv", false);
  }

  void destroy()
  {
    delete pLog;
    delete pCSVOut;
  }

  // This is for testing the CSV_File and OutputBase classes.
  void run()
  {
    ControlTransactionHandle ct;
    LogConfig cfg, cfg2;

    // test the generateUniqueFilename() method in OutputBase
    CSV_File noName;
    std::cout << "\nA warning about an invalid filename should follow:\n";
    CSV_File nullName(NULL, true);
    CSV_File uniqueNoEnding("csvNoEnding", true);
    CSV_File uniqueEnding("csvEnding.csv", true);

    ct = getCT();
    cfg.setAnyPointer2(false);
    cfg.setLogPointer(false);

    // use uniqueEnding file for some settings
    uniqueEnding.setDelimiter(",");
    uniqueEnding.enableTiming(false);
    uniqueEnding.setHumanReadable(false);
    // use a static LogConfig -> writeStaticTransaction() is used
    uniqueEnding.setLogConfig(&cfg);
    uniqueEnding.processTransaction(ct);
    // use normal delimiter again
    uniqueEnding.setDelimiter(";");
    ct->set_mMetaData("The header is written after the filter was added.");
    uniqueEnding.wasAdded();
    uniqueEnding.processTransaction(ct);
    ct->set_mMetaData("But it must not be written more than once.");
    uniqueEnding.wasAdded();
    uniqueEnding.processTransaction(ct);

    // use different settings
    ct = getCT();
    cfg2.setAnyPointer(false);
    cfg2.setLogPointer(false);
    cfg2.setCmdIf(false);
    pCSVOut->enableTiming(true);
    pCSVOut->setHumanReadable(true);
    pCSVOut->wasAdded();
    pCSVOut->processTransaction(ct);
    // use a LogConfig in the CT -> writeDynamicTransaction() is used
    ct->set_mMetaData("A LogConfig object is used in the CT.");
    ct->activateConfig(&cfg2);
    pCSVOut->processTransaction(ct);
    // use an unmodified LogConfig that was set to true -> writeFullTransaction() is used
    ct->set_mMetaData("The LogConfig is reset to true.");
    cfg2.reset(true);
    pCSVOut->processTransaction(ct);
    ct->deactivateConfig();
  }

protected:
  GC_Logger* pLog;
  CSV_File* pCSVOut;
};

#endif
