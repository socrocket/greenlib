// GreenSocket
//
// LICENSETEXT
// 
//   Copyright (C) 2009 : GreenSocs Ltd
//       http://www.greensocs.com/ , email: info@greensocs.com
// 
//   Developed by :
// 
//   Christian Schroeder, Robert Guenzel
//     Technical University of Braunschweig, Dept. E.I.S.
//     http://www.eis.cs.tu-bs.de
//
//
// The contents of this file are subject to the licensing terms specified
// in the file LICENSE. Please consult this file for restrictions and
// limitations that may apply.
// 
// ENDLICENSETEXT


#ifndef __bidir_unidir_wrapper_h__
#define __bidir_unidir_wrapper_h__

#include "greensocket/initiator/single_socket.h"
#include "greensocket/target/single_socket.h"
#include "greensocket/bidirectional/single_socket.h"
#include "greensocket/utils/greensocketaddress_base.h"

#define GS_ADAPTER_VERBOSE
#ifdef GS_ADAPTER_VERBOSE
# define GS_ADAPTER_DUMP(msg)         std::cout<<"@"<<sc_core::sc_time_stamp()<<" /"<<(unsigned)sc_core::sc_delta_count()<<" ("<<sc_core::sc_module::name()<<"): "<<msg<<std::endl
#else
# define GS_ADAPTER_DUMP(msg)
#endif

namespace gs {
namespace socket{


/// Adapter to connect single (target/init) sockets to bidirectional sockets
/// or to connect target AND init sockets to bidirectional ones.
template <unsigned int BUSWIDTH=32,
          typename TRAITS=tlm::tlm_base_protocol_types,
          typename BIND_BASE=bind_checker<TRAITS>,
          typename MM_IF=tlm::tlm_mm_interface,
          typename EXT_FILLER=mm_base<TRAITS, MM_IF>,
          typename POOL=default_pool<TRAITS, EXT_FILLER>
>
class bidir_unidir_wrapper 
: public sc_core::sc_module
{
protected:

  typedef bidir_unidir_wrapper<BUSWIDTH, TRAITS, BIND_BASE,
                               MM_IF, EXT_FILLER, POOL> my_type;
  
  // ////////////////////////////////////////////////////////////
  // Config forward sockets that allow for registering
  // callbacks (bound_to) when binding was successful
  // (based on greensocket/examples/common/conf_fwd_sockets.h)

  template <typename I_MODULE, unsigned int I_BUSWIDTH, typename I_TRAITS, typename I_BIND_BASE,  typename I_MM_IF,
            typename I_EXT_FILLER, typename I_POOL>
  class conf_fwd_init_socket
  : public gs::socket::initiator_socket<I_BUSWIDTH, I_TRAITS, I_BIND_BASE, I_MM_IF, I_EXT_FILLER, I_POOL>
  , public gs::socket::GreenSocketAddress_base
  {
  public:
    typedef gs::socket::initiator_socket<I_BUSWIDTH, I_TRAITS, I_BIND_BASE, I_MM_IF, I_EXT_FILLER, I_POOL> base_type;
    typedef typename base_type::traits_type traits_type;
    typedef void (I_MODULE::*conf_res_cb)(const std::string&, gs::socket::bindability_base<traits_type>*);
    typedef void (I_MODULE::*conf_res_cb_with_own_ptr)(const std::string&, gs::socket::bindability_base<traits_type>*, gs::socket::bindability_base<traits_type>*);
    
    conf_fwd_init_socket(const char* name)
    : base_type(name)
    , gs::socket::GreenSocketAddress_base(name)
    , m_conf_res_cb(0)
    , m_conf_res_cb_with_own_ptr(0)
    , m_mod(NULL)
    , m_type("conf_fwd_init_socket")
    {}
    
    void register_conf_res_cb(I_MODULE* mod, conf_res_cb cb){
      assert((m_mod==mod) || (m_mod==NULL));
      m_mod=mod; 
      m_conf_res_cb=cb;
    }
    
    void register_conf_res_cb(I_MODULE* mod, conf_res_cb_with_own_ptr cb){
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
    I_MODULE* m_mod;
    std::string m_type;
    
  };
  
  template <typename T_MODULE, unsigned int T_BUSWIDTH, typename T_TRAITS, typename T_BIND_BASE>
  class conf_fwd_target_socket
  : public gs::socket::target_socket<T_BUSWIDTH, T_TRAITS, T_BIND_BASE>
  , public gs::socket::GreenSocketAddress_base
  {
  public:
    typedef gs::socket::target_socket<T_BUSWIDTH, T_TRAITS, T_BIND_BASE> base_type;
    typedef typename base_type::traits_type traits_type;
    typedef void (T_MODULE::*conf_res_cb)(const std::string&, gs::socket::bindability_base<traits_type>*);
    typedef void (T_MODULE::*conf_res_cb_with_own_ptr)(const std::string&, gs::socket::bindability_base<traits_type>*, gs::socket::bindability_base<traits_type>*);
    
    conf_fwd_target_socket(const char* name)
    : base_type(name)
    , gs::socket::GreenSocketAddress_base(name)
    , m_conf_res_cb(0)
    , m_conf_res_cb_with_own_ptr(0)
    , m_mod(NULL)
    , m_type("conf_fwd_target_socket")
    {}
    
    void register_conf_res_cb(T_MODULE* mod, conf_res_cb cb){
      assert((m_mod==mod) || (m_mod==NULL));
      m_mod=mod; 
      m_conf_res_cb=cb;
    }
    void register_conf_res_cb(T_MODULE* mod, conf_res_cb_with_own_ptr cb){
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
    T_MODULE* m_mod;
    std::string m_type;
    
  };
  
  template <typename B_MODULE, unsigned int B_BUSWIDTH, typename B_TRAITS, typename B_BIND_BASE,
            typename B_MM_IF, typename B_EXT_FILLER, typename B_POOL>
  class conf_fwd_bidirectional_socket
  : public gs::socket::bidirectional_socket<B_BUSWIDTH, B_TRAITS, B_BIND_BASE, B_MM_IF, B_EXT_FILLER, B_POOL>
  , public gs::socket::GreenSocketAddress_base
  {
  public:
    typedef gs::socket::bidirectional_socket<B_BUSWIDTH, B_TRAITS, B_BIND_BASE, B_MM_IF, B_EXT_FILLER, B_POOL> base_type;
    typedef typename base_type::traits_type traits_type;
    typedef void (B_MODULE::*conf_res_cb)(const std::string&, gs::socket::bindability_base<traits_type>*);
    
    conf_fwd_bidirectional_socket(const char* name)
    : base_type(name)
    , gs::socket::GreenSocketAddress_base(name)
    , m_conf_res_cb(0)
    , m_mod(0)
    , m_type("conf_fwd_bidirectional_socket")
    {}
    
    void register_conf_res_cb(B_MODULE* mod, conf_res_cb cb){
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
    B_MODULE* m_mod;
    std::string m_type;
    
  };
  
public:
  
  typedef conf_fwd_bidirectional_socket<my_type, BUSWIDTH, TRAITS, BIND_BASE, MM_IF, EXT_FILLER, POOL> bidir_socket_type;
  typedef conf_fwd_target_socket<my_type, BUSWIDTH, TRAITS, BIND_BASE>                         target_socket_type;
  typedef conf_fwd_init_socket<my_type, BUSWIDTH, TRAITS, BIND_BASE, MM_IF, EXT_FILLER, POOL>           init_socket_type;
  
  // ////////////////////////////////////////////////////////////
  /// Dummy target to be connected to unused init socket
  SC_MODULE(dummy_target) {
    target_socket_type socket;
    dummy_target(sc_core::sc_module_name name_)
    : socket("dummy_target_socket") {
      // register callbacks to output warnings
      socket.register_b_transport       (this, &dummy_target::b_transport       );    
      socket.register_nb_transport_fw   (this, &dummy_target::nb_transport_fw   );
      socket.register_transport_dbg     (this, &dummy_target::transport_dbg     );
      socket.register_get_direct_mem_ptr(this, &dummy_target::get_direct_mem_ptr);
      // config
      gs::socket::config<typename target_socket_type::traits_type> cfg;
      socket.set_config(cfg);
    }
    void b_transport(typename target_socket_type::payload_type& gp, sc_core::sc_time& time) {
      SC_REPORT_WARNING(name(), "Wrong usage of wrapper: This wrapper is not connected to a target socket - so it cannot handle b_transport.");
    }
    tlm::tlm_sync_enum nb_transport_fw(typename target_socket_type::payload_type& txn,
                                       typename target_socket_type::phase_type&   ph,
                                       sc_core::sc_time&                 t) {
      SC_REPORT_WARNING(name(), "Wrong usage of wrapper: This wrapper is not connected to a target socket - so it cannot handle nb_transport_fw.");
      return tlm::TLM_COMPLETED;
    }
    unsigned int transport_dbg(typename target_socket_type::payload_type& gp) {
      SC_REPORT_WARNING(name(), "Wrong usage of wrapper: This wrapper is not connected to a target socket - so it cannot handle transport_dbg.");
      return 0;
    }
    bool get_direct_mem_ptr(typename target_socket_type::payload_type& gp, tlm::tlm_dmi& dmi) {
      SC_REPORT_WARNING(name(), "Wrong usage of wrapper: This wrapper is not connected to a target socket - so it cannot handle get_direct_mem_ptr.");
      return false;
    }
  };

  // ////////////////////////////////////////////////////////////
  /// Dummy initiator to be connected to unused init socket
  SC_MODULE(dummy_initiator) {
    init_socket_type socket;
    dummy_initiator(sc_core::sc_module_name name_)
    : socket("dummy_init_socket") {
      // register callbacks to output warnings
      socket.register_nb_transport_bw          (this, &dummy_initiator::nb_transport_bw);
      socket.register_invalidate_direct_mem_ptr(this, &dummy_initiator::invalidate_direct_mem_ptr);
      // config
      gs::socket::config<typename init_socket_type::traits_type> cfg;
      socket.set_config(cfg);
    }
    tlm::tlm_sync_enum nb_transport_bw(typename init_socket_type::payload_type& txn,
                                       typename init_socket_type::phase_type&   ph,
                                       sc_core::sc_time&                 t) {
      SC_REPORT_WARNING(name(), "Wrong usage of wrapper: This wrapper is not connected to an init socket - so it cannot handle nb_transport_bw.");
      return tlm::TLM_COMPLETED;
    }
    void invalidate_direct_mem_ptr(sc_dt::uint64 a, sc_dt::uint64 b) {
      SC_REPORT_WARNING(name(), "Wrong usage of wrapper: This wrapper is not connected to an init socket - so it cannot handle invalidate_direct_mem_ptr.");
    }
  };
  
  dummy_target *m_dt;
  dummy_initiator *m_di;
  bool m_use_init_socket;
  bool m_use_target_socket;
  
public:
  
  bidir_socket_type  bidir_socket; // bidirectional port!!
  target_socket_type target_socket;
  init_socket_type   init_socket;
  
  /// Constructor
  /**
   * If you connect only the init port and not the target port, 
   * set the constructor params ("name", true, false).<br>
   * If you connect only the target port and not the init port, 
   * set the constructor params ("name", false, true).<br>
   * If you connect both the target and the init ports, 
   * set the constructor params ("name", true, true).
   *
   * The ports being marked as unused (false) will be connected 
   * internally to a dummy which will print out sc_warnings when
   * being accessed.
   *
   * @param name  Name of the wrapper module
   * @param use_init_socket    If the user wants to connect to the wrapper's init_socket (if not, dummy will be bound to the init socket)
   * @param use_target_socket  If the user wants to connect to the wrapper's target_socket (if not, dummy will be bound to the target socket)
   */
  bidir_unidir_wrapper(sc_core::sc_module_name name, bool use_init_socket, bool use_target_socket)
  : sc_core::sc_module(name)
  , m_dt(NULL)
  , m_di(NULL)
  , m_use_init_socket(use_init_socket)
  , m_use_target_socket(use_target_socket)
  , bidir_socket("wrapper_bidir_socket")
  , target_socket("wrapper_target_socket")
  , init_socket("wrapper_init_socket")
  { 
    assert((use_init_socket || use_target_socket) && "You must enable either target or init or both sockets");
    GS_ADAPTER_DUMP("use init_socket: "<< use_init_socket << ", use target_socket: " << use_target_socket);
    
    // Setup for bidir socket (target part); connect internally to init socket
    bidir_socket.register_b_transport       (this, &bidir_unidir_wrapper::my_bidir_b_transport    );    
    bidir_socket.register_nb_transport_fw   (this, &bidir_unidir_wrapper::my_bidir_nb_transport_fw);
    bidir_socket.register_transport_dbg     (this, &bidir_unidir_wrapper::my_bidir_transport_dbg);
    bidir_socket.register_get_direct_mem_ptr(this, &bidir_unidir_wrapper::my_bidir_get_direct_mem_ptr);
    
    // Setup for bidir socket (initiator part); connect internally to target socket
    bidir_socket.register_nb_transport_bw          (this, &bidir_unidir_wrapper::my_bidir_nb_transport_bw);
    bidir_socket.register_invalidate_direct_mem_ptr(this, &bidir_unidir_wrapper::my_bidir_invalidate_direct_mem_ptr);

    // Setup for target socket; connect internally to bidir socket (init part)
    target_socket.register_b_transport       (this, &bidir_unidir_wrapper::my_target_b_transport    );    
    target_socket.register_nb_transport_fw   (this, &bidir_unidir_wrapper::my_target_nb_transport_fw);
    target_socket.register_transport_dbg     (this, &bidir_unidir_wrapper::my_target_transport_dbg);
    target_socket.register_get_direct_mem_ptr(this, &bidir_unidir_wrapper::my_target_get_direct_mem_ptr);
    
    // Setup for init socket; connect internally to bidir socket (target part)
    init_socket.register_nb_transport_bw          (this, &bidir_unidir_wrapper::my_init_nb_transport_bw);
    init_socket.register_invalidate_direct_mem_ptr(this, &bidir_unidir_wrapper::my_init_invalidate_direct_mem_ptr);
    
    // bind dummies if needed
    if (!use_init_socket) {
      m_dt = new dummy_target("dummy_target");
      init_socket(m_dt->socket);
    }
    if (!use_target_socket) {
      m_di = new dummy_initiator("dummy_initiator");
      m_di->socket(target_socket);
    }
    
    // register bound_to callbacks
    init_socket.  register_conf_res_cb(this, &my_type::init_socket_config_callback  );
    target_socket.register_conf_res_cb(this, &my_type::target_socket_config_callback);
    bidir_socket. register_conf_res_cb(this, &my_type::bidir_socket_config_callback );
    
    // prepare own sockets for configuration with connected ones
    gs::socket::config<typename init_socket_type::traits_type> cfg1;
    cfg1.treat_unknown_as_optional();
    init_socket.set_config(cfg1);
    gs::socket::config<typename target_socket_type::traits_type> cfg2;
    cfg2.treat_unknown_as_optional();
    target_socket.set_config(cfg2);
    gs::socket::config<typename bidir_socket_type::traits_type> cfg3;
    cfg3.treat_unknown_as_optional();
    bidir_socket.set_config(cfg3);
  }
  /// Callback called when init socket get configured; forwards configuration to other sockets
  void init_socket_config_callback(const std::string& other_type, gs::socket::bindability_base<TRAITS>* other) {
    GS_ADAPTER_DUMP("init_socket got configured by "<< ((other) ? other->get_name() : "<unresolvable name>") << " (which is of type "<<other_type <<")");
    if (m_use_init_socket) {
      GS_ADAPTER_DUMP("       configure bidir_socket");
      bidir_socket.set_config(init_socket.get_recent_config());
      if (m_use_target_socket) {
        GS_ADAPTER_DUMP("       configure target_socket");
        target_socket.set_config(init_socket.get_recent_config());
      } else
        GS_ADAPTER_DUMP("       don't configure target_socket");
    } else
      GS_ADAPTER_DUMP("       init_socket connected to dummy: don't configure any other socket");
  }
  /// Callback called when target socket get configured; forwards configuration to other sockets
  void target_socket_config_callback(const std::string& other_type, gs::socket::bindability_base<TRAITS>* other) {
    GS_ADAPTER_DUMP("target_socket got configured by "<< ((other) ? other->get_name() : "<unresolvable name>") << " (which is of type "<<other_type <<")");
    if (m_use_target_socket) {
      GS_ADAPTER_DUMP("       configure bidir_socket");
      bidir_socket.set_config(target_socket.get_recent_config());
      if (m_use_init_socket) {
        GS_ADAPTER_DUMP("       configure init_socket");
        init_socket.set_config(target_socket.get_recent_config());
      } else
        GS_ADAPTER_DUMP("       don't configure init_socket");
    } else
      GS_ADAPTER_DUMP("       target_socket connected to dummy: don't configure any other socket");
  }
  /// Callback called when bidir socket get configured; forwards configuration to other sockets
  void bidir_socket_config_callback(const std::string& other_type, gs::socket::bindability_base<TRAITS>* other) {
    GS_ADAPTER_DUMP("bidir_socket got configured by "<< other->get_name() << " (which is of type "<<other_type <<")");
    if (m_use_init_socket) {
      GS_ADAPTER_DUMP("       configure init_socket");
      init_socket.set_config(bidir_socket.get_recent_config());
    } else
      GS_ADAPTER_DUMP("       don't configure init_socket (connected to dummy)");
    if (m_use_target_socket) {
      GS_ADAPTER_DUMP("       configure target_socket");
      target_socket.set_config(bidir_socket.get_recent_config());
    } else
      GS_ADAPTER_DUMP("       don't configure target_socket (connected to dummy)");
  }
  
  ~bidir_unidir_wrapper() {
    // delete dummies if needed
    if (m_dt != NULL) delete m_dt; m_dt = NULL;
    if (m_di != NULL) delete m_di; m_di = NULL;
  }
  
  // ///////////////////////////////////////////////////////////////////////////////
  // bidir socket (target part); connect internally to init socket
  void my_bidir_b_transport(typename bidir_socket_type::payload_type& gp, sc_core::sc_time& time) {
    init_socket->b_transport(gp, time);
  }
  tlm::tlm_sync_enum my_bidir_nb_transport_fw(typename bidir_socket_type::payload_type& txn,
                                              typename bidir_socket_type::phase_type&   ph,
                                              sc_core::sc_time&                 t) {
    return init_socket->nb_transport_fw(txn, ph, t);
  }
  unsigned int my_bidir_transport_dbg(typename bidir_socket_type::payload_type& gp) {
    return init_socket->transport_dbg(gp);
  }
  bool my_bidir_get_direct_mem_ptr(typename bidir_socket_type::payload_type& gp, tlm::tlm_dmi& dmi) {
    return init_socket->get_direct_mem_ptr(gp, dmi);
  }

  // ///////////////////////////////////////////////////////////////////////////////
  // bidir socket (initiator part); connect internally to target socket
  tlm::tlm_sync_enum my_bidir_nb_transport_bw(typename bidir_socket_type::payload_type& txn,
                                              typename bidir_socket_type::phase_type&   ph,
                                              sc_core::sc_time&                 t) {
    return target_socket->nb_transport_bw(txn, ph, t);
  }
  void my_bidir_invalidate_direct_mem_ptr(sc_dt::uint64 a, sc_dt::uint64 b) {
    target_socket->invalidate_direct_mem_ptr(a, b);
  }
  
  // ///////////////////////////////////////////////////////////////////////////////
  // target socket; connect internally to bidir socket (init part)
  void my_target_b_transport(typename target_socket_type::payload_type& gp, sc_core::sc_time& time) {
    bidir_socket->b_transport(gp, time);
  }
  tlm::tlm_sync_enum my_target_nb_transport_fw(typename target_socket_type::payload_type& txn,
                                               typename target_socket_type::phase_type&   ph,
                                               sc_core::sc_time&                 t) {
    return bidir_socket->nb_transport_fw(txn, ph, t);
  }
  unsigned int my_target_transport_dbg(typename target_socket_type::payload_type& gp) {
    return bidir_socket.fw->transport_dbg(gp);
  }
  bool my_target_get_direct_mem_ptr(typename target_socket_type::payload_type& gp, tlm::tlm_dmi& dmi) {
    return bidir_socket.fw->get_direct_mem_ptr(gp, dmi);
  }
  
  // ///////////////////////////////////////////////////////////////////////////////
  // init socket; connect internally to bidir socket (target part)
  tlm::tlm_sync_enum my_init_nb_transport_bw(typename init_socket_type::payload_type& txn,
                                             typename init_socket_type::phase_type&   ph,
                                             sc_core::sc_time&                 t) {
    return bidir_socket->nb_transport_bw(txn, ph, t);
  }
  void my_init_invalidate_direct_mem_ptr(sc_dt::uint64 a, sc_dt::uint64 b) {
    bidir_socket.bw->invalidate_direct_mem_ptr(a, b);
  }
                                 
};

  
} //end ns socket
} //end ns gs

#endif
