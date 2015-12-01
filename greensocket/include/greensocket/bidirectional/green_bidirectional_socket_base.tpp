//   GreenSocket
//
// LICENSETEXT
//
//   Copyright (C) 2008 : GreenSocs Ltd
// 	 http://www.greensocs.com/ , email: info@greensocs.com
//
//   Developed by :
//    Robert Guenzel <guenzel@eis.cs.tu-bs.de>, 
//    Christian Schroeder <schroeder@eis.cs.tu-bs.de>
//
//
// The contents of this file are subject to the licensing terms specified
// in the file LICENSE. Please consult this file for restrictions and
// limitations that may apply.
// 
// ENDLICENSETEXT


template <unsigned int BUSWIDTH, typename TRAITS, unsigned int N, typename BIND_BASE, typename MM_INTERFACE, typename EXT_FILLER, typename POOL>
gs::socket::bidirectional_socket_base<BUSWIDTH, TRAITS, N, BIND_BASE, MM_INTERFACE, EXT_FILLER, POOL>::~bidirectional_socket_base(){
  if (m_pool) delete m_pool;
}

template <unsigned int BUSWIDTH, typename TRAITS, unsigned int N, typename BIND_BASE, typename MM_INTERFACE, typename EXT_FILLER, typename POOL>
gs::socket::bidirectional_socket_base<BUSWIDTH, TRAITS, N, BIND_BASE, MM_INTERFACE, EXT_FILLER, POOL>::bidirectional_socket_base
  (const char* name, allocation_scheme scheme, EXT_FILLER* ext_filler)
                                                 : init_base_type((std::string(name)+"_init").c_str(), static_cast<typename BIND_BASE::bind_base_type*>(this))
                                                 , target_base_type((std::string(name)+"_target").c_str(), static_cast<typename BIND_BASE::bind_base_type*>(this))
                                                 , bind_checker_type(init_base_type::name(), (init_base_type*)this, BUSWIDTH)
                                                 //, ext_support_type(BUSWIDTH)
                                                 , mm_base_type(scheme)
                                                 , bw(dynamic_cast<target_base_type*>(this))
                                                 , fw(dynamic_cast<init_base_type*>(this))
                                                 , m_filler(ext_filler)
                                                 , base_eoe_done(false)
                                                 , m_port_accessor(this)
{
  m_pool=NULL;
  // hierarchical m_name
  m_name = init_base_type::name();
  m_name = m_name.substr(0, m_name.length()-5); // cut off "_init"
}


//get MMed transaction from pool (use for nb_transport)
template <unsigned int BUSWIDTH, typename TRAITS, unsigned int N, typename BIND_BASE, typename MM_INTERFACE, typename EXT_FILLER, typename POOL>
typename TRAITS::tlm_payload_type* gs::socket::bidirectional_socket_base<BUSWIDTH, TRAITS, N, BIND_BASE, MM_INTERFACE, EXT_FILLER, POOL>::get_transaction(){ 
  if (!m_pool) {
    GS_MSG_OUTPUT(GS_MSG_FATAL, "create_txn() called before pool was constructed (before simulation started). It may only be called during simulation runtime");
  }
  payload_type* tmp=m_pool->construct();
  tmp->acquire();
  return tmp;
}

//return MMed transaction to pool (use for nb_transport)
template <unsigned int BUSWIDTH, typename TRAITS, unsigned int N, typename BIND_BASE, typename MM_INTERFACE, typename EXT_FILLER, typename POOL>
void gs::socket::bidirectional_socket_base<BUSWIDTH, TRAITS, N, BIND_BASE, MM_INTERFACE, EXT_FILLER, POOL>::release_transaction(payload_type* txn){
  txn->release();
}

template <unsigned int BUSWIDTH, typename TRAITS, unsigned int N, typename BIND_BASE, typename MM_INTERFACE, typename EXT_FILLER, typename POOL>
sc_core::sc_object* gs::socket::bidirectional_socket_base<BUSWIDTH, TRAITS, N, BIND_BASE, MM_INTERFACE, EXT_FILLER, POOL>::get_parent() {
  init_base_type* tmp=this; //cast to inti base
  sc_core::sc_object* tmp2=tmp; //cast nach sc_object ist jetzt nicht mehr mehrdeutig da init base genau ei sc_object als base hat
  tmp2 = tmp2->get_parent_object();
  // Note: this has two sc_objects as base. One of init, one of target
  assert(tmp2 == dynamic_cast<sc_core::sc_object*>(dynamic_cast<target_base_type*>(this))->get_parent_object());
  return tmp2; 
}

template <unsigned int BUSWIDTH, typename TRAITS, unsigned int N, typename BIND_BASE, typename MM_INTERFACE, typename EXT_FILLER, typename POOL>
void gs::socket::bidirectional_socket_base<BUSWIDTH, TRAITS, N, BIND_BASE, MM_INTERFACE, EXT_FILLER, POOL>::end_of_elaboration(){
  init_base_type::end_of_elaboration();
  target_base_type::end_of_elaboration();
  // init and target socket part
  assert(init_base_type::m_beoe_disabled == target_base_type::m_eoe_disabled);
  if (init_base_type::m_beoe_disabled) {
    return;
  }
  //ext_support_type::resize_configs();
  assert(init_base_type::size() == target_base_type::size());
  //if (!base_eoe_done) {target_base_type::end_of_elaboration(); base_eoe_done=true;}
  for (unsigned int i=0; i<init_base_type::size(); i++)
    bind_checker_type::check_binding(i); // will call get_num_bindings which will set base_eoe_done=true
  // only init socket part
  if (m_filler==NULL) m_pool=new POOL((mm_base_type*)this); //maybe I should check here that std::string(typeid(EXT_FILLER)).name==std::string(typeid(mm_base_type).name)???
  else m_pool=new POOL(m_filler);    //otherwise pass the filler on to the pool
}

template <unsigned int BUSWIDTH, typename TRAITS, unsigned int N, typename BIND_BASE, typename MM_INTERFACE, typename EXT_FILLER, typename POOL>
void gs::socket::bidirectional_socket_base<BUSWIDTH, TRAITS, N, BIND_BASE, MM_INTERFACE, EXT_FILLER, POOL>::free(payload_type* txn){
  txn->reset();
  m_pool->free(txn);
}

//this function is called when we get info from someone else
// it allows the user to adjust the socket config according to what he learned from the string and the void *
template <unsigned int BUSWIDTH, typename TRAITS, unsigned int N, typename BIND_BASE, typename MM_INTERFACE, typename EXT_FILLER, typename POOL>
void gs::socket::bidirectional_socket_base<BUSWIDTH, TRAITS, N, BIND_BASE, MM_INTERFACE, EXT_FILLER, POOL>::bound_to(const std::string&, typename BIND_BASE::bind_base_type*, unsigned int){
}

template <unsigned int BUSWIDTH, typename TRAITS, unsigned int N, typename BIND_BASE, typename MM_INTERFACE, typename EXT_FILLER, typename POOL>
const std::string& gs::socket::bidirectional_socket_base<BUSWIDTH, TRAITS, N, BIND_BASE, MM_INTERFACE, EXT_FILLER, POOL>::get_type_string(){return s_kind;}

template <unsigned int BUSWIDTH, typename TRAITS, unsigned int N, typename BIND_BASE, typename MM_INTERFACE, typename EXT_FILLER, typename POOL>
const std::string gs::socket::bidirectional_socket_base<BUSWIDTH, TRAITS, N, BIND_BASE, MM_INTERFACE, EXT_FILLER, POOL>::s_kind="basic_green_socket";

template <unsigned int BUSWIDTH, typename TRAITS, unsigned int N, typename BIND_BASE, typename MM_INTERFACE, typename EXT_FILLER, typename POOL>
template<unsigned int M, typename _EXT_FILLER, typename _POOL>
void gs::socket::bidirectional_socket_base<BUSWIDTH, TRAITS, N, BIND_BASE, MM_INTERFACE, EXT_FILLER, POOL>
::bind(gs::socket::bidirectional_socket_base<BUSWIDTH, TRAITS, M, BIND_BASE, MM_INTERFACE, _EXT_FILLER, _POOL>& s){
  init_base_type* own_ib = dynamic_cast<init_base_type*>(this);
  assert(own_ib!=NULL);
  target_base_type* own_tb = dynamic_cast<target_base_type*>(this);
  assert(own_tb!=NULL);
  typename gs::socket::bidirectional_socket_base<BUSWIDTH, TRAITS, M, BIND_BASE, MM_INTERFACE, _EXT_FILLER, _POOL>::init_base_type* other_ib
    = dynamic_cast<typename gs::socket::bidirectional_socket_base<BUSWIDTH, TRAITS, M, BIND_BASE, MM_INTERFACE, _EXT_FILLER, _POOL>::init_base_type*>(&s);
  assert(other_ib!=NULL);
  typename gs::socket::bidirectional_socket_base<BUSWIDTH, TRAITS, M, BIND_BASE, MM_INTERFACE, _EXT_FILLER, _POOL>::target_base_type* other_tb 
    = dynamic_cast<typename gs::socket::bidirectional_socket_base<BUSWIDTH, TRAITS, M, BIND_BASE, MM_INTERFACE, _EXT_FILLER, _POOL>::target_base_type*>(&s);
  assert(other_tb!=NULL);
  // bind
  //std::cout << name() << " bind " << own_ib->name() << " to " << other_tb->name() << std::endl;
  own_ib  ->bind(*other_tb);
  //std::cout << name() << " bind " << other_ib->name() << " to " << own_tb->name() << std::endl;
  other_ib->bind(*own_tb  );
}

template <unsigned int BUSWIDTH, typename TRAITS, unsigned int N, typename BIND_BASE, typename MM_INTERFACE, typename EXT_FILLER, typename POOL>
template<unsigned int M, typename _EXT_FILLER, typename _POOL>
void gs::socket::bidirectional_socket_base<BUSWIDTH, TRAITS, N, BIND_BASE, MM_INTERFACE, EXT_FILLER, POOL>
::operator() (bidirectional_socket_base<BUSWIDTH, TRAITS, M, BIND_BASE, MM_INTERFACE, _EXT_FILLER, _POOL>& s){
  bind(s);
}

template <unsigned int BUSWIDTH, typename TRAITS, unsigned int N, typename BIND_BASE, typename MM_INTERFACE, typename EXT_FILLER, typename POOL>
template<unsigned int M, typename _EXT_FILLER, typename _POOL>
void gs::socket::bidirectional_socket_base<BUSWIDTH, TRAITS, N, BIND_BASE, MM_INTERFACE, EXT_FILLER, POOL>
::bind_to_inner(gs::socket::bidirectional_socket_base<BUSWIDTH, TRAITS, M, BIND_BASE, MM_INTERFACE, _EXT_FILLER, _POOL>& s){
  init_base_type* own_ib = dynamic_cast<init_base_type*>(this);
  assert(own_ib!=NULL);
  target_base_type* own_tb = dynamic_cast<target_base_type*>(this);
  assert(own_tb!=NULL);
  typename gs::socket::bidirectional_socket_base<BUSWIDTH, TRAITS, M, BIND_BASE, MM_INTERFACE, _EXT_FILLER, _POOL>::init_base_type* inner_ib
    = dynamic_cast<typename gs::socket::bidirectional_socket_base<BUSWIDTH, TRAITS, M, BIND_BASE, MM_INTERFACE, _EXT_FILLER, _POOL>::init_base_type*>(&s);
  assert(inner_ib!=NULL);
  typename gs::socket::bidirectional_socket_base<BUSWIDTH, TRAITS, M, BIND_BASE, MM_INTERFACE, _EXT_FILLER, _POOL>::target_base_type* inner_tb 
    = dynamic_cast<typename gs::socket::bidirectional_socket_base<BUSWIDTH, TRAITS, M, BIND_BASE, MM_INTERFACE, _EXT_FILLER, _POOL>::target_base_type*>(&s);
  assert(inner_tb!=NULL);
  // bind
  //std::cout << "hier: " << name() << " bind " << inner_ib->name() << " to " << own_ib->name() << std::endl;
  inner_ib  ->bind(*own_ib);
  //std::cout << "hier: " << name() << " bind " << own_tb->name() << " to " << inner_tb->name() << std::endl;
  own_tb->bind(*inner_tb  );
}

template <unsigned int BUSWIDTH, typename TRAITS, unsigned int N, typename BIND_BASE, typename MM_INTERFACE, typename EXT_FILLER, typename POOL>
unsigned int gs::socket::bidirectional_socket_base<BUSWIDTH, TRAITS, N, BIND_BASE, MM_INTERFACE, EXT_FILLER, POOL>::size(){
  assert(init_base_type::size() == target_base_type::size());
  return init_base_type::size();
}

template <unsigned int BUSWIDTH, typename TRAITS, unsigned int N, typename BIND_BASE, typename MM_INTERFACE, typename EXT_FILLER, typename POOL>
void gs::socket::bidirectional_socket_base<BUSWIDTH, TRAITS, N, BIND_BASE, MM_INTERFACE, EXT_FILLER, POOL>::before_end_of_elaboration(){
  init_base_type::before_end_of_elaboration();
  target_base_type::before_end_of_elaboration();
}

template <unsigned int BUSWIDTH, typename TRAITS, unsigned int N, typename BIND_BASE, typename MM_INTERFACE, typename EXT_FILLER, typename POOL>
void gs::socket::bidirectional_socket_base<BUSWIDTH, TRAITS, N, BIND_BASE, MM_INTERFACE, EXT_FILLER, POOL>::start_of_simulation(){
  init_base_type::start_of_simulation();
  target_base_type::start_of_simulation();
}

// TODO: should return hierarchical name
template <unsigned int BUSWIDTH, typename TRAITS, unsigned int N, typename BIND_BASE, typename MM_INTERFACE, typename EXT_FILLER, typename POOL>
const char* gs::socket::bidirectional_socket_base<BUSWIDTH, TRAITS, N, BIND_BASE, MM_INTERFACE, EXT_FILLER, POOL>::name() const {
  return m_name.c_str();
}
