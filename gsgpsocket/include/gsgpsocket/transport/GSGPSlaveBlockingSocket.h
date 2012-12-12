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

#ifndef __GSGPSLAVEBLOCKINGSOCKET_H__
#define __GSGPSLAVEBLOCKINGSOCKET_H__

#include "gsgpsocket/transport/GSGPSlaveSocket.h"
#include "gsgpsocket/transport/GSGPBlockingSocket.h"

namespace gs {
  namespace gp {

    // ///////// structs for BLOCKING slave socket calls ///////////////// //
    
    /// Blocking Response functor for the GreenBus Socket
    template <typename SOCKET, typename PHASE>
    struct ResponseBlockObj : ResponseObj<SOCKET, PHASE>, Blocking_socket_base_accessor_proxy<SOCKET> {
      ResponseBlockObj(SOCKET& soc) : ResponseObj<SOCKET, PHASE>(soc), Blocking_socket_base_accessor_proxy<SOCKET>(&soc) { }
    };
    /// Blocking Response functor for the GreenBus Socket, specialized for the GenericPhase
    template <typename SOCKET> 
    struct ResponseBlockObj<SOCKET, GenericPhase> : ResponseObj<SOCKET, GenericPhase>, Blocking_socket_base_accessor_proxy<SOCKET> {
      ResponseBlockObj(SOCKET& soc) : ResponseObj<SOCKET, GenericPhase>(soc), Blocking_socket_base_accessor_proxy<SOCKET>(&soc) { }
      GenericPhase block(typename SOCKET::accessHandle &th, GenericPhase &ph, const sc_core::sc_time &d, unsigned int index = 0);
      GenericPhase block(typename SOCKET::accessHandle &th, GenericPhase &ph, unsigned int index = 0 );
      GenericPhase block(typename SOCKET::accessHandle &th, GenericPhase &ph, double d, sc_core::sc_time_unit u, unsigned int index = 0);
      using ResponseObj<SOCKET, GenericPhase>::soc;
    };
    
    
    /// The blocking slave socket for the generic protocol
    template <unsigned int BUSWIDTH, typename TRANSACTION, typename CONFIG, typename PHASE>
    class GSGPSlaveBlockingSocket
    : public GSGPSlaveSocket<BUSWIDTH, TRANSACTION, CONFIG, PHASE>
    , public payload_event_queue_multi_output_if<slave_atom>
    , protected GSGPSocketBlockingBase< GSGPSlaveSocket<BUSWIDTH, TRANSACTION, CONFIG, PHASE> >
    {
    protected:
      typedef GSGPSlaveSocket<BUSWIDTH, TRANSACTION, CONFIG, PHASE> base_type;
      typedef GSGPSlaveBlockingSocket<BUSWIDTH, TRANSACTION, CONFIG, PHASE> my_type;
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

      GSGPSlaveBlockingSocket ( const char* port_name )
      : virtual_base_type(port_name, GSGP_SLAVE_SOCKET_TYPE_NAME)
      , base_type(port_name)
      //, out_port("out_port")
      , Response(*this)
      {
        // bind this as a 'user' to the GSGPSlaveSocket (notify() filters the phases which release the blockings)
        base_type::peq.out_multi_port(*this);
      }
    
      /// Port the user has to bind to, this sockets writes to that port to notify the user (function notify(slave_atom))
      //sc_port<payload_event_queue_output_if<slave_atom> > out_port;
      pseudo_port<payload_event_queue_output_if<slave_atom> > out_port;
      pseudo_port<payload_event_queue_multi_output_if<slave_atom> > out_multi_port;
      
      /// slave sends a response atom
      ResponseBlockObj<my_type, PHASE>         Response;

      typedef typename base_type::transaction transaction;
      typedef typename base_type::accessHandle accessHandle;
      typedef typename base_type::phase phase;
      
      /// Called by the GSGPSlaveSocket peq_cb
      /**
       * Filters out the phases which blocking function calls are waiting for.
       * Forwards the others to the user.
       */
      void notify(slave_atom& tc, unsigned int index);

    protected:
      using base_type::peq; // hide this from user!
      void start_of_simulation(){
        base_type::start_of_simulation();
        if (out_port.operator->()==NULL &&  out_multi_port.operator->()==NULL){
          SC_REPORT_ERROR(base_type::name(), "out_port and out_multi_port not bound. You need to bind one of them!");
        }
      }
      
    };

#   include "gsgpsocket/transport/GSGPSlaveBlockingSocket.tpp"
    
    
    
    /// The default generic blocking slave port.
    template<unsigned int BUSWIDTH>
    class GenericSlaveBlockingPort
    : public GSGPSlaveBlockingSocket<BUSWIDTH, GenericSlaveAccess, GSGPSocketConfig, GenericPhase> {
    protected:
      typedef GSGPSlaveBlockingSocket<BUSWIDTH, GenericSlaveAccess, GSGPSocketConfig, GenericPhase> base_socket_type;
      typedef typename base_socket_type::virtual_base_type virtual_base_type; // type whose constructor needs to be called by derived classes
    public:
      GenericSlaveBlockingPort( const char* port_name )
      : virtual_base_type(port_name, GSGP_SLAVE_SOCKET_TYPE_NAME)
      , GSGPSlaveBlockingSocket<BUSWIDTH, GenericSlaveAccess, GSGPSocketConfig, GenericPhase>(port_name)
      { }
    };
#   define GenericTargetBlockingPort GenericSlaveBlockingPort


  } // end namespace gp
} // end namespace gs

#endif
