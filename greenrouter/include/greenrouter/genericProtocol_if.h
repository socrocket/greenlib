// LICENSETEXT
//
//   Copyright (C) 2007 : GreenSocs Ltd
//       http://www.greensocs.com/ , email: info@greensocs.com
//
//   Developed by :
//
//   Wolfgang Klingauf, Robert Guenzel
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

#ifndef __GenericProtocol_if_h__
#define __GenericProtocol_if_h__

#include "tlm.h"

//--------------------------------------------------------------------------
/**
 * The generic protocol interface.
 */
//--------------------------------------------------------------------------
namespace gs{
namespace gp{

template <typename TRAITS>
class GenericProtocol_if
: public sc_core::sc_interface
{
public:
  typedef typename TRAITS::tlm_payload_type              payload_type;
  typedef typename TRAITS::tlm_phase_type                phase_type;
  typedef tlm::tlm_sync_enum                            sync_enum_type;

  virtual void before_b_transport(unsigned int, payload_type&, sc_core::sc_time&) = 0;
  virtual void invalidate_direct_mem_ptr(unsigned int, sc_dt::uint64, sc_dt::uint64) = 0;
  virtual unsigned int transport_dbg(unsigned int, payload_type& trans) = 0;
  virtual bool get_direct_mem_ptr(unsigned int, payload_type& trans, tlm::tlm_dmi&  dmi_data) = 0;

  virtual sync_enum_type registerMasterAccess(unsigned int, payload_type&, phase_type&, sc_core::sc_time&) =0;
  virtual bool processMasterAccess() =0;
  virtual sync_enum_type registerSlaveAccess(unsigned int, payload_type&, phase_type&, sc_core::sc_time&) =0;
  virtual bool processSlaveAccess() =0;

  virtual bool assignProcessMasterAccessSensitivity(sc_core::sc_spawn_options& opts) =0;
  virtual bool assignProcessSlaveAccessSensitivity(sc_core::sc_spawn_options& opts) =0;


};

}
}

#endif
