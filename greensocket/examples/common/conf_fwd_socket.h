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


#ifndef __CONF_FWD_SOCKET_H__
#define __CONF_FWD_SOCKET_H__

#include "greensocket/initiator/single_socket.h"
#include "greensocket/target/single_socket.h"
#include "greensocket/bidirectional/single_socket.h"

#include "greensocket/initiator/multi_socket.h"
#include "greensocket/target/multi_socket.h"
#include "greensocket/bidirectional/multi_socket.h"

//This file includes two simple sockets derived from greensocket
// these sockets allow for registering callbacks when binding was successful

template <typename MODULE>
class conf_fwd_init_socket : public gs::socket::initiator_socket<>
{
public:
  typedef gs::socket::initiator_socket<> base_type;
  typedef typename base_type::traits_type traits_type;
  typedef void (MODULE::*conf_res_cb)(const std::string&, gs::socket::bindability_base<traits_type>*);
  typedef void (MODULE::*conf_res_cb_with_own_ptr)(const std::string&, gs::socket::bindability_base<traits_type>*, gs::socket::bindability_base<traits_type>*);
  
  conf_fwd_init_socket(const char* name)
    : base_type(name)
    , m_conf_res_cb(0)
    , m_conf_res_cb_with_own_ptr(0)
    , m_mod(NULL)
    , m_type("conf_fwd_init_socket")
  {}
  
  void register_conf_res_cb(MODULE* mod, conf_res_cb cb){
    assert((m_mod==mod) || (m_mod==NULL));
    m_mod=mod; 
    m_conf_res_cb=cb;
  }

  void register_conf_res_cb(MODULE* mod, conf_res_cb_with_own_ptr cb){
    assert((m_mod==mod) || (m_mod==NULL));
    m_mod=mod; 
    m_conf_res_cb_with_own_ptr=cb;
  }

protected:
  //this function is called when we get info from someone else
  // it allows the user to adjust the socket config according to what he learned from the string and the void *
  virtual void bound_to(const std::string& other_name, gs::socket::bindability_base<traits_type>* other, unsigned int){
    if (m_mod && m_conf_res_cb) (m_mod->*m_conf_res_cb)(other_name, other);
    if (m_mod && m_conf_res_cb_with_own_ptr) (m_mod->*m_conf_res_cb_with_own_ptr)(other_name, other, (gs::socket::bindability_base<traits_type>*)this);
  }
  
  virtual const std::string& get_type_string(){return m_type;}

  conf_res_cb m_conf_res_cb;
  conf_res_cb_with_own_ptr m_conf_res_cb_with_own_ptr;
  MODULE* m_mod;
  std::string m_type;

};

template <typename MODULE>
class conf_fwd_target_socket : public gs::socket::target_socket<>
{
public:
  typedef gs::socket::target_socket<> base_type;
  typedef typename base_type::traits_type traits_type;
  typedef void (MODULE::*conf_res_cb)(const std::string&, gs::socket::bindability_base<traits_type>*);
  typedef void (MODULE::*conf_res_cb_with_own_ptr)(const std::string&, gs::socket::bindability_base<traits_type>*, gs::socket::bindability_base<traits_type>*);
  
  conf_fwd_target_socket(const char* name)
    : base_type(name)
    , m_conf_res_cb(0)
    , m_conf_res_cb_with_own_ptr(0)
    , m_mod(NULL)
    , m_type("conf_fwd_target_socket")
  {}
  
  void register_conf_res_cb(MODULE* mod, conf_res_cb cb){
    assert((m_mod==mod) || (m_mod==NULL));
    m_mod=mod; 
    m_conf_res_cb=cb;
  }
  void register_conf_res_cb(MODULE* mod, conf_res_cb_with_own_ptr cb){
    assert((m_mod==mod) || (m_mod==NULL));
    m_mod=mod; 
    m_conf_res_cb_with_own_ptr=cb;
  }

protected:
  //this function is called when we get info from someone else
  // it allows the user to adjust the socket config according to what he learned from the string and the void *
  virtual void bound_to(const std::string& other_name, gs::socket::bindability_base<traits_type>* other, unsigned int){
    if (m_mod && m_conf_res_cb) (m_mod->*m_conf_res_cb)(other_name, other);
    if (m_mod && m_conf_res_cb_with_own_ptr) (m_mod->*m_conf_res_cb_with_own_ptr)(other_name, other, (gs::socket::bindability_base<traits_type>*)this);
  }
  
  virtual const std::string& get_type_string(){return m_type;}

  conf_res_cb m_conf_res_cb;
  conf_res_cb_with_own_ptr m_conf_res_cb_with_own_ptr;
  MODULE* m_mod;
  std::string m_type;

};

template <typename MODULE>
class conf_fwd_bidirectional_socket : public gs::socket::bidirectional_socket<>
{
public:
  typedef gs::socket::bidirectional_socket<> base_type;
  typedef typename base_type::traits_type traits_type;
  typedef void (MODULE::*conf_res_cb)(const std::string&, gs::socket::bindability_base<traits_type>*);
  
  conf_fwd_bidirectional_socket(const char* name)
  : base_type(name)
  , m_conf_res_cb(0)
  , m_mod(0)
  , m_type("conf_fwd_bidirectional_socket")
  {}
  
  void register_conf_res_cb(MODULE* mod, conf_res_cb cb){
    m_mod=mod; 
    m_conf_res_cb=cb;
  }
  
protected:
  //this function is called when we get info from someone else
  // it allows the user to adjust the socket config according to what he learned from the string and the void *
  virtual void bound_to(const std::string& other_name, gs::socket::bindability_base<traits_type>* other, unsigned int){
    if (m_mod && m_conf_res_cb) (m_mod->*m_conf_res_cb)(other_name, other);
  }
  
  virtual const std::string& get_type_string(){return m_type;}
  
  conf_res_cb m_conf_res_cb;
  MODULE* m_mod;
  std::string m_type;
  
};

template <typename MODULE>
class conf_fwd_init_multi_socket 
  : public gs::socket::initiator_multi_socket<>
{
public:
  typedef gs::socket::initiator_multi_socket<> base_type;
  typedef typename base_type::traits_type traits_type;
  typedef void (MODULE::*conf_res_cb)(const std::string&, gs::socket::bindability_base<traits_type>*, unsigned int);
  
  conf_fwd_init_multi_socket(const char* name)
    : base_type(name, base_type::mm_txn_with_data())
    , m_conf_res_cb(0)
    , m_mod(0)
    , m_type("conf_fwd_init_multi_socket")
  {}
  
  void register_conf_res_cb(MODULE* mod, conf_res_cb cb){
    m_mod=mod; 
    m_conf_res_cb=cb;
  }

protected:
  //this function is called when we get info from someone else
  // it allows the user to adjust the socket config according to what he learned from the string and the void *
  virtual void bound_to(const std::string& other_name, gs::socket::bindability_base<traits_type>* other, unsigned int index){
    if (m_mod && m_conf_res_cb) (m_mod->*m_conf_res_cb)(other_name, other, index);
  }
  
  virtual const std::string& get_type_string(){return m_type;}

  conf_res_cb m_conf_res_cb;
  MODULE* m_mod;
  std::string m_type;

};

template <typename MODULE>
class conf_fwd_target_multi_socket : public gs::socket::target_multi_socket<>
{
public:
  typedef gs::socket::target_multi_socket<> base_type;
  typedef typename base_type::traits_type traits_type;
  typedef void (MODULE::*conf_res_cb)(const std::string&, gs::socket::bindability_base<traits_type>*, unsigned int);
  
  conf_fwd_target_multi_socket(const char* name)
    : base_type(name)
    , m_conf_res_cb(0)
    , m_mod(0)
    , m_type("conf_fwd_target_socket")
  {}
  
  void register_conf_res_cb(MODULE* mod, conf_res_cb cb){m_mod=mod; m_conf_res_cb=cb;}

protected:
  //this function is called when we get info from someone else
  // it allows the user to adjust the socket config according to what he learned from the string and the void *
  virtual void bound_to(const std::string& other_name, gs::socket::bindability_base<traits_type>* other, unsigned int index){
    if (m_mod && m_conf_res_cb) (m_mod->*m_conf_res_cb)(other_name, other, index);
  }
  
  virtual const std::string& get_type_string(){return m_type;}

  conf_res_cb m_conf_res_cb;
  MODULE* m_mod;
  std::string m_type;

};

template <typename MODULE>
class conf_fwd_bidirectional_multi_socket : public gs::socket::bidirectional_multi_socket<>
{
public:
  typedef gs::socket::bidirectional_multi_socket<> base_type;
  typedef typename base_type::traits_type traits_type;
  typedef void (MODULE::*conf_res_cb)(const std::string&, gs::socket::bindability_base<traits_type>*, unsigned int);
  
  conf_fwd_bidirectional_multi_socket(const char* name)
  : base_type(name)
  , m_conf_res_cb(0)
  , m_mod(0)
  , m_type("conf_fwd_bidirectional_socket")
  {}
  
  void register_conf_res_cb(MODULE* mod, conf_res_cb cb){m_mod=mod; m_conf_res_cb=cb;}
  
protected:
  //this function is called when we get info from someone else
  // it allows the user to adjust the socket config according to what he learned from the string and the void *
  virtual void bound_to(const std::string& other_name, gs::socket::bindability_base<traits_type>* other, unsigned int index){
    if (m_mod && m_conf_res_cb) (m_mod->*m_conf_res_cb)(other_name, other, index);
  }
  
  virtual const std::string& get_type_string(){return m_type;}
  
  conf_res_cb m_conf_res_cb;
  MODULE* m_mod;
  std::string m_type;
  
};

#endif
