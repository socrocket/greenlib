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

//ctor: an ID is needed to create a callback binder
template <typename T>
gs_callback_binder_fw(int id, T* owner, unsigned int offset)
  : gs_callback_binder_fw_base<TRAITS,CB_BINDER_BASE>(id, owner, offset)
  , m_nb_fw_functor(NULL)
  , m_b_functor(NULL)
  , m_get_dmi_functor(NULL)
  , m_dbg_functor(NULL)  
{
}

//the nb_transport method of the fw interface
sync_enum_type nb_transport_fw(transaction_type& txn,
                            phase_type& p,
                            sc_core::sc_time& t){
  //check if a callback is registered
  if (!m_nb_fw_functor || m_nb_fw_functor->empty()){
    GS_MSG_OUTPUT(GS_MSG_FATAL, "nb_trans No function registered");
  }
    return (*m_nb_fw_functor)(
#ifdef GREEN_USE_WITH_TAG    
      gs_callback_binder_fw_base<TRAITS,CB_BINDER_BASE>::m_id+ gs_callback_binder_fw_base<TRAITS,CB_BINDER_BASE>::m_offset, 
#endif
      txn, p, t); //do the callback
}

//the b_transport method of the fw interface
void b_transport(transaction_type& trans,sc_core::sc_time& t){
  //check if a callback is registered
  if (!m_b_functor || m_b_functor->empty()){
    GS_MSG_OUTPUT(GS_MSG_FATAL, "b_trans. No function registered");
  }
  else
    (*m_b_functor)(
#ifdef GREEN_USE_WITH_TAG    
      gs_callback_binder_fw_base<TRAITS,CB_BINDER_BASE>::m_id+ gs_callback_binder_fw_base<TRAITS,CB_BINDER_BASE>::m_offset, 
#endif
      trans,t); //do the callback
}

//the DMI method of the fw interface
bool get_direct_mem_ptr(transaction_type& trans, tlm::tlm_dmi&  dmi_data){
  //check if a callback is registered
  if (!m_get_dmi_functor || m_get_dmi_functor->empty()){
    dmi_data.allow_none();
    dmi_data.set_start_address(0x0);
    dmi_data.set_end_address((sc_dt::uint64)-1);
    return false;
  }
  else
    return (*m_get_dmi_functor)(
#ifdef GREEN_USE_WITH_TAG    
      gs_callback_binder_fw_base<TRAITS,CB_BINDER_BASE>::m_id+ gs_callback_binder_fw_base<TRAITS,CB_BINDER_BASE>::m_offset, 
#endif
      trans,dmi_data); //do the callback
}

//the debug method of the fw interface
unsigned int transport_dbg(transaction_type& trans){
  //check if a callback is registered
  if (!m_dbg_functor || m_dbg_functor->empty()){
    return 0;
  }
  else
    return (*m_dbg_functor)(
#ifdef GREEN_USE_WITH_TAG    
      gs_callback_binder_fw_base<TRAITS,CB_BINDER_BASE>::m_id+ gs_callback_binder_fw_base<TRAITS,CB_BINDER_BASE>::m_offset, 
#endif
    trans); //do the callback
}


//register callbacks for all fw interface methods at once
// the compiler demands both versions (tagged and untagged) to be there
// because at some point we call them using an if-statement
// so we make sure only the correct one works
void set_callbacks(
  nb_transport_tagged_functor<TRAITS>& nb_fw_functor,
  b_transport_tagged_functor<TRAITS>&  b_functor,
  get_direct_mem_ptr_tagged_functor<TRAITS>& get_dmi_functor,
  transport_dbg_tagged_functor<TRAITS>& dbg_functor
){
#ifdef GREEN_USE_WITH_TAG
  m_nb_fw_functor=&nb_fw_functor;
  m_b_functor=&b_functor;
  m_get_dmi_functor=&get_dmi_functor;
  m_dbg_functor=&dbg_functor;
#else
  assert(0);
#endif
}

void set_callbacks(
  nb_transport_functor<TRAITS>& nb_fw_functor,
  b_transport_functor<TRAITS>&  b_functor,
  get_direct_mem_ptr_functor<TRAITS>& get_dmi_functor,
  transport_dbg_functor<TRAITS>& dbg_functor
){
#ifdef GREEN_USE_WITH_TAG
  assert(0);
#else
  m_nb_fw_functor=&nb_fw_functor;
  m_b_functor=&b_functor;
  m_get_dmi_functor=&get_dmi_functor;
  m_dbg_functor=&dbg_functor;
#endif
}

