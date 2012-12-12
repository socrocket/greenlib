// LICENSETEXT
// 
//   Copyright (C) 2007-2008 : GreenSocs Ltd
//       http://www.greensocs.com/ , email: info@greensocs.com
// 
//   Developed by :
// 
//   Wolfgang Klingauf, Robert Guenzel, Christian Schröder
//     Technical University of Braunschweig, Dept. E.I.S.
//     http://www.eis.cs.tu-bs.de
// 
//   Mark Burton, Marcus Bartholomeu
//     GreenSocs Ltd
// 
// 
// The contents of this file are subject to the licensing terms specified
// in the file LICENSE. Please consult this file for restrictions and
// limitations that may apply.
// 
// ENDLICENSETEXT

// This file is based on the PLBMaster.h (see GreenBus example 'plb')


//
// This GenericProtocol master first sends multiple (=loops) write 
// transactions to the address init_port.target_addr and increases 
// address automatically regarding the burst length of one transaction
// (=burst_length).
// Afterwards the same data are read again.
//
//
// Variable master device 
//  can be configured to different behaviour:
//
// use #define USE_PV to send PV transaction (Transact)
// use #define USE_CC to use multiple data phases (with BytesValid) 
//                    (only if not USE_PV)
// use #define WRITE_RESPONSE_NEEDED to make the master await a response
//                    phase after all write data phases have been finished.
//                    TODO: As long as automatism is not imnplemented, make 
//                          sure that the GP slave will send the response
//                          (e.g. use Slave.h and define WRITE_RESPONSE_SEND)
//                          When being connected to an OSCI device, the GSGP
//                          Socket does already automatically create the phase!
// use #define USE_GPSOCKET if this module should use a GenericProtocol Socket
//                    (based on the GreenSocket). If not defined the module uses
//                    the (old) GreenBus.
// use #define USE_BLOCKING_API to define if this module uses the blocking API
//

//#define USE_PV
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


#include <boost/config.hpp> // needed for SystemC 2.1
#include <systemc>
#include <iostream>

#ifdef USE_GPSOCKET
# ifdef USE_BLOCKING_API
#  include "gsgpsocket/transport/GSGPMasterBlockingSocket.h"
# else
#  include "gsgpsocket/transport/GSGPMasterSocket.h"
# endif
  using namespace gs;
  using namespace gs::gp;
#else
# include "greenbus/transport/GP/GP.h"
  using namespace tlm;
# ifndef USE_STATIC_CASTS
#   warning use static casts!!
# endif
#endif
//#include "greencontrol/config.h"
#include "tlm_utils/peq_with_get.h"           // Payload event queue FIFO



#include <iomanip>
#include "gs_dump.h"  // needed for GS_DUMP macro if not USE_GPSOCKET


class Master 
: public sc_core::sc_module,
#ifdef USE_GPSOCKET
  public payload_event_queue_output_if<master_atom>
#else
  public payload_event_queue_output_if<GS_ATOM>
#endif
{
public:
#ifdef USE_GPSOCKET
# ifdef USE_BLOCKING_API
  GenericMasterBlockingPort<32> init_port;
# else
  GenericMasterPort<32> init_port;
# endif
  typedef GenericMasterPort<32>::accessHandle accessHandle;
  typedef GenericMasterPort<32>::phase phase;
#else
# ifdef USE_BLOCKING_API
  GenericMasterBlockingPort init_port;
# else
  GenericMasterPort init_port;
# endif
  typedef GenericMasterPort::accessHandle accessHandle;
  typedef GenericMasterPort::phase phase;
#endif
  typedef pair<accessHandle, phase> peq_pair;
  tlm_utils::peq_with_get<peq_pair> m_receive_peq;
  
  unsigned char mem[MEMSIZE];

  void sendPV(accessHandle );
  void sendPVT(accessHandle );
    
  void main_action();
  void perform_writes();
  void perform_reads();
  
  peq_pair* wait_for_next_atom();

#ifdef USE_GPSOCKET
  virtual void notify (master_atom& tc);
#else
  virtual void notify (GS_ATOM& tc);
#endif

  virtual void end_of_simulation();
  
  GSDataType mdata;
  
  // configurable parameters
  gs::gs_param<gs_uint32> burst_length; // burst length in byte
  gs::gs_param<gs_uint32> initial_delay; // wait this number of clock cycles initially before start of operation
  gs::gs_param<gs_uint32> loops; // how many transactions shall I generate?
  //gs_uint32 burst_length;
  //gs_uint32 initial_delay;
  //gs_uint32 loops;
  
public:
  // Constructor
  SC_HAS_PROCESS(Master);
  Master(sc_core::sc_module_name name_) //, unsigned long long targetAddress, const char* data, bool rNw) 
  : sc_core::sc_module(name_)
  , init_port("iport")
  , m_receive_peq("receive_peq")
  , mdata(20)
  , burst_length("burst_length", 20)
  , initial_delay("initial_delay", 10)
  , loops("loops", 5)
  {
    SC_THREAD(main_action);

#ifdef USE_BLOCKING_API
    // bind blocking in interface
    init_port.out_port(*this);
#else
    // bind PEQ in interface
    init_port.peq.out_port(*this);
#endif
    
#ifdef USE_GPSOCKET
    // Configure the socket
    GSGPSocketConfig cnf;
#  ifdef WRITE_RESPONSE_NEEDED
    cnf.use_wr_resp = true;
#  else
    cnf.use_wr_resp = false;
#  endif
    init_port.set_config(cnf);
#endif
  }
  
  ~Master(){
  }
};

// ----------- main action -----------------------------------------
void Master::main_action() {
  wait (initial_delay*CLK_CYCLE, sc_core::SC_NS);
  perform_writes();
  mdata.getData().resize(burst_length, 0); // reset data
  wait (initial_delay*CLK_CYCLE, sc_core::SC_NS);
  std::cout << std::endl << "----------------------------------------------------------" << std::endl << std::endl;
  perform_reads();
}

// ----------- WRITE action -----------------------------------------
void Master::perform_writes() {
  GS_DUMP("write mode");
  
  unsigned char data_cnt = 0;
  gs_uint64 addr;
  unsigned num_loops = 0;

  // fill in the write data
  if (mdata.getSize()<burst_length)
    mdata.getData().resize(burst_length);
  

#ifdef USE_PV

  accessHandle tah = init_port.create_transaction();
  tah->setMCmd(Generic_MCMD_WR);
  tah->setMData(mdata);
  tah->setMBurstLength(burst_length.getValue());

  while(1) {
    std::cout << std::endl;

    // fill in new write data
    for (unsigned int i=0; i<burst_length; i++){
      mdata[i]=data_cnt++;
    }

#ifdef USE_GPSOCKET
    addr = init_port.target_addr; addr += (num_loops*burst_length);
#else
    addr = init_port.target_addr.value; addr += (num_loops*burst_length);
#endif
    tah->setMAddr( addr );

    std::cout << "(" << name() << "): data to send: "; for (unsigned int a = 0; a < burst_length; a++) std::cout << (unsigned int) mdata[a] << " "; std::cout << std::endl;
    GS_DUMP("Master send blocking.");
    init_port.Transact(tah);

    if (loops!=0) {
      num_loops++;
      if (num_loops==loops) break;
    }
  }

#else // if not USE_PV

  peq_pair *atom;
  phase ph;
  unsigned bvalid;

  while (1) {
    wait(sc_core::SC_ZERO_TIME); // to beautify output
    std::cout << std::endl;

    // make sure no transaction outstanding
    assert(m_receive_peq.get_next_transaction() == NULL);
    bvalid=0;

    // fill in new write data
    for (unsigned int i=0; i<burst_length; i++){
      mdata[i]=data_cnt++;
    }

    accessHandle tah = init_port.create_transaction();
    tah->setMCmd(Generic_MCMD_WR);
#ifdef USE_GPSOCKET
    addr = init_port.target_addr; addr += (num_loops*burst_length);
#else
    addr = init_port.target_addr.value; addr += (num_loops*burst_length);
#endif
    tah->setMAddr( addr );
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

    if (ph.state == GenericPhase::RequestAccepted) {
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
#ifdef USE_GPSOCKET // changed because of conceptual bug
      ph.setBytesValid( bvalid );
#else
      tah->setMSBytesValid(bvalid);
#endif
      GS_DUMP("Master send data (DataValid).");
#ifdef USE_BLOCKING_API
# ifdef USE_GPSOCKET
      init_port.SendData.block(tah, ph);
# else
      init_port.SendData.block(tah, ph, ph); // this is different because of a bug in GreenBus
# endif
#else
      init_port.SendData(tah,ph);
      atom = wait_for_next_atom();
      tah = atom->first; ph = atom->second;
#endif
      if (ph.state == GenericPhase::DataAccepted) {
        GS_DUMP("Slave accepted the data (DataAccepted).");
      }
      else if (ph.state == GenericPhase::DataError) {
        GS_DUMP("Oh no, data error.");
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
    if(ph.state == GenericPhase::ResponseValid) {
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
    init_port.AckResponse(tah,ph);
#endif

# ifdef USE_GPSOCKET
    init_port.release_transaction(tah);
# endif
    
    if (loops!=0) {
      num_loops++;
      if (num_loops==loops) break;
    }
  }
#endif // end if USE_PV
}

// ----------- READ action -----------------------------------------
void Master::perform_reads() {
  GS_DUMP("read mode");

#ifndef USE_PV
  peq_pair *atom;
  phase ph;
#endif
  gs_uint64 addr;
  
  unsigned num_loops = 0;
  while(true) {
    wait(sc_core::SC_ZERO_TIME); // to beautify output
    std::cout << std::endl;
    // make sure no transaction outstanding
    assert(m_receive_peq.get_next_transaction() == NULL);

    accessHandle tah = init_port.create_transaction();
    
    if (mdata.getSize()<burst_length)
      mdata.getData().resize(burst_length);
    
    tah->setMCmd(Generic_MCMD_RD);
#ifdef USE_GPSOCKET
    addr = init_port.target_addr; addr += (num_loops*burst_length);
#else
    addr = init_port.target_addr.value; addr += (num_loops*burst_length);
#endif
    tah->setMAddr( addr );
    tah->setMBurstLength(burst_length.getValue());
    tah->setMData(mdata);
    
#ifdef USE_PV

    GS_DUMP("Master send blocking.");
    init_port.Transact(tah);
    std::cout << "(" << name() << "): data received: "; for (unsigned int a = 0; a < tah->getMBurstLength(); a++) std::cout << (unsigned int) tah->getMData()[a] << " "; std::cout << std::endl;

#else // if not USE_PV
    
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
      if(ph.state==GenericPhase::ResponseValid){
        GS_DUMP("Slave sent data.");
#ifdef USE_GPSOCKET
        GS_DUMP("data valid: 0x"<<(std::hex)<< (gs_uint64)ph.getBytesValid() <<(std::dec)
                <<" (32bit=" << (gs_uint32)ph.getBytesValid() << ")");
#else
        GS_DUMP("data valid: 0x"<<(std::hex)<< (gs_uint64)tah->getMSBytesValid() <<(std::dec)
                <<" (32bit=" << (gs_uint32)tah->getMSBytesValid() << ")");
#endif
      }
      else {
        std::stringstream ss; ss << "wrong phase: "; IF_GPSOCKET( ss << ph.to_string(); )
        SC_REPORT_ERROR(name(),ss.str().c_str());
      }
      init_port.AckResponse(tah, ph);
    } 
# ifdef USE_GPSOCKET
    while (ph.getBytesValid() < tah->getMBurstLength());
# else
    while (tah->getMSBytesValid() < tah->getMBurstLength());
# endif
    IF_GPSOCKET( std::cout << "(" << name() << "): data received: "; for (unsigned int a = 0; a < tah->getMBurstLength(); a++) std::cout << (unsigned int) tah->getMData()[a] << " "; std::cout << std::endl; )
#endif // end if USE_PV
    
# ifdef USE_GPSOCKET
    init_port.release_transaction(tah);
# endif
    
    if (loops!=0) {
      num_loops++;
      if (num_loops==loops) break;
    }
  } // end while
}

// ----------- others -----------------------------------------
void Master::end_of_simulation() {
}


// ----------- notify -----------------------------------------
#ifdef USE_GPSOCKET
void Master::notify(master_atom &tc) {
#else
void Master::notify(GS_ATOM &tc) {
#endif
  IF_GPSOCKET( GS_DUMP("non-blocking notify "<< tc.second.to_string()<<", "<< gs::tlm_command_writer::to_string(tc.first.get_tlm_transaction()->get_command())); )
  // legacy code, leave out _getMasterAccessHandle and _getPhase !!
  peq_pair *atom = new peq_pair(_getMasterAccessHandle(tc), _getPhase(tc)); // TODO: Memory Leak, delete after used
  sc_core::sc_time t = sc_core::SC_ZERO_TIME;
  m_receive_peq.notify(*atom, t);
}

Master::peq_pair* Master::wait_for_next_atom() {
  peq_pair *atom = NULL;
  atom = m_receive_peq.get_next_transaction();
  if (!atom) {
    wait(m_receive_peq.get_event()); // wait for BEGIN_RESP (END_RESP/completed will be sent by nb_transport_bw)
    atom = m_receive_peq.get_next_transaction();
  }
#ifdef USE_GPSOCKET
  GS_DUMP("got "<< atom->second.to_string()<<" "<<
          gs::tlm_command_writer::to_string(atom->first->get_tlm_transaction()->get_command()) <<
          " out of local peq");
#else
  GS_DUMP("got " << atom->second.toString() << " out of local peq");
#endif
  return atom;
}
