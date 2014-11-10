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
//  Description : This file defines the tlm serial protocol
//                
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __TLM_SERIAL_PROTOCOL_H__
#define __TLM_SERIAL_PROTOCOL_H__

#include "tlm_serial_payload.h"

namespace tlm_serial
{

struct tlm_serial_protocol_types
{
	typedef tlm_serial_payload tlm_payload_type;
	typedef tlm::tlm_phase tlm_phase_type;
};

//phase extensions for ethernet
DECLARE_EXTENDED_PHASE(BEG_TRANSMISSION);

}
#endif /*__TLM_SERIAL_PROTOCOL_H__*/
