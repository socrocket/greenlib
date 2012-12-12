//   GreenSocket
//
// LICENSETEXT
//
//   Copyright (C) 2008 : GreenSocs Ltd
// 	 http://www.greensocs.com/ , email: info@greensocs.com
//
//   Developed by :
//    Robert Guenzel <guenzel@eis.cs.tu-bs.de>, Christian Schroeder
//
//
// The contents of this file are subject to the licensing terms specified
// in the file LICENSE. Please consult this file for restrictions and
// limitations that may apply.
// 
// ENDLICENSETEXT


#ifndef __GREEN_BIDIRECTIONAL_SOCKET_H__
#define __GREEN_BIDIRECTIONAL_SOCKET_H__

#include "greensocket/bidirectional/green_bidirectional_socket_base.h"

namespace gs {
namespace socket {


// This is the green_bidirectional_socket from which other bidirectional_sockets
// shall be derived
template <unsigned int BUSWIDTH=32,
          typename TRAITS=tlm::tlm_base_protocol_types,
          typename BIND_BASE=bind_checker<TRAITS>,
          typename MM_INTERFACE=tlm::tlm_mm_interface,
          typename EXT_FILLER=mm_base<TRAITS,MM_INTERFACE>,
          typename POOL=default_pool<TRAITS, EXT_FILLER>
          >
class bidirectional_socket: 
                             public bidirectional_socket_base<BUSWIDTH,TRAITS, 1, BIND_BASE, MM_INTERFACE, EXT_FILLER, POOL>
{
public:
  //constructor requires an name. The last argument is for future use and shall not be used.
  bidirectional_socket(const char* name, allocation_scheme alloc_scheme=GS_TXN_ONLY, EXT_FILLER* ext_filler=NULL)
    : bidirectional_socket_base<BUSWIDTH,TRAITS, 1, BIND_BASE, MM_INTERFACE, EXT_FILLER, POOL>(name, alloc_scheme, ext_filler)
  {
  }
  ~bidirectional_socket(){}
};
  

} //end ns socket
} //end ns gs

#endif
