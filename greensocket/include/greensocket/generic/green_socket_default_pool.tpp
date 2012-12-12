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

//ctor of the linked list entries. Needs an associated object
template<typename TRAITS,typename EXT_FILLER,unsigned int N>
gs::socket::default_pool<TRAITS,EXT_FILLER,N>::entry::entry(payload_type* content){
  that=content;
  next=NULL;
}
    
//ctor of the POOL. gets a pointer to a filler
template<typename TRAITS,typename EXT_FILLER,unsigned int N>
gs::socket::default_pool<TRAITS,EXT_FILLER,N>::default_pool(void* filler): used(NULL), m_filler(static_cast<EXT_FILLER*>(filler)){
  unused=new entry(m_filler->fill_txn(new payload_type()));  //create first one
  mine.push_back(unused->that); //add to the vector of allocated objects
  //now create the rest of the pool (so that we end up with N objects)
  for (unsigned int i=0; i<N-1; i++){
    entry* e=new entry(m_filler->fill_txn(new payload_type()));
    e->next=unused; //build a linked list
    unused=e;
    mine.push_back(unused->that); //and always remember what we allocated
  }
}

//destructor. clean up what we created
template<typename TRAITS,typename EXT_FILLER,unsigned int N>
gs::socket::default_pool<TRAITS,EXT_FILLER,N>::~default_pool(){
#ifdef GS_DEBUG_GREEN_SOCKET_DEFAULT_POOL
  std::cout<<((used)? "NOT all have returned!":"ALL have returned")<<std::endl;
  if (used){
    unsigned int tmp=0;
    entry* e=used;
    while (e) {tmp++; e=e->next;}
    std::cout<<"  "<<tmp<<" elements are still outside the pool"<<std::endl;
  }
#endif
  //delete all payload_type* that belong to this pool
  for (unsigned int i=0; i<mine.size(); i++){
    delete mine[i];
  }
  
  //delete all unused elements
  while (unused){
    entry* e=unused;
    unused=unused->next;
    delete e;
  }

  //delete all used elements
  while (used){
    entry* e=used;
    used=used->next;
    delete e;
  }
}

//search the vector of objects that we allocated if the provided thing is from our pool
template<typename TRAITS,typename EXT_FILLER,unsigned int N>
bool gs::socket::default_pool<TRAITS,EXT_FILLER,N>::is_from(payload_type* cont){ //slow!!!
  for (unsigned int i=0; i<mine.size(); i++){
    if (mine[i]==cont) return true;
  }
  return false;
}

//create a transaction
template<typename TRAITS,typename EXT_FILLER,unsigned int N>
typename TRAITS::tlm_payload_type* gs::socket::default_pool<TRAITS,EXT_FILLER,N>::construct(){
  entry* e;
  if (unused==NULL){ //we have no available object, so we create a new one
    e=new entry(m_filler->fill_txn(new payload_type()));
    mine.push_back(e->that);
  }
  else{
    e=unused; //we have at least one available, so we take it from our linked list...
    unused=unused->next; //... and move the head of the linked list to the next
  }
  e->next=used; //we put the thing into the linked list of used entries
  used=e;
#ifdef GS_DEBUG_GREEN_SOCKET_DEFAULT_POOL  
  std::cout<<"CTRT "<<used->that<<std::endl;
#endif
  return used->that; // and finally we return the content (the object) from the entries 
}

template<typename TRAITS,typename EXT_FILLER,unsigned int N>
void gs::socket::default_pool<TRAITS,EXT_FILLER,N>::free(payload_type* cont){
#ifdef GS_DEBUG_GREEN_SOCKET_DEFAULT_POOL
  std::cout<<"FREE "<<cont<<std::endl;
#endif
  assert(used); //we should have an entry object available in the used list
  entry* e=used; //we take it
  used=e->next; //and move the head of the used list
  e->that=cont; //we put the object into that entry
  e->next=unused; //and make it the new head of our unused list
  unused=e;
}
