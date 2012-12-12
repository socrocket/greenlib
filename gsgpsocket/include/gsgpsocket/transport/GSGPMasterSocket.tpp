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


/// Request functor for the GreenBus Socket, specialized for the GenericPhase
template <typename SOCKET> 
void RequestObj<SOCKET, GenericPhase>::operator()(typename SOCKET::accessHandle &th, 
                                                  const GenericPhase &ph, const sc_core::sc_time &d, unsigned int index) {
  GS_DUMP_N(this->socket_name(), "Request");
  
  // GreenBus phase RequestValid
  sc_core::sc_time delay = d;
  GenericTransaction &genTr = th.get();
  
  tlm::tlm_generic_payload *tth = th->get_tlm_transaction();
  tlm::tlm_phase tph;
  
  // reset instance-specific extension
  soc->reset_transaction_completed(*tth);
  
  soc->mark_transaction_is_in_req_cycle(*tth);
  
  // synchronize transaction, phase GreenBus->TLM2.0
  ph.state = GenericPhase::RequestValid;
  synchronize_transaction_phase(genTr, ph, *tth, tph);

  // Handle not-GreenBus target
  if ( !soc->is_connected_to_GPSocket(index) ) {
    // delay the tlm request phase until last GreenBus data phase
    if (tth->get_command() == tlm::TLM_WRITE_COMMAND) {
      // enqueue to local peq
      tph = tlm::END_REQ;
      soc->enqueue_to_bw_peq(*tth, tph, delay, index);
      return;
    }
  }

  GS_DUMP_N(this->socket_name(), "send "<< gs::tlm_phase_writer::to_string(tph) << ", " << 
           gs::tlm_command_writer::to_string(tth->get_command()));
  // send transaction over GreenSocket
  tlm::tlm_sync_enum ret_type = (*soc)->nb_transport_fw(*tth, tph, delay);
  
  // handle TLM2.0 return path
  switch (ret_type) {
    case tlm::TLM_ACCEPTED: // nothing needs to be done, return path not used
      break;
    case tlm::TLM_UPDATED: // phase and transaction may be updated
      switch (tph) {
        case tlm::BEGIN_REQ:
          assert(false); exit(1); break;
        case tlm::END_REQ:
          soc->reset_transaction_is_in_req_cycle(*tth);
          // only keep/send END_REQ if there was no error
          // if END_REQ with error -> convert to BEGIN_RESP with error
          if(soc->has_error(*tth)) {
            GS_DUMP_N(this->socket_name(),"BEGIN_REQ with ERROR");
            tph = tlm::BEGIN_RESP;
          }
          // enqueue END_REQ or BEGIN_RESP to local peq
          soc->enqueue_to_bw_peq(*tth, tph, delay, index);
          break;
        case tlm::BEGIN_RESP:
          // only send END_REQ if there was no error
          if (!soc->has_error(*tth)) {
            // enqueue to local peq
            tph = tlm::END_REQ;
            soc->enqueue_to_bw_peq(*tth, tph, delay, index);    // End request
          }
          else{ //error
            if(soc->transaction_is_within_req_cycle(*tth)){ //then it is request error and we better finalize NOW
              tph=tlm::END_RESP;
              delay=sc_core::SC_ZERO_TIME;
              (*soc)->nb_transport_fw(*tth, tph, delay); //return is either ACCEPTED or COMPLETED or UPDATED with an ignorable phase, so we can ignore it
            }
              
          }
          tph = tlm::BEGIN_RESP;
          soc->enqueue_to_bw_peq(*tth, tph, delay, index); // Begin response
          break;
        case tlm::END_RESP:
          assert(false); exit(1); break;
        default:
          if (tph == BEGIN_DATA || tph == END_DATA) {
            assert(false); exit(1); break;
          } 
          // any unknown phase:
          else {
            // TLM2.0 ignorable phase!!
            // is the same as TLM_ACCEPTED!
          }
      }
      break;
    case tlm::TLM_COMPLETED: // transaction completed by callee
      // set instance-specific extension to store the information that END_RESP must not be sent in this case!!
      soc->mark_transaction_completed(*tth);
      GS_DUMP_N(this->socket_name(), "transaction has been completed by slave");
      // write not allowed since write is done with call
      // SendData, not Request (and hiding of data phases is catched above)
      assert(tth->get_command() == tlm::TLM_READ_COMMAND); 
      // only send END_REQ if there was no error
      if (!soc->has_error(*tth)) {
        // enqueue to local peq
        tph = tlm::END_REQ;
        soc->enqueue_to_bw_peq(*tth, tph, delay, index);    // End request
      }
      tph = tlm::BEGIN_RESP;
      if (!soc->is_connected_to_GPSocket(index)){ //TODO: base check on has_extension<bytes_valid>
        bytes_valid* tmp_bv = soc->template get_extension<bytes_valid>(*tth);
        tmp_bv->value=tth->get_data_length();
      }
      soc->enqueue_to_bw_peq(*tth, tph, delay, index); // Begin response
      break;
    default:
      assert(false); exit(1);
      break;
  }
}
template <typename SOCKET> 
void RequestObj<SOCKET, GenericPhase>::operator()(typename SOCKET::accessHandle &th, const GenericPhase &_p, unsigned int index) {
  operator()(th, _p, sc_core::SC_ZERO_TIME, index);
}
template <typename SOCKET> 
void RequestObj<SOCKET, GenericPhase>::operator()(typename SOCKET::accessHandle &th,
                                                  const GenericPhase &_p, double d, sc_core::sc_time_unit u, unsigned int index) {
  operator()(th, _p, sc_core::sc_time(d, u), index);
}
template <typename SOCKET> 
void RequestObj<SOCKET, GenericPhase>::operator()(typename SOCKET::accessHandle &th, unsigned int index) {
  operator()(th, GenericPhase::RequestValid, sc_core::SC_ZERO_TIME, index);
}
template <typename SOCKET> 
void RequestObj<SOCKET, GenericPhase>::operator()(typename SOCKET::accessHandle &th , 
                                                  const sc_core::sc_time &d, unsigned int index) {
  operator()(th, GenericPhase::RequestValid, d, index);
}
template <typename SOCKET> 
void RequestObj<SOCKET, GenericPhase>::operator()(typename SOCKET::accessHandle &th , 
                                                  double d, sc_core::sc_time_unit u, unsigned int index) {
  operator()(th, GenericPhase::RequestValid, sc_core::sc_time(d, u), index);
}


/// SendData functor for the GreenBus Socket, specialized for the GenericPhase
template <typename SOCKET> 
void SendDataObj<SOCKET, GenericPhase>::operator()(typename SOCKET::accessHandle &th, 
                                                   const GenericPhase &ph, const sc_core::sc_time &d, unsigned int index) {
  GS_DUMP_N(this->socket_name(), "SendData");
  // GreenBus phase DataValid
  sc_core::sc_time delay = d;
  
  tlm::tlm_generic_payload *tth = th->get_tlm_transaction();
  tlm::tlm_phase tph;

  // error if transaction completed already
  assert(!soc->is_transaction_completed(*tth));
         
  // synchronize transaction, phase GreenBus->TLM2.0
  ph.state=GenericPhase::DataValid;
  synchronize_transaction_phase(th, ph, *tth, tph);
  assert(tth->get_command() == tlm::TLM_WRITE_COMMAND); // SendData never is a read!
  
  // Handle not-GreenBus target
  if ( !soc->is_connected_to_GPSocket(index) ) {
    // if last data chunk, send tlm request phase, else enqueue local AckData and return (and wait for last SendData!)
    if (ph.getBytesValid() >= th->getMBurstLength()) {
      soc->mark_transaction_is_in_req_cycle(*tth);
      tph = tlm::BEGIN_REQ;
    }
    else { // do local response
      tph = END_DATA; // = AckData
      // enqueue to local peq
      soc->enqueue_to_bw_peq(*tth, tph, delay, index);
      return;
    }
  }
  
  GS_DUMP_N(this->socket_name(), "send " << gs::tlm_phase_writer::to_string(tph) << ", " <<
           gs::tlm_command_writer::to_string(tth->get_command()));
  // send transaction over GreenSocket
  tlm::tlm_sync_enum ret_type = (*soc)->nb_transport_fw(*tth, tph, delay);
  
  // handle TLM2.0 return path (for GSGP AND OSCI case)
  switch (ret_type) {
    case tlm::TLM_ACCEPTED: // nothing needs to be done, return path not used
      break;
    case tlm::TLM_UPDATED: // phase and transaction may be updated
      switch (tph) {
        case tlm::BEGIN_REQ:
          assert(false); exit(1); break;
        // case of not-GSGP target:
        case tlm::END_REQ:
          soc->reset_transaction_is_in_req_cycle(*tth);
          break;
        case tlm::BEGIN_RESP:
          assert(!soc->is_connected_to_GPSocket(index));
          // enqueue to local peq
          tph = END_DATA; // = DataAccepted
          soc->enqueue_to_bw_peq(*tth, tph, delay, index);   // End data
          // if (this master wants a write response and no error)
          if (soc->get_GSGP_config().use_wr_resp && !soc->has_error(*tth)) {
            tph = tlm::BEGIN_RESP;
            soc->enqueue_to_bw_peq(*tth, tph, delay, index); // Begin response
          }
          if ((soc->has_error(*tth) && tth->get_command()==tlm::TLM_WRITE_COMMAND)
              || !soc->get_GSGP_config().use_wr_resp)
          { //in this case we have a data error or no response accepted, so we better finalize the txn now
            tph=tlm::END_RESP;
            delay=sc_core::SC_ZERO_TIME;
            (*soc)->nb_transport_fw(*tth, tph, delay); //return is either ACCEPTED or COMPLETED or UPDATED with an ignorable phase, so we can ignore it
          }
          break;
        default:
          if (tph == BEGIN_DATA) {
            assert(false); exit(1); 
          }
          // case if GSGP target
          else if (tph == END_DATA) {
            // enqueue to local peq
            soc->enqueue_to_bw_peq(*tth, tph, delay, index);
          } 
          // any unknown phase:
          else {
            // TLM2.0 ignorable phase!!
            // is the same as TLM_ACCEPTED!
          }
          break;
      }
      break;
    case tlm::TLM_COMPLETED: // transaction completed by callee
      // set instance-specific extension to store the information that END_RESP must not be sent in this case!!
      soc->mark_transaction_completed(*tth);
      GS_DUMP_N(this->socket_name(), "transaction has been completed by slave");
      // enqueue to local peq
      tph = END_DATA;
      soc->enqueue_to_bw_peq(*tth, tph, delay, index);    // End request
      // if (this master wants a write response and no error)
      if (soc->get_GSGP_config().use_wr_resp && !soc->has_error(*tth)) {
        tph = tlm::BEGIN_RESP;
        soc->enqueue_to_bw_peq(*tth, tph, delay, index); // Begin response
      }
      break;
    default:
      break;
  }  
}
template <typename SOCKET> 
void SendDataObj<SOCKET, GenericPhase>::operator()(typename SOCKET::accessHandle &th, const GenericPhase &_p, unsigned int index) {
  operator()(th, _p, sc_core::SC_ZERO_TIME, index);
}
template <typename SOCKET> 
void SendDataObj<SOCKET, GenericPhase>::operator()(typename SOCKET::accessHandle &th, 
                                                   const GenericPhase &_p, double d, sc_core::sc_time_unit u, unsigned int index) {
  operator()(th, _p, sc_core::sc_time(d, u), index);
}


/// AckResponse functor for the GreenBus Socket, specialized for the GenericPhase
template <typename SOCKET> 
void AckResponseObj<SOCKET, GenericPhase>::operator()(typename SOCKET::accessHandle &th, 
                                                      const GenericPhase &ph, const sc_core::sc_time &d, unsigned int index) {
  GS_DUMP_N(this->socket_name(), "AckResponse");
  // GreenBus phase ResponseAccepted;
  sc_core::sc_time delay = d;
  
  tlm::tlm_generic_payload *tth = th->get_tlm_transaction();
  tlm::tlm_phase tph;
         
  // do not send ACK if transaction completed already
  if (soc->is_transaction_completed(*tth)) return;
         
  // synchronize transaction, phase GreenBus->TLM2.0
  ph.state = GenericPhase::ResponseAccepted;
  synchronize_transaction_phase(th, ph, *tth, tph);
  //assert(tth->get_command() == tlm::TLM_READ_COMMAND);
    
  GS_DUMP_N(this->socket_name(), "send " << gs::tlm_phase_writer::to_string(tph) << ", " <<
           gs::tlm_command_writer::to_string(tth->get_command()));
  // send transaction over GreenSocket
  tlm::tlm_sync_enum ret_type = (*soc)->nb_transport_fw(*tth, tph, delay);
    
  // handle TLM2.0 return path
  switch (ret_type) {
    case tlm::TLM_ACCEPTED: // nothing needs to be done, return path not used
      break;
    case tlm::TLM_UPDATED: // phase and transaction may be updated
      switch (tph) {
        case tlm::BEGIN_REQ:
        case tlm::END_REQ:
        case tlm::END_RESP:
          assert(false); exit(1); break;
        // Handle new (last) data phase
        case tlm::BEGIN_RESP:
          // enqueue to local peq
          soc->enqueue_to_bw_peq(*tth, tph, delay, index);
          break;
        default:
          if (tph == BEGIN_DATA || tph == END_DATA) {
            assert(false); exit(1); break;
          }
          // any unknown phase:
          else {
            // TLM2.0 ignorable phase!!
            // is the same as TLM_ACCEPTED!
          }
      }
      break;
    case tlm::TLM_COMPLETED: // transaction completed by callee
      // set instance-specific extension to store the information that END_RESP must not be sent in this case!!
      soc->mark_transaction_completed(*tth);
      GS_DUMP_N(this->socket_name(), "transaction has been completed by slave");
      break;
    default:
      break;
  }  
}
template <typename SOCKET> 
void AckResponseObj<SOCKET, GenericPhase>::operator()(typename SOCKET::accessHandle &th, const GenericPhase &_p, unsigned int index) {
  operator()(th, _p, sc_core::SC_ZERO_TIME, index);
}
template <typename SOCKET> 
void AckResponseObj<SOCKET, GenericPhase>::operator()(typename SOCKET::accessHandle &th, 
                                                      const GenericPhase &_p, double d, sc_core::sc_time_unit u, unsigned int index) {
  operator()(th, _p, sc_core::sc_time(d, u), index);
}


/// ErrorResponse functor for the GreenBus Socket, specialized for the GenericPhase
template <typename SOCKET> 
void ErrorResponseObj<SOCKET, GenericPhase>::operator()(typename SOCKET::accessHandle &th, 
                                                        const GenericPhase &ph, const sc_core::sc_time &d, unsigned int index) {
  GS_DUMP_N(this->socket_name(), "ErrorResponse");
  // GreenBus phase ResponseError;
  sc_core::sc_time delay = d;
  
  tlm::tlm_generic_payload *tth = th->get_tlm_transaction();
  tlm::tlm_phase tph;
  
  // cannot send this error message if transaction completed already
  // TODO: bail out if other side already completed
  if(soc->is_transaction_completed(*tth)) return;
  
  // synchronize transaction, phase GreenBus->TLM2.0
  ph.state = GenericPhase::ResponseError;
  synchronize_transaction_phase(th, ph, *tth, tph);
  
  GS_DUMP_N(this->socket_name(), "send " << gs::tlm_phase_writer::to_string(tph) << ", " <<
           gs::tlm_command_writer::to_string(tth->get_command()));
  // send transaction over GreenSocket
  tlm::tlm_sync_enum ret_type = (*soc)->nb_transport_fw(*tth, tph, delay);
  
  // handle TLM2.0 return path (for GreenBus AND OSCI case)
  switch (ret_type) {
    case tlm::TLM_ACCEPTED: // nothing needs to be done, return path not used
      break;
    case tlm::TLM_UPDATED:
      switch (tph) {
        case tlm::BEGIN_REQ:
        case tlm::END_REQ:
        case tlm::BEGIN_RESP:
        case tlm::END_RESP:
          assert(false); exit(1); break;
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
    case tlm::TLM_COMPLETED: // nothing needs to be done, return path not used
      // set instance-specific extension to store the information that END_RESP must not be sent in this case!!
      soc->mark_transaction_completed(*tth);
      GS_DUMP_N(this->socket_name(), "transaction has been completed by slave");
      break;
    default:
      break;
  }  
}
template <typename SOCKET> 
void ErrorResponseObj<SOCKET, GenericPhase>::operator()(typename SOCKET::accessHandle &th, const GenericPhase &_p, unsigned int index) {
  operator()(th, _p, sc_core::SC_ZERO_TIME, index);
}
template <typename SOCKET> 
void ErrorResponseObj<SOCKET, GenericPhase>::operator()(typename SOCKET::accessHandle &th, const GenericPhase &_p, 
                                                        double d, sc_core::sc_time_unit u, unsigned int index) {
  operator()(th, _p, sc_core::sc_time(d, u), index);
}


template <unsigned int BUSWIDTH, typename TRANSACTION, typename CONFIG, typename PHASE, bool BIDIR, typename SOCK_TYPE>
inline tlm::tlm_sync_enum GSGPMasterSocket<BUSWIDTH, TRANSACTION, CONFIG, PHASE, BIDIR, SOCK_TYPE>::
bw_nb_transport_cb(unsigned int index, tlm::tlm_generic_payload& tlmtr, 
                   tlm::tlm_phase& tph, sc_core::sc_time& delay) {
  // ////////// BEGIN GSGP Socket ///////////////////////////////////////////////// //
  if (base_socket_type::is_connected_to_GPSocket(index)) { 

    GS_DUMP_N(base_socket_type::socket_name(), "got " << gs::tlm_phase_writer::to_string(tph) << ", " <<
              gs::tlm_command_writer::to_string(tlmtr.get_command()) << 
              (base_socket_type::has_error(tlmtr)? ", ERROR!" : ", no error") << 
              " from socket #" << index << " (GSGP-Socket)");
    
    // reset instance-specific extension
    base_socket_type::reset_transaction_completed(tlmtr);
    
    // handle and forward to internal peq
    tlm::tlm_sync_enum ret = tlm::TLM_ACCEPTED;
    
    switch (tph) {
      case tlm::BEGIN_REQ:
        assert(false); exit(1); break;
      case tlm::END_REQ:
        base_socket_type::reset_transaction_is_in_req_cycle(tlmtr);
        break;
      case tlm::BEGIN_RESP:
        if (base_socket_type::transaction_is_within_req_cycle(tlmtr) 
            && tlmtr.get_response_status() != tlm::TLM_OK_RESPONSE) {
          ret = tlm::TLM_COMPLETED;
        }    
        break;
      case tlm::END_RESP:
        break;
      default:
        if (tph == BEGIN_DATA) {}
        else if (tph == END_DATA) {}
        else // TLM2.0 ignorable phase!!!
          return tlm::TLM_ACCEPTED;
    }
    
    // enqueue to local peq, will be forwarded to user later
    enqueue_to_bw_peq(tlmtr, tph, delay, index);

    return ret;

  }    // ////////// END GSGP Socket ///////////////////////////////////////////////// //
  else // ////////// BEGIN NOT GSGP Socket ///////////////////////////////////////////////// //
  {

    GS_DUMP_N(base_socket_type::socket_name(), "got " << gs::tlm_phase_writer::to_string(tph) << ", " <<
              gs::tlm_command_writer::to_string(tlmtr.get_command()) << 
              (base_socket_type::has_error(tlmtr)? ", ERROR!" : ", no error") << 
              " from socket #" << index << " (non-GSGP-Socket)");
    
    // reset instance-specific extension
    base_socket_type::reset_transaction_completed(tlmtr);
    
    // handle and forward to internal peq
    tlm::tlm_sync_enum ret = tlm::TLM_ACCEPTED;
    bool forward = true;
    
    // Handle not-GreenBus target
    {
      
      // ///////////////  Conversion block  ////////////////////// //
      // Do all conversions that are needed for connecting different 
      // protocols to each other here:
      
      // since this is sent by an osci slave, force the bytes valid to all data
      
      switch (tph) {
        case tlm::BEGIN_REQ:
          assert(false); exit(1); // a slave must not send this
          forward = false;
          break;
        case tlm::END_REQ:
          base_socket_type::reset_transaction_is_in_req_cycle(tlmtr);
          if (tlmtr.get_command() == tlm::TLM_WRITE_COMMAND) {
            // wait for the BEGIN_RESP which then sends the DataAccepted 
            // and the ResponseValid to the user, see case BEGIN_RESP below
            forward = false;
          } else {
            // do nothing, forward
          }
          break;
        case tlm::BEGIN_RESP:
          GS_DUMP_N(this->socket_name(), tlmtr.get_response_string());
          this->template get_extension<bytes_valid>(tlmtr)->value = tlmtr.get_data_length();      
          // if this is a RequestError, complete
          if (base_socket_type::transaction_is_within_req_cycle(tlmtr) 
              && tlmtr.get_response_status() != tlm::TLM_OK_RESPONSE) {
            ret = tlm::TLM_COMPLETED;
          }
          if (tlmtr.get_command() == tlm::TLM_WRITE_COMMAND) {
            // forward this as a DataAccepted phase to the user/master
            tph = END_DATA;
            // if (this master wants a write response AND this is NOT an DataError) send additional phase
            if (base_socket_type::get_GSGP_config().use_wr_resp
                && tlmtr.get_response_status() == tlm::TLM_OK_RESPONSE) {
              // send the DataAccepted phase now and send additional phase below
              enqueue_to_bw_peq(tlmtr, tph, delay, index);
              // also send a ResponseValid phase
              tph = tlm::BEGIN_RESP;
              // sending will be done below
            } 
            // if no additional phase, complete this now, 
            //  otherwise bw path will be used to send END_RESP when ResponseAccepted occurs
            else { 
              ret = tlm::TLM_COMPLETED;
            }
          } else if (tlmtr.get_command() == tlm::TLM_READ_COMMAND) {
            // check if END_REQ phase was skipped: in that case, insert the phase to the peq
            if (base_socket_type::transaction_is_within_req_cycle(tlmtr)) {
              // send the additional RequestAccepted phase now and send current phase below
              tlm::tlm_phase tmpph = tlm::END_REQ;
              enqueue_to_bw_peq(tlmtr, tmpph, delay, index);
            }
            // do nothing, forward
          } else
            assert (false);
          break;
        case tlm::END_RESP:
          assert(false); exit(1); // a slave must not send this
          break;
        default:
          if (tph == BEGIN_DATA || tph == END_DATA) {
            assert(false); exit(1); // at the moment not-GSGP-devices only are allowed to send basic protocol phases
            break;
          }
          // any unknown phase:
          else {
            // TLM2.0 ignorable phase!!
            forward = false;
          }
          break;
      }
      
      // ///////////////  END Conversion block  ////////////////// //
      
      // enqueue to local peq, will be forwarded to user later
      if (forward) enqueue_to_bw_peq(tlmtr, tph, delay, index);
      
    }
    return ret;

  } // ////////// END NOT GSGP Socket ///////////////////////////////////////////////// //
} 


template <unsigned int BUSWIDTH, typename TRANSACTION, typename CONFIG, typename PHASE, bool BIDIR, typename SOCK_TYPE>
inline void GSGPMasterSocket<BUSWIDTH, TRANSACTION, CONFIG, PHASE, BIDIR, SOCK_TYPE>::
enqueue_to_bw_peq(tlm::tlm_generic_payload& tlmtr, tlm::tlm_phase& tph, const sc_core::sc_time& delay, unsigned int index) {
  // enqueue
  m_bw_peq.notify(tlmtr, tph, delay, index);
}


template <unsigned int BUSWIDTH, typename TRANSACTION, typename CONFIG, typename PHASE, bool BIDIR, typename SOCK_TYPE>
inline void GSGPMasterSocket<BUSWIDTH, TRANSACTION, CONFIG, PHASE, BIDIR, SOCK_TYPE>::
bw_peq_cb(tlm::tlm_base_protocol_types::tlm_payload_type& tlmtr,
       const tlm::tlm_base_protocol_types::tlm_phase_type& tph, unsigned int index) 
{ 
  // simply forward the call of the incoming tlm peq to the user
  accessHandle th(this, &tlmtr);
  GenericTransaction &genTr = th.get();
  
  // if END_REQ with not INCOMPLETED and not OK: ignore (drop), because a BEGIN_RESP with Error will follow!
  if (tph == tlm::END_REQ
      && tlmtr.get_response_status() != tlm::TLM_INCOMPLETE_RESPONSE
      && tlmtr.get_response_status() != tlm::TLM_OK_RESPONSE         ) {
    GS_DUMP_N(base_socket_type::socket_name(), "peq cb: drop " << gs::tlm_phase_writer::to_string(tph) << ", " <<
            gs::tlm_command_writer::to_string(tlmtr.get_command()) << " (a BEGIN_RESP with an error will follow)");
    base_socket_type::mark_transaction_is_in_req_cycle(tlmtr);
    return;
  }
  
  synchronize_transaction_phase(tlmtr, tph, genTr, m_phase, base_socket_type::transaction_is_within_req_cycle(tlmtr)); 
  
  // After the sync the transaction is surely no longer in req cycle
  base_socket_type::reset_transaction_is_in_req_cycle(tlmtr);
  
  atom maa(th, m_phase);
  // check if to call with index
  if (peq.out_multi_port.operator ->()!=NULL)
    peq.out_multi_port->notify(maa, index); // forward immediately to user with index
  else
    peq.out_port->notify(maa); // forward immediately to user
}


template <unsigned int BUSWIDTH, typename TRANSACTION, typename CONFIG, typename PHASE, bool BIDIR, typename SOCK_TYPE>
inline void GSGPMasterSocket<BUSWIDTH, TRANSACTION, CONFIG, PHASE, BIDIR, SOCK_TYPE>::
Transact(accessHandle t, unsigned int index) {
  //PORT::b_out->b_transact(t); 
  sc_core::sc_time no_time = sc_core::SC_ZERO_TIME;
  (*this)[index]->b_transport(*t.get_tlm_transaction(), no_time);
}


template <unsigned int BUSWIDTH, typename TRANSACTION, typename CONFIG, typename PHASE, bool BIDIR, typename SOCK_TYPE>
inline void GSGPMasterSocket<BUSWIDTH, TRANSACTION, CONFIG, PHASE, BIDIR, SOCK_TYPE>::
Transact(accessHandle t, sc_core::sc_time& td, unsigned int index) {
  //PORT::b_out->b_transact(t); 
  (*this)[index]->b_transport(*t.get_tlm_transaction(), td);
}


template <unsigned int BUSWIDTH, typename TRANSACTION, typename CONFIG, typename PHASE, bool BIDIR, typename SOCK_TYPE>
inline Handle<TRANSACTION> GSGPMasterSocket<BUSWIDTH, TRANSACTION, CONFIG, PHASE, BIDIR, SOCK_TYPE>::
create_transaction() {
  assert(base_socket_type::is_configured());
  accessHandle ah(this, socket_type::get_transaction());
  ah->reset();
  ah->setMID(MasterPortNumber);
  ah->setMAddr(target_addr.getValue());
  ah->setTransID(UniqueNumber::get());
  return ah;
}
template <unsigned int BUSWIDTH, typename TRANSACTION, typename CONFIG, typename PHASE, bool BIDIR, typename SOCK_TYPE> 
template<typename ACCESS_TYPE>
inline ACCESS_TYPE GSGPMasterSocket<BUSWIDTH, TRANSACTION, CONFIG, PHASE, BIDIR, SOCK_TYPE>::
create_transaction() {
  assert(base_socket_type::is_configured());
  ACCESS_TYPE ah(this, socket_type::get_transaction());
  ah->reset();
  ah->setMID(MasterPortNumber);
  ah->setMAddr(target_addr.getValue());
  ah->setTransID(UniqueNumber::get());
  return ah;
}

template <unsigned int BUSWIDTH, typename TRANSACTION, typename CONFIG, typename PHASE, bool BIDIR, typename SOCK_TYPE>
inline void GSGPMasterSocket<BUSWIDTH, TRANSACTION, CONFIG, PHASE, BIDIR, SOCK_TYPE>::
release_transaction(accessHandle &th) {
  socket_type::release_transaction(th->get_tlm_transaction());
  th->alive = false;
}

template <unsigned int BUSWIDTH, typename TRANSACTION, typename CONFIG, typename PHASE, bool BIDIR, typename SOCK_TYPE>
inline gs::gs_handle GSGPMasterSocket<BUSWIDTH, TRANSACTION, CONFIG, PHASE, BIDIR, SOCK_TYPE>::
get_master_port_number() {
  return MasterPortNumber;
}


template <unsigned int BUSWIDTH, typename TRANSACTION, typename CONFIG, typename PHASE, bool BIDIR, typename SOCK_TYPE>
inline void GSGPMasterSocket<BUSWIDTH, TRANSACTION, CONFIG, PHASE, BIDIR, SOCK_TYPE>::
start_of_simulation() {
  base_socket_type::start_of_simulation();

  // sync target_addr parameter
  gs::socket::multi_init_base<BUSWIDTH>* init = dynamic_cast<gs::socket::multi_init_base<BUSWIDTH>*>(this);
  if (init) {
    if (init->get_hierarch_bind() != this) {
      GSGPMasterSocket* other = dynamic_cast<GSGPMasterSocket*>(init->get_hierarch_bind());
      target_addr_sync.bind(&target_addr, &other->target_addr);
    }
  }

  for (unsigned int i = 0; i < socket_type::size(); i++) {
    // register callback to this Socket at the GreenSocket
    socket_type::register_nb_transport_bw(this, &my_type::bw_nb_transport_cb);
  }
  // TODO: the check bellow is only valid when the socket is not hierarchically bound. But how to guess it socket is bound hierarchically?
  //if (peq.out_port.operator ->()==NULL  &&  peq.out_multi_port.operator ->()==NULL){
  //  SC_REPORT_ERROR(socket_type::name(), "peq.out_port and peq.out_multi_port not bound. You need to bind one of them!");
  //}
}

