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


#include "simple_b_gs_secure_slave.h"
    
simple_b_gs_secure_slave::simple_b_gs_secure_slave(sc_core::sc_module_name name_)
  : sc_core::sc_module(name_)
  , socket("socket")
{
  //register the callback that happens when binding was done
  socket.register_conf_res_cb(this, &simple_b_gs_secure_slave::binding_done);
  
  //register the callback that happens when binding was done
  socket.register_b_transport(this, &simple_b_gs_secure_slave::b_transport);    
  
  //set up our config
  config_type conf;
  //actually we don't care about phases, since we just wanna do
  // b_transport, but as we wanna be bindable to OSCI we set the OSCI phases
  conf.use_optional_phase(tlm::BEGIN_REQ);
  conf.use_optional_phase(tlm::END_REQ);
  conf.use_optional_phase(tlm::BEGIN_RESP);
  conf.use_optional_phase(tlm::END_RESP);
      
  //this slave can only process transactions from which
  // it can check priority and security
  conf.use_mandatory_extension<priority>();
  conf.use_mandatory_extension<secure>();
  
  //we cannot cope with unknown extensions
  conf.treat_unknown_as_rejected();
  
  //assign the configuration to the socket
  socket.set_config(conf);
  
}

void simple_b_gs_secure_slave::binding_done(const std::string& other_type, gs::socket::bindability_base<traits_type>* other){
}

void simple_b_gs_secure_slave::b_transport(tlm::tlm_generic_payload& gp, sc_core::sc_time& time){
  unsigned char* data=gp.get_data_ptr();
  std::cout<<name()<<" got txn"<<std::endl
           <<"  Command:       "<<(
                             (gp.get_command()==tlm::TLM_WRITE_COMMAND)? "Write" 
                             : (gp.get_command()==tlm::TLM_READ_COMMAND)? "Read"
                             : "Idle"
                            )<<std::endl
           <<"  Adress:        "<<gp.get_address()<<std::endl
           <<"  Data  lengtgh: "<<gp.get_data_length()<<std::endl
           <<"  Data:          ";
  tlm::tlm_endianness endianness=tlm::get_host_endianness();
  for (signed int i=(endianness==tlm::TLM_BIG_ENDIAN)?0:gp.get_data_length()-1; 
        (endianness==tlm::TLM_BIG_ENDIAN)?i<(signed int)gp.get_data_length():i>=0; 
        (endianness==tlm::TLM_BIG_ENDIAN)?i++:i--) 
    std::cout<<std::hex<<((data[i]<0x10)? "0":"")<<(unsigned int)data[i];
  
  std::cout<<std::dec<<std::endl
           <<"  Secure:      "<<(socket.get_extension<secure>(gp)? "Yes":"No")<<std::endl
           <<"     Expected:   Yes"<<std::endl
           <<"  Priority:      ";
  priority* prio;
  if (socket.get_extension(prio,gp)) std::cout<<prio->value<<std::endl;
  else std::cout<<"N/A"<<std::endl;
  std::cout<<"     Expected:  A priority"<<std::endl;

  
  gp.set_response_status(tlm::TLM_OK_RESPONSE);
}
