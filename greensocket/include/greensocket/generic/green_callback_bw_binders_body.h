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


//first make sure that whoever included this class fragment knew what he/she was doing
#ifndef GREEN_I_KNOW_VERY_WELL_WHAT_I_AM_DOING_WHEN_INCLUDING_THIS_FILE
# error This file may never ever be included without special care!
#endif

public:
//typedefs according to the used TRAITS class
typedef typename TRAITS::tlm_payload_type              transaction_type;
typedef typename TRAITS::tlm_phase_type                phase_type;  
typedef tlm::tlm_sync_enum                            sync_enum_type;


//ctor: an ID and an offset is needed to create a callback binder
template <typename T>
gs_callback_binder_bw(int id, T* owner, unsigned int offset)
  : gs_callback_binder_bw_base<TRAITS,CB_BINDER_BASE>(id, owner, offset)
  , m_nb_bw_functor(NULL)
  , m_inval_dmi_functor(NULL)
{
}

//the nb_transport method of the bw interface
sync_enum_type nb_transport_bw(transaction_type& txn,
                            phase_type& p,
                            sc_core::sc_time& t){
  //check if a callback is registered
  if (!m_nb_bw_functor || m_nb_bw_functor->empty()){
    GS_MSG_OUTPUT(GS_MSG_FATAL, "nb bw No function registered"); //here we could do an automatic nb->b conversion
  }
    return (*m_nb_bw_functor)(
#ifdef GREEN_USE_WITH_TAG    
    gs_callback_binder_bw_base<TRAITS,CB_BINDER_BASE>::m_id+ gs_callback_binder_bw_base<TRAITS,CB_BINDER_BASE>::m_offset, 
#endif
    txn, p, t); //do the callback
  
}

//the DMI method of the bw interface
void invalidate_direct_mem_ptr(sc_dt::uint64 l, sc_dt::uint64 u){
  //check if a callback is registered
  if (!m_inval_dmi_functor || m_inval_dmi_functor->empty()){
    return;
  }
  else
    (*m_inval_dmi_functor)(
#ifdef GREEN_USE_WITH_TAG    
    gs_callback_binder_bw_base<TRAITS,CB_BINDER_BASE>::m_id+ gs_callback_binder_bw_base<TRAITS,CB_BINDER_BASE>::m_offset,
#endif
    l,u); //do the callback
}

//Register a callback for the nb_transport_bw TLM2 interface method
// the compiler demands both versions (tagged and untagged) to be there
// because at some point we call them using an if-statement
// so we make sure only the correct one works
void set_callbacks(
  nb_transport_tagged_functor<TRAITS>& nb_bw_functor,
  invalidate_direct_mem_ptr_tagged_functor<TRAITS>& inval_dmi_functor
){
#ifdef GREEN_USE_WITH_TAG    
  m_nb_bw_functor=&nb_bw_functor;
  m_inval_dmi_functor=&inval_dmi_functor;
#else
  assert(0);
#endif
}

void set_callbacks(
  nb_transport_functor<TRAITS>& nb_bw_functor,
  invalidate_direct_mem_ptr_functor<TRAITS>& inval_dmi_functor
){
#ifdef GREEN_USE_WITH_TAG    
  assert(0);
#else
  m_nb_bw_functor=&nb_bw_functor;
  m_inval_dmi_functor=&inval_dmi_functor;
#endif
}

