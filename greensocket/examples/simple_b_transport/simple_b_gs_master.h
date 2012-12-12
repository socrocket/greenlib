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


#ifndef __SIMPLE_B_GS_MASTER_H__
#define __SIMPLE_B_GS_MASTER_H__

#include "../common/pseudo_extension_lib.h"
#include "../common/conf_fwd_socket.h"

//This master likes to use priority and cacheable extensions
// but if the connected slaves do not allow that it will
//  adopt to what is possible

SC_MODULE(simple_b_gs_master)
{
  typedef conf_fwd_init_socket<simple_b_gs_master> socket_type;
  typedef socket_type* socket_type_ptr;
  typedef socket_type::traits_type traits_type;
  typedef gs::socket::config<traits_type> config_type;

  socket_type socket0;
  socket_type socket1;
  socket_type socket2;
  socket_type socket3;
  
  SC_HAS_PROCESS(simple_b_gs_master);
  simple_b_gs_master(sc_core::sc_module_name name);
    
private:
  void set_up_config(config_type& conf);
    
  //when a binding is done, we remember what we may do on this socket
  void binding_done(const std::string& other_type, gs::socket::bindability_base<traits_type>* other, unsigned int number);
    
  //those four calls just 'decode' from which socket the callback was made
  void binding_done0(const std::string& other_type, gs::socket::bindability_base<traits_type>* other);
  void binding_done1(const std::string& other_type, gs::socket::bindability_base<traits_type>* other);
  void binding_done2(const std::string& other_type, gs::socket::bindability_base<traits_type>* other);
  void binding_done3(const std::string& other_type, gs::socket::bindability_base<traits_type>* other);

  //just show use of other callback
  void binding_done_with_id(const std::string& other_type, gs::socket::bindability_base<traits_type>* other, gs::socket::bindability_base<traits_type>* caller);

  //the thread
  void run();  

  bool *priority_allowed, *cacheable_allowed;
  socket_type_ptr* sockets;
};

#endif
