// LICENSETEXT
// 
//   Copyright (C) 2007 : GreenSocs Ltd
//       http://www.greensocs.com/ , email: info@greensocs.com
// 
//   Developed by :
// 
//   Robert Guenzel
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

#ifndef __genericSignalBus_h__
#define __genericSignalBus_h__

#include "greenrouter/genericRouter.h"
#include "greenrouter/extensionAddressMap.h"
#include "greensignalsocket/green_signal.h"
namespace gs_generic_signal{
/*
typedef gs::gp::GenericRouter<32, 255, gs_generic_signal::gs_generic_signal_protocol_types, \
     tlm::tlm_response_status, tlm::TLM_ADDRESS_ERROR_RESPONSE, \
     &gs_generic_signal::gs_generic_signal_payload::set_response_status, gs::gp::ExtensionMap<gs_generic_signal::gs_generic_signal_protocol_types> >
     GenericSignalBus;
*/

template <unsigned int BUSWIDTH, unsigned int PORTMAX = 255>
class GenericSignalBus : public gs::gp::GenericRouter<BUSWIDTH, PORTMAX, gs_generic_signal::gs_generic_signal_protocol_types, \
           tlm::tlm_response_status, tlm::TLM_ADDRESS_ERROR_RESPONSE, &gs_generic_signal::gs_generic_signal_payload::set_response_status, \
           gs::gp::ExtensionMap<gs_generic_signal::gs_generic_signal_protocol_types> > {
  public:
    GenericSignalBus (const char* name) : gs::gp::GenericRouter<BUSWIDTH, PORTMAX, gs_generic_signal::gs_generic_signal_protocol_types, \
           tlm::tlm_response_status, tlm::TLM_ADDRESS_ERROR_RESPONSE, &gs_generic_signal::gs_generic_signal_payload::set_response_status, \
           gs::gp::ExtensionMap<gs_generic_signal::gs_generic_signal_protocol_types> > (name) {}
  virtual void bound_to_target(const std::string& other_type, gs::socket::bindability_base<gs_generic_signal::gs_generic_signal_protocol_types>* other, unsigned int index){
   return;
  }
  
  virtual void bound_to_initiator(const std::string& other_type, gs::socket::bindability_base<gs_generic_signal::gs_generic_signal_protocol_types>* other, unsigned int index){
    return;
  }
};
}
//TODO: FOLLOWING TYPEDEF AND CLASS DEFINITION SHOULD BE REMOVED IN THE NEXT
//RELEASE. IT IS KEPT ONLY TO LET THE OLD CODE WORK WITHOUT ANY CHANGE AT ALL.
namespace gs {
namespace gp {
/*
typedef gs::gp::GenericRouter<32, 255, gs_generic_signal::gs_generic_signal_protocol_types,
     tlm::tlm_response_status, tlm::TLM_ADDRESS_ERROR_RESPONSE,
     &gs_generic_signal::gs_generic_signal_payload::set_response_status, gs::gp::ExtensionMap<gs_generic_signal::gs_generic_signal_protocol_types> >
     GenericSignalBus;
*/
template <unsigned int BUSWIDTH, unsigned int PORTMAX = 255>
class GenericSignalBus : public GenericRouter<BUSWIDTH, PORTMAX, gs_generic_signal::gs_generic_signal_protocol_types, \
           tlm::tlm_response_status, tlm::TLM_ADDRESS_ERROR_RESPONSE, &gs_generic_signal::gs_generic_signal_payload::set_response_status, \
           gs::gp::ExtensionMap<gs_generic_signal::gs_generic_signal_protocol_types> > {
  public:
    GenericSignalBus (const char* name) : GenericRouter<BUSWIDTH, PORTMAX, gs_generic_signal::gs_generic_signal_protocol_types, \
           tlm::tlm_response_status, tlm::TLM_ADDRESS_ERROR_RESPONSE, &gs_generic_signal::gs_generic_signal_payload::set_response_status, \
           gs::gp::ExtensionMap<gs_generic_signal::gs_generic_signal_protocol_types> > (name) {}
};
}
}
#endif

