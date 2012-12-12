// LICENSETEXT
//
//   Copyright (C) 2005 - 2008 : GreenSocs Ltd
//       http://www.greensocs.com/ , email: info.com
//
//   Developed by :
//
//  Robert Guenzel, Christian Schroeder
//     Technical University of Braunschweig, Dept. E.I.S.
//     http://www.eis.cs.tu-bs.de
//
//   Mark Burton, Marcus Bartholomeu
//     GreenSocs Ltd
//
//
// The contents of this file are subject to the licensing terms specified
// in the file LICENSE. Please consult this file for restrictions and
// limitations that may apply.
// 
// ENDLICENSETEXT


#ifndef __GREEN_SOCKET_OBSERVER_BASE_H__
#define __GREEN_SOCKET_OBSERVER_BASE_H__

#include "greensocket/monitor/green_socket_monitor.h"

namespace gs{
namespace socket{

template <typename TRAITS>
class gp_observer_base_t
{
  typedef transfer_triplet<TRAITS> triplet;

public:
#ifdef USE_GREEN_CONFIG
  GC_HAS_CALLBACKS();
  boost::shared_ptr<gs::cnf::ParamCallbAdapt_b> cbAdapt;  
#endif

  template <unsigned int BUSWIDTH>
  struct call_converter{
    static bindability_base<TRAITS>* get_connected_target(void* mon_){
      monitor<BUSWIDTH, TRAITS>* mon=static_cast<monitor<BUSWIDTH, TRAITS>*>(mon_);
      return mon->get_connected_target();
    }
    static bindability_base<TRAITS>* get_connected_initiator(void* mon_){
      monitor<BUSWIDTH, TRAITS>* mon=static_cast<monitor<BUSWIDTH, TRAITS>*>(mon_);
      return mon->get_connected_initiator();
    }
    static triplet& get_param(void* mon_){
      monitor<BUSWIDTH, TRAITS>* mon=static_cast<monitor<BUSWIDTH, TRAITS>*>(mon_);
      return mon->get_param();
    }
  };

  typedef bindability_base<TRAITS>* (*get_connected_cb)(void*);
  typedef triplet& (*get_param_cb)(void*);
  
  get_connected_cb m_get_conn_init, m_get_conn_tgt;
  get_param_cb m_get_param;
  void* m_mon;
  std::string m_name;

  template<unsigned int BUSWIDTH>
  gp_observer_base_t(monitor<BUSWIDTH, TRAITS>* mon) 
  #ifdef USE_GREEN_CONFIG
    : m_name(mon->get_param().getName())
  #else
    : m_name("dummy_name")
  #endif
  {
  #ifdef USE_GREEN_CONFIG
    cbAdapt = GC_REGISTER_PARAM_CALLBACK(&mon->get_param(), gp_observer_base_t, decode);
  #else
    mon->register_observer(this);
  #endif
    m_get_conn_init = &call_converter<BUSWIDTH>::get_connected_initiator;
    m_get_conn_tgt  = &call_converter<BUSWIDTH>::get_connected_target;
    m_get_param     = &call_converter<BUSWIDTH>::get_param;
    m_mon=static_cast<void*>(mon);
    
    if (std::string(typeid(typename TRAITS::tlm_payload_type).name())!=std::string(typeid(tlm::tlm_generic_payload).name())){
      std::cerr<<"Observer base does only work for protocols that use the generic playoad"<<std::endl;
      abort();
    }
  }

  const bindability_base<TRAITS>* get_connected_target()const{
    return m_get_conn_tgt(m_mon);
  }
  const bindability_base<TRAITS>* get_connected_initiator()const{
    return m_get_conn_init(m_mon);
  }
  
  virtual ~gp_observer_base_t(){
  #ifdef USE_GREEN_CONFIG
    GC_UNREGISTER_CALLBACK(cbAdapt);
  #endif
  }
  
  const std::string& get_name() const{return m_name;}
  
  #ifdef USE_GREEN_CONFIG
  void decode(gs::gs_param_base& par){
    if (par.is_destructing()) return;
    gs_param<triplet>* tmp_p=static_cast<gs_param<triplet>*>(&par);
    triplet tmp_i=tmp_p->getValue();
  #else
  void decode(){
    triplet tmp_i=m_get_param(m_mon);
  #endif
    triplet* tmp=&tmp_i;
    if (tmp->phase==B_TRANSPORT_ID_PHASE){
      switch (tmp->call_or_return){
        case TLM_CALLID_FW:
          b_call_callback(*tmp->txn, tmp->time); break;
        case TLM_B_TRANSPORT_RETURN:
          b_return_callback(*tmp->txn, tmp->time); break;
        default:
          std::cerr<<"Warning: error decoding "<<m_name<<" during observation."<<std::endl;
          break;
      }
    }
    else{
      switch (tmp->call_or_return){
        case tlm::TLM_ACCEPTED:
        case tlm::TLM_UPDATED:
        case tlm::TLM_COMPLETED:
          nb_return_callback(true, *tmp->txn, tmp->phase, tmp->time, (tlm::tlm_sync_enum)tmp->call_or_return);
          break;
        case socket::TLM_CALLID_FW:
          nb_call_callback(true, *tmp->txn, tmp->phase, tmp->time);
          break;
        case socket::TLM_CALLID_BW:
          nb_call_callback(false, *tmp->txn, tmp->phase, tmp->time);
          break;
        case socket::TLM_ACCEPTED_BW:
          nb_return_callback(false, *tmp->txn, tmp->phase, tmp->time, tlm::TLM_ACCEPTED);
          break;
        case socket::TLM_UPDATED_BW:
          nb_return_callback(false, *tmp->txn, tmp->phase, tmp->time, tlm::TLM_UPDATED);
          break;
        case socket::TLM_COMPLETED_BW:
          nb_return_callback(false, *tmp->txn, tmp->phase, tmp->time, tlm::TLM_COMPLETED);
          break;
        default:
          std::cerr<<"Warning: error decoding "<<m_name<<" during observation."<<std::endl;
      }
    }
  }
  
  virtual void nb_call_callback(bool fwNbw, tlm::tlm_generic_payload& txn, const tlm::tlm_phase& phase, const sc_core::sc_time& time)=0;
  virtual void nb_return_callback(bool fwNbw, tlm::tlm_generic_payload& txn, const tlm::tlm_phase& phase, const sc_core::sc_time& time, tlm::tlm_sync_enum retVal)=0;

  virtual void b_call_callback(tlm::tlm_generic_payload& txn, const sc_core::sc_time& time)=0;
  virtual void b_return_callback(tlm::tlm_generic_payload& txn, const sc_core::sc_time& time)=0;

};

typedef gp_observer_base_t<tlm::tlm_base_protocol_types> gp_observer_base;

}
}
#endif
