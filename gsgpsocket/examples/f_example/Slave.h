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
//


//#define USE_SETSDATA_TO_WRITE_DATA
//#define USE_CC, see Master.h
//#define WRITE_RESPONSE_SEND


#define DATA_CHUNK_BYTE_SIZE 8

#include <boost/config.hpp> // needed for SystemC 2.1
#include <systemc>
#include <list>
using std::list;

#include "gsgpsocket/transport/GSGPSlaveSocket.h"

using namespace gs;
using namespace gs::gp;

#include <iomanip>

#define DELAY(x) 10*x, sc_core::SC_NS


class SlaveMem 
: public sc_core::sc_module, 
  public tlm_f_if<GenericSlaveAccessHandle> // = f_transact(tr)
{
public:
  GenericSlavePort<32> target_port;
  typedef GenericSlavePort<32>::accessHandle accessHandle;
  typedef GenericSlavePort<32>::phase phase;
  
  unsigned char MEM[MEMSIZE];

  bool inWrite;
  int m_delay;

  MAddr m_base, m_high;

  // SlaveMem functions 
  void f_transact( GenericSlaveAccessHandle t);

  void PVTProcess(accessHandle, phase);  
  int IPmodel( GenericSlaveAccessHandle t);

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
    //target_port.bind_b_if(*this);    
    target_port.bind_f_if(*this);    
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


void SlaveMem::f_transact( GenericSlaveAccessHandle t)
{
  GS_DUMP("blocking f_transact");
  (void) IPmodel(_getSlaveAccessHandle(t)); // we dont care about timing
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
          p.setBytesValid( bvalid );
          GS_DUMP("sending data chunk now (delay " << del << ").");
#else // if not USE_CC
          bvalid = tah->getMBurstLength();
          p.setBytesValid( bvalid );
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
      GS_DUMP("data valid: 0x" << (std::hex) << (gs_uint64)p.getBytesValid() << (std::dec) 
              << " (32bit="<< (gs_uint32)p.getBytesValid() <<")");
      
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
      std::stringstream ss; ss << "error: phase "; ss << p.to_string();
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
