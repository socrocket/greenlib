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

//the CTOR only store the provided bus width
/*
template <typename TRAITS>
gs::socket::extension_support_base<TRAITS>::extension_support_base(unsigned int bw): m_bus_width(bw)
{
}

//the dtor does nothing
template <typename TRAITS>
gs::socket::extension_support_base<TRAITS>::~extension_support_base(){
}
*/

//if we wanna get an extension we let the compiler choose the correct function
// depending on the type of the extension internal _ID (work only for greensocket extensions)
template <typename TRAITS>
template <typename T>
bool gs::socket::extension_support_base<TRAITS>::get_extension(T*& ptr, payload_type& txn){
  return get_extension<T>(T::_ID, ptr, txn);
}

//if we wanna get an extension we let the compiler choose the correct function
// depending on the type of the extension internal _ID (work only for greensocket extensions)
template <typename TRAITS>
template <typename T>
T* gs::socket::extension_support_base<TRAITS>::get_extension(payload_type& txn){
  return get_extension<T>(T::_ID, txn);
}

//if we wanna validate an extension we let the compiler choose the correct function
// depending on the type of the extension internal _ID (work only for greensocket extensions)
template <typename TRAITS>
template <typename T>
bool gs::socket::extension_support_base<TRAITS>::validate_extension(payload_type& txn, unsigned int index){
  return validate_extension<T>(T::_ID, txn, index);
} 

//if we wanna invalidate an extension we let the compiler choose the correct function
// depending on the type of the extension internal _ID (work only for greensocket extensions)
template <typename TRAITS>
template <typename T>
void gs::socket::extension_support_base<TRAITS>::invalidate_extension(payload_type& txn, unsigned int index){
  invalidate_extension<T>(T::_ID, txn, index);
}

//implementation of the bool-returning get_extension for guarded data extensions
template <typename TRAITS>
template <typename T>
bool gs::socket::extension_support_base<TRAITS>::get_extension(const gs::ext::gs_guarded_data_id&, T*& ptr, payload_type& txn){
  txn.get_extension(ptr); //firs we take the ptr from the extension array of the paylaod
  
  //if it is not there we create it, and put it into the transaction, and set the valid flag to false
  if (!ptr){ 
    ptr=gs::ext::create_extension<T>();
    txn.set_extension(ptr);
    ptr->m_valid=false;
    return false;
  }
  typename T::guard* g_ptr; //now create a pointer to the guard extension
  txn.get_extension(g_ptr); //and we try to get it.
  if (g_ptr && ptr->m_valid) return true; //if the guard is there AND the m_valid flag is set, the whole extension is treated valid
  return false;
}

//implementation of the bool-returning get_extension for data only extensions
template <typename TRAITS>
template <typename T>
bool gs::socket::extension_support_base<TRAITS>::get_extension(const gs::ext::gs_data_id&, T*& ptr, payload_type& txn){
  txn.get_extension(ptr); //try to get the ptr from the transaction
  if (!ptr){ //if it is not there, create and add
    ptr=gs::ext::create_extension<T>();//new T();
    txn.set_extension(ptr);
  }
  return true; //always returns true, since data only extensions have no determinable validity
}

//implementation of the ptr-returning get_extension for real guard extensions
/*
I distinguish between real guard and memory management (mm) guard:
 The real guard is the thing your model uses, and that is actually only a boolean switch. On or Off. E.g. Cachable or Not.
 However, when a txn travels from init to target the state of the guad might change (on, off, on, off).
 In nb_transport it needs to be set using set_auto_extension, so that it is removed later. However calling
 set_auto_extension twice for one extension can cause an overflow of the auto extension stack within the extension array
 (look into the TLM-2.0 implementation if you dare).
 So we cannot just set the guard using set_auto to switch it on, then use clear extension to switch it off, and then set_auto
 to switch it on again. At the second time we switch it on, we can only use set_extension (not set_auto). But we cannot know
 it was previously set using set_auto. So we need additional info in the guard. Hence it is a guarded data bool.
 The mm guard is then the thing that is set as auto_extension, to make sure the state of the guard is also effected by
 the memory management.
*/
template <typename TRAITS>
template <typename T>
T* gs::socket::extension_support_base<TRAITS>::get_extension(const gs::ext::gs_real_guard_id&, payload_type& txn){
  T* ptr; 
  txn.get_extension(ptr); //first get the ptr from the transaction
  if (!ptr) return NULL; //if not there, the whole guard is invalid, so return NULL
  typename T::guard* g_ptr; //otherwise we create a pointer to its mm guard (a guard is actually a guarded data bool)
  txn.get_extension(g_ptr); //we get the guard
  if (g_ptr && ptr->m_valid) return ptr;  //the whole guard is considered valid if its mm guard is there and the internal bool is valid
  return NULL;
}

//implementation of the ptr-returning get_extension for data only extensions
template <typename TRAITS>
template <typename T>
T* gs::socket::extension_support_base<TRAITS>::get_extension(const gs::ext::gs_data_id&, payload_type& txn){
  T* tmp=txn.template get_extension<T>(); //get the pointer
  if (!tmp) { //test if it is there
    tmp=gs::ext::create_extension<T>();  //if not create and add
    txn.set_extension(tmp);
  } 
  return tmp;
}

//validate a guard extension
template <typename TRAITS>
template <typename T>
bool gs::socket::extension_support_base<TRAITS>::validate_extension(const gs::ext::gs_real_guard_id&, payload_type& txn, unsigned int index){
  T* ptr; //get the extension pointer from the extension array
  txn.get_extension(ptr);
  if (!ptr){ //if it is not there create and add
    ptr=gs::ext::create_extension<T>();
    txn.set_extension(ptr);
  }
  ptr->m_valid=true; //set valid bool to true
  typename T::guard* g_ptr;
  txn.get_extension(g_ptr); //get the mm guard from the extension array
  if (txn.has_mm()){ // if we have mm 
    if (!g_ptr) //if the mm guard is not there add it as auto now (so, when the txn is reset it will be removed, making the guard invalid
      txn.set_auto_extension(T::guard::ID, gs::ext::create_extension<typename T::guard>());
    return true; //return true, i.e. show user that mm was there
  }
  //if there is no mm, we set the mm guard as a normal extension
  txn.set_extension(T::guard::ID, gs::ext::create_extension<typename T::guard>());
  return false;  //and show the user that there was no mm
}

//validate a guarded data extension
// works just like a guard extension
template <typename TRAITS>
template <typename T>
bool gs::socket::extension_support_base<TRAITS>::validate_extension(const gs::ext::gs_guarded_data_id&, payload_type& txn, unsigned int index){
  T* ptr;
  txn.get_extension(ptr);
  if (!ptr){
    ptr=gs::ext::create_extension<T>();
    txn.set_extension(ptr);
  }
  ptr->m_valid=true;
  typename T::guard* g_ptr;
  txn.get_extension(g_ptr);
  if (txn.has_mm()){
    if (!g_ptr) txn.set_auto_extension(T::guard::ID, gs::ext::create_extension<typename T::guard>());
    return true;
  }
  else{
    txn.set_extension(T::guard::ID, gs::ext::create_extension<typename T::guard>());
    return false;  
  }
  return false;
}

//invalidate a guard extension
template <typename TRAITS>
template <typename T>
void gs::socket::extension_support_base<TRAITS>::invalidate_extension(const gs::ext::gs_real_guard_id&, payload_type& txn, unsigned int index){
  //if we do not have mm we just clear the mm guard (since there is no mm this is save)
  if (!txn.has_mm()) 
    txn.template clear_extension<typename T::guard>(); 
  else{
  //if there is mm, we get the ptr to the extension, and if it is there we will only reset the bool
  // to make sure we do not corrupt the auto extension stack
    T* ext=txn.template get_extension<T>();
    if (ext) ext->m_valid=false;
  }
}

//invalidate a guarded data extension
// works just like a guard extension
template <typename TRAITS>
template <typename T>
void gs::socket::extension_support_base<TRAITS>::invalidate_extension(const gs::ext::gs_guarded_data_id&, payload_type& txn, unsigned int index){
  if (!txn.has_mm()) txn.template clear_extension<typename T::guard>();
  else{
    T* ext=txn.template get_extension<T>();
    if (ext) ext->m_valid=false;
  }
}
