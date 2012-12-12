// LICENSETEXT
// 
//   Copyright (C) 2007-2008 : GreenSocs Ltd
//       http://www.greensocs.com/ , email: info@greensocs.com
// 
//   Developed by :
// 
//   Christian Schröder, Robert Guenzel
//     Technical University of Braunschweig, Dept. E.I.S.
//     http://www.eis.cs.tu-bs.de
//
//
// The contents of this file are subject to the licensing terms specified
// in the file LICENSE. Please consult this file for restrictions and
// limitations that may apply.
// 
// ENDLICENSETEXT

// This file is based on the Master.h (see GSGPSocket example 'nb_example')


//
// This GreenSocs GenericProtocol (GSGP) master sends the following
// transactions to the address init_port.target_addr:
//
// - Write commands:
//   1) send normal write transaction
//   2) if write response is enabled: send write transaction with ResponseError 
//      else: send normal write transaction
//   3) send normal write transaction (slave should produce a RequestError)
//   4) send normal write transaction (slave should produce a DataError (or different RequestError TLM2.0 phase))
// - Read commands:
//   1) send normal read transaction
//   2) send read transaction with ResponseError
//   3) send normal read transaction (slave should produce a RequestError)
//   4) send normal read transaction (slave should produce a different RequestError TLM2.0 phase)
//
// Variable master device 
//  can be configured to different behaviour:
//
// use #define USE_CC to use multiple data phases (with BytesValid) 
// use #define WRITE_RESPONSE_NEEDED to make the master await a response
//                    phase after all write data phases have been finished.
//                    TODO: As long as automatism is not imnplemented, make 
//                          sure that the GP slave will send the response
//                          (e.g. use Slave.h and define WRITE_RESPONSE_SEND)
//                          When being connected to an OSCI device, the GSGP
//                          Socket does already automatically create the phase!
// use #define USE_BLOCKING_API to define if this module uses the blocking API
//

//#define USE_CC // has only affect if _not_ USE_PV, also has effect on the Slave!!! (see file "Slave.h")
//#define WRITE_RESPONSE_NEEDED
//#define USE_GPSOCKET // should be set by compiler flag
//#define USE_BLOCKING_API

#define MEMSIZE 262144 // for address space 0x00000 to 0x40000

#ifndef IF_GPSOCKET
#  ifdef USE_GPSOCKET
#    define IF_GPSOCKET(order) order
#  else
#    define IF_GPSOCKET(order)
#  endif
#endif


#include <iostream>

#ifdef USE_BLOCKING_API
# include "gsgpsocket/transport/GSGPMasterBlockingSocket.h"
#else
# include "gsgpsocket/transport/GSGPMasterSocket.h"
#endif
using namespace gs;
using namespace gs::gp;


#include "tlm_utils/peq_with_get.h"           // Payload event queue FIFO
#include <iomanip>


class TestMaster 
: public sc_core::sc_module,
  public payload_event_queue_output_if<master_atom>
{
public:
#ifdef USE_BLOCKING_API
  GenericMasterBlockingPort<32> init_port;
#else
  GenericMasterPort<32> init_port;
#endif
  typedef GenericMasterPort<32>::accessHandle accessHandle;
  typedef GenericMasterPort<32>::phase phase;
  typedef pair<accessHandle, phase> peq_pair;
  tlm_utils::peq_with_get<peq_pair> m_receive_peq;
  
  unsigned char mem[MEMSIZE];

  void sendPV(accessHandle );
  void sendPVT(accessHandle );
    
  void main_action();
  void perform_write(bool produce_response_error = false);
  void perform_read(bool produce_response_error = false);
  
  peq_pair* wait_for_next_atom();

  virtual void notify (master_atom& tc);

  virtual void end_of_simulation();
  
  GSDataType mdata;
  
  // configurable parameters
  gs::gs_param<gs_uint32> burst_length; // burst length in byte
  gs::gs_param<gs_uint32> initial_delay; // wait this number of clock cycles initially before start of operation
  gs::gs_param<gs_uint32> loops; // how many transactions shall I generate?
  //gs_uint32 burst_length;
  //gs_uint32 initial_delay;
  //gs_uint32 loops;
  
  sc_core::sc_time m_initial_wait;
  
public:
  // Constructor
  SC_HAS_PROCESS(TestMaster);
  TestMaster(sc_core::sc_module_name name_, sc_core::sc_time initial_wait=sc_core::SC_ZERO_TIME) //, unsigned long long targetAddress, const char* data, bool rNw) 
  : sc_module(name_)
  , init_port("iport")
  , m_receive_peq("receive_peq")
  , mdata(20)
  , burst_length("burst_length", 20)
  , initial_delay("initial_delay", 10)
  , loops("loops", 5)
  , m_initial_wait(initial_wait)
  {
    SC_THREAD(main_action);

#ifdef USE_BLOCKING_API
    // bind blocking in interface
    init_port.out_port(*this);
#else
    // bind PEQ in interface
    init_port.peq.out_port(*this);
#endif
    
    // Configure the socket
    GSGPSocketConfig cnf;
# ifdef WRITE_RESPONSE_NEEDED
    cnf.use_wr_resp = true;
# else
    cnf.use_wr_resp = false;
# endif
    init_port.set_config(cnf);
  }
  
  ~TestMaster(){
  }
};

// ----------- main action -----------------------------------------
void TestMaster::main_action() {
  wait(m_initial_wait);
  std::cout << std::endl << "------ WRITE #1 ----------------------------------------------------" << std::endl << std::endl;
  perform_write();
  wait(100, sc_core::SC_NS);
  std::cout << std::endl << "------ WRITE #2 ----------------------------------------------------" << std::endl << std::endl;
  perform_write(true);
  wait(100, sc_core::SC_NS);
  std::cout << std::endl << "------ WRITE #3 ----------------------------------------------------" << std::endl << std::endl;
  perform_write();
  wait(100, sc_core::SC_NS);
  std::cout << std::endl << "------ WRITE #4 ----------------------------------------------------" << std::endl << std::endl;
  perform_write();
  wait(100, sc_core::SC_NS);
  std::cout << std::endl << "----------------------------------------------------------" << std::endl << std::endl;
  std::cout << std::endl << "------  READ #1 ----------------------------------------------------" << std::endl << std::endl;
  perform_read();
  wait(100, sc_core::SC_NS);
  std::cout << std::endl << "------  READ #2 ----------------------------------------------------" << std::endl << std::endl;
  perform_read(true);
  wait(100, sc_core::SC_NS);
  std::cout << std::endl << "------  READ #3 ----------------------------------------------------" << std::endl << std::endl;
  perform_read();
  wait(100, sc_core::SC_NS);
  std::cout << std::endl << "------  READ #4 ----------------------------------------------------" << std::endl << std::endl;
  perform_read();
  wait(100, sc_core::SC_NS);
  std::cout << std::endl << "----------------------------------------------------------" << std::endl << std::endl;
  std::cout<< std::endl<<name()<<" !!!!!!!!!!!!!!!! ALL DONE !!!!!!!!!!!!!!!!!!!!!"<<std::endl;
}

// ----------- WRITE action -----------------------------------------
void TestMaster::perform_write(bool produce_response_error) {
#ifdef WRITE_RESPONSE_NEEDED
  if (produce_response_error) {
    GS_DUMP("write transaction with response error");
  } else 
#endif
  {
    GS_DUMP("write transaction");
  }

  // make sure no transaction outstanding
  assert(m_receive_peq.get_next_transaction() == NULL);

  // fill in the write data
  if (mdata.getSize()<burst_length)
    mdata.getData().resize(burst_length);
  for (unsigned int i=0; i<burst_length; i++)
    mdata[i]=i;
  

  peq_pair *atom;
  phase ph;
  unsigned bvalid;

  bvalid=0;

  // fill in new write data
  for (unsigned int i=0; i<burst_length; i++)
    mdata[i]=i;

  accessHandle tah = init_port.create_transaction();
  tah->setMCmd(Generic_MCMD_WR);
  tah->setMData(mdata);
  tah->setMBurstLength(burst_length.getValue());
  
  std::cout << "(" << name() << "): data to send: "; for (unsigned int a = 0; a < burst_length; a++) std::cout << (unsigned int) mdata[a] << " "; std::cout << std::endl;
  GS_DUMP("Master send Request (RequestValid).");
#ifdef USE_BLOCKING_API
  init_port.Request.block(tah, ph);
#else
  init_port.Request(tah);
  atom = wait_for_next_atom();
  tah = atom->first; ph = atom->second;
#endif

  if(ph.state==GenericPhase::RequestAccepted) {
    GS_DUMP("Slave sent RequestAccepted.");
  }
  else if (ph.state==GenericPhase::RequestError) {
    SC_REPORT_WARNING(name(), "Oh no, request error.");
    return;
  }
  else {
    std::stringstream ss; ss << "wrong phase: "; IF_GPSOCKET( ss << ph.to_string(); )
    SC_REPORT_ERROR(name(),ss.str().c_str());
  }


  do {
    // make sure no transaction outstanding
    assert(m_receive_peq.get_next_transaction() == NULL);
# ifdef USE_CC
    bvalid += 8; // send 64 bit per data atom
    if (bvalid > burst_length) 
      bvalid = burst_length;
# else
    bvalid = burst_length; // send all data with one data atom
# endif
    ph.setBytesValid( bvalid );
    GS_DUMP("Master send data (DataValid).");
#ifdef USE_BLOCKING_API
    init_port.SendData.block(tah, ph);
#else
    init_port.SendData(tah,ph);
    atom = wait_for_next_atom();
    tah = atom->first; ph = atom->second;
#endif
    if(ph.state==GenericPhase::DataAccepted) {
      GS_DUMP("Slave accepted the data (DataAccepted).");
    }
    else if (ph.state == GenericPhase::DataError) {
      SC_REPORT_WARNING(name(), "Oh no, data error.");
      return;
    }      
    else {
      std::stringstream ss; ss << "wrong phase: "; IF_GPSOCKET( ss << ph.to_string(); )
      SC_REPORT_ERROR(name(),ss.str().c_str());
    }
  } while (bvalid!=burst_length);

#ifdef WRITE_RESPONSE_NEEDED
  atom = wait_for_next_atom();
  tah = atom->first; ph = atom->second;
  if (ph.state == GenericPhase::ResponseValid) {
    GS_DUMP("Slave sent response (ResponseValid).");
  }
  else if (ph.state == GenericPhase::ResponseError){
    GS_DUMP("Oh no, response error.");
    return;
  }
  else {
    std::stringstream ss; ss << "wrong phase: "; IF_GPSOCKET( ss << ph.to_string(); )
    SC_REPORT_ERROR(name(),ss.str().c_str());
  }
  if (produce_response_error)
    init_port.ErrorResponse(tah,ph);
  else
    init_port.AckResponse(tah,ph);
#endif

  init_port.release_transaction(tah);
  
}

// ----------- READ action -----------------------------------------
void TestMaster::perform_read(bool produce_response_error) {
  if (produce_response_error) {
    GS_DUMP("read transaction with response error");
  } else {
    GS_DUMP("read transaction");
  }
  
  // make sure no transaction outstanding
  assert(m_receive_peq.get_next_transaction() == NULL);

  peq_pair *atom;
  phase ph;
  accessHandle tah = init_port.create_transaction();
  
  if (mdata.getSize()<burst_length)
    mdata.getData().resize(burst_length);
  
  tah->setMCmd(Generic_MCMD_RD);
  tah->setMBurstLength(burst_length.getValue());
  tah->setMData(mdata);
  
  GS_DUMP("Master send Request (RequestValid).");
#ifdef USE_BLOCKING_API
  init_port.Request.block(tah, ph);
#else
  init_port.Request(tah);
  atom = wait_for_next_atom();
  tah = atom->first; ph = atom->second;
#endif
  if (ph.state == GenericPhase::RequestAccepted) {
    GS_DUMP("Slave sent RequestAccepted.");
  }
  else if (ph.state == GenericPhase::RequestError) {
    SC_REPORT_WARNING(name(), "Oh no, request error.");
    return;
  }
  else {
    std::stringstream ss; ss << "wrong phase: "; IF_GPSOCKET( ss << ph.to_string(); )
    SC_REPORT_ERROR(name(),ss.str().c_str());
  }
  
  do {
    GS_DUMP("master waits for data (chunk).");
    atom = wait_for_next_atom();
    tah = atom->first; ph = atom->second;
    if (ph.state == GenericPhase::ResponseValid){
      GS_DUMP("Slave sent data.");
      GS_DUMP("data valid: 0x"<<(std::hex)<< (gs_uint64)ph.getBytesValid() <<(std::dec)
              <<" (32bit=" << (gs_uint32)ph.getBytesValid() << ")");
    }
    else {
      std::stringstream ss; ss << "wrong phase: "; IF_GPSOCKET( ss << ph.to_string(); )
      SC_REPORT_ERROR(name(),ss.str().c_str());
    }
    if (produce_response_error) {
      init_port.ErrorResponse(tah, ph);
      break;

    
    } else
      init_port.AckResponse(tah, ph);
  } 
  while (ph.getBytesValid() < tah->getMBurstLength());
  IF_GPSOCKET( std::cout << "(" << name() << "): data received: "; for (unsigned int a = 0; a < tah->getMBurstLength(); a++) std::cout << (unsigned int) tah->getMData()[a] << " "; std::cout << std::endl; )
  
  init_port.release_transaction(tah);
  
}

// ----------- others -----------------------------------------
void TestMaster::end_of_simulation() {
}


// ----------- notify -----------------------------------------
void TestMaster::notify(master_atom &tc) {
  IF_GPSOCKET( GS_DUMP("non-blocking notify "<< tc.second.to_string()<<", "<< gs::tlm_command_writer::to_string(tc.first.get_tlm_transaction()->get_command())); )
  // legacy code, leave out _getMasterAccessHandle and _getPhase !!
  peq_pair *atom = new peq_pair(_getMasterAccessHandle(tc), _getPhase(tc)); // TODO: Memory Leak, delete after used
  sc_core::sc_time t = sc_core::SC_ZERO_TIME;
  m_receive_peq.notify(*atom, t);
}

TestMaster::peq_pair* TestMaster::wait_for_next_atom() {
  peq_pair *atom = NULL;
  atom = m_receive_peq.get_next_transaction();
  if (!atom) {
    wait(m_receive_peq.get_event()); // wait for BEGIN_RESP (END_RESP/completed will be sent by nb_transport_bw)
    atom = m_receive_peq.get_next_transaction();
  }
  GS_DUMP("got "<< atom->second.to_string()<<" "<<
          gs::tlm_command_writer::to_string(atom->first->get_tlm_transaction()->get_command()) <<
          " out of local peq");
  return atom;
}
