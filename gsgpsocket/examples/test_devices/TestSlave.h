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

// This file is based on the Slave.h (see GSGPSocket example 'nb_example')


//
// GenericProtocol Slave with variable behaviour:
//
// use #define USE_CC   (also see Master.h) to send multiple responses during a write 
//                      transaction (using BytesValid)
// use #define DATA_CHUNK_BYTE_SIZE 8  to set the size of the CC data chunks
//
// use #define WRITE_RESPONSE_SEND to make the slave send a write response
//
// use #define USE_BLOCKING_API to define if this module uses the blocking API
//                              (only for compile check, does nothing)
//


//
//
// This GreenSocs GenericProtocol (GSGP) slave responds the following
// transactions (counted with transaction_counter):
//
//     (Write commands)
//   1) receive normal write transaction
//   2) if write response is enabled: receive write transaction (master should send ResponseError) 
//      else: receive normal write transaction
//   3) receive write transaction and produce a RequestError
//   4) receive write transaction and produce a DataError
//     (Read commands)
//   5) receive normal read transaction
//   6) receive read transaction (master should send ResponseError)
//   7) receive read transaction and produce a RequestError
//   8) receive read transaction and produce a RequestError (only OSCI slaves will do a different error transmission here)
//
//


//#define USE_CC, see Master.h
#define DATA_CHUNK_BYTE_SIZE 8
//#define WRITE_RESPONSE_SEND
//#define USE_BLOCKING_API // only for compile check, does nothing


#include <list>
using std::list;

#ifdef USE_BLOCKING_API
# include "gsgpsocket/transport/GSGPSlaveBlockingSocket.h"
#else
# include "gsgpsocket/transport/GSGPSlaveSocket.h"
#endif
using namespace gs;
using namespace gs::gp;

#include <iomanip>

#define DELAY(x) 10*x, sc_core::SC_NS


class TestSlave 
: public sc_core::sc_module, 
  public tlm_b_if<GenericSlaveAccessHandle>, // = b_transact(tr)
  public payload_event_queue_output_if<slave_atom>  // = notify(atom)
{
public:
#ifdef USE_BLOCKING_API
  GenericSlaveBlockingPort<32> target_port;
#else
  GenericSlavePort<32> target_port;
#endif
  typedef GenericSlavePort<32>::accessHandle accessHandle;
  typedef GenericSlavePort<32>::phase phase;
  
  unsigned char MEM[MEMSIZE];

  bool inWrite;
  int m_delay;
  unsigned int transaction_counter;

  MAddr m_base, m_high;

  // tlm_b_if implementation
  void b_transact(GenericSlaveAccessHandle);
  // payload_event_queue_output_if implementation
  void notify(slave_atom&);
  void PVTProcess(accessHandle, phase);  
  int IPmodel( GenericSlaveAccessHandle t);

#ifdef USE_BLOCKING_API
  // SC_METHOD is a dummy to compile blocking call for test
  void blocking_response_dummy();
#endif
  
  // tlm_slave_if implementation
  virtual void setAddress(MAddr  base, MAddr  high);
  virtual void getAddress(MAddr& base, MAddr& high);

    
  SC_HAS_PROCESS(TestSlave);

  /**
   * Constructor. 
   * @param name_ Module name.
   * @param delay_ Access delay in cycles.
   */
  TestSlave(sc_core::sc_module_name name_, int delay_) :
    sc_core::sc_module(name_),
    target_port("tport"),
    inWrite(false),
    m_delay(delay_),
    transaction_counter(0),
    m_base(0),
    m_high(0)
  {
    target_port.bind_b_if(*this);    
#ifdef USE_BLOCKING_API
    target_port.out_port(*this);
    // dummy 
    SC_METHOD(blocking_response_dummy);
    dont_initialize();
#else
    target_port.peq.out_port(*this);
#endif
    GS_DUMP("memory size: " << MEMSIZE);
    // Configure the socket
    GSGPSocketConfig cnf;
#ifdef WRITE_RESPONSE_SEND
    cnf.use_wr_resp = true;
#else
    cnf.use_wr_resp = false;
#endif
    target_port.set_config(cnf);
  }
};


void TestSlave::b_transact( GenericSlaveAccessHandle t)
{
  GS_DUMP("blocking b_transact");
  (void) IPmodel(_getSlaveAccessHandle(t)); // we dont care about timing
}


void TestSlave::notify(slave_atom &tc)
{
  GS_DUMP("non-blocking notify " << tc.second.to_string() << ", " << 
                       gs::tlm_command_writer::to_string(tc.first.get_tlm_transaction()->get_command()));
  accessHandle tah=_getSlaveAccessHandle(tc);
  phase p=tc.second;
  PVTProcess(tah,p);
}

/* This method plays out the protocol. We can do this how we like, so long as we
 * stick to the generic protocol. We can hard wire in some number, ask the IP
 * block, be random, whatever. These numbers determin which "real" bus protocol
 * we are essencially modelling */
void TestSlave::PVTProcess(  accessHandle tah,  phase p)
{
  switch (p.state) {
    
  case GenericPhase::RequestValid:
    {
      transaction_counter++;
      assert(!inWrite);
      if (((transaction_counter-1)%4) == 2 || ((((transaction_counter-1)%4) == 3) && (tah->getMCmd() == Generic_MCMD_RD))){
        GS_DUMP("decline cmd");
        target_port.ErrorRequest(tah, p, DELAY(1));
        break;
      }
      else
        target_port.AckRequest(tah, p, DELAY(1));
      
      if (tah->getMCmd() == Generic_MCMD_RD) {
        GS_DUMP("accepted READ cmd.");
        int del = IPmodel(tah);
        gs_uint64 bvalid;
#ifdef USE_CC
        bvalid = DATA_CHUNK_BYTE_SIZE; // first data chunk, others see handling of ResponseAck
        if (bvalid > tah->getMBurstLength())
          bvalid = tah->getMBurstLength();
        p.setBytesValid( bvalid );
        GS_DUMP("sending data chunk now (delay " << del << ").");
#else // if not USE_CC
        bvalid = tah->getMBurstLength();
        p.setBytesValid( bvalid );
        GS_DUMP("sending data now (delay " << del << ").");
#endif
        tah->setSResp(Generic_SRESP_DVA);
        GS_DUMP("bvalid is "<<p.getBytesValid());
        target_port.Response(tah, p, DELAY(del));
      } else {
        GS_DUMP("accepted WRITE cmd (in 1 cycle), now waiting for data.");
        inWrite=true;
      }
    }
    break;
    
    
  case GenericPhase::DataAccepted:
    {
      GS_DUMP("master sent DataAccepted. Transaction finished OK.");
    }
    break;
    
    
  case GenericPhase::DataValid:
    {
      GS_DUMP("receiving data from master.");
      GS_DUMP("data valid: 0x" << (std::hex) << (gs_uint64)p.getBytesValid() << (std::dec) 
              << " (32bit="<< (gs_uint32)p.getBytesValid() <<")");
      
      assert(tah->getMBurstLength() <= MEMSIZE);
      if (p.getBytesValid() < tah->getMBurstLength()) {
        GS_DUMP("ack data chunk (0x" <<(std::hex)<< (gs_uint64)p.getBytesValid() <<(std::dec)
                << " bytes valid) (not yet all bytes valid).");
        target_port.AckData(tah,p,DELAY(p.getBytesValid()-1));
        break;
      }
      
      GSDataType my_data;
      my_data.set(tah->getMData());
      GS_DUMP("Got " << (unsigned)tah->getMBurstLength() <<" data bytes sent to addr=0x" <<(std::hex)
              <<(gs_uint64)tah->getMAddr()<<" (local addr=0x"<<(gs_uint64)(tah->getMAddr()-m_base) <<(std::dec)<<"):");
      for (unsigned int i=0; i<tah->getMBurstLength(); i++){
        MEM[i+(tah->getMAddr()-m_base)]=my_data[i];
      }
      GSDataType::dtype* vec = &my_data.getData();
      assert(vec->size() >= tah->getMBurstLength()); std::cout << "            ";
      for (unsigned int i = 0; i<tah->getMBurstLength(); i++) { std::cout << (unsigned int)vec->at(i) << " "; } std::cout << std::endl;

      if (((transaction_counter-1)%4) == 3) {
        GS_DUMP("send DataError");
        target_port.ErrorData(tah,p,DELAY(p.getBytesValid()-1));
        inWrite = false;
        break;
      }
      else
        target_port.AckData(tah,p,DELAY(tah->getMBurstLength()-1));
#ifdef WRITE_RESPONSE_SEND
      GS_DUMP("send DataResp");
      // send write response
      tah->setSResp(Generic_SRESP_DVA);
      target_port.Response(tah,p,DELAY(tah->getMBurstLength()-1));
#endif
      inWrite = false;
    }
    break;
    
  case GenericPhase::ResponseAccepted:
    {
      GS_DUMP("master sent ResponseAccepted.");
      GS_DUMP("bvalid is "<<p.getBytesValid());
#ifdef USE_CC
      if (tah->getMCmd() == Generic_MCMD_RD) {
        gs_uint64 bvalid = p.getBytesValid();
        // send next data chunk if there is one left to send
        if (bvalid < tah->getMBurstLength()) {
          bvalid += DATA_CHUNK_BYTE_SIZE; // increment data chunk
          if (bvalid > tah->getMBurstLength()) 
            bvalid = tah->getMBurstLength();
          p.setBytesValid( bvalid );
          GS_DUMP("sending data chunk now.");
          tah->setSResp(Generic_SRESP_DVA);
          target_port.Response(tah, p, DELAY(1));
        }
      }
#else // if not USE_CC
        // nothing needed to be done
#endif // end if USE_CC
     
    }
    break;
    
    
  default:
    {
      std::stringstream ss; ss << "error: phase " << p.to_string();
      SC_REPORT_WARNING(name(),ss.str().c_str());
    }
    break;
  }  
}
  


/* Here is what the user should end up writing
   a very simple model,
   to work in a 'co-war' like environment, where the model writer thinks about
   timing up front, then this model can return a timing number, or a strucutre
   of timing numbers if required */

int TestSlave::IPmodel( GenericSlaveAccessHandle t)
{
  if (t->getMCmd()==Generic_MCMD_RD){
    GS_DUMP("processing (blocking or nb) READ: burstlength="<<(unsigned)t->getMBurstLength()
            <<", local addr=0x"<<(std::hex)<< (gs_uint64)(t->getMAddr()-m_base) <<(std::dec)<<", data=");
    std::cout << "         stored data to transmit: ";
    for (unsigned int i=0; i<t->getMBurstLength(); i++) {
      std::cout << (unsigned int)MEM[i+(t->getMAddr()-m_base)] << " ";
    } std::cout << std::endl;
    for (unsigned int i=0; i<t->getMBurstLength(); i++) {
      t->getSData()[i] = MEM[i+(t->getMAddr()-m_base)];
    }
    return m_delay;
  }

  if (t->getMCmd()==Generic_MCMD_WR){
    GS_DUMP("processing (blocking or nb) WRITE: burstlength="<<(unsigned)t->getMBurstLength()
            <<", local addr=0x"<<(std::hex)<< (gs_uint64)(t->getMAddr()-m_base) <<(std::dec)<<", data=");
    GSDataType my_data;
    my_data.set(t->getMData());
    std::cout << "         ";
    for (unsigned int i=0; i<t->getMBurstLength(); i++) {
      std::cout << (unsigned int)my_data[i] << " ";
      MEM[i+(t->getMAddr()-m_base)]=my_data[i];
    } std::cout << std::endl;
    return m_delay*2;
  }

  return 0;

}



void TestSlave:: setAddress(MAddr  base, MAddr  high) 
{
  target_port.base_addr =base;
  m_base=base;
  target_port.high_addr =high;
}

void TestSlave:: getAddress(MAddr& base, MAddr& high)
{
  base =m_base;
  high =m_high;
}

#ifdef USE_BLOCKING_API
// dummy to compile the blocking call
void TestSlave::blocking_response_dummy() {
  GenericPhase ph;
  accessHandle tah;
  tah->setSResp(Generic_SRESP_DVA);
  target_port.Response.block(tah, ph);
}
#endif
