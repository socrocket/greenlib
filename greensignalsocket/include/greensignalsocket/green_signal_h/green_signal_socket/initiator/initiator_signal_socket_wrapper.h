#ifndef INITIATOR_SIGNAL_SOCKET_WRAPPER_H_
#define INITIATOR_SIGNAL_SOCKET_WRAPPER_H_

#include "tlm.h"
#include "greensignalsocket/green_signal_h/gs_generic_signal_ifs.h"
#include "greensignalsocket/green_signal_h/green_signal_socket/generic/wrapper_base.h"

namespace gs_generic_signal
{

/*!
* @brief: The wrapper class basically is a bypass route for all the transport calls
* intiated from the initiator_signal_socket.It adds the src_id to the payload passed 
* through it.The src_id is generated using the number of instances generated for this 
* socket. 
*/
template <typename SOCKET>
class initiator_signal_socket_wrapper: public wrapper_base,public gs_generic_signal_fw_transport_if<>
{
public:
	
	unsigned int get_id(){return m_id;}
	void b_transport(gs_generic_signal_payload & trans, sc_core::sc_time& t)
	{
		trans.set_src_id(m_id);
		(*sock)->b_transport(trans,t);
	}

	

	tlm::tlm_sync_enum nb_transport_fw(gs_generic_signal_payload& trans, 
	signal_phase& phase, sc_core::sc_time& t)
	{
		trans.set_src_id(m_id);
		tlm::tlm_sync_enum retval=(*sock)->nb_transport_fw(trans,phase,t);
		return( retval);
	}

	

	bool get_direct_mem_ptr(gs_generic_signal_payload& trans, tlm::tlm_dmi& dmi_data)
	{
		trans.set_src_id(m_id);
		return((*sock)->get_direct_mem_ptr(trans,dmi_data));
	}
	

	unsigned int transport_dbg(gs_generic_signal_payload& trans)
	{
		trans.set_src_id(m_id);
		return((*sock)->transport_dbg(trans));
	}

	initiator_signal_socket_wrapper(SOCKET* convsock)
      : sock(convsock)
      , m_id(instance_counter++)
	{
		//  sc_core::sc_object * obj= ((sc_core::sc_object*)convsock)->get_parent();
		//  std::string name = obj->name()+ std::string(".") +convsock->name() ;
		  
          (*wrapper_base::get_decode_map())[ convsock->name()]= m_id;
    }
    
	initiator_signal_socket_wrapper(initiator_signal_socket_wrapper& x)
      : sock(x.sock)
      , m_id(instance_counter++)
	{
	}
    
	~initiator_signal_socket_wrapper(){
      ///no decrement of inst counter to keep the IDs unique
      ///instance_counter--;
    }
private:
	///this is the hook of the convinience socket
	SOCKET * sock;
	///this is the instance counter
	static unsigned int instance_counter;
    
    ///and THIS is the id of the wrapper
    unsigned int m_id;
};

template <typename SOCKET>
unsigned int initiator_signal_socket_wrapper<SOCKET>::instance_counter=0;

}
#endif /*INITIATOR_SIGNAL_SOCKET_WRAPPER_H_*/
