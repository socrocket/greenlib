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


#ifndef __SIMPLE_B_GS_SECURE_SLAVE_H__
#define __SIMPLE_B_GS_SECURE_SLAVE_H__

#include "../common/pseudo_extension_lib.h"
#include "../common/conf_fwd_socket.h"

//This is a slave that requires the priority and secure extensions

SC_MODULE(simple_b_gs_secure_slave)
{
  typedef conf_fwd_target_socket<simple_b_gs_secure_slave> socket_type;
  typedef socket_type::traits_type traits_type;
  typedef gs::socket::config<traits_type> config_type;
 
  socket_type socket;
    
  simple_b_gs_secure_slave(sc_core::sc_module_name name_);
  
private:
  void binding_done(const std::string& other_type, gs::socket::bindability_base<traits_type>* other);
  
  void b_transport(tlm::tlm_generic_payload& gp, sc_core::sc_time& time);
};

#endif
