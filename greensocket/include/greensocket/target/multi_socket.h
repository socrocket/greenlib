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


#ifndef __GREEN_TARGET_MULTI_SOCKET_H__
#define __GREEN_TARGET_MULTI_SOCKET_H__

#include "greensocket/target/green_target_socket_base.h"

namespace gs{
namespace socket{
//This is the green_target_socket from which other target_sockets
// shall be derived
template <unsigned int BUSWIDTH=32,
          typename TRAITS=tlm::tlm_base_protocol_types,
          typename BIND_BASE=bind_checker<TRAITS>
          >
class target_multi_socket: 
                             public target_socket_base<BUSWIDTH,TRAITS,0,BIND_BASE>
{
public:
  //the constructor requires a name
  target_multi_socket(const char* name) : target_socket_base<BUSWIDTH,TRAITS,0,BIND_BASE>(name){}
};

} //end ns socket
} //end ns gs

#endif
