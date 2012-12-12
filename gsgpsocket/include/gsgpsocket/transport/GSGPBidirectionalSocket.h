// LICENSETEXT
// 
//   Copyright (C) 2008 : GreenSocs Ltd
//       http://www.greensocs.com/ , email: info@greensocs.com
// 
//   Developed by :
// 
//   Christian Schröder, Robert Guenzel
//     Technical University of Braunschweig, Dept. E.I.S.
//     http://www.eis.cs.tu-bs.de
// 
// 
// The contents of this file are subject to the licensing terms specified
// in the file LICENSE. Please consult this file for restrictions and
// limitations that may apply.
// 
// ENDLICENSETEXT

#ifndef __GSGPBIDIRECTIONALSOCKET_H__
#define __GSGPBIDIRECTIONALSOCKET_H__

#include "GSGPMasterSocket.h"
#include "GSGPSlaveSocket.h"

#define GSGP_BIDIRECTIONAL_SOCKET_TYPE_NAME "GSGPBidirectionalSocket"

namespace gs {
  namespace gp {
    
    /// Bidirectional socket (GenericMasterAPI bidirectional)
    template <unsigned int BUSWIDTH, typename TRANSACTION, typename CONFIG, typename PHASE>
    class GSGPBidirectionalSocket
    : public GSGPMasterSocket<BUSWIDTH, TRANSACTION, CONFIG, PHASE, true>
    , public GSGPSlaveSocket<BUSWIDTH, TRANSACTION, CONFIG, PHASE, true>
    {
    public:
      typedef TRANSACTION transaction;
      typedef Handle<TRANSACTION> accessHandle;
      typedef PHASE  phase;
      typedef Handle<TRANSACTION> created_transaction_handle;
      typedef TRANSACTION created_transaction;  
      typedef unevenpair<accessHandle, phase> atom; // most time this is bidir_atom
    protected:
      typedef GSGPBidirectionalSocket<BUSWIDTH, TRANSACTION, CONFIG, PHASE> my_type;
      typedef GSGPMasterSocket<BUSWIDTH, TRANSACTION, CONFIG, PHASE, true> master_socket_type;
      typedef GSGPSlaveSocket<BUSWIDTH, TRANSACTION, CONFIG, PHASE, true> slave_socket_type;
      typedef typename master_socket_type::virtual_base_type virtual_base_type; // type whose constructor needs to be called by derived classes
      
      /// peq dummy struct (member peq) that represents the peq, forwards the call to the underlying Master AND Slave sockets
      class peq_struct {
      protected:
        my_type *m_owner;
      public:
        /// Constructor getting the owner ptr
        peq_struct(my_type *owner) : m_owner(owner) { }
        /// port which the user has to bind to to receive nb calls (notify(atom))
        void out_port(payload_event_queue_output_if<atom> &intf) {
          m_owner->master_socket_type::peq.out_port(intf);
          m_owner->slave_socket_type::peq.out_port(intf);
        }
        void out_multi_port(payload_event_queue_multi_output_if<atom> &intf) {
          m_owner->master_socket_type::peq.out_multi_port(intf);
          m_owner->slave_socket_type::peq.out_multi_port(intf);
        }
      };
      
    public:
      using master_socket_type::get_other_side;
      //using slave_socket_type::bind_b_if;
      
      GSGPBidirectionalSocket ( const char* port_name ) 
      : virtual_base_type(port_name, GSGP_BIDIRECTIONAL_SOCKET_TYPE_NAME)
      , master_socket_type((std::string(port_name) + "_master").c_str(), GSGP_BIDIRECTIONAL_SOCKET_TYPE_NAME)
      , slave_socket_type((std::string(port_name) + "_slave").c_str(), GSGP_BIDIRECTIONAL_SOCKET_TYPE_NAME)
      , peq(this)
      , data_width(BUSWIDTH)
      {
      }

      /// Payload event queue dummy for legacy support (user module binds to its out_port)
      peq_struct peq;

      /// Deprecated: data width of this port
      const gs_uint32 data_width; // read-only! initialize to template param

      /// Registers the callback according to the configuration
      virtual void start_of_simulation();
    };

    /// User class for a standard GSGP Bidirectional Socket
    template<unsigned int BUSWIDTH>
    class GenericBidirectionalPort
    : public GSGPBidirectionalSocket<BUSWIDTH, GenericBidirectionalAccess, GSGPSocketConfig, GenericPhase> {
    protected:
      typedef GSGPBidirectionalSocket<BUSWIDTH, GenericBidirectionalAccess, GSGPSocketConfig, GenericPhase> base_socket_type;
      typedef typename base_socket_type::virtual_base_type virtual_base_type; // type whose constructor needs to be called by derived classes
    public:
      // is_bidirectional now needs to be set as template parameter BIDIR
      GenericBidirectionalPort( const char* port_name )
      : virtual_base_type(port_name, GSGP_BIDIRECTIONAL_SOCKET_TYPE_NAME)
      , GSGPBidirectionalSocket<BUSWIDTH, GenericBidirectionalAccess, GSGPSocketConfig, GenericPhase>(port_name)
      { }
    };

    #include "GSGPBidirectionalSocket.tpp"

  } // end namespace gp
} // end namespace gs


#endif
