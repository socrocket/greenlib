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


#ifndef __SIMPLE_B_GS_SECURE_ADAPTER_H__
#define __SIMPLE_B_GS_SECURE_ADAPTER_H__

#include "../common/pseudo_extension_lib.h"
#include "../common/conf_fwd_socket.h"

//This adapter will forward the master config to
// the slave and add the secure extension

//It also forwards the slave config to the master
// and removes the secure extension

//The adapter is (apart from the secure extension)
//fully configuration agnostic

//so basically the master and slave handle their bindability
// on their own, the adapter only helps them resolve the
// conflict with the secure extension


SC_MODULE(simple_b_gs_secure_adapter)
{
  typedef conf_fwd_target_socket<simple_b_gs_secure_adapter> t_socket_type;
  typedef conf_fwd_init_socket<simple_b_gs_secure_adapter>   i_socket_type;
  typedef t_socket_type::traits_type traits_type;
  typedef gs::socket::config<traits_type> config_type;
 
  t_socket_type t_socket;
  i_socket_type i_socket;
    
  simple_b_gs_secure_adapter(sc_core::sc_module_name name_);
    
private:
  void t_binding_done(const std::string& other_type, gs::socket::bindability_base<traits_type>* other);

  void i_binding_done(const std::string& other_type, gs::socket::bindability_base<traits_type>* other);
  
  void b_transport(tlm::tlm_generic_payload& gp, sc_core::sc_time& time);
};

#endif
