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


#ifndef __GREEN_SOCKET_CONFIG_OSCI_CONFIG_H__
#define __GREEN_SOCKET_CONFIG_OSCI_CONFIG_H__

#include "greensocket/generic/green_socket_config.h"

namespace gs{
namespace socket{

//this class provides global access to a OSCI compliant
// GreenSocket config.
//It is used to check bindability when connected to a non
// GreenSocket TLM2 socket
template <typename TRAITS>
struct osci_config{

  static config<TRAITS>& get_cfg(){
    static gs::socket::osci_config<TRAITS> tmp;
    return tmp.m_cfg;
  }

  
private:
  osci_config(){
    //in the OSCI BP we need all the phases and we reject unkown extensions (and we know no extensions)
    //  ignorable extensions are neither mandatory, nor optional, nor rejected. They are ignorable.
    m_cfg.use_mandatory_phase(tlm::BEGIN_REQ);
    m_cfg.use_mandatory_phase(tlm::END_REQ);
    m_cfg.use_mandatory_phase(tlm::BEGIN_RESP);
    m_cfg.use_mandatory_phase(tlm::END_RESP);
    m_cfg.set_string_ptr(new std::string("plain_osci_socket"));
    m_cfg.treat_unknown_as_rejected();
    m_cfg.set_invalid(false);
  }
  config<TRAITS> m_cfg;
};

//If you have custom TRAITS class define a template specialization for it!

} //end ns socket
} //ens ns gs

#endif
