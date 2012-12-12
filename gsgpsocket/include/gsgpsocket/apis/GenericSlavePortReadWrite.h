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


#ifndef GENERIC_SLAVE_PORT_READ_WRITE_H
#define GENERIC_SLAVE_PORT_READ_WRITE_H

#include <boost/config.hpp> // needed for SystemC 2.1
#include <systemc>

#include <gsgpsocket/transport/GSGPSlaveSocket.h>


namespace gs {
namespace gp {


class GenericSlavePortReadWrite
  : public gs::tlm_f_if<gs::gp::GenericSlavePort<32>::accessHandle> // = f_transact(tr)
{
public:
  gs::gp::GenericSlavePort<32> target_port;
  typedef gs::gp::GenericSlavePort<32>::accessHandle accessHandle;

  // Implement tlm_f_if
  void f_transact(accessHandle t);

  // Abstract methods that must be implemented by the user
  virtual void on_read(const unsigned& addr, GSDataType& readval, const unsigned& burst_length, const unsigned master_id) = 0;
  virtual void on_write(const unsigned& addr, const GSDataType& writeval, const unsigned& burst_length, const unsigned master_id) = 0;

  // Constructor
  GenericSlavePortReadWrite(const char* name)
    : target_port(name)
  {
    target_port.bind_f_if(*this);    
  }
};


// Implement tlm_f_if to call on_read() or on_write() depending on the command from the transaction 

inline
void
GenericSlavePortReadWrite::f_transact(accessHandle t)
{
  if (t->getMCmd() == gs::Generic_MCMD_RD)
    on_read(t->getMAddr(), t->getSData(), t->getMBurstLength(), t->getMID());
  else if (t->getMCmd() == gs::Generic_MCMD_WR)
    on_write(t->getMAddr(), t->getMData(), t->getMBurstLength(), t->getMID());
  else
    SC_REPORT_ERROR(target_port.name(), "Invalid command.");
}


} // namespace gp
} // namespace gs


#endif // GENERIC_SLAVE_PORT_READ_WRITE_H
