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

#ifndef __genericScheduler_if_h__
#define __genericScheduler_if_h__

#include <map>

//--------------------------------------------------------------------------
/**
 * The generic scheduler interface
 */
//--------------------------------------------------------------------------
namespace gs{
namespace gp{

template <typename TRAITS>
class GenericScheduler_if
: public sc_core::sc_interface
{
public:
  typedef std::pair<typename TRAITS::tlm_payload_type*, typename TRAITS::tlm_phase_type> pair_type;
  virtual void enqueue(typename TRAITS::tlm_payload_type*, const typename TRAITS::tlm_phase_type&, unsigned int) =0;
  virtual pair_type& dequeue(bool remove=true) =0;
  virtual bool isPending() =0; //differs from isempyt because there could be something inside that is not "valid" yet
  virtual bool isEmpty() =0;

};

}
}
#endif
