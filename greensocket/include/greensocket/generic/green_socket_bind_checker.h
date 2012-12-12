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

#ifndef __green_socket_bind_checker_h__
#define __green_socket_bind_checker_h__

#include "tlm.h"
#include "greensocket/generic/green_socket_config.h"
#include "greensocket/generic/green_socket_osci_config.h"
#include "greensocket/generic/green_socket_extension_support_base.h"
#include "greensocket/generic/green_callback_binders.h"
#include "greensocket/utils/gs_msg_output.h"
#include <vector>
#include <map>
#include <stdarg.h>

namespace gs{
namespace socket{

//This is the base class for the GreenSocket.
// It is used to determine whether a socket is greensocket (using a dynamic cast)
// and to exchange the configuration information
// see gsock_tech.pdf for details
template <typename TRAITS>
class bindability_base{
public:
  virtual gs::socket::config<TRAITS>&       get_config(unsigned int)=0;
  virtual bool get_t_piece_end(bindability_base*&, unsigned int&)=0;
  virtual const char* get_name()const =0;
  virtual sc_core::sc_object* get_parent() =0;
  virtual ~bindability_base(){}
};

//this is the interface from the user side into the bind checker
template <typename TRAITS>
class bind_checker_base
{
public:
  //set a configuration for all the bindings of a socket
  virtual void set_config(const gs::socket::config<TRAITS>&)=0;
  
  //set a configuration for a specific binding of a multi socket
  //  NOTE: works only after the bindings have been resolved
  virtual void set_config(const gs::socket::config<TRAITS>&, unsigned int)=0;
  
  //return the resolved config for a certain binding
  virtual gs::socket::config<TRAITS>& get_recent_config(unsigned int index=0)=0;
  
  virtual ~bind_checker_base(){}
};

//This class handles the extension access
// it is used by both the target and the initiator socket
template <typename TRAITS>
class bind_checker
  : public bindability_base<TRAITS>
  , public bind_checker_base<TRAITS>
  , public extension_support_base<TRAITS> //make the static functions of the extension support API available as "member" functions
{
public:


  //the types used within the bind checking facility
  typedef TRAITS traits_type;
  typedef typename traits_type::tlm_payload_type     payload_type;
  typedef typename traits_type::tlm_phase_type       phase_type;
  typedef bindability_base<traits_type>       bind_base_type;
  typedef config<traits_type>                 config_type;
  typedef bind_checker_base<traits_type>      bind_checker_base_type;
  typedef extension_support_base<traits_type> ext_support_type;
  typedef gs_callback_binder_base             root_type;
  typedef gs_multi_to_multi_bind_base<traits_type> multi_root_type;

  //template constructor. see gsock_tech.pdf
  template<typename SOCKET_CALLBACK_BASE>
  bind_checker(const char*, SOCKET_CALLBACK_BASE*, unsigned int);

  template<typename SOCKET_CALLBACK_BASE>
  bind_checker(const char*, SOCKET_CALLBACK_BASE*);
  
  virtual ~bind_checker();
  
  //convert the configuration of the class to a string
  std::string to_string();

  //this function applies a configuration to a socket derived from this class
  void set_config(const gs::socket::config<traits_type>&);
  
  void set_config(const gs::socket::config<traits_type>&, unsigned int);
  
  //this function returns the current configuration of a socket derived from this class
  gs::socket::config<traits_type>& get_recent_config(unsigned int index=0);
  
  //this function returns the name of a socket derived from this class
  virtual const char* get_name() const;

  //this function returns the parent of the socket that is derived from this class
  // that means the sc_module owning the socket
  virtual sc_core::sc_object* get_parent();

  //This function is called after bindability check
  // it provides the type (as a string) of and a pointer to the connected socket
  // sockets may override this function to react to the result of the binding
  virtual void bound_to(const std::string&, bindability_base<traits_type>*, unsigned int)=0;
  
  //This function must return a reference to a string that identifies the type of the socket
  // sockets shall override this function to return a string that represents their own type
  virtual const std::string& get_type_string()=0;
  
  //this function checks the binding with the index provided ad an argument
  void check_binding(unsigned int);
  
  //this function gets the other side of the binding
  // in case of an init socket it will return a pointer to the target socket
  // in case of a target socket it will return a pointer to the init socket
  //if it returns NULL the other side is not a greensocket
  //the first argument specifies the binding index of which to get the other side
  //after the call the second argument will contain the index of the binding at the other side
  bindability_base<traits_type>* get_other_side(unsigned int,unsigned int&);
  
  
  unsigned int get_bus_width();
  
protected:

  //internal helper to get the size of the socket
  unsigned int get_num_bindings();

  //used as part of the owner-type-independent-member-function-pointer-pattern
  // see gsock_tech.pdf for details
  template<typename SOCKET_CALLBACK_BASE>
  static unsigned int get_num_bindings_int(void*);

  //used as part of the owner-type-independent-member-function-pointer-pattern
  // see gsock_tech.pdf for details
  template<typename SOCKET_CALLBACK_BASE>
  static sc_core::sc_interface* get_interface(void*, unsigned int);
  
  //the function demanded by the bindability_base
  virtual gs::socket::config<traits_type>& get_config(unsigned int); 
  
  //the function demanded by the bindability_base
  virtual bool get_t_piece_end(bindability_base<traits_type>*&, unsigned int&);
  
  //a helper to resize the number of configurations to the correct number of 
  // bindings
  void resize_configs();
    
  std::vector<config<traits_type> > m_configs; //the vector of configurations (one per binding)
  const char* m_name; //a pointer to the name of the socket
  
  std::vector<bool> cb_done; //a vector to remember if a bound_to callback was already done for a given binding
  
  enum {gs_bind_check_not_allowed, gs_bind_check_allowed, gs_bind_check_running} bind_check_state; //a state variable for the bind check

  //used as part of the owner-type-independent-member-function-pointer-pattern
  // see gsock_tech.pdf for details  
  unsigned int (*get_num_bindings_ptr)(void*);
  sc_core::sc_interface* (*get_interface_ptr)(void*, unsigned int);
  
  void* m_socket; //a pointer to the socket derived from this class
  
  unsigned int m_bus_width;
};


} //end ns socket
} //end ns gs


#include "greensocket/generic/green_socket_bind_checker.tpp"
#endif
