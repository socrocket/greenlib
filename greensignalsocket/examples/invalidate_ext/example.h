#ifndef EXAMPLE_H_
#define EXAMPLE_H_

#include "tlm.h"
#include "greensignalsocket/green_signal.h"

//Following example shows that, you may use SAME PAYLOAD
//again and again to transmit the values of different signals,
//but in that case you should INVALIDATE the extension corresponding
//to the signal after it is transmitted.

using namespace gs_generic_signal;
GS_GUARD_ONLY_EXTENSION(PICS_TXD0);
GS_GUARD_ONLY_EXTENSION(PIC_IIC);
GS_GUARD_ONLY_EXTENSION(PIC_TIMER4);
SC_MODULE(Master) {

  gs_generic_signal::initiator_signal_socket  socket;

  SC_CTOR(Master): socket("master_socket") {
    SC_THREAD(run);
    socket.register_nb_transport_bw(this, &Master::nb_trans);
   
    gs::socket::config<gs_generic_signal_protocol_types> tmp;
    tmp.use_mandatory_extension<PIC_TIMER4>();
    tmp.use_mandatory_extension<PIC_IIC>();
    tmp.use_mandatory_extension<PICS_TXD0>();

    
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

    signal_phase  ph;
    sc_core::sc_time t=sc_core::SC_ZERO_TIME;
    unsigned char data;
    bool flag =true;
    //Get the transaction from socket(once only)
    gs_generic_signal_payload* p=socket.get_transaction();
    p->set_data_ptr(&data);
    p->set_ack_requirement(flag);

    data=0x11;
    socket.validate_extension<PICS_TXD0>(*p);
    socket->nb_transport_fw(*p,ph,t);
    socket.invalidate_extension<PICS_TXD0>(*p);
   
    wait(10,sc_core::SC_NS);

    data=0x22;
    socket.validate_extension<PICS_TXD0>(*p);
    socket->nb_transport_fw(*p,ph,t);
    socket.invalidate_extension<PICS_TXD0>(*p);

    wait(10,sc_core::SC_NS);

    data=0x33;
    socket.validate_extension<PIC_TIMER4>(*p);
    socket->nb_transport_fw(*p,ph,t);
    socket.invalidate_extension<PIC_TIMER4>(*p);

    wait(10,sc_core::SC_NS);

    data=0x44;
    socket.validate_extension<PICS_TXD0>(*p);
    socket->nb_transport_fw(*p,ph,t);
    socket.invalidate_extension<PICS_TXD0>(*p);

    wait(10,sc_core::SC_NS);
    
    data=0x55;
    socket.validate_extension<PIC_TIMER4>(*p);
    socket->nb_transport_fw(*p,ph,t);
    socket.invalidate_extension<PIC_TIMER4>(*p);

    wait(10,sc_core::SC_NS);

    data=0x66;
    socket.validate_extension<PIC_IIC>(*p);
    socket->nb_transport_fw(*p,ph,t);
    socket.invalidate_extension<PIC_IIC>(*p);

    wait(10,sc_core::SC_NS);
    
    data=0x77;
    socket.validate_extension<PICS_TXD0>(*p);
    socket->nb_transport_fw(*p,ph,t);
    socket.invalidate_extension<PICS_TXD0>(*p);

    //Finally releasing the transaction
    socket.release_transaction(p);
    
    wait(10,sc_core::SC_NS);

  }
};

SC_MODULE(Slave) {
  
  target_signal_socket<Slave> socket;

  SC_CTOR(Slave) : socket("socket") {
    socket.register_nb_transport_fw(this, &Slave::trans);

    gs::socket::config<gs_generic_signal_protocol_types> tmp;
    tmp.use_mandatory_extension<PIC_TIMER4>();
    tmp.use_mandatory_extension<PIC_IIC>();
    tmp.use_mandatory_extension<PICS_TXD0>();

    socket.set_config(tmp);
  }
  
  tlm::tlm_sync_enum trans(gs_generic_signal_payload& trans, signal_phase& ph, sc_core::sc_time& delay) {
  unsigned char last_val=0;

    if (socket.get_extension<PICS_TXD0>(trans))
      {
    	if(trans.is_ack_required()==true)
    		ph=gs_generic_signal::ACK;
    	std::cout<<"PICS_TXD0 enabled"<<std::endl;

       last_val=socket.get_last_value<PICS_TXD0>();
       std::cout << hex <<"last value of PICS_TXD0 signal "<< (unsigned int)last_val << endl;
      }
    else  if (socket.get_extension<PIC_TIMER4>(trans))
      {
    	if(trans.is_ack_required()==true)
    		ph=gs_generic_signal::ACK;
    	std::cout<<"PIC_TIMER4 enabled"<<std::endl;

       last_val=socket.get_last_value<PIC_TIMER4>();
       std::cout<< hex <<"last value of PIC_TIMER4 signal "<< (unsigned int)last_val << endl;
      }
    else   if (socket.get_extension<PIC_IIC>(trans))
      {
    	if(trans.is_ack_required()==true)
    		ph=gs_generic_signal::ACK;
    	std::cout<<"PIC_IIC enabled"<<std::endl;

       last_val=socket.get_last_value<PIC_IIC>();
       std::cout<< hex <<"last value of PIC_IIC signal "<< (unsigned int)last_val << endl;
      }
    else
      std::cout<<"ERROR : disabled"<<std::endl;
    return tlm::TLM_COMPLETED;
  }
};


#endif /*EXAMPLE_H_*/
