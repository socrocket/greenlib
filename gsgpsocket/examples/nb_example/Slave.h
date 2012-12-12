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

// This file is based on the PLBSlave.h (see GreenBus example 'plb')


//
// GenericProtocol Slave with variable behaviour:
//
// use #define USE_SETSDATA_TO_WRITE_DATA to make the slave use 'tr.setSData(data)'
//                                        instead of 'tr.getMData()[i] = DataChunk'
// 
// use #define USE_CC   (also see Master.h) to send multiple responses during a write 
//                      transaction (using BytesValid)
// use #define DATA_CHUNK_BYTE_SIZE 8  to set the size of the CC data chunks
//
// use #define WRITE_RESPONSE_SEND to make the slave send a write response
//
// use #define USE_GPSOCKET if this module should use a GenericProtocol Socket
//                    (based on the GreenSocket). If not defined the module uses
//                    the (old) GreenBus.
// use #define USE_BLOCKING_API to define if this module uses the blocking API
//                              (only for compile check, does nothing)
//


//#define USE_SETSDATA_TO_WRITE_DATA
//#define USE_CC, see Master.h
//#define USE_GPSOCKET // should be set by compiler flag
//#define WRITE_RESPONSE_SEND
//#define USE_BLOCKING_API // only for compile check, does nothing


#define DATA_CHUNK_BYTE_SIZE 8

#include <boost/config.hpp> // needed for SystemC 2.1
#include <systemc>
#include <list>
using std::list;

#ifdef USE_GPSOCKET
# ifdef USE_BLOCKING_API
#  include "gsgpsocket/transport/GSGPSlaveBlockingSocket.h"
# else
#  include "gsgpsocket/transport/GSGPSlaveSocket.h"
# endif
  using namespace gs;
  using namespace gs::gp;
#else
# include "greenbus/transport/GP/GP.h"
  using namespace tlm;
#endif

#include <iomanip>
#include "gs_dump.h"  // needed for GS_DUMP macro if not USE_GPSOCKET

#define DELAY(x) 10*x, sc_core::SC_NS


class SlaveMem 
: public sc_core::sc_module, 
#ifdef USE_GPSOCKET
  public tlm_b_if<GenericSlaveAccessHandle>, // = b_transact(tr)
  public payload_event_queue_output_if<slave_atom>  // = notify(atom)
#else
  public tlm_b_if<GenericTransactionHandle>,
  public payload_event_queue_output_if<GS_ATOM>  
#endif
{
public:
#ifdef USE_GPSOCKET
# ifdef USE_BLOCKING_API
  GenericSlaveBlockingPort<32> target_port;
# else
  GenericSlavePort<32> target_port;
# endif
  typedef GenericSlavePort<32>::accessHandle accessHandle;
  typedef GenericSlavePort<32>::phase phase;
#else
# ifdef USE_BLOCKING_API
  GenericSlaveBlockingPort target_port;
# else
  GenericSlavePort target_port;
# endif
  typedef GenericSlavePort::accessHandle accessHandle;
  typedef GenericSlavePort::phase phase;
#endif
  
  unsigned char MEM[MEMSIZE];

  bool inWrite;
  int m_delay;

  MAddr m_base, m_high;

  // SlaveMem functions 
#ifdef USE_GPSOCKET
  // tlm_b_if implementation
  void b_transact(GenericSlaveAccessHandle);
  // payload_event_queue_output_if implementation
  void notify(slave_atom&);
#else
  // tlm_b_if implementation
  void b_transact(GenericTransactionHandle);
  // payload_event_queue_output_if implementation
  void notify(GS_ATOM&);
#endif
  void PVTProcess(accessHandle, phase);  
  int IPmodel( GenericSlaveAccessHandle t);

#ifdef USE_BLOCKING_API
  // SC_METHOD is a dummy to compile blocking call for test
  void blocking_response_dummy();
#endif
  
  // tlm_slave_if implementation
  virtual void setAddress(MAddr  base, MAddr  high);
  virtual void getAddress(MAddr& base, MAddr& high);

    
  SC_HAS_PROCESS(SlaveMem);

  /**
   * Constructor. 
   * @param name_ Module name.
   * @param delay_ Access delay in cycles.
   */
  SlaveMem(sc_core::sc_module_name name_, int delay_) :
    sc_module(name_),
    target_port("tport"),
    inWrite(false),
    m_delay(delay_),
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
#ifdef USE_GPSOCKET
    // Configure the socket
    GSGPSocketConfig cnf;
#  ifdef WRITE_RESPONSE_SEND
    cnf.use_wr_resp = true;
#  else
    cnf.use_wr_resp = false;
#  endif
    target_port.set_config(cnf);
#endif
  }
};


#ifdef USE_GPSOCKET
void SlaveMem::b_transact( GenericSlaveAccessHandle t)
#else
void SlaveMem::b_transact( GenericTransactionHandle t)
#endif
{
  GS_DUMP("blocking b_transact");
  (void) IPmodel(_getSlaveAccessHandle(t)); // we dont care about timing
}


#ifdef USE_GPSOCKET
void SlaveMem::notify(slave_atom &tc)
#else
void SlaveMem::notify(GS_ATOM &tc)
#endif
{
  IF_GPSOCKET( GS_DUMP("non-blocking notify " << tc.second.to_string() << ", " << 
                       gs::tlm_command_writer::to_string(tc.first.get_tlm_transaction()->get_command())); )
  accessHandle tah=_getSlaveAccessHandle(tc);
  phase p=tc.second;
  PVTProcess(tah,p);
}

/* This method plays out the protocol. We can do this how we like, so long as we
 * stick to the generic protocol. We can hard wire in some number, ask the IP
 * block, be random, whatever. These numbers determin which "real" bus protocol
 * we are essencially modelling */
void SlaveMem::PVTProcess(  accessHandle tah,  phase p)
{
  switch (p.state) {
    
  case GenericPhase::RequestValid:
    {
      if (inWrite) {
        //keep writes in order !!!
        GS_DUMP("I am busy... error");
        target_port.ErrorRequest(tah, p, DELAY(1));
      } 
      else {
        target_port.AckRequest(tah, p, DELAY(1));
        
        if (tah->getMCmd() == Generic_MCMD_RD) {
          GS_DUMP("accepted READ cmd.");
          int del = IPmodel(tah);
          gs_uint64 bvalid;
#ifdef USE_CC
          bvalid = DATA_CHUNK_BYTE_SIZE; // first data chunk, others see handling of ResponseAck
          if (bvalid > tah->getMBurstLength())
            bvalid = tah->getMBurstLength();
#  ifdef USE_GPSOCKET
          p.setBytesValid( bvalid );
#  else
          tah->setMSBytesValid( bvalid );
#  endif
          GS_DUMP("sending data chunk now (delay " << del << ").");
#else // if not USE_CC
          bvalid = tah->getMBurstLength();
#  ifdef USE_GPSOCKET
          p.setBytesValid( bvalid );
#  else
          tah->setMSBytesValid( bvalid );
#  endif
          GS_DUMP("sending data now (delay " << del << ").");
#endif
          tah->setSResp(Generic_SRESP_DVA);
          target_port.Response(tah, p, DELAY(del));
        } else {
          GS_DUMP("accepted WRITE cmd (in 1 cycle), now waiting for data.");
          inWrite=true;
        }
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
#ifdef USE_GPSOCKET
      GS_DUMP("data valid: 0x" << (std::hex) << (gs_uint64)p.getBytesValid() << (std::dec) 
              << " (32bit="<< (gs_uint32)p.getBytesValid() <<")");
#else
      GS_DUMP("data valid: 0x" << (std::hex) << (gs_uint64)tah->getMSBytesValid() << (std::dec)
              << " (32bit="<< (gs_uint32)tah->getMSBytesValid() <<")");
#endif
      
#ifdef USE_GPSOCKET
      if (tah->getMBurstLength() > MEMSIZE) {
        // send DataError
        target_port.ErrorData(tah,p,DELAY(p.getBytesValid()-1));
        break;
      }
      if (p.getBytesValid() < tah->getMBurstLength()) {
        GS_DUMP("ack data chunk (0x" <<(std::hex)<< (gs_uint64)p.getBytesValid() <<(std::dec)
                << " bytes valid) (not yet all bytes valid).");
        // send DataAck
        target_port.AckData(tah,p,DELAY(p.getBytesValid()-1));
        break;
      }
#else
      if (tah->getMBurstLength() > MEMSIZE) {
        // send DataError
        target_port.ErrorData(tah,p,DELAY(tah->getMSBytesValid()-1));
        break;
      }
      if (tah->getMSBytesValid() < tah->getMBurstLength()) {
        GS_DUMP("ack data chunk (0x" <<(hex)<< (gs_uint64)tah->getMSBytesValid() <<(dec)
                <<" bytes valid) (not yet all bytes valid).");
        // send DataAck
        target_port.AckData(tah,p,DELAY(tah->getMSBytesValid()-1));
        break;
      }
#endif
      
      GSDataType my_data;
      my_data.set(tah->getMData());
      GS_DUMP("Got " << (unsigned)tah->getMBurstLength() <<" data bytes sent to addr=0x" <<(std::hex)
              <<(gs_uint64)tah->getMAddr()<<" (local addr=0x"<<(gs_uint64)(tah->getMAddr()-m_base) <<(std::dec)<<"):");
      for (unsigned int i=0; i<tah->getMBurstLength(); i++){
        MEM[i+(tah->getMAddr()-m_base)]=my_data[i];
      }
#ifdef USE_GPSOCKET
      GSDataType::dtype* vec = &my_data.getData();
#else
      std::vector<gs_uint8>* vec = &my_data.getData();
#endif
      assert(vec->size() >= tah->getMBurstLength()); std::cout << "            ";
      for (unsigned int i = 0; i<tah->getMBurstLength(); i++) { std::cout << (unsigned int)vec->at(i) << " "; } std::cout << std::endl;

      // send DataAck
      target_port.AckData(tah,p,DELAY(tah->getMBurstLength()-1));
      
#ifdef WRITE_RESPONSE_SEND
      // send write response
      tah->setSResp(Generic_SRESP_DVA);
      target_port.Response(tah,p,DELAY(tah->getMBurstLength()-1));
#endif
      
      inWrite =false;
    }
    break;
    
  case GenericPhase::ResponseAccepted:
    {
      GS_DUMP("master sent ResponseAccepted.");

#ifdef USE_CC
      if (tah->getMCmd() == Generic_MCMD_RD) {
#  ifdef USE_GPSOCKET
        gs_uint64 bvalid = p.getBytesValid();
#  else
        gs_uint64 bvalid = tah->getMSBytesValid();
#  endif
        // send next data chunk if there is one left to send
        if (bvalid < tah->getMBurstLength()) {
          bvalid += DATA_CHUNK_BYTE_SIZE; // increment data chunk
          if (bvalid > tah->getMBurstLength()) 
            bvalid = tah->getMBurstLength();
#  ifdef USE_GPSOCKET
          p.setBytesValid( bvalid );
#  else
          tah->setMSBytesValid( bvalid );
#  endif
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
      std::stringstream ss; ss << "error: phase "; IF_GPSOCKET( ss << p.to_string(); )
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

int SlaveMem::IPmodel( GenericSlaveAccessHandle t)
{
  if (t->getMCmd()==Generic_MCMD_RD){
    GS_DUMP("processing (blocking or nb) READ: burstlength="<<(unsigned)t->getMBurstLength()
            <<", local addr=0x"<<(std::hex)<< (gs_uint64)(t->getMAddr()-m_base) <<(std::dec)<<", data=");
    std::cout << "         stored data to transmit: ";
    for (unsigned int i=0; i<t->getMBurstLength(); i++) {
      std::cout << (unsigned int)MEM[i+(t->getMAddr()-m_base)] << " ";
    } std::cout << std::endl;
#ifdef USE_SETSDATA_TO_WRITE_DATA
    /*GSDataType tmp(t->getMBurstLength());
    for (unsigned int i=0; i<t->getMBurstLength(); i++) {
      tmp[i] = MEM[i+(t->getMAddr()-m_base)];
    } 
     faster:  */
    GSDataType::dtype tmpd(&MEM[(t->getMAddr()-m_base)], t->getMBurstLength()); // pesudo-vector without own vector data (but pointer to mem)
    GSDataType tmp(tmpd); // data object getting pointer to the pseudo-vector
    
    t->setSData(tmp);
#else
    for (unsigned int i=0; i<t->getMBurstLength(); i++) {
      t->getSData()[i] = MEM[i+(t->getMAddr()-m_base)];
    }
#endif
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



void SlaveMem:: setAddress(MAddr  base, MAddr  high) 
{
  target_port.base_addr =base;
  m_base=base;
  target_port.high_addr =high;
}

void SlaveMem:: getAddress(MAddr& base, MAddr& high)
{
  base =m_base;
  high =m_high;
}

#ifdef USE_BLOCKING_API
// dummy to compile the blocking call
void SlaveMem::blocking_response_dummy() {
  GenericPhase ph;
  accessHandle tah;
  tah->setSResp(Generic_SRESP_DVA);
  target_port.Response.block(tah, ph);
}
#endif
