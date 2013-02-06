#ifndef EXAMPLE_H_
#define EXAMPLE_H_

#include "tlm.h"
#include "greensignalsocket/green_signal.h"

using namespace gs_generic_signal;
GS_GUARD_ONLY_EXTENSION(foo);
GS_GUARD_ONLY_EXTENSION(bar);
GS_GUARD_ONLY_EXTENSION(baa);

SC_MODULE(Master)
{

  gs_generic_signal::initiator_signal_socket  socket;

  SC_CTOR(Master): socket("master_socket")//,multi("multi_socket")
  {
    SC_THREAD(run);
    socket.register_nb_transport_bw(this, &Master::nb_trans);
   
    gs::socket::config<gs_generic_signal_protocol_types> tmp;
    tmp.use_mandatory_extension<bar>();
    socket.set_config(tmp);
  }
  
  void b_trans(unsigned int id, tlm::tlm_generic_payload &trans, sc_core::sc_time& delay)
  {
	  
  }

  tlm::tlm_sync_enum nb_trans(gs_generic_signal_payload& trans, signal_phase& ph, sc_core::sc_time& delay) {
    std::cout<<"BAR"<<std::endl;
    return tlm::TLM_COMPLETED;
  }

  void run() {
    wait(10,sc_core::SC_NS);
    gs_generic_signal_payload* p=socket.get_transaction();
    signal_phase  ph;
    sc_core::sc_time t=sc_core::SC_ZERO_TIME;
    unsigned char data=0x0f;
    bool flag =true;
    p->set_data_ptr(&data);
    p->set_ack_requirement(flag);
    socket.validate_extension<bar>(*p);
    socket->nb_transport_fw(*p,ph,t);
    socket.release_transaction(p);
    
  }
};

SC_MODULE(Slave)
{
  
  target_signal_socket<Slave> socket;

  SC_CTOR(Slave): socket("socket")//, multi("target_multi")
  {
    socket.register_nb_transport_fw(this, &Slave::trans);

    gs::socket::config<gs_generic_signal_protocol_types> tmp;
    tmp.use_mandatory_extension<bar>();
    socket.set_config(tmp);
    
  }
  
  tlm::tlm_sync_enum trans(gs_generic_signal_payload& trans, signal_phase& ph, sc_core::sc_time& delay)
  {
    if (socket.get_extension<bar>(trans))
      {
    	if(trans.is_ack_required()==true)
    		ph=gs_generic_signal::ACK;
    	std::cout<<"bar enabled"<<std::endl;
      }
    else
      std::cout<<"bar disabled"<<std::endl;
    
    //check the last signal value here
    unsigned char last_val=0;
    last_val=socket.get_last_value<foo>();
    std::cout <<"last value of foo signal "<< (unsigned int)last_val << endl; 
    last_val=socket.get_last_value<bar>();
    std::cout <<"last value of bar signal "<< (unsigned int)last_val << endl; 
    return tlm::TLM_COMPLETED;
  }
};


#endif /*EXAMPLE_H_*/
