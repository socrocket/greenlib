#ifndef INLINESYNC_H
#define INLINESYNC_H

#include <systemc.h>
#include <tlm.h>
//#include <tlm_utils/simple_initiator_socket.h>
//#include <tlm_utils/simple_target_socket.h>
#include "greenthreads/thread_safe_event.h"
#include <semaphore.h>
#include <pthread.h>

//#include <greensocket/generic/gs_extension.h>
#include <greensocket/target/multi_socket.h>
#include <greensocket/initiator/multi_socket.h>

#include <greenrouter/genericRouter_if.h>
#include <greenrouter/genericRouter.h>
#include <greenrouter/protocol/SimpleBus/simpleBusProtocol.h>
#include <greenrouter/scheduler/fixedPriorityScheduler.h>

#define COUT if (0) cout << sc_core::sc_get_current_process_handle().name() << ":" << pthread_self() << ":"

namespace gs {
  namespace gt {
  
    template <unsigned int BUSWIDTH = 32> class inLineSync :sc_core::sc_module 
    {
    public:
    
      gs::socket::target_multi_socket<BUSWIDTH,tlm::tlm_base_protocol_types> target_socket;  
      gs::socket::initiator_multi_socket<BUSWIDTH,tlm::tlm_base_protocol_types> init_socket;
    
    private:
      gs::gp::GenericRouter<BUSWIDTH> *internalRouter;
    
      event_async syncUnlock;
    
      pthread_t mainThread;
    
      unsigned int l_port;
      tlm::tlm_generic_payload * l_trans;
      sc_core::sc_time * l_delay;
      bool txnPending;
      sem_t txnDone;
      sem_t txnMutex;
      
    public:
      
      SC_CTOR(inLineSync):  target_socket("input"),  init_socket("output")
        {
          mainThread=pthread_self();
          sem_init(&txnDone, 0, 0);
          sem_init(&txnMutex, 0, 1);
        
          internalRouter=createRouter();
        
          internalRouter->init_socket(init_socket);
          target_socket(internalRouter->target_socket);
        
          txnPending=0;
          
          SC_THREAD(processTxn);
        }
    
    
      // NB, the WHOLE point is that this b_transport will be called from a remote
      // system, hence it will be in a different thread !
      void b_transport( unsigned int from, tlm::tlm_generic_payload& trans, sc_core::sc_time& delay )
        {
          COUT << "b_transport txn requested\n";

//          if we are before the end of elaboration, we should not allow the txn !!!!
                  
          // We're in the same thread - dont do anything, pass through.
          if ( pthread_equal(pthread_self(), mainThread)) {
            internalRouter->b_tr(from,trans,delay);
          } else {
            sem_wait(&txnMutex); // only allow one txn at a time, as we only have one
            // set of l_* variables.
            l_port=from;
            l_trans=&trans;
            l_delay=&delay;
            txnPending=true;
            COUT << "Unlock SystemC\n";
            syncUnlock.notify();
            COUT << "Qemu waiting for SystemC\n";
            sem_wait(&txnDone); // we really hold this thread now.
            sem_post(&txnMutex);
            COUT << "txn done, Qemu running again\n";
          }
        }
      
      void assign_address (sc_dt::uint64 baseAddress, sc_dt::uint64 highAddress, unsigned int portNumber)
        {
          internalRouter->assign_address(baseAddress, highAddress, portNumber);
        }
      
          
    
    private:
      void b_transport_safe()
        {
          if (txnPending) {
            COUT << "Doing txn\n";
            internalRouter->b_tr(l_port, *l_trans,*l_delay);
            txnPending=false;
            COUT << "Done txn\n";
            // just one thing, before we allow the other thread to
            // continue.... TIME !
            sem_post(&txnDone); // release the other thread.
            COUT << "Finished txn\n";
          }  
        }
    
      void processTxn()
        {
          while (1) {
          
            COUT << "Process pending txn\n";
            b_transport_safe();
            COUT << "Processed txn\n";
            wait(syncUnlock);        // await - dont fall of the end of time
          }
        }

      class SafeRouter : public gs::gp::GenericRouter<BUSWIDTH>
      {
        inLineSync *parent;
        sem_t mutex;
      public:
      
        SafeRouter(const char* name, inLineSync *p):
            gs::gp::GenericRouter<BUSWIDTH>(name)
          {
            sem_init(&mutex, 0, 1);
            gs::gp::GenericRouter<BUSWIDTH>::target_socket.register_b_transport(this, &SafeRouter::b_transport);
            gs::gp::GenericRouter<BUSWIDTH>::target_socket.register_transport_dbg(this, &SafeRouter::tr_dbg);
            gs::gp::GenericRouter<BUSWIDTH>::target_socket.register_get_direct_mem_ptr(this, &SafeRouter::get_dmi);

            parent=p;
          }
        void b_transport( unsigned int from, tlm::tlm_generic_payload& trans, sc_core::sc_time& delay )
          {
            parent->b_transport(from, trans, delay);
          }
        void b_tr( unsigned int from, tlm::tlm_generic_payload& trans, sc_core::sc_time& delay )
          {
            // The transaction will be processed in the SystemC thread, no need
            // to take the mutex.
            gs::gp::GenericRouter<BUSWIDTH>::b_tr(from, trans, delay);
          }

        unsigned int tr_dbg(unsigned int from, tlm::tlm_generic_payload& trans)
          {
            sem_wait(&mutex);
            unsigned int r=gs::gp::GenericRouter<BUSWIDTH>::tr_dbg(from,trans);
            sem_post(&mutex);
            return r;
          }
        
        bool get_dmi(unsigned int from, tlm::tlm_generic_payload& trans, tlm::tlm_dmi& dmi_data) 
          {
            sem_wait(&mutex);
            bool r=gs::gp::GenericRouter<BUSWIDTH>::get_dmi(from, trans, dmi_data);
            sem_post(&mutex);
            return r;
          }
      
      };
    
    
      gs::gp::GenericRouter<BUSWIDTH> *createRouter()
        {
          gs::gp::SimpleBusProtocol<BUSWIDTH> *protocol;
          gs::gp::fixedPriorityScheduler *scheduler;
          SafeRouter *router;
        
          protocol = new gs::gp::SimpleBusProtocol<BUSWIDTH>(sc_gen_unique_name("protocol"),10);
          scheduler = new gs::gp::fixedPriorityScheduler(sc_gen_unique_name("scheduler"));
          router = new SafeRouter(sc_gen_unique_name("router"), this);
        
          protocol->router_port(*router);
          protocol->scheduler_port(*scheduler);
          router->protocol_port(*protocol);
        
          return router;
        }
    };
  }
}


#undef COUT
  
#endif
