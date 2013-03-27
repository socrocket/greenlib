/*
Copyright (c) 2008, Intel Corporation
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.
    * Neither the name of Intel Corporation nor the names of its contributors
      may be used to endorse or promote products derived from this software
      without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

//
// ChangeLog GreenSocs
//
// 2008-12-01 Christian Schroeder: callback
// 2008-12-03 Christian Schroeder: 
//             renamed function gr_event::add_static gr_event::add_callback
// 2009-01-21 Christian Schroeder: changed namespace
// 2009-01-26  made contructor gr_event(gr_event) protected;
//             deprecated notify_delayed
// 2009-01-29 Christian Schroeder: renamings
// 2009-01-29 FURTHER CHANGES SEE SVN LOG!
//


#ifndef _GR_EVENT_H_
#define _GR_EVENT_H_

#include <set>

#include <boost/config.hpp>
#include <systemc>

#include "greenreg/gr_externs.h"
#include "greenreg/framework/core/gr_common.h"
#include "greenreg/utils/greenreg_utils.h"
#include "greenreg/sysc_ext/utils/gr_report.h"

// forward declaration for parameter adapter class
namespace gs {
  template<typename T>
  class gs_param_greenreg;
}

namespace gs {
namespace reg {

class gr_sc_sensitive;

/// Event class handling events or callbacks
/**
 * This event handles either a callback or an event.
 * As soon as a callback is registered, it is automatically processed
 * when getting a notify.
 * The timing setting enables or disables the event notification
 * as far as no callback is registered.
 */
class gr_event : public sc_core::sc_event
{
protected:
  /// Wrapper class which allows to register a 0 param callback but internally using a 2 param callback
  /**
   * Has a member m_cb which is the callback to be added and registered with the event
   */
  class callback_2_0_wrapper {
  public:
    callback_2_0_wrapper(gs::reg_utils::void_callback *cb) : user_cb(cb) {
      m_cb = new gs::reg_utils::void_cpp_callback_2< callback_2_0_wrapper, gs::reg::transaction_type*, const sc_core::sc_time>();   
      m_cb->bind( this, &callback_2_0_wrapper::cb_func);
    }
    ~callback_2_0_wrapper() { delete m_cb; }
    void cb_func(transaction_type* &tr, const sc_core::sc_time& delay) {
      //GR_INFO("A (void-transaction-time) callback is converted from with-param-callback to paramless (void-void) callback.");
      user_cb->execute(); 
    }
    /// Member callback to be added and registered at event
    gs::reg_utils::void_cpp_callback_2< callback_2_0_wrapper, gs::reg::transaction_type*, const sc_core::sc_time>* m_cb;
  protected:
    gs::reg_utils::void_callback *user_cb;
  };
  std::vector<callback_2_0_wrapper*> m_callback_2_0_wrappers;
  
protected:
    
  friend class gr_sc_sensitive; // allows GR_SENSITIVE to add callback for GR_FUNCTION
  friend class gs::gs_param_greenreg<unsigned int>; // allows gs parameters to add callbacks
  friend class func_functor<transaction_type*>; // allows delayer to call notify_as_callback_been_delayed
  
  typedef gs::reg_utils::callback_2<void, transaction_type*, const sc_core::sc_time> callback_type;
  
  void add_callback( gs::reg_utils::callback_base & _callback);

  void add_payload_callback( gs::reg_utils::callback_base & _callback);

  gr_event( gr_event & _event) {
    assert(false && "should never reach here!"); exit(666);
  }

  /// Immediately makes the callback, called by notify functions
  void notify_as_callback();

  /// Immediately makes the callback, called by notify function getting transaction but no delay
  void notify_as_callback(transaction_type* const &_transaction);
  
  /// Called by delayer after the notify has been delayed
  void notify_as_callback_been_delayed(tlm::tlm_generic_payload* const &_tlm_transaction);

public:
  gr_event();
  
  virtual ~gr_event();

  void enable_timing();
  void disable_timing();
  bool is_timing_enabled();
  
  void cancel();

  void notify();
  void notify( const sc_core::sc_time& );
  void notify( double, sc_core::sc_time_unit );

  // internal
  
  /// Causes a notify (callback) with the transaction parameter
  void notify(transaction_type* _transaction);
  
  /// Causes a delayed notify (callback) with the transaction parameter
  void notify_with_delay(transaction_type* _transaction);

  // deprecated:
  void notify_delayed();
  void notify_delayed( const sc_core::sc_time& );
  void notify_delayed( double, sc_core::sc_time_unit );

//    gr_event_or_list&  operator | ( const gr_event& ) const;
//    gr_event_and_list& operator & ( const gr_event& ) const;
    
  /// Setter for delay
  void set_delay(const sc_core::sc_time delay);
  
  /// Getter for delay
  const sc_core::sc_time& get_delay() const;
  
protected:
  
  inline void legacy_event_switch_warning() {
    if (!legacy_warned) { 
      GR_FORCE_WARNING("DEPRECATED WARNING: You are using events in notification rules! \nDue to performance reasons you should use GR_FUNCTIONs not SC_METHODs \nand disable the event switch register-wise.\nThis warning will only occur one time!"); 
      legacy_warned = true; 
    }
  }
    
  /// If this event has enabled event notification. This can be set/modified register-wide (@see I_register).
  bool m_timed;
  
  typedef std::set< callback_type *> callback_storage_type;
  callback_storage_type m_callbacks;
  bool m_callback_registered;

  /// delay time this rule adds to a delayed notification
  sc_core::sc_time m_delay;
  /// If this event has been marked as delayed (and has a valid delay time been set)
  bool m_delayed;
  
  /// Functor peq delaying the call
  func_functor<tlm::tlm_generic_payload*> *m_delayer;
  /// Socket used for creating temporary GSGP transaction access handle
  gs::socket::extension_support_base<tlm::tlm_base_protocol_types>* m_ext_supp_base;
  
  static bool legacy_warned;
};

} // end namespace gs::reg
} // end namespace gs::reg

#endif /*_GR_EVENT_H_*/
