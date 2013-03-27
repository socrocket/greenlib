//   GreenReg framework
//
// LICENSETEXT
//
//   Copyright (C) 2008 : GreenSocs Ltd
//      http://www.greensocs.com/ , email: info@greensocs.com
//
//   Developed by :
//   
//   Robert Guenzel <guenzel@eis.cs.tu-bs.de>,
//   Christian Schroeder <schroeder@eis.cs.tu-bs.de>,
//     Technical University of Braunschweig, Dept. E.I.S.
//     http://www.eis.cs.tu-bs.de
//
//
//   This program is free software.
// 
//   If you have no applicable agreement with GreenSocs Ltd, this software
//   is licensed to you, and you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
// 
//   If you have a applicable agreement with GreenSocs Ltd, the terms of that
//   agreement prevail.
// 
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
// 
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
//   02110-1301  USA 
// 
// ENDLICENSETEXT


#ifndef __GR_CBPEQ_H__
#define __GR_CBPEQ_H__


#include "tlm_utils/peq_with_cb_and_phase.h"


namespace gs {
    
    
template <typename MODULE, typename ARG>
class func_wrapper{
public:
  typedef void (MODULE::*cb)(const ARG&);

  struct func_container{
    cb cb_;
  };

  static inline void call_function(void* mod, void* fn, const ARG& arg){
    MODULE* tmp_mod=static_cast<MODULE*>(mod);
    func_container* tmp_cb =static_cast<func_container*> (fn);
    return (tmp_mod->*(tmp_cb->cb_))(arg);
  }

  static inline void delete_func_container(void* fn){
    func_container* tmp_cb =static_cast<func_container*> (fn);
    if (tmp_cb) delete tmp_cb;
  }
};

template<typename ARG>
struct func_delayer_traits
{
  typedef ARG tlm_payload_type;
  typedef ARG tlm_phase_type;
};

template<typename ARG>
class func_functor
: protected tlm_utils::peq_with_cb_and_phase<func_functor<ARG>, func_delayer_traits<ARG> >
{
public:
  typedef void (*call_fn)(void*,void*, const ARG&);
  typedef void (*del_fn)(void*);
  
  func_functor()
    : tlm_utils::peq_with_cb_and_phase<func_functor<ARG>, func_delayer_traits<ARG> >(this, &func_functor<ARG>::callback)
    , m_fn(0), m_del_fn(0), m_mod(0), m_mem_fn(0)
    {
    }
    
  ~func_functor(){if (m_del_fn) (*m_del_fn)(m_mem_fn);}

  template <typename MODULE>
  void set_function(MODULE* mod, void (MODULE::*cb)(const ARG &)){
    m_fn=&func_wrapper<MODULE,ARG>::call_function;
    m_del_fn=&func_wrapper<MODULE,ARG>::delete_func_container;
    m_del_fn(m_mem_fn);
    typename func_wrapper<MODULE,ARG>::func_container* tmp= new typename func_wrapper<MODULE,ARG>::func_container();
    tmp->cb_=cb;
    m_mod=static_cast<void*>(mod);
    m_mem_fn=static_cast<void*>(tmp);
  }

  void operator()(const ARG& argument, const sc_core::sc_time& t){
		assert( !func_functor::empty() );
    // TODO: remove this const_cast as soon as the TLM2.0 patch has been released which fixes the notify function signature!!
    tlm_utils::peq_with_cb_and_phase<func_functor<ARG>, func_delayer_traits<ARG> >::notify(tmp, const_cast<ARG&>(argument), t);
  }

  void operator()(const ARG& argument){
		assert( !func_functor::empty() );
    m_fn(m_mod,m_mem_fn, argument);
  }

  void callback(ARG&, const ARG& argument){
		assert( !func_functor::empty() );
    m_fn(m_mod,m_mem_fn, argument);
  }

  bool empty(){return (m_mod==0 || m_mem_fn==0 || m_fn==0);}

protected: 
  call_fn m_fn;
  del_fn m_del_fn;
  void* m_mod;
  void* m_mem_fn;
  ARG tmp;
private:
  func_functor& operator=(const func_functor&);
};

// Example usage:
/*
SC_MODULE(bar){
  SC_CTOR(bar){
    SC_THREAD(run);
    delayer.set_function(this, &bar::foo);
  }
  
  void run(){
    std::cout<<&goo<<" "<<&zoo<<std::endl;
    delayer(55, sc_core::sc_time(10,sc_core::SC_NS));
    delayer(44, sc_core::sc_time(5, sc_core::SC_NS));
    delayer(33, sc_core::SC_ZERO_TIME);
    delayer(22);
  }

  void foo(int const& value){
    std::cout<<sc_core::sc_delta_count()<<" "<<sc_core::sc_time_stamp()<<" "<<sc_core::sc_module::name()<<" "<<value<<std::endl;
  }


  func_functor<int> delayer;
  int goo, zoo;

};

int sc_main(int, char**){

  bar b("B");

  sc_core::sc_start();

  return 0;
}
 */

} // end namespace gs

#endif // __GR_CBPEQ_H__
