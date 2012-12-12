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

//the CTOR just stores the used allocation scheme info
template <typename TRAITS, typename MM_INTERFACE>
gs::socket::mm_base<TRAITS,MM_INTERFACE>::mm_base(allocation_scheme scheme)
  : m_scheme(scheme)
  {}

//the txn fill function
template <typename TRAITS, typename MM_INTERFACE>
typename gs::socket::mm_base<TRAITS,MM_INTERFACE>::payload_type* gs::socket::mm_base<TRAITS,MM_INTERFACE>::
fill_txn(payload_type* txn){
  txn->set_mm(this); //set the memory manager into the txn
  
  //in case we support byte enable or data array allocation, we add the extension we need for this
  if (m_scheme!=GS_TXN_ONLY) {
    per_txn_data_and_be* tmp=new per_txn_data_and_be();
    txn->set_extension(tmp);
  }
  return txn; //return the filled txn
}

//get the reserved data array size
template <typename TRAITS, typename MM_INTERFACE>
unsigned int gs::socket::mm_base<TRAITS,MM_INTERFACE>::get_reserved_data_size(payload_type& txn){
  assert(m_scheme==GS_TXN_WITH_DATA | m_scheme==GS_TXN_WITH_BE_AND_DATA); //make sure the allocation scheme is right
  per_txn_data_and_be* tmp; //get the extension
  txn.get_extension(tmp);
  assert(tmp); //make sure it is there
  return tmp->value.data.size(); //return the size of available data 
}

//get the reserved byte enable array size
template <typename TRAITS, typename MM_INTERFACE>
unsigned int gs::socket::mm_base<TRAITS,MM_INTERFACE>::get_reserved_be_size(payload_type& txn ){
  assert(m_scheme==GS_TXN_WITH_BE | m_scheme==GS_TXN_WITH_BE_AND_DATA);//make sure the allocation scheme is right
  per_txn_data_and_be* tmp;//get the extension
  txn.get_extension(tmp);
  assert(tmp); //make sure it is there
  return tmp->value.be.size();//return the size of available byte enables
}

#ifndef GS_DATA_ARRAY_INIT_PATTERN
#define GS_DATA_ARRAY_INIT_PATTERN 0
#endif

//reserve the requested data size
template <typename TRAITS, typename MM_INTERFACE>
void gs::socket::mm_base<TRAITS,MM_INTERFACE>::reserve_data_size(payload_type& txn, unsigned int size){
  assert((m_scheme==GS_TXN_WITH_DATA) | (m_scheme==GS_TXN_WITH_BE_AND_DATA));//make sure the allocation scheme is right
  per_txn_data_and_be* tmp;//get the extension
  txn.get_extension(tmp);
  assert(tmp);//make sure it is there
  
  if (tmp->value.data.size()<size) { //if the data size is too small we increase the vector size
    tmp->value.data.resize(size);
  }
  txn.set_data_ptr(&(tmp->value.data[0])); //then we set the data pointer to the vector's internal array
  txn.set_data_length(size); //and we set the data length
#ifdef GS_ALLOCATE_DATA_ARRAY_WITH_PATTERN
  for (unsigned int i=0; i<size; i++) tmp->value.data[i]=GS_DATA_ARRAY_INIT_PATTERN;
#endif
}

//see reserve_data_size. works in the same way
template <typename TRAITS, typename MM_INTERFACE>
void gs::socket::mm_base<TRAITS,MM_INTERFACE>::reserve_be_size(payload_type& txn, unsigned int size){
  assert(m_scheme==GS_TXN_WITH_BE | m_scheme==GS_TXN_WITH_BE_AND_DATA);
  per_txn_data_and_be* tmp;
  txn.get_extension(tmp);
  assert(tmp);
  if (tmp->value.be.size()<size) {
    tmp->value.be.resize(size);
  }  
  txn.set_byte_enable_ptr(&(tmp->value.be[0]));
  txn.set_byte_enable_length(size);
}

