// GreenSocket
//
// LICENSETEXT
// 
//   Copyright (C) 2009 : GreenSocs Ltd
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

// moved and renamed from project GSGPSocket, file GSGPSocket.h

#ifndef __GREENSOCKETADDRESS_BASE_H__
#define __GREENSOCKETADDRESS_BASE_H__

#include "greencontrol/config.h"

namespace gs {
namespace socket {

  
// /////////////////////////////////////////////////////////////////////////////// //
/// Base class for Sockets (e.g. GSGPSocket), 
/// contains GreenConfig address parameters
class GreenSocketAddress_base
{
public:
  
  // Makes this dynamically castable
  virtual ~GreenSocketAddress_base() { }
  
  /// configurable base and high address of this socket, TODO: multi address ranges
  gs_param<gs_uint64> base_addr, high_addr;
#ifdef GS_SOCKET_ADDRESS_ARRAY
  gs_param<gs_uint16> num_address_range;
  gs_param<gs_uint64 *> parr_base_addr;
  gs_param<gs_uint64 *> parr_high_addr;
#endif
  
  GreenSocketAddress_base(std::string portname, gs_uint16 tot_addr_range = 0) 
  : base_addr(portname + ".base_addr")
  , high_addr(portname + ".high_addr")
#ifdef GS_SOCKET_ADDRESS_ARRAY
  , num_address_range(portname + ".num_address_range", tot_addr_range)
  , parr_base_addr(portname + ".parr_base_addr")
  , parr_high_addr(portname + ".parr_high_addr")
#endif
  { 
  }
  
};
  
} //end ns socket
} //end ns gs

#endif
