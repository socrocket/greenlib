// LICENSETEXT
// 
//   Copyright (C) 2007-2008 : GreenSocs Ltd
//       http://www.greensocs.com/ , email: info@greensocs.com
// 
//   Developed by :
// 
//   Wolfgang Klingauf, Robert Guenzel, Christian SchrÃ¶der
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


#ifndef GENERIC_MASTER_PORT_READ_WRITE_H
#define GENERIC_MASTER_PORT_READ_WRITE_H

#include <boost/config.hpp> // needed for SystemC 2.1
#include <systemc>

#include <gsgpsocket/transport/GSGPMasterSocket.h>
#include <tlm_utils/peq_with_get.h>


namespace gs {
namespace gp {


class GenericMasterPortReadWrite
  : public gs::payload_event_queue_output_if<gs::gp::GenericMasterPort<32>::atom>
{
public:
  gs::gp::GenericMasterPort<32> init_port;
  typedef gs::gp::GenericMasterPort<32>::accessHandle accessHandle;
  typedef gs::gp::GenericMasterPort<32>::phase phase;
  typedef gs::gp::GenericMasterPort<32>::atom atom;

private: // users should not need to access the peq directly
  tlm_utils::peq_with_get<atom> m_receive_peq;
  void notify (atom& tc);

public:


  // Methods available for this port
  atom* get_next_atom();
  atom* wait_for_next_atom();
  void init_read(const unsigned& addr, GSDataType& readval, const unsigned burst_length = 4);
  void init_write(const unsigned& addr, const GSDataType& writeval, const unsigned burst_length = 4);

  // Constructor
  GenericMasterPortReadWrite(sc_core::sc_module_name name)
    : init_port(name)
    , m_receive_peq("receive_peq")
  {
    // bind PEQ in interface
    init_port.peq.out_port(*this);
  }
  
};


// Implement payload_event_queue_output_if::notify
inline
void
GenericMasterPortReadWrite::notify(atom &tc)
{
  GS_DUMP_N("GenericMasterPortReadWrite::notify", "non-blocking notify "<< tc.second.to_string()<<", "<< gs::tlm_command_writer::to_string(tc.first.get_tlm_transaction()->get_command()));
  // legacy code, leave out _getMasterAccessHandle and _getPhase !!
  atom *atom_p = new atom(_getMasterAccessHandle(tc), _getPhase(tc)); // TODO: Memory Leak, delete after used
  sc_core::sc_time t = sc_core::SC_ZERO_TIME;
  m_receive_peq.notify(*atom_p, t);
}


inline
GenericMasterPortReadWrite::atom*
GenericMasterPortReadWrite::get_next_atom()
{
  atom *atom_p = m_receive_peq.get_next_transaction();
  if (atom_p) {
    GS_DUMP_N("GenericMasterPortReadWrite::get_next_atom", "got "<< atom_p->second.to_string()<<" "<<
            gs::tlm_command_writer::to_string(atom_p->first->get_tlm_transaction()->get_command()) <<
            " out of local peq");
  }
  return atom_p;
}


inline
GenericMasterPortReadWrite::atom*
GenericMasterPortReadWrite::wait_for_next_atom()
{
  atom *atom_p = get_next_atom();
  if (!atom_p) {
    sc_core::wait(m_receive_peq.get_event()); // wait for BEGIN_RESP (END_RESP/completed will be sent by nb_transport_bw)
    atom_p = get_next_atom();
  }
  return atom_p;
}


inline
void
GenericMasterPortReadWrite::init_read(const unsigned& addr, GSDataType& readval, const unsigned burst_length)
{
  if (readval.getSize()<burst_length)
    readval.getData().resize(burst_length);

  accessHandle tah = init_port.create_transaction();
  tah->setMCmd(Generic_MCMD_RD);
  tah->setMAddr(addr);
  tah->setMData(readval);
  tah->setMBurstLength(burst_length);
  init_port.Transact(tah);
  init_port.release_transaction(tah);
}


inline
void
GenericMasterPortReadWrite::init_write(const unsigned& addr, const GSDataType& writeval, const unsigned burst_length)
{
  accessHandle tah = init_port.create_transaction();
  tah->setMCmd(Generic_MCMD_WR);
  tah->setMAddr(addr);
  tah->setMData(writeval);
  tah->setMBurstLength(burst_length);
  init_port.Transact(tah);
  init_port.release_transaction(tah);
}


} // namespace gp
} // namespace gs


#endif // GENERIC_MASTER_PORT_READ_WRITE_H
