/*
 * master.h
 *
 * Copyright (C) 2014, GreenSocs Ltd.
 *
 * Developped by Manish Aggarwal, Ruchir Bharti
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses>.
 *
 * Linking GreenSocs code, statically or dynamically with other modules
 * is making a combined work based on GreenSocs code. Thus, the terms and
 * conditions of the GNU General Public License cover the whole
 * combination.
 *
 * In addition, as a special exception, the copyright holders, GreenSocs
 * Ltd, give you permission to combine GreenSocs code with free software
 * programs or libraries that are released under the GNU LGPL, under the
 * OSCI license, under the OCP TLM Kit Research License Agreement or
 * under the OVP evaluation license.You may copy and distribute such a
 * system following the terms of the GNU GPL and the licenses of the
 * other code concerned.
 *
 * Note that people who make modified versions of GreenSocs code are not
 * obligated to grant this special exception for their modified versions;
 * it is their choice whether to do so. The GNU General Public License
 * gives permission to release a modified version without this exception;
 * this exception also makes it possible to release a modified version
 * which carries forward this exception.
 *
 */

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
