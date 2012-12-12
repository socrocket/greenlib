// LICENSETEXT
//
//   Copyright (C) 2005 - 2008 : GreenSocs Ltd
//       http://www.greensocs.com/ , email: info.com
//
//   Developed by :
//
//  Robert Guenzel, Christian Schroeder
//     Technical University of Braunschweig, Dept. E.I.S.
//     http://www.eis.cs.tu-bs.de
//
//   Mark Burton, Marcus Bartholomeu
//     GreenSocs Ltd
//
//
// The contents of this file are subject to the licensing terms specified
// in the file LICENSE. Please consult this file for restrictions and
// limitations that may apply.
// 
// ENDLICENSETEXT


#ifndef __GS_CALLBACKS_H__
#define __GS_CALLBACKS_H__

#include "tlm.h"

/*
This file contains some macros to define functors that use
the owner-type-independent-member-function-pointer-pattern.

The code has proven to be pretty robust and explaining it
is more difficlut than the code itself.
I propose pumping it through the pre-processor and looking at
it. It's not that complex.

Iff (note the double 'f') you really need more info
write a mail to guenzel at eis.cs.tu-bs.de
*/


#define GS_FUNC_WRAPPER(func) \
template <typename MODULE, typename TRAITS> \
class func##_wrapper{ \
public: \
  typedef typename TRAITS::tlm_payload_type payload_type; \
  typedef typename TRAITS::tlm_phase_type   phase_type; \
\
  typedef GS_FUNC_RETURN (MODULE::*cb)(GS_FUNC_ARGS); \
\
  struct func_container{ \
    cb cb_; \
  }; \
\
  struct func_container_tagged{ \
    cb cb_; \
  }; \
\
  static inline GS_FUNC_RETURN call_function(void* mod, void* fn, GS_FUNC_ARGS){ \
    MODULE* tmp_mod=static_cast<MODULE*>(mod); \
    func_container* tmp_cb =static_cast<func_container*> (fn); \
    return (tmp_mod->*(tmp_cb->cb_))(GS_FUNC_ARGS_WITHOUT_TYPES); \
  } \
\
  static inline void delete_func_container(void* fn){ \
    func_container* tmp_cb =static_cast<func_container*> (fn); \
    if (tmp_cb) delete tmp_cb; \
  }\
\
}


#define GS_FUNC_FUNCTOR(func) \
template <typename TRAITS> \
class func##_functor{ \
public: \
  typedef typename TRAITS::tlm_payload_type payload_type; \
  typedef typename TRAITS::tlm_phase_type   phase_type; \
  typedef GS_FUNC_RETURN (*call_fn)(void*,void*, GS_FUNC_ARGS); \
  typedef void (*del_fn)(void*); \
\
  func##_functor(): m_fn(0), m_del_fn(0), m_mod(0), m_mem_fn(0){} \
  ~func##_functor(){if (m_del_fn) (*m_del_fn)(m_mem_fn);}  \
\
  template <typename MODULE> \
  void set_function(MODULE* mod, GS_FUNC_RETURN (MODULE::*cb)(GS_FUNC_ARGS)){ \
    m_fn=&func##_wrapper<MODULE,TRAITS>::call_function; \
    m_del_fn=&func##_wrapper<MODULE,TRAITS>::delete_func_container; \
    m_del_fn(m_mem_fn); \
    typename func##_wrapper<MODULE,TRAITS>::func_container* tmp= new typename func##_wrapper<MODULE,TRAITS>::func_container(); \
    tmp->cb_=cb; \
    m_mod=static_cast<void*>(mod); \
    m_mem_fn=static_cast<void*>(tmp); \
  } \
\
  GS_FUNC_RETURN operator()(GS_FUNC_ARGS){ \
    return m_fn(m_mod,m_mem_fn, GS_FUNC_ARGS_WITHOUT_TYPES); \
  } \
\
  GS_FUNC_RETURN operator()(unsigned int index, GS_FUNC_ARGS){ \
    return m_fn(m_mod,m_mem_fn, GS_FUNC_ARGS_WITHOUT_TYPES); \
  } \
\
  bool empty(){return (m_mod==0 || m_mem_fn==0 || m_fn==0);}\
\
protected: \
  call_fn m_fn;\
  del_fn m_del_fn; \
  void* m_mod; \
  void* m_mem_fn; \
private: \
  func##_functor& operator=(const func##_functor&); \
}

namespace gs{
namespace socket{
#define GS_FUNC_RETURN tlm::tlm_sync_enum
#define GS_FUNC_ARGS payload_type& txn, phase_type& ph, sc_core::sc_time& time
#define GS_FUNC_ARGS_WITHOUT_TYPES txn,ph,time
GS_FUNC_WRAPPER(nb_transport);
GS_FUNC_FUNCTOR(nb_transport);
#undef GS_FUNC_RETURN
#undef GS_FUNC_ARGS
#undef GS_FUNC_ARGS_WITHOUT_TYPES

#define GS_FUNC_RETURN void
#define GS_FUNC_ARGS payload_type& txn, sc_core::sc_time& time
#define GS_FUNC_ARGS_WITHOUT_TYPES txn,time
GS_FUNC_WRAPPER(b_transport);
GS_FUNC_FUNCTOR(b_transport);
#undef GS_FUNC_RETURN
#undef GS_FUNC_ARGS
#undef GS_FUNC_ARGS_WITHOUT_TYPES

#define GS_FUNC_RETURN bool
#define GS_FUNC_ARGS payload_type& txn, tlm::tlm_dmi& dmi
#define GS_FUNC_ARGS_WITHOUT_TYPES txn,dmi
GS_FUNC_WRAPPER(get_direct_mem_ptr);
GS_FUNC_FUNCTOR(get_direct_mem_ptr);
#undef GS_FUNC_RETURN
#undef GS_FUNC_ARGS
#undef GS_FUNC_ARGS_WITHOUT_TYPES

#define GS_FUNC_RETURN void
#define GS_FUNC_ARGS sc_dt::uint64 start, sc_dt::uint64 end
#define GS_FUNC_ARGS_WITHOUT_TYPES start,end
GS_FUNC_WRAPPER(invalidate_direct_mem_ptr);
GS_FUNC_FUNCTOR(invalidate_direct_mem_ptr);
#undef GS_FUNC_RETURN
#undef GS_FUNC_ARGS
#undef GS_FUNC_ARGS_WITHOUT_TYPES

#define GS_FUNC_RETURN unsigned int
#define GS_FUNC_ARGS payload_type& txn
#define GS_FUNC_ARGS_WITHOUT_TYPES txn
GS_FUNC_WRAPPER(transport_dbg);
GS_FUNC_FUNCTOR(transport_dbg);
#undef GS_FUNC_RETURN
#undef GS_FUNC_ARGS
#undef GS_FUNC_ARGS_WITHOUT_TYPES


#define GS_FUNC_RETURN tlm::tlm_sync_enum
#define GS_FUNC_ARGS unsigned int i, payload_type& txn, phase_type& ph, sc_core::sc_time& time
#define GS_FUNC_ARGS_WITHOUT_TYPES i,txn,ph,time
GS_FUNC_WRAPPER(nb_transport_tagged);
GS_FUNC_FUNCTOR(nb_transport_tagged);
#undef GS_FUNC_RETURN
#undef GS_FUNC_ARGS
#undef GS_FUNC_ARGS_WITHOUT_TYPES

#define GS_FUNC_RETURN void
#define GS_FUNC_ARGS unsigned int i, payload_type& txn, sc_core::sc_time& time
#define GS_FUNC_ARGS_WITHOUT_TYPES i,txn,time
GS_FUNC_WRAPPER(b_transport_tagged);
GS_FUNC_FUNCTOR(b_transport_tagged);
#undef GS_FUNC_RETURN
#undef GS_FUNC_ARGS
#undef GS_FUNC_ARGS_WITHOUT_TYPES

#define GS_FUNC_RETURN bool
#define GS_FUNC_ARGS unsigned int i, payload_type& txn, tlm::tlm_dmi& dmi
#define GS_FUNC_ARGS_WITHOUT_TYPES i,txn,dmi
GS_FUNC_WRAPPER(get_direct_mem_ptr_tagged);
GS_FUNC_FUNCTOR(get_direct_mem_ptr_tagged);
#undef GS_FUNC_RETURN
#undef GS_FUNC_ARGS
#undef GS_FUNC_ARGS_WITHOUT_TYPES

#define GS_FUNC_RETURN void
#define GS_FUNC_ARGS unsigned int i, sc_dt::uint64 start, sc_dt::uint64 end
#define GS_FUNC_ARGS_WITHOUT_TYPES i,start,end
GS_FUNC_WRAPPER(invalidate_direct_mem_ptr_tagged);
GS_FUNC_FUNCTOR(invalidate_direct_mem_ptr_tagged);
#undef GS_FUNC_RETURN
#undef GS_FUNC_ARGS
#undef GS_FUNC_ARGS_WITHOUT_TYPES

#define GS_FUNC_RETURN unsigned int
#define GS_FUNC_ARGS unsigned int i, payload_type& txn
#define GS_FUNC_ARGS_WITHOUT_TYPES i,txn
GS_FUNC_WRAPPER(transport_dbg_tagged);
GS_FUNC_FUNCTOR(transport_dbg_tagged);
#undef GS_FUNC_RETURN
#undef GS_FUNC_ARGS
#undef GS_FUNC_ARGS_WITHOUT_TYPES

}
}
#endif
