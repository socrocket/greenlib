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

#ifndef __FILTER_DUMMY_H__
#define __FILTER_DUMMY_H__

#include "greencontrol/gclog/filter_base.h"

// This dummy implements the FilterBase methods
class Filter_dummy : public FilterBase
{
public:
  // The number of filter ports has to be specified
  Filter_dummy(unsigned int ports)
  {
    numPorts = ports;
  }

  virtual ~Filter_dummy(void) {}

  // This method is not used here
  void processTransaction(ControlTransactionHandle& th)
  {
  }

  // This method provides access to the protected processFilterPort() method of the base class
  void useFilterPort(unsigned int port, ControlTransactionHandle& th)
  {
    processFilterPort(port, th);
  }

  unsigned int numFilterPorts()
  {
    return numPorts;
  }

protected:

  // This callback gets called when this filter is added to another filter
  void activate()
  {
    std::cout << "\nThe activate() callback of the Filter_dummy is being called.\n";
  }

  unsigned int numPorts;
};

#endif
