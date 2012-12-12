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

#ifndef __GREEN_initiator_socket_base_H__
#define __GREEN_initiator_socket_base_H__

#include <vector>
#include "greensocket/generic/gs_extension.h"
#include "greensocket/generic/gs_callbacks.h"
#include "greensocket/generic/green_initiator_socket_callback_base.h"
#include "greensocket/generic/green_socket_bind_checker.h"
#include "greensocket/generic/green_socket_mm_base.h"
#include "greensocket/generic/green_socket_default_pool.h"
#include "greensocket/generic/green_socket_timing_support_base.h"
#include "greensocket/utils/gs_msg_output.h"

namespace gs {
namespace socket{


//This is the green_initiator_socket from which other initiator_sockets
// shall be derived. For explanation of template args look into gsock_tech.pdf
template <unsigned int BUSWIDTH=32,
          typename TRAITS=tlm::tlm_base_protocol_types,
          unsigned int N=0,
          typename BIND_BASE=bind_checker<TRAITS>,
          typename MM_INTERFACE=tlm::tlm_mm_interface,
          typename EXT_FILLER=mm_base<TRAITS,MM_INTERFACE>,
          typename POOL=default_pool<TRAITS, EXT_FILLER>
          >
class initiator_socket_base: 
                              public initiator_socket_callback_base<BUSWIDTH,TRAITS, N, typename BIND_BASE::root_type, typename BIND_BASE::multi_root_type>
                            , public BIND_BASE
                            //, public BIND_BASE::ext_support_type
                            , public mm_base<TRAITS,MM_INTERFACE>
                            , public initiator_timing_support_base<TRAITS,BIND_BASE>
{
public:
//typedef section
  typedef TRAITS                                                       traits_type;
  typedef initiator_socket_callback_base<BUSWIDTH,TRAITS,N, typename BIND_BASE::root_type, typename BIND_BASE::multi_root_type>     base_type;
  typedef typename base_type::base_target_socket_type                  base_target_socket_type;
  typedef typename TRAITS::tlm_payload_type                            payload_type;
  typedef typename TRAITS::tlm_phase_type                              phase_type;
  typedef tlm::tlm_sync_enum                                           sync_enum_type;
  
  
  typedef BIND_BASE bind_checker_type;
  
  //typedef typename BIND_BASE::ext_support_type                  ext_support_type;
  typedef  allocation_scheme                            allocation_scheme_type;
  
  typedef mm_base<TRAITS,MM_INTERFACE>              mm_base_type;
  
  /*
  Allocation schemes as functions. Prefer those over the raw enum values, as the enum values may change in future releases,
  whereas the function names will stay
  */
  static allocation_scheme_type mm_txn_only(){return GS_TXN_ONLY;}
  static allocation_scheme_type mm_txn_with_data(){return GS_TXN_WITH_DATA;}
  static allocation_scheme_type mm_txn_with_be(){return GS_TXN_WITH_BE;}
  static allocation_scheme_type mm_txn_with_be_and_data(){return GS_TXN_WITH_BE_AND_DATA;}
  
  
public:
  //constructor requires a name. The last argument is for future use and shall not be used.
  initiator_socket_base(const char* name, allocation_scheme alloc_scheme=mm_txn_only(), EXT_FILLER* ext_filler=NULL);

  ~initiator_socket_base();
  
  //get a memory managed transaction from the pool.
  // Its ref count is already set to 1
  payload_type* get_transaction();

  //Return a memory managed transaction to the pool.
  // It shall be called when an initiator has no longer use for a transaction
  void release_transaction(payload_type* txn);

  //test if a transaction is from the pool of this socket (for debug only. This call is SLOW!!)
  bool is_from(payload_type* gp){
    return m_pool->is_from(gp);
  }
  
protected:

  //ATTENTION: derived socket should not implement end_of_elaboration
  // if they do, they have to call this end_of_elaboration before anything else
  void end_of_elaboration();
  
  //the function from the memory management interface (see TLM-2.0 LRM). May be overridden by derived sockets
  virtual void free(payload_type* txn);
  
  //this function is called when we were successfully bound to someone
  // override it to react to the resolved config, but call this one from the derived class
  virtual void bound_to(const std::string&, typename BIND_BASE::bind_base_type*, unsigned int);
  
  //override this function when deriving from the greensocket
  virtual const std::string& get_type_string();

  const static std::string s_kind;
  POOL* m_pool;
  EXT_FILLER * m_filler;  
};

} //end ns socket
} //end ns gs

#include "greensocket/initiator/green_initiator_socket_base.tpp"
#endif
