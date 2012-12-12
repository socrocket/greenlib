// LICENSETEXT
// 
//   Copyright (C) 2007-2008 : GreenSocs Ltd
//       http://www.greensocs.com/ , email: info@greensocs.com
// 
//   Developed by :
// 
//   Christian Schroeder, Robert Guenzel 
//     Technical University of Braunschweig, Dept. E.I.S.
//     http://www.eis.cs.tu-bs.de
//
//   Based on: generic.static_casts.blocking.h by
//     Wolfgang Klingauf, Robert Guenzel
//       Technical University of Braunschweig, Dept. E.I.S.
//       http://www.eis.cs.tu-bs.de
//     Mark Burton, Marcus Bartholomeu
//       GreenSocs Ltd
// 
// 
// The contents of this file are subject to the licensing terms specified
// in the file LICENSE. Please consult this file for restrictions and
// limitations that may apply.
// 
// ENDLICENSETEXT

#ifndef __GSGPMASTERBLOCKINGSOCKET_H__
#define __GSGPMASTERBLOCKINGSOCKET_H__

#include "gsgpsocket/transport/GSGPMasterSocket.h"
#include "gsgpsocket/transport/GSGPBlockingSocket.h"

namespace gs {
  namespace gp {

    // ///////// structs for BLOCKING master socket calls ///////////////// //
    
    /// Blocking Request functor for the GreenBus Socket
    template <typename SOCKET, typename PHASE>
    struct RequestBlockObj : RequestObj<SOCKET, PHASE>, Blocking_socket_base_accessor_proxy<SOCKET> {
      RequestBlockObj(SOCKET& soc) : RequestObj<SOCKET, PHASE>(soc), Blocking_socket_base_accessor_proxy<SOCKET>(&soc) { }
    };
    /// Blocking Request functor for the GreenBus Socket, specialized for the GenericPhase
    template <typename SOCKET> 
    struct RequestBlockObj<SOCKET, GenericPhase> : RequestObj<SOCKET, GenericPhase>, Blocking_socket_base_accessor_proxy<SOCKET> {
      RequestBlockObj(SOCKET& soc) : RequestObj<SOCKET, GenericPhase>(soc), Blocking_socket_base_accessor_proxy<SOCKET>(&soc) { }
      GenericPhase block(typename SOCKET::accessHandle &th, GenericPhase &ph, const sc_core::sc_time &d);
      GenericPhase block(typename SOCKET::accessHandle &th, GenericPhase &ph );
      GenericPhase block(typename SOCKET::accessHandle &th, GenericPhase &ph, double d, sc_core::sc_time_unit u);
      GenericPhase block(typename SOCKET::accessHandle &th);
      GenericPhase block(typename SOCKET::accessHandle &th, const sc_core::sc_time &d);
      GenericPhase block(typename SOCKET::accessHandle &th, double d, sc_core::sc_time_unit u);
      using RequestObj<SOCKET, GenericPhase>::soc;
    };
    
    /// Blocking SendData functor for the GreenBus Socket
    template <typename SOCKET, typename PHASE>
    struct SendDataBlockObj : SendDataObj<SOCKET, PHASE>, Blocking_socket_base_accessor_proxy<SOCKET> {
      SendDataBlockObj(SOCKET& soc) : SendDataObj<SOCKET, PHASE>(soc), Blocking_socket_base_accessor_proxy<SOCKET>(&soc) { }
    };
    /// Blocking SendData functor for the GreenBus Socket, specialized for the GenericPhase
    template <typename SOCKET>
    struct SendDataBlockObj<SOCKET, GenericPhase> : SendDataObj<SOCKET, GenericPhase>, Blocking_socket_base_accessor_proxy<SOCKET> {
      SendDataBlockObj(SOCKET& soc) : SendDataObj<SOCKET, GenericPhase>(soc), Blocking_socket_base_accessor_proxy<SOCKET>(&soc) { }
      GenericPhase block(typename SOCKET::accessHandle &th, GenericPhase &ph, const sc_core::sc_time &d);
      GenericPhase block(typename SOCKET::accessHandle &th, GenericPhase &ph );
      GenericPhase block(typename SOCKET::accessHandle &th, GenericPhase &ph, double d, sc_core::sc_time_unit u);
      using SendDataObj<SOCKET, GenericPhase>::soc;
    };
   

    /// The blocking master socket for the generic protocol
    template <unsigned int BUSWIDTH, typename TRANSACTION, typename CONFIG, typename PHASE>
    class GSGPMasterBlockingSocket
    : public GSGPMasterSocket<BUSWIDTH, TRANSACTION, CONFIG, PHASE>
    , public payload_event_queue_multi_output_if<master_atom> // to bind this as a 'user' to the GSGPMasterSocket (filters the phases which release the blockings)
    , protected GSGPSocketBlockingBase< GSGPMasterSocket<BUSWIDTH, TRANSACTION, CONFIG, PHASE> >
    {
    protected:
      
      typedef GSGPMasterSocket<BUSWIDTH, TRANSACTION, CONFIG, PHASE> base_type;
      typedef GSGPMasterBlockingSocket<BUSWIDTH, TRANSACTION, CONFIG, PHASE> my_type;
      typedef GSGPSocketBlockingBase< base_type > blocking_base_type;
      typedef typename base_type::virtual_base_type virtual_base_type; // type whose constructor needs to be called by derived classes
      
      using blocking_base_type::last_delta;
      using blocking_base_type::released_blockings_in_this_delta;
      using blocking_base_type::waiters;
      
      using base_type::socket_name;
      
      
      // allow objects derived from the proxy to access the protected function socket_name
      friend class Socket_name_proxy<my_type>;
      // allow objects derived from the proxy class to access the blocking socket base members
      friend class Blocking_socket_base_accessor_proxy<my_type>;
      
    public:
      
      GSGPMasterBlockingSocket ( const char* port_name )
      : virtual_base_type(port_name, GSGP_MASTER_SOCKET_TYPE_NAME)
      , base_type ( port_name )
      //, out_port("out_port")
      , Request(*this)
      , SendData(*this)
      {
        // bind this as a 'user' to the GSGPMasterSocket (notify() filters the phases which release the blockings)
        base_type::peq.out_multi_port(*this);
      }
      
      /// Port the user has to bind to, this sockets writes to that port to notify the user (function notify(master_atom))
      //sc_port<payload_event_queue_output_if<master_atom> > out_port;
      pseudo_port<payload_event_queue_output_if<master_atom> > out_port;
      pseudo_port<payload_event_queue_multi_output_if<master_atom> > out_multi_port;
      

      /// master sends a request atom
      RequestBlockObj<my_type, PHASE>          Request;
      /// master sends a data atom
      SendDataBlockObj<my_type, PHASE>         SendData;
      
      typedef typename base_type::transaction transaction;
      typedef typename base_type::accessHandle accessHandle;
      typedef typename base_type::phase phase;
      
      /// Called by the GSGPMasterSocket peq_cb
      /**
       * Filters out the phases which blocking function calls are waiting for.
       * Forwards the others to the user.
       */
      void notify(master_atom& tc, unsigned int index);
      
    protected:
      using base_type::peq; // hide this from user!
      void start_of_simulation(){
        base_type::start_of_simulation();
        if (out_port.operator->()==NULL  &&  out_multi_port.operator->()==NULL){
          SC_REPORT_ERROR(base_type::name(), "out_port and out_multi_port not bound. You need to bind one of them!");
        }
      }
    };

#   include "gsgpsocket/transport/GSGPMasterBlockingSocket.tpp"
    
    
  
    /// The default generic blocking master initiator port.
    template<unsigned int BUSWIDTH>
    class GenericMasterBlockingPort
    : public GSGPMasterBlockingSocket<BUSWIDTH, GenericMasterAccess, GSGPSocketConfig, GenericPhase> {
    protected:
      typedef GSGPMasterBlockingSocket<BUSWIDTH, GenericMasterAccess, GSGPSocketConfig, GenericPhase> base_socket_type;
      typedef typename base_socket_type::virtual_base_type virtual_base_type; // type whose constructor needs to be called by derived classes
    public:
      GenericMasterBlockingPort( const char* port_name )
      : virtual_base_type(port_name, GSGP_MASTER_SOCKET_TYPE_NAME)
      , GSGPMasterBlockingSocket<BUSWIDTH, GenericMasterAccess, GSGPSocketConfig, GenericPhase>(port_name)
      { }
    };
#   define GenericInitiatorBlockingPort GenericMasterBlockingPort
    
    // The default router initiator port.
    //typedef GenericInitiatorBlockingAPI<initiator_port<GenericTransaction,GenericRouterAccess,GenericPhase> > GenericRouterBlockingPort;
    

  } // end namespace gp
} // end namespace gs

#endif
