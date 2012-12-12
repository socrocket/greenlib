/*
   This is the original file ExplicitLTTarget.h from TLM2 distribuition
   with only 3 minor modifications to use the GreenSocs Quantum Keeper.

   1) #include <greensocket/generic/gs_quantumkeeper.h>
   2) change the name of myBTransport second argument from t to time_arg
   3) declare a gs_quantumkeeper to substitute the old sc_time t argument
*/


/*****************************************************************************

  The following code is derived, directly or indirectly, from the SystemC
  source code Copyright (c) 1996-2008 by all Contributors.
  All Rights reserved.

  The contents of this file are subject to the restrictions and limitations
  set forth in the SystemC Open Source License Version 3.0 (the "License");
  You may not use this file except in compliance with such restrictions and
  limitations. You may obtain instructions on how to receive a copy of the
  License at http://www.systemc.org/. Software distributed by Contributors
  under the License is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
  ANY KIND, either express or implied. See the License for the specific
  language governing rights and limitations under the License.

 *****************************************************************************/

// DONOTRELICENSE

#ifndef __EXPLICIT_LT_TARGET_H__
#define __EXPLICIT_LT_TARGET_H__

#include "tlm.h"
#include "tlm_utils/simple_target_socket.h"
//#include <systemc>
#include <cassert>
#include <vector>
#include <queue>
//#include <iostream>
#include <greensocket/generic/gs_quantumkeeper.h>

class ExplicitLTTarget : public sc_core::sc_module
{
public:
  typedef tlm::tlm_generic_payload                 transaction_type;
  typedef tlm::tlm_phase                           phase_type;
  typedef tlm::tlm_sync_enum                       sync_enum_type;
  typedef tlm_utils::simple_target_socket<ExplicitLTTarget>     target_socket_type;

public:
  target_socket_type socket;

  gs::cnf::gs_param<std::string> current_command;
  gs::cnf::gs_param<sc_dt::uint64> current_addr;

public:
  SC_HAS_PROCESS(ExplicitLTTarget);
  ExplicitLTTarget(sc_core::sc_module_name name) :
    sc_core::sc_module(name),
    socket("socket"),
    current_command("current_command"),
    current_addr("current_addr")
  {
    // register nb_transport method
    socket.register_b_transport(this, &ExplicitLTTarget::myBTransport);
  }

  void myBTransport(transaction_type& trans, sc_core::sc_time& time_arg)
  {
    gs::gs_quantumkeeper t(time_arg);
    sc_dt::uint64 address = trans.get_address();
    assert(address < 400);

    current_addr = trans.get_address();

    unsigned int& data = *reinterpret_cast<unsigned int*>(trans.get_data_ptr());
    if (trans.get_command() == tlm::TLM_WRITE_COMMAND) {
      current_command = "TLM_WRITE_COMMAND";
      std::cout << name() << ": Received write request: A = 0x"
                << std::hex << (unsigned int)address << ", D = 0x" << data
                << std::dec << " @ " << sc_core::sc_time_stamp()
                << std::endl;

      *reinterpret_cast<unsigned int*>(&mMem[address]) = data;

      // Synchronization on demand (eg need to assert an interrupt)
      // Wait for passed timing annotation + wait for an extra 50 ns
      wait(t + sc_core::sc_time(50, sc_core::SC_NS));
      t = sc_core::SC_ZERO_TIME;

      // We are synchronized, we can read/write sc_signals, wait,...

      *reinterpret_cast<unsigned int*>(trans.get_data_ptr()) =
        *reinterpret_cast<unsigned int*>(&mMem[address]);

    } else {
      current_command = "TLM_READ_COMMAND";
      std::cout << name() << ": Received read request: A = 0x"
                << std::hex << (unsigned int)address
                << std::dec << " @ " << sc_core::sc_time_stamp()
                << std::endl;

      data = *reinterpret_cast<unsigned int*>(&mMem[address]);

      // Finish transaction (use timing annotation)
      t += sc_core::sc_time(100, sc_core::SC_NS);
    }

    trans.set_response_status(tlm::TLM_OK_RESPONSE);
  }

private:
  unsigned char mMem[400];
};

#endif
