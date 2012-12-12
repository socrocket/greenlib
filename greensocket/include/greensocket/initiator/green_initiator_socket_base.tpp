//   GreenSocket
//
// LICENSETEXT
//
//   Copyright (C) 2007 : GreenSocs Ltd
// 	 http://www.greensocs.com/ , email: info@greensocs.com
//
//   Developed by :
//   
//   Robert Guenzel <guenzel@eis.cs.tu-bs.de>,
//     Technical University of Braunschweig, Dept. E.I.S.
//     http://www.eis.cs.tu-bs.de
//
//
// The contents of this file are subject to the licensing terms specified
// in the file LICENSE. Please consult this file for restrictions and
// limitations that may apply.
// 
// ENDLICENSETEXT

// the DTOR only needs to delete the POOL
template <unsigned int BUSWIDTH, typename TRAITS, unsigned int N, typename BIND_BASE, typename MM_INTERFACE, typename EXT_FILLER, typename POOL>
gs::socket::initiator_socket_base<BUSWIDTH, TRAITS, N, BIND_BASE, MM_INTERFACE, EXT_FILLER, POOL>::~initiator_socket_base(){
  if (m_pool) delete m_pool;
}

//the CTOR initializes the base classes and sets the pool ptr to NULL
// see gs_tech.pdf
template <unsigned int BUSWIDTH, typename TRAITS, unsigned int N, typename BIND_BASE, typename MM_INTERFACE, typename EXT_FILLER, typename POOL>
gs::socket::initiator_socket_base<BUSWIDTH, TRAITS, N, BIND_BASE, MM_INTERFACE, EXT_FILLER, POOL>::initiator_socket_base
  (const char* name, allocation_scheme scheme, EXT_FILLER* ext_filler): base_type(name, static_cast<typename BIND_BASE::bind_base_type*>(this))
                                                 , bind_checker_type(base_type::name(), (base_type*)this, BUSWIDTH)
                                                 //, ext_support_type(BUSWIDTH)
                                                 , mm_base_type(scheme)
                                                 , m_filler(ext_filler)
{
  m_pool=NULL;
}


//get MMed transaction from pool (use for nb_transport)
template <unsigned int BUSWIDTH, typename TRAITS, unsigned int N, typename BIND_BASE, typename MM_INTERFACE, typename EXT_FILLER, typename POOL>
typename TRAITS::tlm_payload_type* gs::socket::initiator_socket_base<BUSWIDTH, TRAITS, N, BIND_BASE, MM_INTERFACE, EXT_FILLER, POOL>::get_transaction(){ 
  if (!m_pool) {
    GS_MSG_OUTPUT(GS_MSG_FATAL, "create_txn() called before pool was constructed (before simulation started). It may only be called during simulation runtime");
  }
  payload_type* tmp=m_pool->construct();
  tmp->acquire(); //acquire on behalf of user. A must do according to TLM-2.0 LRM anyway
  return tmp;
}

//return MMed transaction to pool (use for nb_transport)
// the txn->release could be called by the user as well, but when the user calls get_transaction on the socket is only consequent to 
// have a call on the socket at the end as well
template <unsigned int BUSWIDTH, typename TRAITS, unsigned int N, typename BIND_BASE, typename MM_INTERFACE, typename EXT_FILLER, typename POOL>
void gs::socket::initiator_socket_base<BUSWIDTH, TRAITS, N, BIND_BASE, MM_INTERFACE, EXT_FILLER, POOL>::release_transaction(payload_type* txn){
  txn->release(); 
}


//execute end_of_elab only if not disabled in the base class (disabled means the socket is bound hierarchically)
template <unsigned int BUSWIDTH, typename TRAITS, unsigned int N, typename BIND_BASE, typename MM_INTERFACE, typename EXT_FILLER, typename POOL>
void gs::socket::initiator_socket_base<BUSWIDTH, TRAITS, N, BIND_BASE, MM_INTERFACE, EXT_FILLER, POOL>::end_of_elaboration(){
  if (base_type::m_beoe_disabled) {
    return;
  }
  for (unsigned int i=0; i<base_type::size(); i++) //check all the bindings one after the other
    bind_checker_type::check_binding(i);
  
  //if not extension filler was provided then use the socket (the mm_base actually) as the filler
  if (m_filler==NULL) m_pool=new POOL((mm_base_type*)this); //maybe I should check here that std::string(typeid(EXT_FILLER)).name==std::string(typeid(mm_base_type).name)???
  else m_pool=new POOL(m_filler);    //otherwise pass the filler on to the pool
}

//when the ref count hits zero, we reset the txn (means all auto_extension are removed, all sticky extension stay in) and then we send it back to the pool
template <unsigned int BUSWIDTH, typename TRAITS, unsigned int N, typename BIND_BASE, typename MM_INTERFACE, typename EXT_FILLER, typename POOL>
void gs::socket::initiator_socket_base<BUSWIDTH, TRAITS, N, BIND_BASE, MM_INTERFACE, EXT_FILLER, POOL>::free(payload_type* txn){
  txn->reset();
  m_pool->free(txn);
}

//this function is called when we are bound to someone else
template <unsigned int BUSWIDTH, typename TRAITS, unsigned int N, typename BIND_BASE, typename MM_INTERFACE, typename EXT_FILLER, typename POOL>
void gs::socket::initiator_socket_base<BUSWIDTH, TRAITS, N, BIND_BASE, MM_INTERFACE, EXT_FILLER, POOL>::bound_to(const std::string& other_type, typename BIND_BASE::bind_base_type* base_type, unsigned int index){
  //inform timing support class that binding is okay.
  gs::socket::initiator_timing_support_base<TRAITS,BIND_BASE >::bound_to(other_type, base_type,index);
}

//return the type string as required by the bind checker
template <unsigned int BUSWIDTH, typename TRAITS, unsigned int N, typename BIND_BASE, typename MM_INTERFACE, typename EXT_FILLER, typename POOL>
const std::string& gs::socket::initiator_socket_base<BUSWIDTH, TRAITS, N, BIND_BASE, MM_INTERFACE, EXT_FILLER, POOL>::get_type_string(){return s_kind;}

template <unsigned int BUSWIDTH, typename TRAITS, unsigned int N, typename BIND_BASE, typename MM_INTERFACE, typename EXT_FILLER, typename POOL>
const std::string gs::socket::initiator_socket_base<BUSWIDTH, TRAITS, N, BIND_BASE, MM_INTERFACE, EXT_FILLER, POOL>::s_kind="basic_green_socket";
