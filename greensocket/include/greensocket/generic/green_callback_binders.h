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

#ifndef __GS_MULTI_SOCKET_BASES_H__
#define __GS_MULTI_SOCKET_BASES_H__

#include <map>
#ifdef GS_BE_BINDABLE_TO_OSCI_MULTI_SOCKETS
#include "tlm_utils/multi_socket_bases.h"
#endif

#include "greensocket/generic/gs_callbacks.h"

namespace gs{
namespace socket{

/*
** This is the base class has three jobs:
** a) hold the index and tag offset for a given binder
** b) keep a pointer to the owner (the socket) of the binder
** c) allow the bind checker to cast a given sc_interface into this 
**    (template free) class to test if the interface belongs to a greensocket binder
*/
class gs_callback_binder_base
{
public:

    int get_index(){return m_id;} //get the index (or ID) of a binder
    
  void* get_owner(){ //get the owner of a binder
    return m_owner;
  }
  
  //set the owner of a binder
  template<typename T>
  void  set_owner(T* owner) {
    m_owner=static_cast<void*>(owner);
  }
  
  //set the tag offset of a binder
  void set_offset(unsigned int offset){m_offset=offset;}
  
  template<typename T>
  gs_callback_binder_base(int id, T* owner, unsigned int offset)
    : m_owner(static_cast<void*>(owner))
    , m_id(id)
    , m_offset(offset)
  {
  }

protected:
  void* m_owner; //the owner
  unsigned int m_id; //the ID
  unsigned int m_offset;  //the tag offset
};


/*
** This class is the base of fw binders. It allows to ask a fw binder about the socket
** that is bound to it.
** The fw binders exist in two flavors with and without tag (i.e. index+tag). Both share what is in this class
*/
template <typename TRAITS, typename CB_BINDER_BASE>
class gs_callback_binder_fw_base 
  : public tlm::tlm_fw_transport_if<TRAITS>
  , public CB_BINDER_BASE
{
public:
    template<typename T>
    gs_callback_binder_fw_base(int id, T* owner, unsigned int offset)
      : CB_BINDER_BASE(id, owner, offset)
    {
    }

    //getter method to get the port that is bound to that callback binder
    // NOTE: this will only return a valid value at end of elaboration
    //  (but not before end of elaboration!)
    sc_core::sc_port_base* get_other_side(){return m_caller_port;}
    //the SystemC standard callback register_port:
    // - called when a port if bound to the interface
    // - allows to find out who is bound to that callback binder
    // - needed to make get_other_side work
    void register_port(sc_core::sc_port_base& b, const char* name){
      m_caller_port=&b;
    }

protected:
    //the port bound to that callback binder
    sc_core::sc_port_base* m_caller_port;
};

//define the template nature of the fw binders. This default implementation
// is actually never used, as we will define partial template sepcialization
// for TAGGED=true and TAGGED=false below, such that any conceivable set of 
// template args will fall into one of the two specializations
template <typename TRAITS, bool TAGGED, typename CB_BINDER_BASE>
class gs_callback_binder_fw{};

/*
Below is the specialization of the fw binder when TAGGED=true.
The class implements the fw interface.
It allows to register a callback for each of the fw interface methods.
The callbacks simply forward the fw interface call, but add the id (an int)
of the callback binder to the signature of the call.
They also add a tag offset (if available).

The specialization share a lot of code. To avoid code replication and to make
sure that a bug fix in one specialization automatically translates into the other
we use file fragment inclusion to handle the shared code.
*/

template <typename TRAITS, typename CB_BINDER_BASE> //partial template specialization
class gs_callback_binder_fw<TRAITS, true, CB_BINDER_BASE> : public gs_callback_binder_fw_base<TRAITS, CB_BINDER_BASE>{
//the #def below is used to "activate" the file fragment. It makes sure the file fragment was included knowing
// that it is just a fragment. So if you accidentally include this you will get a sensible error message. Not a couple
// of hundred weird compiler errors
#define GREEN_I_KNOW_VERY_WELL_WHAT_I_AM_DOING_WHEN_INCLUDING_THIS_FILE

//the #def below activates the TAGGING specific things in the file fragement
#define GREEN_USE_WITH_TAG

//now include the actual fragment
#include "greensocket/generic/green_callback_fw_binders_body.h"

//undef the things above
#undef GREEN_USE_WITH_TAG
#undef GREEN_I_KNOW_VERY_WELL_WHAT_I_AM_DOING_WHEN_INCLUDING_THIS_FILE

//the menbers (actually their types) are highly specialization depend
// so they are not part of the fragment
  private:
    
    //the callbacks
    nb_transport_tagged_functor<TRAITS>* m_nb_fw_functor;
    b_transport_tagged_functor<TRAITS>*  m_b_functor;
    get_direct_mem_ptr_tagged_functor<TRAITS>* m_get_dmi_functor;
    transport_dbg_tagged_functor<TRAITS>* m_dbg_functor;
    
};

/*
Below is the specialization of the fw binder when TAGGED=false.
The class implements the fw interface.
It allows to register a callback for each of the fw interface methods.
The callbacks simply forward the fw interface calls.

The specialization share a lot of code. To avoid code replication and to make
sure that a bug fix in one specialization automatically translates into the other
we use file fragment inclusion to handle the shared code.
*/

template <typename TRAITS, typename CB_BINDER_BASE>
class gs_callback_binder_fw<TRAITS, false, CB_BINDER_BASE> : public gs_callback_binder_fw_base<TRAITS, CB_BINDER_BASE>{

//the #def below is used to "activate" the file fragment. It makes sure the file fragment was included knowing
// that it is just a fragment. So if you accidentally include this you will get a sensible error message. Not a couple
// of hundred weird compiler errors
#define GREEN_I_KNOW_VERY_WELL_WHAT_I_AM_DOING_WHEN_INCLUDING_THIS_FILE

//now include the file. We do NOT activate the TAGGING specific parts of the file fragment
#include "greensocket/generic/green_callback_fw_binders_body.h"

//undef the thing above
#undef GREEN_I_KNOW_VERY_WELL_WHAT_I_AM_DOING_WHEN_INCLUDING_THIS_FILE

//the menbers (actually their types) are highly specialization depend
// so they are not part of the fragment
  private:
    //the callbacks
    nb_transport_functor<TRAITS>* m_nb_fw_functor;
    b_transport_functor<TRAITS>*  m_b_functor;
    get_direct_mem_ptr_functor<TRAITS>* m_get_dmi_functor;
    transport_dbg_functor<TRAITS>* m_dbg_functor;
};

/*
** This class is the base of bw binders. 
** It's purpose is just to make the class hierarchy of fw and bw binders symetrical
*/

template <typename TRAITS, typename CB_BINDER_BASE>
class gs_callback_binder_bw_base
  : public tlm::tlm_bw_transport_if<TRAITS>
  , public CB_BINDER_BASE
{
public:
    template <typename T>
    gs_callback_binder_bw_base(int id, T* owner, unsigned int offset)
      : CB_BINDER_BASE(id, owner, offset)
    {
    }
};

//define the template nature of the bw binders. This default implementation
// is actually never used, as we will define partial template sepcialization
// for TAGGED=true and TAGGED=false below, such that any conceivable set of 
// template args will fall into one of the two specializations
template <typename TRAITS, bool TAGGED, typename CB_BINDER_BASE>
class gs_callback_binder_bw{};


/*
Below is the specialization of the bw binder when TAGGED=true.
The class implements the bw interface.
It allows to register a callback for each of the fw interface methods.
The callbacks simply forward the fw interface call, but add the id (an int)
of the callback binder to the signature of the call.
They also add a tag offset (if available).

The specializations share a lot of code. To avoid code replication and to make
sure that a bug fix in one specialization automatically translates into the other
we use file fragment inclusion to handle the shared code.

Have look into the corresponding fw binder class to see comments
*/
template <typename TRAITS, typename CB_BINDER_BASE>
class gs_callback_binder_bw<TRAITS, true, CB_BINDER_BASE>: public gs_callback_binder_bw_base<TRAITS, CB_BINDER_BASE>{
#define GREEN_I_KNOW_VERY_WELL_WHAT_I_AM_DOING_WHEN_INCLUDING_THIS_FILE
#define GREEN_USE_WITH_TAG
#include "greensocket/generic/green_callback_bw_binders_body.h"
#undef GREEN_USE_WITH_TAG
#undef GREEN_I_KNOW_VERY_WELL_WHAT_I_AM_DOING_WHEN_INCLUDING_THIS_FILE
  private:
    //the callbacks
    nb_transport_tagged_functor<TRAITS>* m_nb_bw_functor;
    invalidate_direct_mem_ptr_tagged_functor<TRAITS>* m_inval_dmi_functor;
};

/*
Below is the specialization of the bw binder when TAGGED=false.
The class implements the bw interface.
It allows to register a callback for each of the fw interface methods.
The callbacks simply forward the fw interface calls.

The specialization share a lot of code. To avoid code replication and to make
sure that a bug fix in one specialization automatically translates into the other
we use file fragment inclusion to handle the shared code.

Have look into the corresponding fw binder class to see comments
*/

template <typename TRAITS, typename CB_BINDER_BASE>
class gs_callback_binder_bw<TRAITS, false, CB_BINDER_BASE>: public gs_callback_binder_bw_base<TRAITS, CB_BINDER_BASE>{
#define GREEN_I_KNOW_VERY_WELL_WHAT_I_AM_DOING_WHEN_INCLUDING_THIS_FILE
#include "greensocket/generic/green_callback_bw_binders_body.h"
#undef GREEN_I_KNOW_VERY_WELL_WHAT_I_AM_DOING_WHEN_INCLUDING_THIS_FILE
  private:
    //the callbacks
    nb_transport_functor<TRAITS>* m_nb_bw_functor;
    invalidate_direct_mem_ptr_functor<TRAITS>* m_inval_dmi_functor;
};


/*
This class forms the base for multi initiator sockets.
It enforces a multi initiator socket to implement all functions
needed to do hierarchical bindings.
*/
template <unsigned int BUSWIDTH = 32,
          typename TRAITS = tlm::tlm_base_protocol_types,
          unsigned int N=0,
          typename CB_BINDER_BASE = gs_callback_binder_base
#if !(defined SYSTEMC_VERSION & SYSTEMC_VERSION <= 20050714)
          ,sc_core::sc_port_policy POL = sc_core::SC_ONE_OR_MORE_BOUND
#endif
          >
class multi_init_base: public tlm::tlm_initiator_socket<BUSWIDTH,
                                                  TRAITS,
                                                  N
#if !(defined SYSTEMC_VERSION & SYSTEMC_VERSION <= 20050714)
                                                  ,POL
#endif
                                                  >{
public:
  //typedef for the base type: the standard tlm initiator socket
  typedef tlm::tlm_initiator_socket<BUSWIDTH,
                              TRAITS,
                              N
#if !(defined SYSTEMC_VERSION & SYSTEMC_VERSION <= 20050714)
                              ,POL
#endif
                              > base_type;
  
  //this method shall disable the code that does the callback binding
  // that registers callbacks to binders
  virtual void disable_cb_bind()=0;
  
  //this method shall return the multi_init_base to which the
  // multi_init_base is bound hierarchically
  //  If the base is not bound hierarchically it shall return a pointer to itself
  virtual multi_init_base* get_hierarch_bind()=0;
  
  //this method shall return a vector of the callback binders of multi initiator socket
  virtual std::vector<gs_callback_binder_bw<TRAITS, N!=1, CB_BINDER_BASE>* >& get_binders()=0;
  
  //this method shall return a vector of all target interfaces bound to this multi init socket
  virtual std::vector<tlm::tlm_fw_transport_if<TRAITS>*>&  get_sockets()=0;
  
  //this method shall get the tag offset
  virtual unsigned int  get_socket_id_int()=0;
  
  //ctor and dtor
  virtual ~multi_init_base(){}
  multi_init_base(const char* name):base_type(name){}
};

/*
This class forms the base for multi target sockets.
It enforces a multi target socket to implement all functions
needed to do hierarchical bindings.
*/
template <unsigned int BUSWIDTH = 32,
          typename TRAITS = tlm::tlm_base_protocol_types,
          unsigned int N=0,
          typename CB_BINDER_BASE = gs_callback_binder_base
#if !(defined SYSTEMC_VERSION & SYSTEMC_VERSION <= 20050714)
          ,sc_core::sc_port_policy POL = sc_core::SC_ONE_OR_MORE_BOUND
#endif
          >
class multi_target_base: public tlm::tlm_target_socket<BUSWIDTH, 
                                                TRAITS,
                                                N
#if !(defined SYSTEMC_VERSION & SYSTEMC_VERSION <= 20050714)                                                
                                                ,POL
#endif
                                                >{
public:
  //typedef for the base type: the standard tlm target socket
  typedef tlm::tlm_target_socket<BUSWIDTH, 
                              TRAITS,
                              N
#if !(defined SYSTEMC_VERSION & SYSTEMC_VERSION <= 20050714)
                              ,POL
#endif
                              > base_type;
  
  //this method shall return the multi_init_base to which the
  // multi_init_base is bound hierarchically
  //  If the base is not bound hierarchically it shall return a pointer to itself                                                
  virtual multi_target_base* get_hierarch_bind()=0;
  
  //this method shall inform the multi target socket that it is bound
  // hierarchically and to which other multi target socket it is bound hierarchically
  virtual void set_hierarch_bind(multi_target_base*)=0;
  
  //this method shall return a vector of the callback binders of multi initiator socket
  virtual std::vector<gs_callback_binder_fw<TRAITS,N!=1,CB_BINDER_BASE>* >& get_binders()=0;
  
  //this method shall return a map of all multi initiator sockets that are bound to this multi target
  // the key of the map is the index at which the multi initiator i bound, while the value
  //  is the interface of the multi initiator socket that is bound at that index
  virtual std::map<unsigned int, tlm::tlm_bw_transport_if<TRAITS>*>&  get_multi_binds()=0;
  
  //this method shall get the tag offset
  virtual unsigned int  get_socket_id_int()=0;
  
  //ctor and dtor
  virtual ~multi_target_base(){}
  multi_target_base(const char* name):base_type(name){}
};


//this class is used to allow initiators to identify multi sockets
// they simply try to cast a bound socket into that class. If it succeeds
// they know the target is a multi socket and treat it differently than single sockets
template <typename TRAITS>
class gs_multi_to_multi_bind_base{
public:
  virtual ~gs_multi_to_multi_bind_base(){}
  virtual tlm::tlm_fw_transport_if<TRAITS>* get_last_binder(tlm::tlm_bw_transport_if<TRAITS>*)=0;
};


//when a greensocket is used with N=1 the signatures of the callbacks match
// the TLM-2.0 signatures. If it is used with N!=1 the signature is extended
// with an additional unsigned int that identifies the index of the binding
// over which the call was received.

//to avoid an awful lot of code replication we use type designator classes
// for the used functor types and the function signatures. That means the
// classes contain a bunch of typedefs, and depending on the template args
// the types will differ. So the using class can just do type_designator_class<X>::some_type_def
// and depending on X will use the correct type. Consequently, the using class
// does not need to care about the distinction.

//this is the type designator class template for the functor types
// the default is empty, but since we are going to sepcialize it
// for both possible Boolean values, it will never be used
template <typename TRAITS, bool TAGGED>
struct multi_socket_functor_types{
};

//this class contains the functor types when they shall use tags.
template <typename TRAITS>
struct multi_socket_functor_types<TRAITS, true>{
  typedef nb_transport_tagged_functor<TRAITS> nb_functor_type;
  typedef b_transport_tagged_functor<TRAITS>  b_functor_type;
  typedef get_direct_mem_ptr_tagged_functor<TRAITS> get_dmi_functor_type;
  typedef transport_dbg_tagged_functor<TRAITS> dbg_functor_type;
  typedef invalidate_direct_mem_ptr_tagged_functor<TRAITS> inval_dmi_functor_type;
};

//this class contains the functor types when they shall NOT use tags.
template <typename TRAITS>
struct multi_socket_functor_types<TRAITS, false>{
  typedef nb_transport_functor<TRAITS> nb_functor_type;
  typedef b_transport_functor<TRAITS>  b_functor_type;
  typedef get_direct_mem_ptr_functor<TRAITS> get_dmi_functor_type;
  typedef transport_dbg_functor<TRAITS> dbg_functor_type;
  typedef invalidate_direct_mem_ptr_functor<TRAITS> inval_dmi_functor_type;
};

//this is the type designator class template for the function signatures
// the default is empty, but since we are going to sepcialize it
// for both possible Boolean values, it will never be used
template <typename MODULE, typename TRAITS, bool TAGGED>
struct multi_socket_function_signatures{};

//this class contains the function signatures with tags.
template <typename MODULE, typename TRAITS>
struct multi_socket_function_signatures<MODULE, TRAITS, true>{
  typedef typename TRAITS::tlm_payload_type              transaction_type;
  typedef typename TRAITS::tlm_phase_type                phase_type;
  typedef tlm::tlm_sync_enum                            sync_enum_type;

  typedef sync_enum_type (MODULE::*nb_cb)(unsigned int, transaction_type&, phase_type&, sc_core::sc_time&);
  typedef void (MODULE::*b_cb)(unsigned int, transaction_type&, sc_core::sc_time&);
  typedef unsigned int (MODULE::*dbg_cb)(unsigned int, transaction_type& txn);
  typedef bool (MODULE::*get_dmi_cb)(unsigned int, transaction_type& txn, tlm::tlm_dmi& dmi);
  typedef void (MODULE::*inval_dmi_cb)(unsigned int, sc_dt::uint64, sc_dt::uint64);
};

//this class contains the function signatures without tags.
template <typename MODULE, typename TRAITS>
struct multi_socket_function_signatures<MODULE, TRAITS, false>{
  typedef typename TRAITS::tlm_payload_type              transaction_type;
  typedef typename TRAITS::tlm_phase_type                phase_type;
  typedef tlm::tlm_sync_enum                            sync_enum_type;

  typedef sync_enum_type (MODULE::*nb_cb)(transaction_type&, phase_type&, sc_core::sc_time&);
  typedef void (MODULE::*b_cb)(transaction_type&, sc_core::sc_time&);
  typedef unsigned int (MODULE::*dbg_cb)(transaction_type& txn);
  typedef bool (MODULE::*get_dmi_cb)(transaction_type& txn, tlm::tlm_dmi& dmi);
  typedef void (MODULE::*inval_dmi_cb)(sc_dt::uint64, sc_dt::uint64);
};


}
}

#endif
