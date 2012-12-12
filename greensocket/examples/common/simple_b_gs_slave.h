// LICENSETEXT
//
//   Copyright (C) 2005 - 2008 : GreenSocs Ltd
//       http://www.greensocs.com/ , email: info.com
//
//   Developed by :
//
//  Robert Guenzel, Christian Schroeder
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


#ifndef __SIMPLE_B_GS_SLAVE_H__
#define __SIMPLE_B_GS_SLAVE_H__

#include "../common/pseudo_extension_lib.h"
#include "../common/conf_fwd_socket.h"

//this is a simple slave that can be configured (through the ctor)
// if it can use cacheable and/or priority

SC_MODULE(simple_b_gs_slave)
{
  typedef conf_fwd_target_multi_socket<simple_b_gs_slave> socket_type;
  typedef socket_type::traits_type traits_type;
  typedef gs::socket::config<traits_type> config_type;

  socket_type socket;
  
  simple_b_gs_slave(sc_core::sc_module_name name_, bool use_cachable_, bool use_prio_);
  
private:
  void binding_done(const std::string& other_type, gs::socket::bindability_base<traits_type>* other, unsigned int);
  
  //just dump the transaction info
  void b_transport(unsigned int, tlm::tlm_generic_payload& gp, sc_core::sc_time& time);
  
  bool use_cacheable, use_prio;
  
};

#endif
