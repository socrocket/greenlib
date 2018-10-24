//   green_socket
//
// LICENSETEXT
//
//   Copyright (C) 2008 : GreenSocs Ltd
// 	 http://www.greensocs.com/ , email: info@greensocs.com
//
//   Developed by :
//    Robert Guenzel <guenzel@eis.cs.tu-bs.de>
//
//
// The contents of this file are subject to the licensing terms specified
// in the file LICENSE. Please consult this file for restrictions and
// limitations that may apply.
// 
// ENDLICENSETEXT


//owner-type-independent-member-function-pointer-pattern
// see gsock_tech.pdf for details  
template <typename TRAITS>
template<typename SOCKET_CALLBACK_BASE>
unsigned int gs::socket::bind_checker<TRAITS>::get_num_bindings_int(void* mod)
{
  SOCKET_CALLBACK_BASE* me_as_socket=static_cast<SOCKET_CALLBACK_BASE*>(mod);
  return me_as_socket->size();
}

//owner-type-independent-member-function-pointer-pattern
// see gsock_tech.pdf for details  
template <typename TRAITS>
template<typename SOCKET_CALLBACK_BASE>
 sc_core::sc_interface* gs::socket::bind_checker<TRAITS>::get_interface(void* mod, unsigned int index)
{
  SOCKET_CALLBACK_BASE* me_as_socket=static_cast<SOCKET_CALLBACK_BASE*>(mod);
  me_as_socket->SOCKET_CALLBACK_BASE::end_of_elaboration();
  return me_as_socket->operator[](index);
}

//provide a member function get_num_bindings that is calling the size() function of the
// socket derived from this class
template <typename TRAITS>
unsigned int gs::socket::bind_checker<TRAITS>::get_num_bindings()
{
  return get_num_bindings_ptr(m_socket);
}

//get the other side of the link. see gsock_tech.pdf
template <typename TRAITS>
gs::socket::bindability_base<TRAITS>* gs::socket::bind_checker<TRAITS>::
 get_other_side(unsigned int index, unsigned int& other_index)
{
  sc_core::sc_interface* other=get_interface_ptr(m_socket, index);
  if (other){
    bindability_base<traits_type>* retVal=NULL;
    gs::socket::gs_callback_binder_base* binder=dynamic_cast<gs::socket::gs_callback_binder_base*>(other);
    if (binder) {
      bool tmp_bool=static_cast<bindability_base<traits_type>*>(binder->get_owner())->get_t_piece_end(retVal, other_index);
      if (tmp_bool){ 
        return retVal;
      }
      other_index=binder->get_index();
      return static_cast<bindability_base<traits_type>*>(binder->get_owner());
    }
    return retVal;
  }
  else
    assert(0);
  return NULL;
}

//the CTOR
// store the name of the socket, set the check state to check-not-allowed, because socket is not
// bound yet
// store the pointer to the derived socket as a void* (owner-type-independent-member-function-pointer-pattern see gsock_tech.pdf for details)
// create an initial slot for a config and the callback-done flag
// remember the function pointer for the owner-type-independent-member-function-pointer-pattern see gsock_tech.pdf for details
template <typename TRAITS>
template<typename SOCKET_CALLBACK_BASE>
gs::socket::bind_checker<TRAITS>::bind_checker(const char* name, SOCKET_CALLBACK_BASE* socket, unsigned int bus_width)
  : m_name(name)
  , bind_check_state(gs_bind_check_not_allowed)
  , m_socket(static_cast<void*>(socket))
  , m_bus_width(bus_width)
{
  m_configs.resize(1);
  cb_done.resize(1);
  get_interface_ptr=&get_interface<SOCKET_CALLBACK_BASE>;
  get_num_bindings_ptr=&get_num_bindings_int<SOCKET_CALLBACK_BASE>;
}

//the CTOR
// store the name of the socket, set the check state to check-not-allowed, because socket is not
// bound yet
// store the pointer to the derived socket as a void* (owner-type-independent-member-function-pointer-pattern see gsock_tech.pdf for details)
// create an initial slot for a config and the callback-done flag
// remember the function pointer for the owner-type-independent-member-function-pointer-pattern see gsock_tech.pdf for details
template <typename TRAITS>
template<typename SOCKET_CALLBACK_BASE>
gs::socket::bind_checker<TRAITS>::bind_checker(const char* name, SOCKET_CALLBACK_BASE* socket)
  : m_name(name)
  , bind_check_state(gs_bind_check_not_allowed)
  , m_socket(static_cast<void*>(socket))
  , m_bus_width(socket->get_bus_width())
{
  m_configs.resize(1);
  cb_done.resize(1);
  get_interface_ptr=&get_interface<SOCKET_CALLBACK_BASE>;
  get_num_bindings_ptr=&get_num_bindings_int<SOCKET_CALLBACK_BASE>;
}

//DTOR has no job
template <typename TRAITS>
gs::socket::bind_checker<TRAITS>::~bind_checker()
{
}

//get the parent of the socket
template <typename TRAITS>
sc_core::sc_object* gs::socket::bind_checker<TRAITS>::get_parent(){
  //cast this thing into an sc_object (should work as it is an sc_port
  // in case of an initiator socket, or an sc_export in case of a target socket)
  sc_core::sc_object* me_as_object=dynamic_cast<sc_core::sc_object*>(this);
  if (!me_as_object){
    GS_MSG_OUTPUT(GS_MSG_FATAL, "Unexpected class hierarchy for a GreenSocket");
  }
  //use get_parent function of sc_object
  return me_as_object->get_parent_object();
}

//get the name of the socket
template <typename TRAITS>
const char* gs::socket::bind_checker<TRAITS>::get_name()const{
  return m_name;
}

//default implementation of get_t_piece_end returns false, because normally a block of IP
// is no T-piece (see gsock_tech.pdf)
template <typename TRAITS>
bool gs::socket::bind_checker<TRAITS>::get_t_piece_end(bindability_base<traits_type>*&, unsigned int&){
  return false;
}

//set the config for all links
template <typename TRAITS>
void gs::socket::bind_checker<TRAITS>::set_config(const config<traits_type>& config_){
  for (unsigned int i=0; i<m_configs.size(); i++){
    set_config(config_, i);
  }
}

//set the config for a single link
template <typename TRAITS>
void gs::socket::bind_checker<TRAITS>::set_config(const config<traits_type>& config_, unsigned int index){
  if (index>=m_configs.size()){
    GS_MSG_OUTPUT(GS_MSG_FATAL, "Number of connected sockets on "<<get_name()<<" has not yet been resolved. Use set_config(config, index) only during end_of_elaboration or later");
  }
  
  m_configs[index]=config_; //set the config into the appropriate slot
  m_configs[index].m_type_string=&get_type_string(); //set the type string pointer of the config to the type string of this socket
  m_configs[index].invalid=false; //it is an assigned config now, so it is not invalid any more
  if (bind_check_state==gs_bind_check_allowed) check_binding(index); //if we can do a bind check, we do it now
}

//the bind check
template <typename TRAITS>
void gs::socket::bind_checker<TRAITS>::check_binding(unsigned int index){
  resize_configs(); //make sure we have the configs we need
  assert(m_configs.size()>index); //make sure the index is sane
  
  if (m_configs[index].invalid) { //a config was never assigned to this socket or link!
    GS_MSG_OUTPUT(GS_MSG_FATAL, "Socket "<<get_name()<<" has no configuration at end of elaboration");
  }
  
  //get the ptr to the other side, and the index of the link at the other side
  unsigned int other_index=0;
  bindability_base<traits_type>* other=get_other_side(index,other_index);
  
  //guard this socket against reentrant calls to check_binding coming from get_config
  bind_check_state=gs_bind_check_running;
  
  //if the other side is no greensocket we assume the default config for this traits class
  // such a default config needs to be a class of type 
  // template <typename TRAITS> osci_config; For TRAITS=tlm_base_protocol_types this class is pre-defined
  // for your own traits class create a template specialization of osci_config
  //if the other side is a greensocket we get the config for the corresponding index from the other side
  config<traits_type>& other_conf=(!other)? osci_config<traits_type>::get_cfg() : other->get_config(other_index);
  
  //we get a ptr to the name of the other socket to allow for meaningful error messages
  const char* other_name=(!other)? "osci_socket" : other->get_name();
  
  //other side never got a real config
  if (other_conf.invalid) {
    GS_MSG_OUTPUT(GS_MSG_FATAL, other->get_name()<<" has no configuration at end of elaboration");
  }
  
  //we initialze the bool that tells us whether or not to perform the bound_to callback:
  // if the other side enforces a reevaluation of the callback we set it to true
  // if we did not yet do a callback we set it to true as well
  bool do_cb=!cb_done[index] | other_conf.force_reeval; //do a callback if we checking this binding for the first time or because the other side enforces it
  other_conf.force_reeval=false; //reset the enforcement of the callback (because we just used that info)

  //open a local scope so the tmp config is dropped at its end
  {
    config<traits_type> tmp=m_configs[index]; //remember the current config
    m_configs[index].merge_with(get_name(), other_name, other_conf); //merge current config with other side's config
    do_cb|=tmp.diff(m_configs[index]); //if the resolved config differs from the initial one, we'll do a callback
  }
  if (do_cb){ 
    config<traits_type> tmp=m_configs[index]; //remember the resolved config
    bound_to(*other_conf.m_type_string, other, index); //do the callback
    cb_done[index]=true; //remember that we have done a callback
    if (m_configs[index].force_reeval)  //if the changed config now forces us to re-evaluate
      check_binding(index); //we are forced to re-evaluate
    else
    if (tmp.diff(m_configs[index])){ //the callback changed the config
      //so let's check if that change will also effect the resolved config
      m_configs[index].merge_with(get_name(), other_name, other_conf);
      if (tmp.diff(m_configs[index])) //the new resolved config differs from the old
        check_binding(index); //so we redo the bind check
    }
  }
  bind_check_state=gs_bind_check_allowed; //from now on bind checks are okay.
}


//helper to resize configs
template <typename TRAITS>
void gs::socket::bind_checker<TRAITS>::resize_configs(){
  if (m_configs.size()==1 && get_num_bindings()>1){ //if we only have one config slot (i.e. just the one from the CTOR) but we have more than one binding
    m_configs.resize(get_num_bindings()); //we create enough slots for all configs
    cb_done.resize(get_num_bindings(), false); //we remember that we did not yet do callbacks
    for (unsigned int i=1; i<m_configs.size(); i++) //we copy the initial config to all slots
      m_configs[i]=m_configs[0]; //assign initial config to all bound sockets
  }
}

//get the config a a link (when asked by another socket during its bind check)
template <typename TRAITS>
gs::socket::config<TRAITS>& gs::socket::bind_checker<TRAITS>::get_config(unsigned int index){
  resize_configs(); //first make sure we have a config
  assert(m_configs.size()>index);
  if (bind_check_state!=gs_bind_check_running) 
    check_binding(index); //now we first check our binding, because the bound_to callback we do may change the config we wanna return
  return m_configs[index]; //now return the requested config
}

//called by owner. return the currently resolved config for the provided link
template <typename TRAITS>
gs::socket::config<TRAITS>& gs::socket::bind_checker<TRAITS>::get_recent_config(unsigned int index){
  return m_configs[index];
}

//dump our configs into a string
template <typename TRAITS>
std::string gs::socket::bind_checker<TRAITS>::to_string(){
  std::stringstream s;
  s<<"Configuration of GreenSocket "<<m_name<<std::endl;
  for (int i=0; i<m_configs.size(); i++)
    GS_MSG_OUTPUT(GS_MSG_INFO, "Index "<<i<<std::endl<<m_configs[i].to_string());
  return s.str();
}
