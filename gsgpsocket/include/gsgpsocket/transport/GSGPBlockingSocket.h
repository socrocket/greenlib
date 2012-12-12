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

#ifndef __GSGPBLOCKINGSOCKET_H__
#define __GSGPBLOCKINGSOCKET_H__


namespace gs {
  namespace gp {
    
    template <class PORT>
    class GSGPSocketBlockingBase {
    public:
      typedef std::pair<sc_core::sc_event*, typename PORT::phase*> waitpayload;
      typedef typename std::map<tlm::tlm_generic_payload*, waitpayload>::iterator waitid;

      GSGPSocketBlockingBase(){}
      virtual ~GSGPSocketBlockingBase(){}
      
    protected:      
      /// map of all waiting blocking functions
      std::map<tlm::tlm_generic_payload*, waitpayload > waiters;
      
      /// Set of all transactions whose blocking calls have been released in THIS delta cycle
      std::set<tlm::tlm_generic_payload*> released_blockings_in_this_delta;
      
      /// Stores the last delta where the peq_cb was called
      sc_dt::uint64 last_delta;
    };
    
    
    /// Proxy class which allows access to the BLOCKING socket's protected base members
    template <typename SOCKET>
    class Blocking_socket_base_accessor_proxy {
    protected:
      SOCKET* soc;
    public:
      typedef std::pair<sc_core::sc_event*, typename SOCKET::phase*> waitpayload;
      typedef typename std::map<tlm::tlm_generic_payload*, waitpayload>::iterator waitid;
      
      Blocking_socket_base_accessor_proxy(SOCKET *_soc) { soc = _soc; }

      std::map<tlm::tlm_generic_payload*, typename SOCKET::waitpayload >& get_waiters() { return soc->waiters; }
    };
    

  } // end namespace gp
} // end namespace gs


#endif
