// LICENSETEXT
//
//   Copyright (C) 2005 - 2008 : GreenSocs Ltd
//       http://www.greensocs.com/ , email: info.com
//
//   Developed by :
//
//  Robert Guenzel, Christian Schroeder
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

 
#ifndef __GREEN_INITIATOR_SOCKET_H__
#define __GREEN_INITIATOR_SOCKET_H__

#include "greensocket/initiator/green_initiator_socket_base.h"

namespace gs {
namespace socket{


//this version of the init socket just set the number of binding to 1
// which means exactly one binding is allowed
template <unsigned int BUSWIDTH=32,
          typename TRAITS=tlm::tlm_base_protocol_types,
          typename BIND_BASE=bind_checker<TRAITS>,
          typename MM_INTERFACE=tlm::tlm_mm_interface,
          typename EXT_FILLER=mm_base<TRAITS,MM_INTERFACE>,
          typename POOL=default_pool<TRAITS, EXT_FILLER>
          >
class initiator_socket: 
                             public initiator_socket_base<BUSWIDTH,TRAITS, 1, BIND_BASE, MM_INTERFACE, EXT_FILLER, POOL>
{
public:
  //constructor requires an name. The last argument is for future use and shall not be used.
  initiator_socket(const char* name, allocation_scheme alloc_scheme=GS_TXN_ONLY, EXT_FILLER* ext_filler=NULL)
    : initiator_socket_base<BUSWIDTH,TRAITS, 1, BIND_BASE, MM_INTERFACE, EXT_FILLER, POOL>(name, alloc_scheme, ext_filler)
  {
  }
  ~initiator_socket(){}
};

} //end ns socket
} //end ns gs

#endif
