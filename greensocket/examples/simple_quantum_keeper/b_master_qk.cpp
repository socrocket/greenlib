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


#include "b_master_qk.h"
#include <greensocket/generic/gs_quantumkeeper.h>


b_master_qk::b_master_qk(sc_core::sc_module_name name)
: sc_core::sc_module(name)
, socket("socket")
, done_transaction("done_transaction")
{
  //set up our config
  config_type conf;
  set_up_config(conf);
      
  //assign the configuration to the sockets
  socket.set_config(conf);
  
  //define our thread
  SC_THREAD(run);
}

void b_master_qk::set_up_config(config_type& conf){
  //actually we don't care about phases, since we just wanna do
  // b_transport, but as we wanna be bindable to OSCI we set the OSCI phases
  conf.use_optional_phase(tlm::BEGIN_REQ);
  conf.use_optional_phase(tlm::END_REQ);
  conf.use_optional_phase(tlm::BEGIN_RESP);
  conf.use_optional_phase(tlm::END_RESP);
      
  //we cannot cope with unknown extensions
  conf.treat_unknown_as_rejected();
}

void b_master_qk::run()
{
  // read and write tansaction
  tlm::tlm_generic_payload read_txn;
  tlm::tlm_generic_payload write_txn;

  // Save (keep) the local TD time
  gs::gs_quantumkeeper time;

  unsigned int data=0xbeef0000;

  for (unsigned int transfer_count=1; transfer_count <= 2; transfer_count++){ //we make 2 transfers

    data++; //change data

    // SEND WRITE COMMAND
      
    //set some members of the txn
    write_txn.set_command(tlm::TLM_WRITE_COMMAND);
    write_txn.set_address(0x10 * transfer_count);
    write_txn.set_data_ptr((unsigned char*)&data);
    write_txn.set_data_length(4);
    write_txn.set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);
 
    // Add some time decouple to test GAV output
    time += transfer_count * sc_core::sc_time(10,sc_core::SC_NS);

    //and send
    std::cout<<"calling b_trans"<<std::endl;
    socket->b_transport(write_txn, time);
    std::cout << "done calling b_trans"
              << " @ " << sc_core::sc_time_stamp()
              << " (td= " << time << " )"
              << std::endl;
    if (write_txn.get_response_status()!=tlm::TLM_OK_RESPONSE)
      std::cout<<name()<<" Error in transfer "<<transfer_count<<std::endl;

    //synchronize
    std::cout << "waiting for " << time << std::endl;
    sc_core::wait(time); //wait for time
    time = sc_core::SC_ZERO_TIME;

    //flag a param the end of transaction
    done_transaction = true;

    // SEND READ COMMAND
      
    //set some members of the txn
    read_txn.set_command(tlm::TLM_READ_COMMAND);
    read_txn.set_address(0x10 * transfer_count);
    read_txn.set_data_ptr((unsigned char*)&data);
    read_txn.set_data_length(4);
    read_txn.set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);

    // Add some time decouple to test GAV output
    time += transfer_count * sc_core::sc_time(15,sc_core::SC_NS);

    //and send
    std::cout<<"calling b_trans"<<std::endl;
    socket->b_transport(read_txn, time);
    std::cout << "done calling b_trans"
              << " @ " << sc_core::sc_time_stamp()
              << " (td= " << time << " )"
              << std::endl;
    if (read_txn.get_response_status()!=tlm::TLM_OK_RESPONSE)
      std::cout<<name()<<" Error in transfer "<<transfer_count<<std::endl;

    //synchronize
    std::cout << "waiting for " << time << std::endl;
    sc_core::wait(time); //wait for time
    time = sc_core::SC_ZERO_TIME;

    //flag a param the end of transaction
    done_transaction = true;
  }
}
