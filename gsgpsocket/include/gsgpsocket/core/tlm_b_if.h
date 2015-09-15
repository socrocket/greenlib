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



#ifndef __TLM_B_IF_H__
#define __TLM_B_IF_H__

#include <boost/config.hpp> // needed for SystemC 2.1
#include <systemc>

namespace gs {

  template <class TRANSACTION>
  class tlm_b_if : public virtual sc_core::sc_interface
  {
  public:
    virtual void b_transact( TRANSACTION) = 0;
    virtual ~tlm_b_if(){}
  };

  template <class TRANSACTION>
  class tlm_multi_b_if : public virtual sc_core::sc_interface
  {
  public:
    virtual void b_transact(TRANSACTION, unsigned int index) = 0;
    virtual ~tlm_multi_b_if(){}
  };
  
  template <class TRANSACTION>
  class tlm_td_b_if : public virtual sc_core::sc_interface
  {
  public:
    virtual void b_transact( TRANSACTION, sc_core::sc_time& time) = 0;
    virtual ~tlm_td_b_if(){}
  };

  template <class TRANSACTION>
  class tlm_td_multi_b_if : public virtual sc_core::sc_interface
  {
  public:
    virtual void b_transact(TRANSACTION, sc_core::sc_time& time,
                            unsigned int index) = 0;
    virtual ~tlm_td_multi_b_if(){}
  };

  /*
   * Allow to specify the b_transact function instead of beeing stuck with the
   * b_transact name.
   */
  template<class MOD, class TRANSACTION>
  class tlm_b_if_wrapper:
    public gs::tlm_b_if<TRANSACTION>
  {
    public:
      tlm_b_if_wrapper() {}
      ~tlm_b_if_wrapper() {}

      void register_b_transport(
          void (MOD::*b_transact)(gs::gp::GenericSlaveAccessHandle), MOD *mod)
      {
        this->b_transport = b_transact;
        this->mod = mod;
      }

    private:
      void (MOD::*b_transport)(gs::gp::GenericSlaveAccessHandle);
      MOD *mod;

      void b_transact(gs::gp::GenericSlaveAccessHandle ah)
      {
        if (!b_transport || !mod)
        {
          SC_REPORT_ERROR(__FILE__, "No b_transport has been registered.");
        }

        (*mod.*b_transport)(ah);
      }
  };
  
} // end of namespace gs

#endif
