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


#ifndef __B_MASTER_QK_H__
#define __B_MASTER_QK_H__

#include "../common/pseudo_extension_lib.h"
#include "../common/conf_fwd_socket.h"
#include <greencontrol/config.h>


SC_MODULE(b_master_qk)
{
  typedef gs::socket::initiator_socket<> socket_type;
  typedef socket_type::traits_type traits_type;
  typedef gs::socket::config<traits_type> config_type;

  socket_type socket;
  
  SC_HAS_PROCESS(b_master_qk);
  b_master_qk(sc_core::sc_module_name name);
    
  gs::cnf::gs_param<bool> done_transaction;

private:
  void set_up_config(config_type& conf);

  //the thread
  void run();  
};

#endif
