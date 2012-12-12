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


#include "simple_b_gs_slave.h"
  
simple_b_gs_slave::simple_b_gs_slave(sc_core::sc_module_name name_, bool use_cachable_, bool use_prio_)
  : sc_core::sc_module(name_)
  , socket("socket")
  , use_cacheable(use_cachable_)
  , use_prio(use_prio_)
{
  //register the callback that happens when binding was done
  socket.register_conf_res_cb(this, &simple_b_gs_slave::binding_done);
  
  //register the callback for b_transport
  socket.register_b_transport(this, &simple_b_gs_slave::b_transport);    
  
  //set up our config
  config_type conf;
  //actually we don't care about phases, since we just wanna do
  // b_transport, but as we wanna be bindable to OSCI we set the OSCI phases
  conf.use_optional_phase(tlm::BEGIN_REQ);
  conf.use_optional_phase(tlm::END_REQ);
  conf.use_optional_phase(tlm::BEGIN_RESP);
  conf.use_optional_phase(tlm::END_RESP);
      
  //depending on our ctor args, we use cacheable or priority
  //
  //Note that the else branch could have been left out
  // because we treat unknowns as rejected
  // but since our socket only allows accesses to extensions we know
  // we tell him we know the extension but reject it
  if (use_cacheable) conf.use_mandatory_extension<cacheable>();
  else conf.reject_extension<cacheable>();
  if (use_prio)      conf.use_mandatory_extension<priority>();
  else  conf.reject_extension<priority>();
  
  //we cannot cope with unknown extensions
  conf.treat_unknown_as_rejected();
  
  //assign the configuration to the socket
  socket.set_config(conf);
  
  unsigned int my_id=rand()%1000;
  socket.set_socket_id(my_id);
  std::cout<<name()<<" using tag "<<my_id<<" for my socket"<<std::endl;
  
}

void simple_b_gs_slave::binding_done(const std::string& other_type, gs::socket::bindability_base<traits_type>* other, unsigned int){
}

//just dump the transaction info
void simple_b_gs_slave::b_transport(unsigned int id, tlm::tlm_generic_payload& gp, sc_core::sc_time& time){
  unsigned char* data=gp.get_data_ptr();
  std::cout<<name()<<" got txn (from socket with tag "<<id<<")"<<std::endl
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
           <<"  Cachable:      "<<(socket.get_extension<cacheable>(gp)? "Yes":"No")<<std::endl
           <<"     Expected:   "<<(use_cacheable? "Yes":"No")<<std::endl
           <<"  Priority:      ";
  priority* prio;
  if (socket.get_extension(prio,gp)) std::cout<<prio->value<<std::endl;
  else std::cout<<"N/A"<<std::endl;
  std::cout<<"     Expected:   "<<(use_prio? "A priority" : "N/A")<<std::endl;
  time+=sc_core::sc_time(10, sc_core::SC_NS);
  gp.set_response_status(tlm::TLM_OK_RESPONSE);
}
