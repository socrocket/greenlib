// LICENSETEXT
// 
//   Copyright (C) 2008 : GreenSocs Ltd
//       http://www.greensocs.com/ , email: info@greensocs.com
// 
//   Developed by :
// 
//   Christian Schröder, Robert Guenzel
//     Technical University of Braunschweig, Dept. E.I.S.
//     http://www.eis.cs.tu-bs.de
// 
// 
// The contents of this file are subject to the licensing terms specified
// in the file LICENSE. Please consult this file for restrictions and
// limitations that may apply.
// 
// ENDLICENSETEXT


/// AckRequest functor for the GreenBus Socket, specialized for the GenericPhase
template <typename SOCKET> 
void AckRequestObj<SOCKET, GenericPhase>::operator()(typename SOCKET::accessHandle/*GenericSlaveAccessHandle*/ &th, const GenericPhase &ph, const sc_core::sc_time &d, unsigned int index) {
  GS_DUMP_N(this->socket_name(), "AckRequest (delay="<<d.to_string()<<")");
  // GreenBus phase RequestAccepted;
  sc_core::sc_time delay = d;
  
  tlm::tlm_generic_payload *tth = th->get_tlm_transaction();
  tlm::tlm_phase tph;
  GenericTransaction &genTr = th.get();
  
  // do not send ACK if transaction completed already
  if (soc->is_transaction_completed(*tth)) return;
  
  // synchronize transaction, phase GreenBus->TLM2.0
  ph.state = GenericPhase::RequestAccepted;
  synchronize_transaction_phase(genTr, ph, *tth, tph);
  
  // do not send if request was a write request of an OSCI master
  if ( !soc->is_connected_to_GPSocket(index) && tth->get_command() == tlm::TLM_WRITE_COMMAND) {
    // schedule DataValid to peq to be sent immediately to the user
    tph = BEGIN_DATA;
    // make all data sent by OSCI master valid for GreenBus (MSBystesValid)
    socket::extension_support_base<tlm::tlm_base_protocol_types>* ext_support = genTr.get_extension_support_base();
    ext_support->template get_extension<bytes_valid>(*tth)->value = tth->get_data_length();
    // enqueue to local peq, will be forwarded to user later
    soc->enqueue_to_fw_peq(*tth, tph, d, index); // delayed as the user delayed the AckRequest
    return;
  }
    
  GS_DUMP_N(this->socket_name(), "send " << gs::tlm_phase_writer::to_string(tph) << ", " <<
            gs::tlm_command_writer::to_string(tth->get_command()));
  // send transaction over GreenSocket
  tlm::tlm_sync_enum ret_type = (*soc)->nb_transport_bw(*tth, tph, delay);
  
  // handle TLM2.0 return path (for GreenBus AND OSCI case)
  switch (ret_type) {
    case tlm::TLM_ACCEPTED: // nothing needs to be done, return path not used
      break;
    case tlm::TLM_UPDATED: // phase and transaction may be updated
      if (   tph == tlm::BEGIN_REQ
          || tph == tlm::END_REQ
          || tph == tlm::BEGIN_RESP
          || tph == tlm::END_RESP
          || tph == BEGIN_DATA
          || tph == END_DATA) {
        // enqueue to local peq, will be forwarded to user later
        soc->enqueue_to_fw_peq(*tth, tph, delay, index);
      }
      // any unknown phase:
      else {
        // TLM2.0 ignorable phase!!
        // is the same as TLM_ACCEPTED!
      }
      break;
    case tlm::TLM_COMPLETED: // transaction completed by callee
      GS_DUMP_N(this->socket_name(), "transaction has been completed by master (RequestAccepted phase)");
      // set instance-specific extension to store the information that END_RESP must not be sent in this case!!
      soc->mark_transaction_completed(*tth);
      break;
    default:
      break;
  }  
}
template <typename SOCKET> 
void AckRequestObj<SOCKET, GenericPhase>::operator()(typename SOCKET::accessHandle &th, const GenericPhase &_p, unsigned int index) {
  operator()(th, _p, sc_core::SC_ZERO_TIME, index);
}
template <typename SOCKET> 
void AckRequestObj<SOCKET, GenericPhase>::operator()(typename SOCKET::accessHandle &th, const GenericPhase &_p, double d, sc_core::sc_time_unit u, unsigned int index) {
  operator()(th, _p, sc_core::sc_time(d, u), index);
}


/// ErrorRequest functor for the GreenBus Socket, specialized for the GenericPhase
template <typename SOCKET> 
void ErrorRequestObj<SOCKET, GenericPhase>::operator()(typename SOCKET::accessHandle &th, const GenericPhase &ph, const sc_core::sc_time &d, unsigned int index) {
  GS_DUMP_N(this->socket_name(), "ErrorRequest");
  // GreenBus phase RequestError;
  sc_core::sc_time delay = d;
  
  tlm::tlm_generic_payload *tth = th->get_tlm_transaction();
  tlm::tlm_phase tph;
  
  // cannot send this error message if transaction completed already
  assert(!soc->is_transaction_completed(*tth));
  
  // synchronize transaction, phase GreenBus->TLM2.0
  ph.state = GenericPhase::RequestError;
  synchronize_transaction_phase(th, ph, *tth, tph); // this is a BEGIN_RESP! // note: an interconnect may insert a END_REQ phase!
  
  // instance-specific extension: this transaction has been sent as an error by this slave!!
  soc->mark_transaction_sent_as_error(*tth);
  GS_DUMP_N(this->socket_name(), "transaction locally marked as error");

  GS_DUMP_N(this->socket_name(), "send " << gs::tlm_phase_writer::to_string(tph) << ", " <<
            gs::tlm_command_writer::to_string(tth->get_command()));
  // send transaction over GreenSocket
  tlm::tlm_sync_enum ret_type = (*soc)->nb_transport_bw(*tth, tph, delay);
  
  // handle TLM2.0 return path (for GreenBus AND OSCI case)
  switch (ret_type) {
    case tlm::TLM_ACCEPTED: // nothing needs to be done, return path not used
      break;
    case tlm::TLM_UPDATED:
      switch (tph) {
        case tlm::BEGIN_REQ:
        case tlm::END_REQ:
        case tlm::BEGIN_RESP:
          assert(false); exit(1); break;
        case tlm::END_RESP:
          soc->mark_transaction_completed(*tth);
          GS_DUMP_N(this->socket_name(), "transaction has been completed by updating phase by master (RequestError phase)");
          break;        
        default:
          if (tph == BEGIN_DATA || tph == END_DATA) {
            assert(false); exit(1); 
          } 
          // any unknown phase:
          else {
            // TLM2.0 ignorable phase!!
            // is the same as TLM_ACCEPTED!
          }
      }
      break;
    case tlm::TLM_COMPLETED: // nothing needs to be done, return path not used
      // set instance-specific extension to store the information that END_RESP must not be sent in this case!!
      soc->mark_transaction_completed(*tth);
      GS_DUMP_N(this->socket_name(), "transaction has been completed by master (RequestError phase)");
      break;
    default:
      break;
  }  
}
template <typename SOCKET> 
void ErrorRequestObj<SOCKET, GenericPhase>::operator()(typename SOCKET::accessHandle &th, const GenericPhase &_p, unsigned int index) {
  operator()(th, _p, sc_core::SC_ZERO_TIME, index);
}
template <typename SOCKET> 
void ErrorRequestObj<SOCKET, GenericPhase>::operator()(typename SOCKET::accessHandle &th, const GenericPhase &_p, double d, sc_core::sc_time_unit u, unsigned int index) {
  operator()(th, _p, sc_core::sc_time(d, u), index);
}


/// AckData functor for the GreenBus Socket, specialized for the GenericPhase
template <typename SOCKET> 
void AckDataObj<SOCKET, GenericPhase>::operator()(typename SOCKET::accessHandle &th, const GenericPhase &ph, const sc_core::sc_time &d, unsigned int index) {
  GS_DUMP_N(this->socket_name(), "AckData");
  // GreenBus phase DataAccepted;
  sc_core::sc_time delay = d;
  
  tlm::tlm_generic_payload *tth = th->get_tlm_transaction();
  tlm::tlm_phase tph;
  
  // do not send ACK if transaction completed already
  if (soc->is_transaction_completed(*tth)) return;

  // synchronize transaction, phase GreenBus->TLM2.0
  ph.state = GenericPhase::DataAccepted;
  synchronize_transaction_phase(th, ph, *tth, tph);
  
  assert(tth->get_command() == tlm::TLM_WRITE_COMMAND);
  
  // send a END_REQ instead of a DataAck (END_DATA) if master is OSCI
  if ( !soc->is_connected_to_GPSocket(index) ) {  // && tth->get_command() == tlm::TLM_WRITE_COMMAND (see assert)
    tph = tlm::END_REQ;
  }
  
  GS_DUMP_N(this->socket_name(), "send " << gs::tlm_phase_writer::to_string(tph) << ", " << gs::tlm_command_writer::to_string(tth->get_command()));
  // send transaction over GreenSocket
  tlm::tlm_sync_enum ret_type = (*soc)->nb_transport_bw(*tth, tph, delay);
  
  // handle TLM2.0 return path
  switch (ret_type) {
    case tlm::TLM_ACCEPTED: // nothing needs to be done, return path not used
      break;
    case tlm::TLM_COMPLETED: // transaction completed by callee
      GS_DUMP_N(this->socket_name(), "transaction has been completed by master (DataAccepted/END_REQ phase)");
      // set instance-specific extension to store the information that END_RESP must not be sent in this case!!
      soc->mark_transaction_completed(*tth);
      break;
    case tlm::TLM_UPDATED: // phase and transaction may be updated
      switch (tph) {
        case tlm::BEGIN_REQ:
        case tlm::END_REQ:
        case tlm::BEGIN_RESP:
        case tlm::END_RESP:
          assert(false); exit(1); break;
        default:
          // Handle new (last) data phase
          if (tph == BEGIN_DATA) {
            // enqueue to local peq, will be forwarded to user later
            soc->enqueue_to_fw_peq(*tth, tph, delay, index);
          }
          else if (tph == END_DATA) {
            assert(false); exit(1); 
          } 
          // any unknown phase:
          else {
            // TLM2.0 ignorable phase!!
            // is the same as TLM_ACCEPTED!
          }
      }
      break;
    default:
      break;
  }

  // send BEGIN_RESP to OSCI master to finish transaction
  if ( !soc->is_connected_to_GPSocket(index) && !soc->is_transaction_completed(*tth) 
      // only if not write response (if wr_resp: wait with this until user sends write resp!)
      && !soc->get_GSGP_config().use_wr_resp) {
    tph = tlm::BEGIN_RESP;
    tth->set_response_status(tlm::TLM_OK_RESPONSE);

    GS_DUMP_N(this->socket_name(), "send" << gs::tlm_phase_writer::to_string(tph) << ", " <<
              gs::tlm_command_writer::to_string(tth->get_command()) << " with TLM_OK_RESPONSE");
    // send transaction over GreenSocket
    // delay must stay at least the same as the previously sent END_REQ !!
    tlm::tlm_sync_enum ret_type = (*soc)->nb_transport_bw(*tth, tph, delay);
    
    // handle TLM2.0 return path
    switch (ret_type) {
      case tlm::TLM_ACCEPTED: // nothing needs to be done, return path not used
        break;
      case tlm::TLM_COMPLETED: // transaction completed by callee
        GS_DUMP_N(this->socket_name(), "transaction has been completed by master (DataAccepted/BEGIN_RESP phase)");
        // set instance-specific extension to store the information that END_RESP must not be sent in this case!!
        soc->mark_transaction_completed(*tth);
        break;
      case tlm::TLM_UPDATED: // phase and transaction may be updated
        switch (tph) {
          case tlm::BEGIN_REQ:
          case tlm::END_REQ:
          case tlm::BEGIN_RESP:
            assert(false); exit(1); break;
          case tlm::END_RESP: // allowed!
            break;
          default:
            if (tph == BEGIN_DATA || tph == END_DATA) {
              assert(false); exit(1); 
            } 
            // any unknown phase:
            else {
              // TLM2.0 ignorable phase!!
              // is the same as TLM_ACCEPTED!
            }
        }
        break;
      default:
        break;
    }
  }
}
template <typename SOCKET> 
void AckDataObj<SOCKET, GenericPhase>::operator()(typename SOCKET::accessHandle &th, const GenericPhase &_p, unsigned int index) {
  operator()(th, _p, sc_core::SC_ZERO_TIME, index);
}
template <typename SOCKET> 
void AckDataObj<SOCKET, GenericPhase>::operator()(typename SOCKET::accessHandle &th, const GenericPhase &_p, double d, sc_core::sc_time_unit u, unsigned int index) {
  operator()(th, _p, sc_core::sc_time(d, u), index);
}


/// ErrorData functor for the GreenBus Socket, specialized for the GenericPhase
template <typename SOCKET> 
void ErrorDataObj<SOCKET, GenericPhase>::operator()(typename SOCKET::accessHandle &th, const GenericPhase &ph, const sc_core::sc_time &d, unsigned int index) {
  GS_DUMP_N(this->socket_name(), "ErrorData");
  // GreenBus phase DataError;
  sc_core::sc_time delay = d;
  
  tlm::tlm_generic_payload *tth = th->get_tlm_transaction();
  tlm::tlm_phase tph;
  
  // cannot send this error message if transaction completed already
  assert(!soc->is_transaction_completed(*tth));
  
  // synchronize transaction, phase GreenBus->TLM2.0
  if (soc->is_connected_to_GPSocket(index))
    ph.state = GenericPhase::DataError;
  else
    ph.state = GenericPhase::RequestError;
  synchronize_transaction_phase(th, ph, *tth, tph);
  
  // instance-specific extension: this transaction has been sent as an error by this slave!!
  soc->mark_transaction_sent_as_error(*tth);
  GS_DUMP_N(this->socket_name(), "transaction locally marked as error");

  GS_DUMP_N(this->socket_name(), "send " << gs::tlm_phase_writer::to_string(tph) << ", " <<
           gs::tlm_command_writer::to_string(tth->get_command()));
  // send transaction over GreenSocket
  tlm::tlm_sync_enum ret_type = (*soc)->nb_transport_bw(*tth, tph, delay);
  
  // handle TLM2.0 return path (for GreenBus AND OSCI case)
  switch (ret_type) {
    case tlm::TLM_ACCEPTED: // nothing needs to be done, return path not used
      break;
    case tlm::TLM_COMPLETED: // nothing needs to be done, return path not used
      GS_DUMP_N(this->socket_name(), "transaction has been completed by master (DataError phase)");
      // set instance-specific extension to store the information that END_RESP must not be sent in this case!!
      soc->mark_transaction_completed(*tth);
      break;
    case tlm::TLM_UPDATED:
      switch (tph) {
        case tlm::BEGIN_REQ:
        case tlm::END_REQ:
        case tlm::BEGIN_RESP:
          assert(false); exit(1); break;
        case tlm::END_RESP:
          assert(!soc->is_connected_to_GPSocket(index));
          GS_DUMP_N(this->socket_name(), "transaction has been completed by master by updating (DataError phase)");
          // set instance-specific extension to store the information that END_RESP must not be sent in this case!!
          soc->mark_transaction_completed(*tth);
          break;
        default:
          if (tph == BEGIN_DATA || tph == END_DATA) {
            assert(false); exit(1); 
          } 
          // any unknown phase:
          else {
            // TLM2.0 ignorable phase!!
            // is the same as TLM_ACCEPTED!
          }
      }
      break;
    default:
      break;
  }  
}
template <typename SOCKET> 
void ErrorDataObj<SOCKET, GenericPhase>::operator()(typename SOCKET::accessHandle &th, const GenericPhase &_p, unsigned int index) {
  operator()(th, _p, sc_core::SC_ZERO_TIME, index);
}
template <typename SOCKET> 
void ErrorDataObj<SOCKET, GenericPhase>::operator()(typename SOCKET::accessHandle &th, const GenericPhase &_p, double d, sc_core::sc_time_unit u, unsigned int index) {
  operator()(th, _p, sc_core::sc_time(d, u), index);
}


/// Response functor for the GreenBus Socket, specialized for the GenericPhase
template <typename SOCKET> 
void ResponseObj<SOCKET, GenericPhase>::operator()(typename SOCKET::accessHandle &th, const GenericPhase &ph, const sc_core::sc_time &d, unsigned int index) {
  GS_DUMP_N(this->socket_name(), "Response (delay="<<d.to_string()<<", status=" 
            << th->get_tlm_transaction()->get_response_string() << ", bytes_valid="<<ph.getBytesValid());
  // GreenBus phase ResponseValid
  sc_core::sc_time delay = d;
  
  tlm::tlm_generic_payload *tth = th->get_tlm_transaction();
  tlm::tlm_phase tph;
  
  // error if transaction completed already
  assert(!soc->is_transaction_completed(*tth));
  // error is response status was not set
  assert(tth->get_response_status() != tlm::TLM_INCOMPLETE_RESPONSE); // the user must setSResp!
  // check if this is a read - or a write with enabled write response configuration
  assert(tth->get_command() == tlm::TLM_READ_COMMAND
         || ( tth->get_command() == tlm::TLM_WRITE_COMMAND && soc->get_GSGP_config().use_wr_resp ) );
  
  // synchronize transaction, phase GreenBus->TLM2.0
  ph.state = GenericPhase::ResponseValid;
  synchronize_transaction_phase(th, ph, *tth, tph);
  
  // Handle not-GreenBus initiator
  if ( !soc->is_connected_to_GPSocket(index) ) {
    // if last data chunk, send (the only) tlm response phase, else return (and wait for last Response!)
    if (ph.getBytesValid() >= th->getMBurstLength())
      tph = tlm::BEGIN_RESP;
    else {
      soc->template get_extension<bytes_valid>(*tth)->value = ph.getBytesValid();
      tph = tlm::END_RESP; // = AckResponse
      // enqueue to local peq, will be forwarded to user later
      soc->enqueue_to_fw_peq(*tth, tph, delay, index);
      return;
    }
  }
  
  GS_DUMP_N(this->socket_name(), "send " << gs::tlm_phase_writer::to_string(tph) << ", " <<
            gs::tlm_command_writer::to_string(tth->get_command()));
  // send transaction over GreenSocket
  tlm::tlm_sync_enum ret_type = (*soc)->nb_transport_bw(*tth, tph, delay);
  
  // handle TLM2.0 return path (for GreenBus AND OSCI case)
  switch (ret_type) {
    case tlm::TLM_ACCEPTED: // nothing needs to be done, return path not used
      break;
    case tlm::TLM_UPDATED: // phase and transaction may be updated
      switch (tph) {
        case tlm::BEGIN_REQ:
        case tlm::END_REQ:
        case tlm::BEGIN_RESP:
          assert(false); exit(1); break;
        // Master cannot answer with any other phase (only this slave is allowed to send further Responses)
        case tlm::END_RESP:
          // enqueue to local peq, will be forwarded to user later
          soc->enqueue_to_fw_peq(*tth, tph, delay, index);
          break;
        default:
          if (tph == BEGIN_DATA || tph == END_DATA) {
            assert(false); exit(1); 
          } 
          // any unknown phase:
          else {
            // TLM2.0 ignorable phase!!
            // is the same as TLM_ACCEPTED!
          }
      }
      break;
    case tlm::TLM_COMPLETED: // transaction completed by callee
      GS_DUMP_N(this->socket_name(), "transaction has been completed by master (Response phase), send fake ResponseAccepted to user");
      // enqueue to local peq, will be forwarded to user later
      tph = tlm::END_RESP;
      soc->enqueue_to_fw_peq(*tth, tph, delay, index);    // End response
      // set instance-specific extension to store the information that END_RESP must not be sent in this case!!
      soc->mark_transaction_completed(*tth);
      break;
    default:
      break;
  }  
}
template <typename SOCKET> 
void ResponseObj<SOCKET, GenericPhase>::operator()(typename SOCKET::accessHandle &th, const GenericPhase &_p, unsigned int index) {
  operator()(th, _p, sc_core::SC_ZERO_TIME, index);
}
template <typename SOCKET> 
void ResponseObj<SOCKET, GenericPhase>::operator()(typename SOCKET::accessHandle &th, const GenericPhase &_p, double d, sc_core::sc_time_unit u, unsigned int index) {
  operator()(th, _p, sc_core::sc_time(d, u), index);
}



template <unsigned int BUSWIDTH, typename TRANSACTION, typename CONFIG, typename PHASE, bool BIDIR, typename SOCK_TYPE>
inline tlm::tlm_sync_enum GSGPSlaveSocket<BUSWIDTH, TRANSACTION, CONFIG, PHASE, BIDIR, SOCK_TYPE>::
fw_nb_transport_cb(unsigned int index, tlm::tlm_generic_payload& tlmtr, 
                   tlm::tlm_phase& tph, sc_core::sc_time& delay) {

  // ////////// BEGIN GSGP Socket ///////////////////////////////////////////////// //
  if (base_socket_type::is_connected_to_GPSocket(index)) { 
    
    GS_DUMP_N(base_socket_type::socket_name(), "got " << gs::tlm_phase_writer::to_string(tph) << ", " <<
              gs::tlm_command_writer::to_string(tlmtr.get_command()) << 
              (base_socket_type::has_error(tlmtr)? ", ERROR!" : ", no error") << 
              " from socket #" << index << " (GSGP-Socket)");
    
    // reset instance-specific completed extension
    base_socket_type::reset_transaction_completed(tlmtr);
    
   
    // remember: each time before sending check is_transaction_completed(tlmtr)
    
    switch (tph) {
      case tlm::BEGIN_REQ:
        // reset instance specific error extension if this is a new transaction
        base_socket_type::reset_transaction_sent_as_error(tlmtr); // reset instance-specific extension
      case tlm::END_REQ:
      case tlm::BEGIN_RESP:
        break;
      case tlm::END_RESP:
        // if this is a END_RESP to a previously sent error phase (ResponseError)
        if (base_socket_type::has_error(tlmtr) && base_socket_type::is_transaction_sent_as_error(tlmtr)) {
          // ignore this phase, drop!
          return tlm::TLM_ACCEPTED;
        }
        break;
      default:
        if (tph == BEGIN_DATA) {}
        else if (tph == END_DATA) {}
        else // TLM2.0 ignorable phase!!!
          return tlm::TLM_ACCEPTED;
    }

    // enqueue to local peq, will be forwarded to user later
    enqueue_to_fw_peq(tlmtr, tph, delay, index);

    return tlm::TLM_ACCEPTED;
    
  }    // ////////// END GSGP Socket ///////////////////////////////////////////////// //
  else // ////////// BEGIN NOT GSGP Socket ///////////////////////////////////////////////// //
  {

    GS_DUMP_N(base_socket_type::socket_name(), "got " << gs::tlm_phase_writer::to_string(tph) << ", " <<
              gs::tlm_command_writer::to_string(tlmtr.get_command()) << 
              (base_socket_type::has_error(tlmtr)? ", ERROR!" : ", no error") << 
              " from socket #" << index << " (non-GSGP-Socket)");
    
    tlm::tlm_sync_enum ret = tlm::TLM_ACCEPTED;

    // reset instance-specific completed extension
    base_socket_type::reset_transaction_completed(tlmtr);
    
    // remember: each time before sending check is_transaction_completed(tlmtr)
    
    // Handle not-GreenBus target
    {
      
      switch (tph) {
        case tlm::BEGIN_REQ:
          // reset instance specific error and phase status extension if this is a new transaction
          base_socket_type::reset_transaction_sent_as_error(tlmtr); // reset instance-specific extension
        case tlm::END_REQ:
        case tlm::BEGIN_RESP:
          break;
        case tlm::END_RESP:
          // if this is a END_RESP to a previously sent error phase (ResponseError)
          if (base_socket_type::has_error(tlmtr) && base_socket_type::is_transaction_sent_as_error(tlmtr)) {
            // ignore this phase, drop!
            return tlm::TLM_ACCEPTED;
          }
          break;
          default:
          if (tph == BEGIN_DATA) {}
          else if (tph == END_DATA) {}
          else // TLM2.0 ignorable phase!!!
            return tlm::TLM_ACCEPTED;
      }

      // ///////////////  Conversion block  ////////////////////// //
      // Do all conversions that are needed for connecting different 
      // protocols to each other here:
      
      // since this is sent by an osci master, force the bytes valid to all data
      GS_DUMP_N(this->socket_name(), "Setting bV 1");
      this->template get_extension<bytes_valid>(tlmtr)->value = tlmtr.get_data_length();
      
      
      // do nothing (but completing the transaction) if this is a END_RESP
      // of an OSCI master which finishes a write command
      if ( tph == tlm::END_RESP  &&  tlmtr.get_command() == tlm::TLM_WRITE_COMMAND ) {
        tlmtr.set_response_status(tlm::TLM_OK_RESPONSE);
        ret = tlm::TLM_COMPLETED;
        if (!base_socket_type::get_GSGP_config().use_wr_resp){
          return ret; 
        }
        // else enqueue
      }
      
      // ///////////////  END Conversion block  ////////////////// //
      
      // enqueue to local peq, will be forwarded to user after the delay
      enqueue_to_fw_peq(tlmtr, tph, delay, index);
      
    }
    return ret;

  } // ////////// END NOT GSGP Socket ///////////////////////////////////////////////// //
}


template <unsigned int BUSWIDTH, typename TRANSACTION, typename CONFIG, typename PHASE, bool BIDIR, typename SOCK_TYPE>
inline void GSGPSlaveSocket<BUSWIDTH, TRANSACTION, CONFIG, PHASE, BIDIR, SOCK_TYPE>::
fw_b_transport_cb(unsigned int index, tlm::tlm_generic_payload& tlmtr, sc_core::sc_time& time) { 
  // do b_transport only if all nb_transport calls are done
  if (m_outstanding_nb_count > 0) {
    sc_core::wait(m_outstanding_nb_drained);
  }

  accessHandle th(this, &tlmtr);

  // check if to call with index (multiport)
  if (is_multi_port) {
    // if loosely timed, don't care about the time
    if (!at_mode && has_f) {
      multi_f_in->f_transact(th, index);
    }
    else {
      // call the time-decoupled version if registered...
      if (has_td_b) {
        td_multi_b_in->b_transact(th, time, index);
      }
      // ... or wait before the non-TD call
      else if (has_b) {
        if (time != sc_core::SC_ZERO_TIME)
          sc_core::wait(time);
        multi_b_in->b_transact(th, index);
      }
      else {
        SC_REPORT_ERROR(socket_type::name(), "This socket does not implement b_transport interface, as neither f_transport() nor b_transport() are registered.");
      }
    }
  }

  // must be single port then
  else {
    // if loosely timed, don't care about the time
    if (!at_mode && has_f) {
      f_in->f_transact(th);
    }
    else {
      // call the time-decoupled version if registered...
      if (has_td_b) {
        td_b_in->b_transact(th, time);
      }
      // ... or wait before the non-TD call
      else if (has_b) {
        if (time != sc_core::SC_ZERO_TIME)
          sc_core::wait(time);
        b_in->b_transact(th);
      }
      else {
        SC_REPORT_ERROR(socket_type::name(), "This socket does not implement b_transport interface, as neither f_transport() nor b_transport() are registered.");
      }
    }
  }

  // TODO: was passiert hier?
  if (th->getSResp() == gs::Generic_SRESP_NULL) {
    th->setSResp(gs::Generic_SRESP_DVA);
  }
}


template <unsigned int BUSWIDTH, typename TRANSACTION, typename CONFIG, typename PHASE, bool BIDIR, typename SOCK_TYPE>
inline void GSGPSlaveSocket<BUSWIDTH, TRANSACTION, CONFIG, PHASE, BIDIR, SOCK_TYPE>::
enqueue_to_fw_peq(tlm::tlm_generic_payload& tlmtr, tlm::tlm_phase& tph, const sc_core::sc_time& delay, unsigned int index) {
  // acquire, release is called in function fw_peq_cb
  //  acquire is done for each phase although it is only needed for the
  //  END_RESP phase. Checking this here would be less efficient and
  //  setting the acquire distributed in the code would complicate 
  //  maintainability of the code!
  tlmtr.acquire();
  // enqueue
  m_outstanding_nb_count++;
  m_fw_peq.notify(tlmtr, tph, delay, index);
}


template <unsigned int BUSWIDTH, typename TRANSACTION, typename CONFIG, typename PHASE, bool BIDIR, typename SOCK_TYPE>
inline void GSGPSlaveSocket<BUSWIDTH, TRANSACTION, CONFIG, PHASE, BIDIR, SOCK_TYPE>::
fw_peq_cb(tlm::tlm_generic_payload& tlmtr, const tlm::tlm_phase& tph, unsigned int index) {
  // simply forward the call of the incoming tlm peq to the user
  accessHandle th(this, &tlmtr);
  GenericTransaction &genTr = th.get();
  
  synchronize_transaction_phase(tlmtr, tph, genTr, m_phase, false); 

  atom slaa(th, m_phase);
  GS_DUMP_N(this->socket_name(), "Send "<<m_phase.to_string());

  // check if to call with index (multiport)
  if (is_multi_port) {
    // if loosely timed or don't have nb, call wrapper to f_transact
    if (has_f && (!at_mode || !has_nb))
      nb_from_f(slaa, index);
    else if (has_nb)
      peq.out_multi_port->notify(slaa, index); // forward to user
    else
      SC_REPORT_ERROR(socket_type::name(), "This socket does not implement nb_transport interface, as neither f_transact() nor notify() are registered.");
  }

  // must be single port then
  else {
    // if loosely timed or don't have nb, call wrapper to f_transact
    if (has_f && (!at_mode || !has_nb))
      nb_from_f(slaa, 0);
    else if (has_nb)
      peq.out_port->notify(slaa); // forward to user
    else
      SC_REPORT_ERROR(socket_type::name(), "This socket does not implement nb_transport interface, as neither f_transact() nor notify() are registered.");
  }

  // release the transaction, acquire see enqueue_to_fw_peq function!
  tlmtr.release();

  // handle the outstanding transaction count
  m_outstanding_nb_count--;
  if (m_outstanding_nb_count == 0) {
    m_outstanding_nb_drained.notify();
  }
}


// CS: This is unclear code: Here we must not use getMCmd and the AckRequest (etc) functors
//     because we must not assume this uses the GenericPhase; e.g. PCIe does not compile 
//     with this in its original way - needed to be hacked to work with this!
template <unsigned int BUSWIDTH, typename TRANSACTION, typename CONFIG, typename PHASE, bool BIDIR, typename SOCK_TYPE>
inline void GSGPSlaveSocket<BUSWIDTH, TRANSACTION, CONFIG, PHASE, BIDIR, SOCK_TYPE>::
nb_from_f(atom &tc, unsigned int index) {

  accessHandle tah=tc.first;//_getSlaveAccessHandle(tc);
  phase p=tc.second;

  switch (p.state) {
    
  case GenericPhase::RequestValid:
    // send AckRequest
    AckRequest(tah, (GenericPhase)p);

    if (tah->getMCmd() == Generic_MCMD_RD) {

      // do the read
      if (is_multi_port)
        multi_f_in->f_transact(tah, index);
      else
        f_in->f_transact(tah);

      // response
      p.setBytesValid( tah->getMBurstLength() );
      tah->setSResp(Generic_SRESP_DVA);
      Response(tah, p);
    }

    break;

  case GenericPhase::DataValid:
    // do the write
    if (is_multi_port)
      multi_f_in->f_transact(tah, index);
    else
      f_in->f_transact(tah);
    // ack data
    AckData(tah,p);
    break;

  case GenericPhase::DataAccepted:
  case GenericPhase::ResponseAccepted:
    break;

  default:
    std::stringstream ss; ss << "error: phase " << p.to_string();
    SC_REPORT_WARNING("GSGPSlaveSocket::nb_from_f",ss.str().c_str());
    break;
  }
}


template <unsigned int BUSWIDTH, typename TRANSACTION, typename CONFIG, typename PHASE, bool BIDIR, typename SOCK_TYPE>
inline void GSGPSlaveSocket<BUSWIDTH, TRANSACTION, CONFIG, PHASE, BIDIR, SOCK_TYPE>::
start_of_simulation() {
  base_socket_type::start_of_simulation();

  // sync base_addr and high_addr parameters
  gs::socket::multi_target_base<BUSWIDTH>* target = dynamic_cast<gs::socket::multi_target_base<BUSWIDTH>*>(this);
  if (target) {
    if (target->get_hierarch_bind() != this) {
      GreenSocketAddress_base* other = dynamic_cast<GreenSocketAddress_base*>(target->get_hierarch_bind());
      base_addr_sync.bind(&base_addr, &other->base_addr);
      high_addr_sync.bind(&high_addr, &other->high_addr);
    }
  }

  // register callback to this Socket at the GreenSocket
  socket_type::register_nb_transport_fw(this, &my_type::fw_nb_transport_cb);

  // check if single port or multi port depending on registered callbacks
  is_single_port = ((f_in.operator ->() != NULL) || (peq.out_port.operator ->() != NULL) ||
                    (b_in.operator ->() != NULL) || (td_b_in.operator ->() != NULL));
  is_multi_port = ((multi_f_in.operator ->() != NULL) || (peq.out_multi_port.operator ->() != NULL) ||
                   (multi_b_in.operator ->() != NULL) || (td_multi_b_in.operator ->() != NULL));

  // error if no callback is bound
  if (!is_single_port && !is_multi_port) {
    // TODO: not an error, as it can be hierarchy bound. Is there a way to check if no hierarchy bound and then issue some error?
    //SC_REPORT_ERROR(socket_type::name(), "The callbacks for nb_transport or b_transport are not bound. Please consult the documentation.");
  }

  // error if mixed single and multi port
  if (is_single_port && is_multi_port) {
    SC_REPORT_ERROR(socket_type::name(), "Callbacks registered for both single and multi port. Please check your design.");
  }

  // check registered interfaces
  has_f = (f_in.operator ->() != NULL) || (multi_f_in.operator ->() != NULL);
  has_b = (b_in.operator ->() != NULL) || (multi_b_in.operator ->() != NULL);
  has_td_b = (td_b_in.operator ->() != NULL) || (td_multi_b_in.operator ->() != NULL);
  has_nb = (peq.out_port.operator ->() != NULL) || (peq.out_multi_port.operator ->() != NULL);

  // TODO: do the following checks only when not hierarchically bound
  if (0) {
    // check the non-blocking interface
    if (!has_f && !has_nb) {
      SC_REPORT_WARNING(socket_type::name(), "This socket does not implement nb_transport interface, as neither f_transport() nor notify() are registered.");
    }

    // check the blocking interface
    if (!has_f && !has_b && !has_td_b) {
      SC_REPORT_WARNING(socket_type::name(), "This socket does not implement b_transport interface, as neither f_transport() nor b_transport() are registered.");
    }
  }
}

template <unsigned int BUSWIDTH, typename TRANSACTION, typename CONFIG, typename PHASE, bool BIDIR, typename SOCK_TYPE>
void GSGPSlaveSocket<BUSWIDTH, TRANSACTION, CONFIG, PHASE, BIDIR, SOCK_TYPE>::
bind_b_if(b_if_type& other) {
  // register blocking callback function to this Socket at the GreenSocket
  socket_type::register_b_transport(this, &my_type::fw_b_transport_cb);

  b_in(other);
}

template <unsigned int BUSWIDTH, typename TRANSACTION, typename CONFIG, typename PHASE, bool BIDIR, typename SOCK_TYPE>
void GSGPSlaveSocket<BUSWIDTH, TRANSACTION, CONFIG, PHASE, BIDIR, SOCK_TYPE>::
bind_b_if(multi_b_if_type& other) {
  // register blocking callback function to this Socket at the GreenSocket
  socket_type::register_b_transport(this, &my_type::fw_b_transport_cb);

  multi_b_in(other);
}

template <unsigned int BUSWIDTH, typename TRANSACTION, typename CONFIG, typename PHASE, bool BIDIR, typename SOCK_TYPE>
void GSGPSlaveSocket<BUSWIDTH, TRANSACTION, CONFIG, PHASE, BIDIR, SOCK_TYPE>::
bind_b_if(td_b_if_type& other) {
  // register blocking callback function to this Socket at the GreenSocket
  socket_type::register_b_transport(this, &my_type::fw_b_transport_cb);

  td_b_in(other);
}

template <unsigned int BUSWIDTH, typename TRANSACTION, typename CONFIG, typename PHASE, bool BIDIR, typename SOCK_TYPE>
void GSGPSlaveSocket<BUSWIDTH, TRANSACTION, CONFIG, PHASE, BIDIR, SOCK_TYPE>::
bind_b_if(td_multi_b_if_type& other) {
  // register blocking callback function to this Socket at the GreenSocket
  socket_type::register_b_transport(this, &my_type::fw_b_transport_cb);

  td_multi_b_in(other);
}

template <unsigned int BUSWIDTH, typename TRANSACTION, typename CONFIG, typename PHASE, bool BIDIR, typename SOCK_TYPE>
void GSGPSlaveSocket<BUSWIDTH, TRANSACTION, CONFIG, PHASE, BIDIR, SOCK_TYPE>::
bind_f_if(f_if_type& other) {
  // register blocking callback function to this Socket at the GreenSocket
  socket_type::register_b_transport(this, &my_type::fw_b_transport_cb);

  f_in(other);
}

template <unsigned int BUSWIDTH, typename TRANSACTION, typename CONFIG, typename PHASE, bool BIDIR, typename SOCK_TYPE>
void GSGPSlaveSocket<BUSWIDTH, TRANSACTION, CONFIG, PHASE, BIDIR, SOCK_TYPE>::
bind_f_if(multi_f_if_type& other) {
  // register blocking callback function to this Socket at the GreenSocket
  socket_type::register_b_transport(this, &my_type::fw_b_transport_cb);

  multi_f_in(other);
}
