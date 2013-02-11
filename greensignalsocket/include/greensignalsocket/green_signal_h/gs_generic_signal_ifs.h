//******************************************************************
//@file gs_generic_signal_ifs.h
//
// @brief This file is creating new protocol types
// for which the payload will be gs_generic_signal_payload
// and phases will be gs_generic_signal_phases.In future
// this may include further defintions of interfaces.
//
//==================================================================
// Author:
// Ashwani Singh, CircuitSutra Technologies
//
//==================================================================

#ifndef GS_GENERIC_SIGNAL_IFS_H_
#define GS_GENERIC_SIGNAL_IFS_H_

#include "gs_generic_signal_phase.h"
#include "gs_generic_signal_p.h"
#include "greensocket/generic/green_socket_extension_support_base.h"
#include "greensocket/generic/green_socket_bind_checker.h"
#include "greensocket/generic/green_callback_binders.h"

namespace gs_generic_signal
{
/*!
* @brief: This structure basically is defining a new protocol
* gs_generic_signal_protocol, for which the payload will be of gs_generic_signal_payload 
* and for which the phase will be signal_phase
*/
struct gs_generic_signal_protocol_types
{
	typedef gs_generic_signal_payload tlm_payload_type;
	typedef signal_phase tlm_phase_type;
};

template<typename TYPES= gs_generic_signal_protocol_types>
class gs_generic_signal_fw_transport_if:
	public virtual tlm::tlm_fw_nonblocking_transport_if<typename TYPES::tlm_payload_type, typename TYPES::tlm_phase_type>,
	public virtual tlm::tlm_blocking_transport_if<typename TYPES::tlm_payload_type>,
	public virtual tlm::tlm_fw_direct_mem_if<typename TYPES::tlm_payload_type>,
	public virtual tlm::tlm_transport_dbg_if<typename TYPES::tlm_payload_type>
	{
		
	};
	
template<typename TYPES= gs_generic_signal_protocol_types>
class gs_generic_signal_bw_transport_if: public virtual tlm::tlm_bw_nonblocking_transport_if<typename TYPES::tlm_payload_type ,typename TYPES::tlm_phase_type >
, public virtual tlm::tlm_bw_direct_mem_if
{};

/*
 * this class is normally used for dynamic casts.
 */
class gs_generic_signal_bind_base{};

/*
 * this class would normally contain your custom configuration structure.
 */
class gs_generic_signal_config{};


/*
 * bind checker must be derived from bind_base.
 */
class gs_generic_signal_bind_checker:
public gs::socket::bind_checker<gs_generic_signal_protocol_types>
{
    public:
    /*
     * this ctor must be there (provides name and pointer to owning socket).
     */
    template <typename A_TYPE>
    gs_generic_signal_bind_checker(const char* n, A_TYPE* p, unsigned int w):
        gs::socket::bind_checker< gs_generic_signal_protocol_types >(n, p, w)
    {

    }

    /*
     * fill txn must set mm ptr.
     */
    gs_generic_signal_payload* fill_txn(gs_generic_signal_payload* t)
    { 
        gs_generic_signal_mm_interface* test = 
                            dynamic_cast<gs_generic_signal_mm_interface*>(this);
        if (!test) std::cout << "Extension support casting error" << std::endl;
        else t->set_mm(test);
        return t;
    }
};
}
#endif /*GS_GENERIC_SIGNAL_IFS_H_*/
