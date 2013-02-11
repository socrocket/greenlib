#ifndef INITIATOR_SIGNAL_SOCKET_H_
#define INITIATOR_SIGNAL_SOCKET_H_

#include "greensocket/initiator/single_socket.h"
#include "greensignalsocket/green_signal_h/gs_generic_signal_ifs.h"
#include "greensignalsocket/green_signal_h/green_signal_socket/initiator/initiator_signal_socket_wrapper.h"

using namespace gs::socket;
namespace gs_generic_signal
{
typedef gs::socket::initiator_socket<32,gs_generic_signal_protocol_types,
gs_generic_signal_bind_checker,gs_generic_signal_mm_interface>

initiator_signal_base_socket;

/*!
* @brief: The initiator_signal_socket basically derived from the initiator_signal_base_socket
* with gs_generic_signal_protocol_types as traits.
* 
*/
class initiator_signal_socket:
	public virtual initiator_signal_base_socket
{
public:
	typedef initiator_signal_socket_wrapper<initiator_signal_base_socket> wrapper_types;
	initiator_signal_socket(const char* name, gs::socket::allocation_scheme alloc_scheme=gs::socket::GS_TXN_ONLY):
	initiator_signal_base_socket( name, alloc_scheme)
	{
			wrapper= new wrapper_types((initiator_signal_base_socket*)this);
	}
	
	
	wrapper_types * operator->(){return wrapper;}
	~initiator_signal_socket(){delete  wrapper;}
private:
	wrapper_types * wrapper;
};

}
#endif /*INITIATOR_SIGNAL_SOCKET_H_*/
