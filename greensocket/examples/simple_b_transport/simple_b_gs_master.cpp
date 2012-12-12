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


#include "simple_b_gs_master.h"

simple_b_gs_master::simple_b_gs_master(sc_core::sc_module_name name)
: sc_core::sc_module(name)
, socket0("socket0")
, socket1("socket1")
, socket2("socket2")
, socket3("socket3")
{
  sockets=new socket_type_ptr[4];
  sockets[0]=&socket0;
  sockets[1]=&socket1;
  sockets[2]=&socket2;
  sockets[3]=&socket3;
  //register the callback that happens when binding was done
  socket0.register_conf_res_cb(this, &simple_b_gs_master::binding_done0);
  socket1.register_conf_res_cb(this, &simple_b_gs_master::binding_done1);
  socket2.register_conf_res_cb(this, &simple_b_gs_master::binding_done2);
  socket3.register_conf_res_cb(this, &simple_b_gs_master::binding_done3);
  
  socket0.register_conf_res_cb(this, &simple_b_gs_master::binding_done_with_id);
  socket1.register_conf_res_cb(this, &simple_b_gs_master::binding_done_with_id);
  socket2.register_conf_res_cb(this, &simple_b_gs_master::binding_done_with_id);
  socket3.register_conf_res_cb(this, &simple_b_gs_master::binding_done_with_id);

  //set up our config
  config_type conf;
  set_up_config(conf);
      
  //assign the configuration to the sockets
  for (unsigned int i=0; i<4; i++)
    sockets[i]->set_config(conf);
  
  priority_allowed=new bool[4];
  cacheable_allowed=new bool[4];
  
  //define our thread
  SC_THREAD(run);
}

void simple_b_gs_master::set_up_config(config_type& conf){
  //actually we don't care about phases, since we just wanna do
  // b_transport, but as we wanna be bindable to OSCI we set the OSCI phases
  conf.use_optional_phase(tlm::BEGIN_REQ);
  conf.use_optional_phase(tlm::END_REQ);
  conf.use_optional_phase(tlm::BEGIN_RESP);
  conf.use_optional_phase(tlm::END_RESP);
      
  //we could use priority and cacheability so we set those as ignorable
  conf.use_optional_extension<priority>();
  conf.use_optional_extension<cacheable>();
  
  //we cannot cope with unknown extensions
  conf.treat_unknown_as_rejected();
}

//when a binding is done, we remember what we may do on this socket
void simple_b_gs_master::binding_done(const std::string& other_type, gs::socket::bindability_base<traits_type>* other, unsigned int number){
  const char* other_name=(!other)? "osci_socket_o" : other->get_name();
  std::cout<<sockets[number]->name()<<" is connected to "<<other_name<<" which is of type "<<other_type<<std::endl;
  config_type tmp=sockets[number]->get_recent_config();
  std::cout<<tmp.to_string()<<std::endl;
  
  switch (tmp.has_extension<priority>()) {
    case gs::ext::gs_mandatory:
      std::cout<<"  The priority extension has to be used."<<std::endl;
      priority_allowed[number]=true;
      break;
    case gs::ext::gs_optional:
      std::cout<<"  The priority extension may be used."<<std::endl;
      priority_allowed[number]=true;
      break;
    default:
      std::cout<<"  The priority extension must not be used."<<std::endl;
      priority_allowed[number]=false;
  }
  switch (tmp.has_extension<cacheable>()) {
    case gs::ext::gs_mandatory:
      std::cout<<"  The cacheable extension has to be used."<<std::endl;
      cacheable_allowed[number]=true;
      break;
    case gs::ext::gs_optional:
      std::cout<<"  The cacheable extension may be used."<<std::endl;
      cacheable_allowed[number]=true;
      break;
    default:
      std::cout<<"  The cacheable extension must not be used."<<std::endl;
      cacheable_allowed[number]=false;
  }
}

//those four calls just 'decode' from which socket the callback was made
void simple_b_gs_master::binding_done0(const std::string& other_type, gs::socket::bindability_base<traits_type>* other){
  binding_done(other_type, other, 0);
}
void simple_b_gs_master::binding_done1(const std::string& other_type, gs::socket::bindability_base<traits_type>* other){
  binding_done(other_type, other, 1);
}
void simple_b_gs_master::binding_done2(const std::string& other_type, gs::socket::bindability_base<traits_type>* other){
  binding_done(other_type, other, 2);
}
void simple_b_gs_master::binding_done3(const std::string& other_type, gs::socket::bindability_base<traits_type>* other){
  binding_done(other_type, other, 3);
}

void simple_b_gs_master::binding_done_with_id(const std::string& other_type, gs::socket::bindability_base<traits_type>* other, gs::socket::bindability_base<traits_type>* caller){
  if (other)
    std::cout<<"bound to callback from "<<caller->get_name()<<": Bound to "<<other->get_name()<<std::endl;
  else
    std::cout<<"bound to callback from "<<caller->get_name()<<": Bound to plain osci"<<std::endl;
}

void simple_b_gs_master::run(){
  //one transaction for each slave
  tlm::tlm_generic_payload* txn=new tlm::tlm_generic_payload[4];
  sc_core::sc_time time;
  unsigned int data=0xbeef0000;
  for (unsigned int transfer=0; transfer<2; transfer++){ //we make 2 transfers to each slave
    for (unsigned int slave=0; slave<4; slave++){ //we send to slave 0 then to slave 1 and finally to slave 2
      if (cacheable_allowed[slave]) 
        //if we can do cacheable we activate it
        sockets[slave]->validate_extension<cacheable>(txn[slave]);
      if (priority_allowed[slave])  {
        //if we can do priority we set a value for it
        priority* prio;
        sockets[slave]->get_extension<priority>(prio, txn[slave]);
        prio->value=slave+transfer+10;
        //and we activate it
        sockets[slave]->validate_extension<priority>(txn[slave]);
      }
      //set some members of the txn
      txn[slave].set_command(tlm::TLM_WRITE_COMMAND);
      txn[slave].set_address(slave);
      txn[slave].set_data_ptr((unsigned char*)&data);
      txn[slave].set_data_length(4);
      txn[slave].set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);
      //and send
      std::cout<<"calling b_trans"<<std::endl;
      (*sockets[slave])->b_transport(txn[slave], time);
      std::cout<<"done calling b_trans"<<std::endl;
      data++; //change data
      if (txn[slave].get_response_status()!=tlm::TLM_OK_RESPONSE)
        std::cout<<name()<<" Error in transfer "<<transfer<<" to slave "<<slave<<std::endl;
      if (cacheable_allowed[slave]) sockets[slave]->invalidate_extension<cacheable>(txn[slave]); //invalidate the extension because there is no mm
      if (priority_allowed[slave])  sockets[slave]->invalidate_extension<priority>(txn[slave]); //invalidate the extension because there is no mm
      sc_core::wait(time); //wait for time
    }
  }
}
