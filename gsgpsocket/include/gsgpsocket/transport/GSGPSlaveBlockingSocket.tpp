// LICENSETEXT
// 
//   Copyright (C) 2008 : GreenSocs Ltd
//       http://www.greensocs.com/ , email: info@greensocs.com
// 
//   Developed by :
// 
//   Christian Schroeder, Robert Guenzel
//     Technical University of Braunschweig, Dept. E.I.S.
//     http://www.eis.cs.tu-bs.de
// 
// 
// The contents of this file are subject to the licensing terms specified
// in the file LICENSE. Please consult this file for restrictions and
// limitations that may apply.
// 
// ENDLICENSETEXT

#ifdef GS_BLOCKING_FRAGMENT1
# error Macro GS_ BLOCKING_ FRAGMENT1 already defined!
#endif
#ifdef GS_BLOCKING_FRAGMENT2
# error Macro GS_ BLOCKING_ FRAGMENT2 already defined!
#endif

#define GS_BLOCKING_FRAGMENT1(t, ph) \
  sc_core::sc_event myEvent;                                     \
  typename Blocking_socket_base_accessor_proxy<SOCKET>::waitid waitID = this->get_waiters().insert( \
   std::pair<tlm::tlm_generic_payload*, typename Blocking_socket_base_accessor_proxy<SOCKET>::waitpayload> \
   (th->get_tlm_transaction(), typename Blocking_socket_base_accessor_proxy<SOCKET>::waitpayload(&myEvent, &ph)) \
   ).first

#define GS_BLOCKING_FRAGMENT2(out)      \
  wait(*(waitID->second.first));   \
  ph = *(waitID->second.second);   \
  this->get_waiters().erase(waitID);      \
  return ph


/// Blocking Response functor for the GreenBus Socket, specialized for the GenericPhase
template <typename SOCKET> 
GenericPhase ResponseBlockObj<SOCKET, GenericPhase>::block(typename SOCKET::accessHandle &th, GenericPhase &ph, const sc_core::sc_time &d, unsigned int index) {
  GS_BLOCKING_FRAGMENT1(th, ph);
  operator()(th, ph, d, index);
  GS_BLOCKING_FRAGMENT2(th);
}
template <typename SOCKET> 
GenericPhase ResponseBlockObj<SOCKET, GenericPhase>::block(typename SOCKET::accessHandle &th, GenericPhase &ph, unsigned int index ) {
  return block(th, ph, sc_core::SC_ZERO_TIME, index);
}
template <typename SOCKET> 
GenericPhase ResponseBlockObj<SOCKET, GenericPhase>::block(typename SOCKET::accessHandle &th, GenericPhase &ph, double d, sc_core::sc_time_unit u, unsigned int index) {
  return block(th, ph, sc_core::sc_time(d,u), index);
}

// TODO: GSGPMasterBlockingSocket::notify is exactly the same (expect m_bw_peq/m_fw_peq)! combine!
template <unsigned int BUSWIDTH, typename TRANSACTION, typename CONFIG, typename PHASE>
inline void GSGPSlaveBlockingSocket<BUSWIDTH, TRANSACTION, CONFIG, PHASE>::
notify(slave_atom& tc, unsigned int index)
{ 
  GS_DUMP_N(socket_name(), "Blocking Socket got " << tc.second.to_string() << ", " << 
            gs::tlm_command_writer::to_string(tc.first->get_tlm_transaction()->get_command()) << "(notify on socket #"<<index<<")");
  
  tlm::tlm_generic_payload *tlmtrptr = tc.first.get_tlm_transaction();
  // Make sure that a blocking function is not release at the same delta with a subsequent phase notified to the user
  if (sc_core::sc_delta_count() != last_delta)
    released_blockings_in_this_delta.clear();
  last_delta = sc_core::sc_delta_count();
  // if  this transaction is in the set (the blocking function has already been released within this delta cycle)
  if (released_blockings_in_this_delta.find(tlmtrptr) != released_blockings_in_this_delta.end()) {
    // synchronize transaction, phase GreenBus->TLM2.0
    tlm::tlm_phase tph;
    synchronize_transaction_phase(tc.first, tc.second, *tlmtrptr, tph);
    // re-enque this transaction into peq with SC_ZERO_TIME;
    base_type::m_fw_peq.notify(*tlmtrptr, tph, sc_core::SC_ZERO_TIME, index);
    return;
  }
  
  typename GSGPSocketBlockingBase<base_type>::waitid 
  id = GSGPSocketBlockingBase<base_type>::waiters.find(tlmtrptr);
  // if this callback is a release of a blocking call
  if (id!=blocking_base_type::waiters.end()) {
    // add this transaction to set released_blockings_in_this_delta
    released_blockings_in_this_delta.insert(tlmtrptr);
    // notify blocking call (not user)
    *(id->second.second) = tc.second;
    id->second.first->notify();
  } else {
    // simply forward the call of the incoming tlm peq to the user
    if (out_multi_port.operator->()!=NULL)
      out_multi_port->notify(tc, index); // forward immediately to user
    else      
      out_port->notify(tc); // forward immediately to user
  }
  
}

#undef GS_BLOCKING_FRAGMENT1
#undef GS_BLOCKING_FRAGMENT2
