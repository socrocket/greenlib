//   GreenSocket
//
// LICENSETEXT
//
//   Copyright (C) 2007 : GreenSocs Ltd
// 	 http://www.greensocs.com/ , email: info@greensocs.com
//
//   Developed by :
//   
//   Ashwani Singh, CircuitSutra Technologies(www.circuitsutra.com)
//
//
// The contents of this file are subject to the licensing terms specified
// in the file LICENSE. Please consult this file for restrictions and
// limitations that may apply.
// 
// ENDLICENSETEXT

//compare two timing infos (check for equality)
bool gs::socket::timing_info::operator ==( timing_info& rhs) 
{
  std::map<tlm::tlm_phase, sc_core::sc_time>::iterator it1;
  std::map<tlm::tlm_phase, sc_core::sc_time>::iterator it2;

  //if the map sizes are different we treat the infos to be different
  if(m_timing_map.size() != rhs.m_timing_map.size())
   return false;

  //if the sizes match, iterate over both maps. Once we encounter
  // a difference in used phases (first element of iterator)
  // or a difference in timing for the same phase, we signal inequality
  it1=m_timing_map.begin();
  it2= rhs.m_timing_map.begin();
  for(;it1!= m_timing_map.end(), it2!=rhs.m_timing_map.end();it1++, it2++)
  {
      if(it1->first==it2->first){
          if(it1->second!=it2->second)
	      return false;
      }
	  else
	      return false;
  }
  return true;
}

//check for non default timing
bool gs::socket::timing_info::is_non_default() 
{
  if (m_timing_map.size()==0) return false; //if map is empty, we have default timing
  
  std::map<tlm::tlm_phase, sc_core::sc_time>::iterator it;
  
  //go through the map, once we hit a phase whose timing is not SC_ZERO_TIME
  // we detected non-default timing
  bool retVal=false;
  for(it=m_timing_map.begin();it!= m_timing_map.end();it++)
  {
    if (it->second!=sc_core::SC_ZERO_TIME){
      retVal=true;
      break;
    }
  }
  return retVal;
}

//inequality is the invers of equality
bool gs::socket::timing_info::operator !=(timing_info& rhs) 
{
    return !(operator==(rhs));
}

//simply store the provided delay in the map
void gs::socket::timing_info::set_start_time(const tlm::tlm_phase& phase, const sc_core::sc_time & delay)
{
    m_timing_map[phase]=delay;
}

//simply return the requested delay (if not available sc_time will default to SC_ZERO_TIME)
sc_core::sc_time& gs::socket::timing_info::get_start_time(const tlm::tlm_phase &phase)
{
    return(m_timing_map[phase]);
} 

//the ctor has nothing to do
template<typename TRAITS, typename BIND_BASE>
gs::socket::initiator_timing_support_base<TRAITS,BIND_BASE>::initiator_timing_support_base()
{
   
}

//register the provided callback with the functor object
template<typename TRAITS, typename BIND_BASE>
template<typename MODULE>
void gs::socket::initiator_timing_support_base<TRAITS,BIND_BASE>::set_timing_listener_callback(MODULE* owner, void(MODULE::*timing_cb) (timing_info))
{
    m_time_cb.set_function(owner, timing_cb);
}

//store a timing that is supposed to be used for all bindings
template<typename TRAITS, typename BIND_BASE>
void gs::socket::initiator_timing_support_base<TRAITS,BIND_BASE>::set_initiator_timing(timing_info& my_timing)
{
    timing_support_base::inc_recursion_count(); //increment the recursion counter
    m_one_for_all_timing=my_timing; //set local default timing for yet unbound connections
    for(unsigned int i=0; i<m_timing_list.size(); i++)
    { //if there are already some bound targets, inform them
        if (my_timing!=m_timing_list[i]) //but only if they timing changed
        {
            m_timing_list[i]=my_timing;
            if (m_bound[i]) transmit_timing(i);
        }
    }
    timing_support_base::dec_recursion_count(); //decrement the recursion count
}

//store a timing info for a given bindings
template<typename TRAITS, typename BIND_BASE>
void gs::socket::initiator_timing_support_base<TRAITS,BIND_BASE>::set_initiator_timing(timing_info& my_timing, unsigned int index)
{
  timing_support_base::inc_recursion_count(); //increment the recursion counter
  
  //if this info is for a binding that we have not yet been informed to be bound
  // we will make all our vectors large enough to handle the provided index
  if (index>=m_timing_list.size()) m_timing_list.resize(index+1, m_one_for_all_timing); //yet unbound bindings get the local default timing
  if (index>=m_bound.size())  m_bound.resize(index+1, false); //yet unbound bindings are marked as unbound
  if (index>=other_socket.size())  other_socket.resize(index+1, NULL); //yet unbound bindings point to nowhere
  
  //if the provided timing is different from the one we have for this binding...
  if (my_timing!=m_timing_list[index]){
    GS_MSG_OUTPUT(GS_MSG_INFO, "diff in timing of "<<index);
    m_timing_list[index]=my_timing; //...store this timing for this binding
    if (m_bound[index]) transmit_timing(index); //if the binding already has been resolved, transmit the timing info to the target
    else GS_MSG_OUTPUT(GS_MSG_INFO, "not bound yet in timing of "<<index);
  }
  timing_support_base::dec_recursion_count(); //decrement the recursion count
}

//the target tells us that it changed its timing, so do the callback if available
template<typename TRAITS, typename BIND_BASE>
void gs::socket::initiator_timing_support_base<TRAITS,BIND_BASE>::set_target_timing(timing_info& other_timing){
  if (!m_time_cb.empty()) m_time_cb(other_timing);
}


//a binding has been resolved
template<typename TRAITS, typename BIND_BASE>
void gs::socket::initiator_timing_support_base<TRAITS,BIND_BASE>::bound_to(const std::string& other_type,typename BIND_BASE::bind_base_type* other, unsigned int index)
{
  //make our vectors large enough to handle the binding
  if (index>=m_bound.size())  m_bound.resize(index+1, false); //yet unbound bindings are marked as unbound
  if (index>=m_timing_list.size()) m_timing_list.resize(index+1, m_one_for_all_timing); //yet unbound bindings get the local default timing
  if (index>=other_socket.size())  other_socket.resize(index+1, NULL); //yet unbound bindings point to nowhere
  
  m_bound[index]=true; //mark the binding as okay
  
  if(other) { //if the other socket is a greensocket (bindability base)
    other_socket[index]=other; //remember the pointer to it
    if (m_timing_list[index].is_non_default()) transmit_timing(index); //and if the timing for the binding is non-default transmit it
  }
  
}

//helper to transmit the timing info
template<typename TRAITS, typename BIND_BASE>
void gs::socket::initiator_timing_support_base<TRAITS,BIND_BASE>::transmit_timing(unsigned int index){
  
  //make sure that the transmission won't seg fault
  assert((other_socket.size()>index && other_socket[index]!=NULL) 
          && "Socket used without default timing, but connected to a non matching socket.");
  
  //cast the bindability base into the target timing info base class
  // that should succeed if the target is a correct greensocket
  other_socket_type * slave_socket = dynamic_cast<other_socket_type *> (other_socket[index]);
  assert(slave_socket!=NULL && "Unexpected class hierarchy.");
  
  //now inform the target about our timing info
  slave_socket->set_initiator_timing(m_timing_list[index]);
      
}

/*
** All calls below are symetrical to the ones of the intiator socket. So look into the comments for the initiator
** socket to know what is going on.
*/

template<typename TRAITS, typename BIND_BASE>
gs::socket::target_timing_support_base<TRAITS,BIND_BASE>::target_timing_support_base()
{
   
}

template<typename TRAITS, typename BIND_BASE>
template<typename MODULE>
void gs::socket::target_timing_support_base<TRAITS,BIND_BASE>::set_timing_listener_callback(MODULE* owner, void(MODULE::*timing_cb) (timing_info))
{

 m_time_cb.set_function(owner, timing_cb);
}

template<typename TRAITS, typename BIND_BASE>
void gs::socket::target_timing_support_base<TRAITS,BIND_BASE>::set_target_timing(timing_info& my_timing)
{
  timing_support_base::inc_recursion_count();
  m_one_for_all_timing=my_timing; //set local default timing  
  for (unsigned int i=0; i<m_timing_list.size(); i++){ //if there are already some bound targets, inform them
    if (my_timing!=m_timing_list[i]){
      m_timing_list[i]=my_timing;
      if (m_bound[i]) transmit_timing(i);
    }
  }
  timing_support_base::dec_recursion_count();
}

template<typename TRAITS, typename BIND_BASE>
void gs::socket::target_timing_support_base<TRAITS,BIND_BASE>::set_target_timing(timing_info& my_timing, unsigned int index)
{
  timing_support_base::inc_recursion_count();
  if (index>=m_timing_list.size()) m_timing_list.resize(index+1, m_one_for_all_timing);
  if (index>=m_bound.size())  m_bound.resize(index+1, false);
  if (index>=other_socket.size())  other_socket.resize(index+1, NULL);
  
  if (my_timing!=m_timing_list[index]){
    GS_MSG_OUTPUT(GS_MSG_INFO, "diff in timing of "<<index);
    m_timing_list[index]=my_timing;
    if (m_bound[index]) transmit_timing(index);
    else GS_MSG_OUTPUT(GS_MSG_INFO, "not bound yet in timing of "<<index);
  }
  timing_support_base::dec_recursion_count();
}

template<typename TRAITS, typename BIND_BASE>
void gs::socket::target_timing_support_base<TRAITS,BIND_BASE>::set_initiator_timing(timing_info& other_timing){
  if (!m_time_cb.empty()) m_time_cb(other_timing);
}

template<typename TRAITS, typename BIND_BASE>
void gs::socket::target_timing_support_base<TRAITS,BIND_BASE>::bound_to(const std::string& other_type,typename BIND_BASE::bind_base_type* other, unsigned int index)
{
  if (index>=m_bound.size())  m_bound.resize(index+1, false);
  if (index>=m_timing_list.size()) m_timing_list.resize(index+1, m_one_for_all_timing);
  if (index>=other_socket.size())  other_socket.resize(index+1, NULL);
  
  m_bound[index]=true;
  if(other) {
    other_socket[index]=other;
    if (m_timing_list[index].is_non_default()) transmit_timing(index);
  }
}

template<typename TRAITS, typename BIND_BASE>
void gs::socket::target_timing_support_base<TRAITS,BIND_BASE>::transmit_timing(unsigned int index){
  assert((other_socket.size()>index && other_socket[index]!=NULL) 
          && "Socket used without default timing, but connected to a non matching socket.");
  other_socket_type * master_socket = dynamic_cast<other_socket_type *>(other_socket[index]);
  assert(master_socket!=NULL && "Unexpected class hierarchy.");  
  master_socket->set_target_timing(m_timing_list[index]);
} 
