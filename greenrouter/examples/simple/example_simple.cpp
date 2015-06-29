// LICENSETEXT
//
//   Copyright (C) 2007 : GreenSocs Ltd
//       http://www.greensocs.com/ , email: info@greensocs.com
//
//   Developed by :
//
//   Wolfgang Klingauf, Robert Guenzel
//     Technical University of Braunschweig, Dept. E.I.S.
//     http://www.eis.cs.tu-bs.de
//
//   Mark Burton, Marcus Bartholomeu
//     GreenSocs Ltd
//
//
// The contents of this file are subject to the licensing terms specified
// in the file LICENSE. Please consult this file for restrictions and
// limitations that may apply.
//
// ENDLICENSETEXT


// uncomment the following line if you want 1000 runs...
//#define SPEEDTEST

#define MEMSIZE 10000

#ifdef USE_GPSOCKET
  // GreenControl + GreenConfig
# include "greencontrol/config.h"
#endif

#include "greenrouter/genericRouter.h"
#include "greenrouter/protocol/SimpleBus/simpleBusProtocol.h"
#include "greenrouter/scheduler/fixedPriorityScheduler.h"

#include <boost/config.hpp> // needed for SystemC 2.1
#include <systemc>

#include "sillysort.h"
#include "simplememory.h"

/// main
int sc_main(int, char**)
{
//  sc_report_handler::set_actions(SC_ERROR, SC_ABORT);

#ifdef USE_GPSOCKET
  /// GreenControl Core instance
  gs::ctr::GC_Core core;
  // GreenConfig Plugin
  gs::cnf::ConfigDatabase cnfdatabase("ConfigDatabase");
  gs::cnf::ConfigPlugin configPlugin(&cnfdatabase);
#endif

  gs::gp::SimpleBusProtocol<32> p("Protocol", 10);
  gs::gp::fixedPriorityScheduler s("Scheduler");
  gs::gp::GenericRouter<32> r("Router");

  r.protocol_port(p);
  p.router_port(r);
  p.scheduler_port(s);

  sillysort m("sillysort");

  simplememory s1("simplememory1");
  s1.target_port.base_addr = 0x0;
  s1.target_port.high_addr = 0x10;

  simplememory s2("simplememory2");
  s2.target_port.base_addr = 0x11;
  s2.target_port.high_addr = 0xFFFF;

  m.init_port(r.target_socket);
  r.init_socket(s1.target_port);
  r.init_socket(s2.target_port);

 // m.init_port(s.target_port);

  sc_core::sc_start();

  return 0;
}
