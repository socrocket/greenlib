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


#include "simple_b_gs_secure_adapter.h"
    
simple_b_gs_secure_adapter::simple_b_gs_secure_adapter(sc_core::sc_module_name name_)
  : sc_core::sc_module(name_)
  , t_socket("t_socket")
  , i_socket("i_socket")
{
  //register the callback that happens when binding was done
  t_socket.register_conf_res_cb(this, &simple_b_gs_secure_adapter::t_binding_done);
  i_socket.register_conf_res_cb(this, &simple_b_gs_secure_adapter::i_binding_done);
  
  t_socket.register_b_transport(this, &simple_b_gs_secure_adapter::b_transport);    
  
  //we adopt the config of the connected initiator but do not allow the secure extension
  {
  config_type conf;
  conf.treat_unknown_as_optional();
  conf.reject_extension<secure>();
  t_socket.set_config(conf);
  }
  
  {
  //we adopt the config of the connected target but require the secure extension
  config_type conf;
  conf.use_mandatory_extension<secure>();
  conf.treat_unknown_as_optional();
  i_socket.set_config(conf);
  }
}


void simple_b_gs_secure_adapter::t_binding_done(const std::string& other_type, gs::socket::bindability_base<traits_type>* other){
  config_type t_sock_conf=t_socket.get_recent_config();
  t_sock_conf.use_mandatory_extension<secure>(); //add secure to the config of the init
  i_socket.set_config(t_sock_conf); //and assign it to the i_socket
}

void simple_b_gs_secure_adapter::i_binding_done(const std::string& other_type, gs::socket::bindability_base<traits_type>* other){
  config_type i_sock_conf=i_socket.get_recent_config();
  i_sock_conf.remove_extension<secure>(); //remove secure from the config of the target
  t_socket.set_config(i_sock_conf); //and assign it to the t_socket
}

void simple_b_gs_secure_adapter::b_transport(tlm::tlm_generic_payload& gp, sc_core::sc_time& time){
  bool needs_manual_invalidation=!(i_socket.validate_extension<secure>(gp));
  i_socket->b_transport(gp, time);
  if (needs_manual_invalidation) i_socket.invalidate_extension<secure>(gp);
}
