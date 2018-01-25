#ifndef EXAMPLE_H_
#define EXAMPLE_H_

#include "tlm.h"
#include <stdint.h>
#include "greensignalsocket/green_signal.h"

using namespace gs_generic_signal;
GS_GUARD_ONLY_EXTENSION(foo);
GS_GUARD_ONLY_EXTENSION(bar);
GS_GUARD_ONLY_EXTENSION(baa);

SC_MODULE(Master)
{

  gs_generic_signal::initiator_signal_socket socket;
  gs_generic_signal::initiator_signal_socket socket2;

  SC_CTOR(Master): socket("master_socket"), socket2("master_socket2")
  {
    SC_THREAD(run);
   
    gs::socket::config<gs_generic_signal_protocol_types> tmp;
    tmp.use_mandatory_extension<bar>();
    socket.set_config(tmp);
    socket2.set_config(tmp);
  }

  void run() {
    wait(10,sc_core::SC_NS);
    switch (test_id)
    {
      case 0:
      case 1:
      {
        gs_generic_signal_payload* p = socket.get_transaction();
        signal_phase  ph;
        sc_core::sc_time t = sc_core::SC_ZERO_TIME;
        unsigned char data = 0x0f;
        bool flag = true;
        p->set_data_ptr(&data);
        p->set_ack_requirement(flag);
        socket.validate_extension<bar>(*p);
        if (test_id == 1)
          socket->nb_transport_fw(*p, ph, t);
        else
          socket->b_transport(*p, t);
        socket.release_transaction(p);
      }
      break;
      case 2:
      case 3:
      {
        gs_generic_signal_payload* p = socket2.get_transaction();
        signal_phase  ph;
        sc_core::sc_time t = sc_core::SC_ZERO_TIME;
        unsigned char data = 0x0f;
        bool flag = true;
        p->set_data_ptr(&data);
        p->set_ack_requirement(flag);
        socket2.validate_extension<bar>(*p);
        if (test_id == 2)
          socket2->nb_transport_fw(*p, ph, t);
        else
          socket2->b_transport(*p, t);
        socket2.release_transaction(p);
      break;
      }
    }
  }

  uint8_t test_id;
};

SC_MODULE(Slave)
{
  
  target_signal_socket<Slave> socket;

  SC_CTOR(Slave): socket("socket")
  {
    gs::socket::config<gs_generic_signal_protocol_types> tmp;
    tmp.use_mandatory_extension<bar>();
    socket.set_config(tmp);

  }

  uint8_t test_id;
};

SC_MODULE(SlaveMulti)
{

  target_signal_multi_socket<SlaveMulti> socket;

  SC_CTOR(SlaveMulti): socket("socket")
  {
    gs::socket::config<gs_generic_signal_protocol_types> tmp;
    tmp.use_mandatory_extension<bar>();
    socket.set_config(tmp);
    
  }

  uint8_t test_id;
};


#endif /*EXAMPLE_H_*/
