//   GreenSocket
//
// LICENSETEXT
//
//   Copyright (C) 2007 : GreenSocs Ltd
// 	 http://www.greensocs.com/ , email: info@greensocs.com
//
//   Developed by :
//   
//   Robert Guenzel <guenzel@eis.cs.tu-bs.de>,
//     Technical University of Braunschweig, Dept. E.I.S.
//     http://www.eis.cs.tu-bs.de
//
//
// The contents of this file are subject to the licensing terms specified
// in the file LICENSE. Please consult this file for restrictions and
// limitations that may apply.
// 
// ENDLICENSETEXT

#ifndef __GREEN_target_socket_base_H__
#define __GREEN_target_socket_base_H__

#include <vector>
#include "greensocket/generic/gs_extension.h"
#include "greensocket/generic/gs_callbacks.h"
#include "greensocket/generic/green_target_socket_callback_base.h"
#include "greensocket/generic/green_socket_bind_checker.h"
#include "greensocket/generic/green_socket_timing_support_base.h"

namespace gs{
namespace socket{
//This is the green_target_socket from which other target_sockets
// shall be derived
template <unsigned int BUSWIDTH=32,
          typename TRAITS=tlm::tlm_base_protocol_types,
          unsigned int N=0,
          typename BIND_BASE=bind_checker<TRAITS>
          >
class target_socket_base: 
                             public target_socket_callback_base<BUSWIDTH,TRAITS,N, typename BIND_BASE::root_type, typename BIND_BASE::multi_root_type>
                            ,public BIND_BASE
                            //,public BIND_BASE::ext_support_type
	                        ,public target_timing_support_base<TRAITS,BIND_BASE>
{
public:
//typedef section
  typedef TRAITS                                                       traits_type;
  typedef target_socket_callback_base<BUSWIDTH,TRAITS,N, typename BIND_BASE::root_type, typename BIND_BASE::multi_root_type>                 base_type;
  typedef typename base_type::base_initiator_socket_type          base_initiator_socket_type;
  typedef typename TRAITS::tlm_payload_type                       payload_type;
  typedef typename TRAITS::tlm_phase_type                         phase_type;
  typedef tlm::tlm_sync_enum                                      sync_enum_type;
  typedef BIND_BASE bind_checker_type;
  //typedef typename BIND_BASE::ext_support_type             ext_support_type;

public:

  //the constructor requires a name
  target_socket_base(const char* name); 
   
protected:  

  //ATTENTION: derived socket should not implement end_of_elaboration
  // if they do, they have to call this end_of_elaboration before anything else
  void end_of_elaboration();
  
  //this function is called when we were successfully bound to someone
  // override it to react to the resolved config. Call this function from the derived class as well
  virtual void bound_to(const std::string&, typename BIND_BASE::bind_base_type*, unsigned int);
  
  //override this function when deriving from the greensocket
  virtual const std::string& get_type_string();

  const static std::string s_kind;
  bool base_eoe_done; //this bool is used by some dervided sockets (legacy code) it could be removed but we don't wanna risk loosing the legacy code
};

} //end ns socket
} //end ns gs

#include "greensocket/target/green_target_socket_base.tpp"

#endif
