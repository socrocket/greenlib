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

#ifndef __FILTERBASE_TEST_H__
#define __FILTERBASE_TEST_H__

#include "greencontrol/log.h"
#include "TestCase.h"
#include "Filter_dummy.h"

// GCLOG_RETURN should be enabled
class FilterBase_Test : public TestCase
{
public:
  FilterBase_Test() {}
  virtual ~FilterBase_Test(void) {}

  std::string identify()
  {
    return "FilterBase_Test";
  }

  void setup()
  {
    pLog = new GC_Logger();
  }

  void destroy()
  {
    delete pLog;
  }

  // This is for testing the FilterBase and ServiceFilter classes
  void run()
  {
    DebugTextfile textOut("FilterBase_Test.txt", false);
    ControlTransactionHandle ct;

    // These Filter_dummys are being used to test the automatic creation of the filter ports.
    // However, the ports are being created when a filter is added to the dummy. So there are no warnings here.
    Filter_dummy invalidFilter(-1), noPortFilter(0), f(1), f2(100), toHigh(101);

    ct = getCT();

    // Test 1: Trying to create filters with an invalid number of filter ports
    textOut.writePure("Test 1\nTrying to create filters with an invalid number of filter ports.\n");

    // trying to add a NULL pointer and creating a filter with a negative number of filter ports
    std::cout << "\nA warning about creating a filter with too many filter ports should follow.\n";
    std::cout << "That warning should be followed by another warning about adding a NULL pointer:\n";
    invalidFilter.addFilter(0, NULL);

    // trying to create too many filter ports, the activate() callback of invalidFilter should be called
    std::cout << "\nA warning about creating a filter with too many filter ports should follow.\n";
    std::cout << "That warning should be followed by a message about activate() callback of the Filter_dummy.\n";
    toHigh.addFilter(0, &invalidFilter);

    // f doesn't have more than one filter port (index 0)
    std::cout << "\nA warning about a non existing filter port should follow:\n";
    f.addFilter(1, &toHigh);

    // the activate callback must not be called a second time
    std::cout << "\nThere should be no message about the activate() callback until the next test.\n";
    f.addFilter(0, &invalidFilter);

    // this filter has no filter ports
    std::cout << "\nA warning about a non existing filter port should follow:\n";
    noPortFilter.addFilter(0, &invalidFilter);

    // no warning, just add filters
    f2.addFilter(0, &invalidFilter);
    f2.addFilter(2, &textOut);

    // trying to use a non existing filter port
    std::cout << "\nA warning about a non existing filter port should follow:\n";
    noPortFilter.useFilterPort(0, ct);

    // using filter ports, but an output filter is attached to filter port 2 only
    textOut.writePure("Only one transaction should be logged.");
    f2.useFilterPort(0, ct);
    f2.useFilterPort(1, ct);
    f2.useFilterPort(2, ct);

    // Test 2: Testing the ServiceFilter.
    textOut.writePure("\nTest 2\nTesting the ServiceFilter with 2 filter ports.\n");

    ServiceFilter sFilt(2);

    std::cout << "\nA warning about a non existing filter port should follow:\n";
    sFilt.addFilter(2, &invalidFilter);
    sFilt.addFilter(0, &textOut);
    sFilt.addFilter(1, &textOut);

    // the AV_SERVICE CT should not be logged
    textOut.writePure("\nThis should be followed by two transactions, one NO_SERVICE CT and one CONFIG_SERVICE CT.\n");
    sFilt.processTransaction(ct);
    ct->set_mService(AV_SERVICE);
    sFilt.processTransaction(ct);
    ct->set_mService(CONFIG_SERVICE);
    sFilt.processTransaction(ct);
  }

protected:
  GC_Logger* pLog;
};

#endif
