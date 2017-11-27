#ifndef INLINESYNC_H
#define INLINESYNC_H

#include <systemc.h>
#include <tlm.h>
//#include <tlm_utils/simple_initiator_socket.h>
//#include <tlm_utils/simple_target_socket.h>
#include "greenthreads/thread_safe_event.h"
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
    
      event_async processTxnEvent;
      pthread_t mainThread;
    
      unsigned int l_port;
      tlm::tlm_generic_payload * l_trans;
      sc_core::sc_time * l_delay;
      tlm::tlm_dmi * l_dmi_data;
      unsigned int l_result;
      
      gs::gt::sem_i txnDone;
      gs::gt::spin_mutex txnMutex;

      event_async processDmiEvent;
      event_async processDbgEvent;

    public:
      
      SC_CTOR(inLineSync):  target_socket("input"),  init_socket("output"),
                            txnDone(0)
        {
          mainThread=pthread_self();
        
          internalRouter=createRouter();
        
          internalRouter->init_socket(init_socket);
          target_socket(internalRouter->target_socket);
          
          SC_THREAD(processTxn);

          SC_METHOD(processDmiMethod);
          sensitive << processDmiEvent;
          dont_initialize();

          SC_METHOD(processDbgMethod);
          sensitive << processDbgEvent;
          dont_initialize();
        }

    
      // NB, the WHOLE point is that this b_transport will be called from a remote
      // system, hence it will be in a different thread !
      void b_transport( unsigned int from, tlm::tlm_generic_payload& trans, sc_core::sc_time& delay )
        {
          COUT << "b_transport txn requested at time "<<sc_time_stamp()<<"\n";

//          if we are before the end of elaboration, we should not allow the txn !!!!
                  
          // We're in the same thread - dont do anything, pass through.
          if ( pthread_equal(pthread_self(), mainThread)) {
            internalRouter->b_tr(from,trans,delay);
          } else {
            txnMutex.lock();
            l_port=from;
            l_trans=&trans;
            l_delay=&delay;
            COUT << "Unlock SystemC\n";
            processTxnEvent.notify();
            centralSyncPolicy::share.releaseLock();
            COUT << "Qemu waiting for SystemC\n";
            txnDone.wait(); // we really hold this thread now.
            txnMutex.unlock();
            COUT << "txn done, Qemu running again\n";
          }
        }

      bool get_dmi(unsigned int from, tlm::tlm_generic_payload& trans, tlm::tlm_dmi& dmi_data)
        {
          COUT << "get dmi requested\n";

          // We're in the same thread - dont do anything, pass through.
          if ( pthread_equal(pthread_self(), mainThread)) {
            return internalRouter->get_dmi(from,trans, dmi_data);
          } else {
            txnMutex.lock();
            l_port=from;
            l_trans=&trans;
            l_dmi_data=&dmi_data;
            COUT << "Unlock SystemC\n";
            processDmiEvent.notify();
            centralSyncPolicy::share.releaseLock();
            COUT << "Qemu waiting for SystemC\n";
            txnDone.wait(); // we really hold this thread now.
            bool r=(bool)l_result; // pick this up before releasing the mutex!
            txnMutex.unlock();
            COUT << "txn done, Qemu running again\n";
            return r;
          }
        }

      unsigned int tr_dbg(unsigned int from, tlm::tlm_generic_payload& trans)
        {
          COUT << "debug transport requested\n";

          // We're in the same thread - dont do anything, pass through.
          if ( pthread_equal(pthread_self(), mainThread)) {
            return internalRouter->tr_dbg(from,trans);
          } else {
            txnMutex.lock();
            l_port=from;
            l_trans=&trans;
            COUT << "Unlock SystemC\n";
            processDbgEvent.notify();
            centralSyncPolicy::share.releaseLock();
            COUT << "Qemu waiting for SystemC\n";
            txnDone.wait(); // we really hold this thread now.
            unsigned int r=l_result; // pick this up before releasing the mutex!
            txnMutex.unlock();
            COUT << "tr dbg done, Qemu running again\n";
            return r;
          }
        }

      void assign_address (sc_dt::uint64 baseAddress, sc_dt::uint64 highAddress, unsigned int portNumber)
        {
          internalRouter->assign_address(baseAddress, highAddress, portNumber);
        }
      
          
    
    private:
      void b_transport_safe()
        {
        }
      // note this MUST be in a thread, not a method, to allow models to call wait!    
      void processTxn()
        {
          while (1) {
            wait(processTxnEvent);        // await - dont fall of the end of time
            COUT << "Doing txn\n";
            internalRouter->b_tr(l_port, *l_trans,*l_delay);
            txnDone.post(); // release the other thread.
            centralSyncPolicy::share.takeLock();
            COUT << "Finished txn\n";
          }
        }

      // must not call wait (clause l of 11.2.6)
      void processDmiMethod()
        {
          COUT << "Doing dmi\n";
          l_result=internalRouter->get_dmi(l_port, *l_trans, *l_dmi_data);
          txnDone.post(); // release the other thread.
          centralSyncPolicy::share.takeLock();
          COUT << "Finished dmi\n";
        }

      // must not call wait (clause t of 11.3.4)
      void processDbgMethod()
        {
          COUT << "Doing dbg\n";
          l_result=internalRouter->tr_dbg(l_port, *l_trans);
          txnDone.post(); // release the other thread.
          centralSyncPolicy::share.takeLock();
          COUT << "Finished dbg\n";
        }

      class SafeRouter : public gs::gp::GenericRouter<BUSWIDTH>
      {
        inLineSync *parent;

      public:
      
        SafeRouter(const char* name, inLineSync *p):
            gs::gp::GenericRouter<BUSWIDTH>(name)
          {
            parent=p;
            gs::gp::GenericRouter<BUSWIDTH>::target_socket.register_b_transport(this, &SafeRouter::b_tr_in);
            gs::gp::GenericRouter<BUSWIDTH>::target_socket.register_transport_dbg(this, &SafeRouter::tr_dbg_in);
            gs::gp::GenericRouter<BUSWIDTH>::target_socket.register_get_direct_mem_ptr(this, &SafeRouter::get_dmi_in);
          }
        void b_tr_in( unsigned int from, tlm::tlm_generic_payload& trans, sc_core::sc_time& delay )
          {
            parent->b_transport(from, trans, delay);
          }
        void b_tr( unsigned int from, tlm::tlm_generic_payload& trans, sc_core::sc_time& delay )
          {
            gs::gp::GenericRouter<BUSWIDTH>::b_tr(from, trans, delay);
          }

        unsigned int tr_dbg_in(unsigned int from, tlm::tlm_generic_payload& trans)
          {
            return parent->tr_dbg(from, trans);
          }
        unsigned int tr_dbg(unsigned int from, tlm::tlm_generic_payload& trans)
          {
            return gs::gp::GenericRouter<BUSWIDTH>::tr_dbg(from,trans);
          }

        bool get_dmi_in(unsigned int from, tlm::tlm_generic_payload& trans, tlm::tlm_dmi& dmi_data)
          {
            return parent->get_dmi(from, trans, dmi_data);
          }
        bool get_dmi(unsigned int from, tlm::tlm_generic_payload& trans, tlm::tlm_dmi& dmi_data) 
          {
            return gs::gp::GenericRouter<BUSWIDTH>::get_dmi(from, trans, dmi_data);
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
