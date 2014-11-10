
#ifndef __SIMPLE_TLM_SERIAL_MASTER__
#define __SIMPLE_TLM_SERIAL_MASTER__

#include "greenserialsocket/tlm_serial_initiator_socket.h"

using namespace tlm_serial;

class SerialMaster: public sc_module
{
 public:
   tlm_serial_initiator_socket<> master_sock;

   SC_HAS_PROCESS(SerialMaster);

   SerialMaster(sc_module_name nm):sc_module(nm),master_sock("master_sock")
   {
     SC_THREAD(run);
   }
   void run();
};

   void SerialMaster::run()
   {
      sc_time t(0,SC_NS);
      unsigned char data[4] = {'a', 'b', 'c', 'd'};
      bool data_par[4] = {true, true, true, false};

      tlm_serial_payload *txn= master_sock.get_transaction();

      txn->set_command(SERIAL_SEND_CHAR_COMMAND);
      txn->set_data_ptr(data);
      txn->set_data_length(sizeof(data));
      txn->set_response_status(SERIAL_OK_RESPONSE);
      txn->set_num_stop_bits_in_end(1);
      txn->set_parity_bits(data_par);
      txn->set_enable_bits(SERIAL_PARITY_ENABLED|SERIAL_STOP_BITS_ENABLED);

#ifdef USE_NB_CALL
      tlm::tlm_phase ph = tlm_serial::BEG_TRANSMISSION;
      master_sock->nb_transport_fw(*txn, ph, t);
#else
      master_sock->b_transport(*txn,t);
#endif
   }

#endif
