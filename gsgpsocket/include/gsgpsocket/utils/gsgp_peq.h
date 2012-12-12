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


#ifndef __GSGP_PEQ_H__
#define __GSGP_PEQ_H__

#include "tlm_utils/peq_with_cb_and_phase.h"

#define GS_FUNC_RETURN void
#define GS_FUNC_ARGS payload_type& txn, const phase_type& ph, unsigned int inx
#define GS_FUNC_ARGS_WITHOUT_TYPES txn, ph, inx
GS_FUNC_WRAPPER(gp_nb_peq_callback);
GS_FUNC_FUNCTOR(gp_nb_peq_callback);
#undef GS_FUNC_RETURN
#undef GS_FUNC_ARGS
#undef GS_FUNC_ARGS_WITHOUT_TYPES  


namespace gs {
  namespace gp {
    

    struct gp_p2p_data{
      unsigned int m_index; // index of the multi port responsible for this transaction
      tlm::tlm_phase m_phase;
      sc_dt::uint64  m_addr;
      unsigned int m_bytes_valid;
      tlm::tlm_response_status m_response;
    };

    struct gp_peq_traits{
    public:
      typedef tlm::tlm_generic_payload tlm_payload_type;
      typedef gp_p2p_data tlm_phase_type;
    };

    struct gp_peq : public tlm_utils::peq_with_cb_and_phase<gp_peq, gp_peq_traits>{
      typedef gs::socket::extension_support_base<tlm::tlm_base_protocol_types> ext_support_type;

      template <typename MODULE>
      gp_peq(MODULE* mod, void (MODULE::*cb)(gp_peq_traits::tlm_payload_type& trans, const tlm::tlm_phase& phase, unsigned int index), ext_support_type* ext_support)
      : tlm_utils::peq_with_cb_and_phase<gp_peq, gp_peq_traits>(this, &gp_peq::peq_cb)
      , m_ext_support(ext_support)
      {
        m_functor.set_function(mod, cb);
      }
      
      void notify(gp_peq_traits::tlm_payload_type& trans, tlm::tlm_phase& phase, unsigned int index){
        p2p_tmp.m_index=index;
        p2p_tmp.m_phase=phase;
        p2p_tmp.m_addr=trans.get_address();
        p2p_tmp.m_bytes_valid=m_ext_support->get_extension<bytes_valid>(trans)->value;
        p2p_tmp.m_response=trans.get_response_status();
        tlm_utils::peq_with_cb_and_phase<gp_peq, gp_peq_traits>::notify(trans, p2p_tmp);
      }

      void notify(gp_peq_traits::tlm_payload_type& trans, tlm::tlm_phase& phase, const sc_core::sc_time& time, unsigned int index){
        p2p_tmp.m_index=index;
        p2p_tmp.m_phase=phase;
        p2p_tmp.m_addr=trans.get_address();
        p2p_tmp.m_bytes_valid=m_ext_support->get_extension<bytes_valid>(trans)->value;
        p2p_tmp.m_response=trans.get_response_status();
        tlm_utils::peq_with_cb_and_phase<gp_peq, gp_peq_traits>::notify(trans, p2p_tmp, time);
      }
        
    protected:
      void peq_cb(gp_peq_traits::tlm_payload_type& trans, const gp_peq_traits::tlm_phase_type& p2p){
        trans.set_address(p2p.m_addr);
        trans.set_response_status(p2p.m_response);
        m_ext_support->get_extension<bytes_valid>(trans)->value=p2p.m_bytes_valid;
        m_functor(trans, p2p.m_phase, p2p.m_index);
      }

      gp_p2p_data p2p_tmp;
      ext_support_type* m_ext_support;
      gp_nb_peq_callback_functor<tlm::tlm_base_protocol_types> m_functor;
    };

  } // end namespace gp
} // end namespace gs
        
#endif
