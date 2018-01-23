#ifndef ASYNCTESTNODE_H
#define ASYNCTESTNODE_H

//#define DEBUG
#define NOSLEEP
#if (defined(_WIN32) || defined(_WIN64))
#include <windows.h>
#define usleep(time) Sleep(time / 1000)
#else
#include <unistd.h>
#endif

#ifdef NOSLEEP
#undef usleep
#define usleep(time) if (0) while (0)
#endif

#include <systemc>
#include <tlm>
//#include <tlm_utils/tlm_quantumkeeper.h>
#include <semaphore.h>
#include <queue>
#include <vector>
#include <greensocket/target/multi_socket.h>
#include <greensocket/initiator/multi_socket.h>
#include <greenthreads/thread_safe_event.h>



extern "C" void *processQueueStart(void *c) ;

class asynctestnode : sc_core::sc_module
{
public:
  
  gs::socket::target_multi_socket<32,tlm::tlm_base_protocol_types> target_socket;  
  gs::socket::initiator_multi_socket<32,tlm::tlm_base_protocol_types> init_socket;

  std::queue<tlm::tlm_generic_payload *> queue;
  std::vector<unsigned int> addresses;
  
  sem_t dmilock;
  sem_t txnReady;
  sem_t tQueue;
  pthread_t scThread;
  
  sc_core::sc_event processEvt;
  
  gs::socket::config<tlm::tlm_base_protocol_types> cfg;
    
  gs::gt::syncSource syncSource;

  
SC_CTOR(asynctestnode):  target_socket("input"),  init_socket("output"), syncSource(true)
    {
      scThread=pthread_self();
      
      init_socket.set_config(cfg);
      target_socket.set_config(cfg);
      
      target_socket.register_b_transport(this, &asynctestnode::b_tr);
      target_socket.register_get_direct_mem_ptr(this, &asynctestnode::get_dmi);
      
      sem_init(&dmilock,0,1);
      sem_init(&tQueue,0,1);

      
      SC_THREAD(startProcessQueue);

    }

  void addAdd(unsigned int add)
    {
      addresses.push_back(add);
    }

  void startProcessQueue() 
    {
      wait(SC_ZERO_TIME);
      pthread_t id;
      void *t = this;
      pthread_create(&id, NULL, processQueueStart, t);
    }
  

  // We are in SystemC
  void b_tr( unsigned int from, tlm::tlm_generic_payload& trans, sc_core::sc_time& delay )
    {
      // pass the txn onto the processing queue.
      // Note - this is totally NOT what you should do in normal TLM, as
      // this isn't "our" transaction - but, we will play this game for this
      // test.
      assert(pthread_self() == scThread);

      wait(rand()%100, SC_NS); // we'll do stuff, it takes time....

      // return the txn into the queue - this will be picked up by the other
      // thread to be re-used...
      sem_wait(&tQueue);
      queue.push(&trans);
      sem_post(&tQueue);
    }

  bool get_dmi(unsigned int from, tlm::tlm_generic_payload& trans, tlm::tlm_dmi& dmi_data)
    {
      sem_wait(&dmilock);
      // remember who you passed DMI's to.
      sem_post(&dmilock);
      return true;
    }
  
  void processQueue()
    {
      sc_core::sc_time myTime=sc_core::SC_ZERO_TIME;

      while (1) {
        tlm::tlm_generic_payload *txn;

        if (queue.empty()) {
          txn = new tlm::tlm_generic_payload();
        } else {
          sem_wait(&tQueue);
          txn = queue.front(); queue.pop();
          sem_post(&tQueue);
        }
        usleep(rand()%100);
        myTime+=sc_core::sc_time(rand()%1000, sc_core::SC_NS);

#ifdef DEBUG
        std::cout << name() << " processing txn at sc_time "<< sc_time_stamp() << " our time " << myTime <<" ( "<<queue.size()<<" txn's in my queue)\n";
#endif

        std::random_shuffle(addresses.begin(), addresses.end());
        txn->set_address(addresses.front());
//        cout << name() << " sending to " <<addresses.front()<<"\n";

        if (init_socket.size() > 1) {
          init_socket[rand()%init_socket.size()]->b_transport(*txn, myTime);
        } else {
          init_socket[0]->b_transport(*txn, myTime);
        }
        syncSource.syncAt(myTime);// + quantum);

      }
      
    }
  
  
};


extern "C" void *processQueueStart(void *c)
{
  static_cast< asynctestnode * >(c)->processQueue();
  return NULL;
}
  
#endif
