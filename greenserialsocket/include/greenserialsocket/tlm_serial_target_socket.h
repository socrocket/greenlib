///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// Copyright (C) 2005 - 2010 : GreenSocs Ltd
// http://www.greensocs.com/ , email: info.comy
//
//
//============================================================================
//      Project : tlm serial
//       Author : Manish Aggarwal, CircuitSutra Technologies(www.circuitsutra.com)
//			 $Id:
//
//  Description : This file defines the serial target bidirectional sockets
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __TLM_SERIAL_TARGET_SOCKET_H__
#define __TLM_SERIAL_TARGET_SOCKET_H__

#include "greensocket/target/green_target_socket_base.h"
#include "tlm_serial_protocol.h"
#include "tlm_serial_payload.h"

namespace tlm_serial
{

template<unsigned int NUMBINDS=1,
	typename BASE_SOCKET=gs::socket::target_socket_base<32,tlm_serial_protocol_types,
        NUMBINDS,gs::socket::bind_checker<tlm_serial_protocol_types> > >
class tlm_serial_target_socket:public BASE_SOCKET
{
public:
	typedef BASE_SOCKET base_type;
	typedef tlm_serial::tlm_serial_protocol_types traits_type;

	tlm_serial_target_socket(const char* name):gs::socket::target_socket_base<32,tlm_serial_protocol_types, NUMBINDS,
                                                     gs::socket::bind_checker<tlm_serial_protocol_types> >(name)
        {
          configure_socket();
        }

	~tlm_serial_target_socket()
        { }

protected:
	void configure_socket()
        {
            gs::socket::config<traits_type> cfg;
            cfg.treat_unknown_as_rejected();
            //the base protocol phases are always mandatory
            cfg.use_mandatory_phase(tlm_serial::BEG_TRANSMISSION);
            BASE_SOCKET::set_config(cfg);
        }
};

}

#endif /* __TLM_SERIAL_INITIATOR_SOCKET_H__ */
