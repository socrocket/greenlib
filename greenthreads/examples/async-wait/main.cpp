/*
 *
 * Copyright (C) 2015, GreenSocs Ltd.
 *
 * Developed by Mark Burton
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

#include <iostream>
#include <pthread.h>
#include <systemc>
#include "node.h"

#include <greenthreads/inlinesync.h>
#include "greenrouter/genericRouter.h"
#include "greenrouter/protocol/SimpleBus/simpleBusProtocol.h"
#include "greenrouter/scheduler/fixedPriorityScheduler.h"
#include "greenthreads/inlinesync.h"


static gs::gp::GenericRouter<32> *create_generic_router_32()
{
  gs::gp::SimpleBusProtocol<32> *protocol;
  gs::gp::fixedPriorityScheduler *scheduler;
  gs::gp::GenericRouter<32> *router;

  protocol = new gs::gp::SimpleBusProtocol<32>(
      sc_gen_unique_name("protocol"), 10);
	scheduler = new gs::gp::fixedPriorityScheduler(
      sc_gen_unique_name("scheduler"));
	router = new gs::gp::GenericRouter<32>(
      sc_gen_unique_name("router"));

	protocol->router_port(*router);
	protocol->scheduler_port(*scheduler);
	router->protocol_port(*protocol);

  return router;
}

int sc_main(int argc, char **argv) {

  srand(0); // try to keep things as determanistic as possible....
  
  tlm_utils::tlm_quantumkeeper::set_global_quantum(sc_core::sc_time(1000,SC_NS));

  int maxport = 10;
  int perport=1;
  gs::gp::GenericRouter<32> *router1 = create_generic_router_32();
  for (int port=0;port<maxport;port++)
  {
    unsigned int add=port*perport;
    asynctestnode *atn=new asynctestnode(sc_gen_unique_name("asyncTestNode"));
    for (int pp=0; pp<maxport; pp++) {
      if (pp!=port) atn->addAdd(pp*perport);
    }
    
    gs::gt::inLineSync<32> *insync_in = new gs::gt::inLineSync<>(sc_gen_unique_name("insync_in"));
    insync_in->init_socket(atn->target_socket);
    insync_in->assign_address (add,add+perport-1, 0);
    router1->init_socket(insync_in->target_socket);
    
//    gs::gt::inLineSync<32> *insync_out = new gs::gt::inLineSync<>(sc_gen_unique_name("insync_out"));
//    insync_out->init_socket(router1->target_socket);
//    atn->init_socket(insync_out->target_socket);

    atn->init_socket(router1->target_socket);
  }
//  sc_start();
  sc_start(sc_time(5000000,SC_NS));
  
  

  return 0;
}





