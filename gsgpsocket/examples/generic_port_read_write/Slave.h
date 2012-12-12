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
// use #define WRITE_RESPONSE_SEND to make the slave send a write response
//


//#define USE_SETSDATA_TO_WRITE_DATA
//#define WRITE_RESPONSE_SEND

#include <gsgpsocket/apis/GenericSlavePortReadWrite.h>

#define DATA_CHUNK_BYTE_SIZE 8

#include <boost/config.hpp> // needed for SystemC 2.1
#include <systemc>
#include <list>
using std::list;

using namespace gs;
using namespace gs::gp;

#include <iomanip>

#define DELAY(x) 10*x, sc_core::SC_NS


class SlaveMem
  : public sc_core::sc_module
  , public GenericSlavePortReadWrite
{
public:
  
  unsigned char MEM[MEMSIZE];

  bool inWrite;
  int m_delay;

  MAddr m_base, m_high;

  // implement read and write callbacks for GenericSlavePortReadWrite
  void on_read(const unsigned& addr, GSDataType& readval, const unsigned& burst_length, const unsigned master_id);
  void on_write(const unsigned& addr, const GSDataType& writeval, const unsigned& burst_length, const unsigned master_id);

  // tlm_slave_if implementation
  virtual void setAddress(MAddr  base, MAddr  high);
  virtual void getAddress(MAddr& base, MAddr& high);

    
  /**
   * Constructor. 
   * @param name_ Module name.
   * @param delay_ Access delay in cycles.
   */
  SlaveMem(sc_core::sc_module_name name_, int delay_) :
    sc_module(name_),
    GenericSlavePortReadWrite("target_port"),
    inWrite(false),
    m_delay(delay_),
    m_base(0),
    m_high(0)
  {
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

  virtual ~SlaveMem() {}

};


void SlaveMem::on_read(const unsigned& addr, GSDataType& readval, const unsigned& burst_length, const unsigned master_id)
{
    GS_DUMP("processing (blocking or nb) READ: burstlength="<< burst_length
            <<", local addr=0x"<<(std::hex)<< (gs_uint64)(addr-m_base) <<(std::dec)<<", data=");
    std::cout << "         stored data to transmit: ";
    for (unsigned int i=0; i<burst_length; i++) {
      std::cout << (unsigned int)MEM[i+(addr-m_base)] << " ";
    } std::cout << std::endl;
#ifdef USE_SETSDATA_TO_WRITE_DATA
    /*GSDataType tmp(t->getMBurstLength());
    for (unsigned int i=0; i<t->getMBurstLength(); i++) {
      tmp[i] = MEM[i+(t->getMAddr()-m_base)];
    } 
     faster:  */
    GSDataType::dtype tmpd(&MEM[(addr-m_base)], burst_length); // pesudo-vector without own vector data (but pointer to mem)
    GSDataType tmp(tmpd); // data object getting pointer to the pseudo-vector
    
    t->setSData(tmp);
#else
    for (unsigned int i=0; i<burst_length; i++) {
      readval[i] = MEM[i+(addr-m_base)];
    }
#endif
}


void SlaveMem::on_write(const unsigned& addr, const GSDataType& writeval, const unsigned& burst_length, const unsigned master_id)
{
    GS_DUMP("processing (blocking or nb) WRITE: burstlength="<<burst_length
            <<", local addr=0x"<<(std::hex)<< (gs_uint64)(addr-m_base) <<(std::dec)<<", data=");
    GSDataType my_data;
    my_data.set(writeval);
    std::cout << "         ";
    for (unsigned int i=0; i<burst_length; i++) {
      std::cout << (unsigned int)my_data[i] << " ";
      MEM[i+(addr-m_base)]=my_data[i];
    } std::cout << std::endl;
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
