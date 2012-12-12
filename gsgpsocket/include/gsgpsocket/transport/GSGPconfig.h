// LICENSETEXT
// 
//   Copyright (C) 2008 : GreenSocs Ltd
//       http://www.greensocs.com/ , email: info@greensocs.com
// 
//   Developed by :
// 
//   Christian Schröder, Robert Guenzel
//     Technical University of Braunschweig, Dept. E.I.S.
//     http://www.eis.cs.tu-bs.de
// 
// 
// The contents of this file are subject to the licensing terms specified
// in the file LICENSE. Please consult this file for restrictions and
// limitations that may apply.
// 
// ENDLICENSETEXT



#ifndef __GSGPCONFIG_H__
#define __GSGPCONFIG_H__

#include "greensocket/generic/green_socket_config.h"

namespace gs {
  namespace gp {
    
    template <unsigned int BUSWIDTH, typename HANDLE, typename CONFIG, typename PHASE>
    class GenericInitiatorSocket;
    
    
    // ///////////////////  Configuration Classes  ///////////////////// //
    
    // Forward declaration for proxy class
    class configure_gs_params;
    
    
    /// GenericProtocol Socket Configuration class for user and internal usage in Socket, configurable with gs_params
    /**
     * The user may set the configuration using an instance created with the public constructor.
     * The members are copied to the members of an object created with the protected constructor
     * by the friend class Socket.
     * This is used for configurating the underlying GreenSocket.
     * Alternatively to setting with assignment, this class
     * can be configured using the member gs_params.
     */
    class GSGPSocketConfig {
      
      /// Friend class implemented by Socket
      friend class configure_gs_params;
      
    public:
      
      // name of the owing socket
      std::string m_socket_name;
      
      // if true: mandatory, otherwise ignorable
      gs::gs_param<bool> use_RDEX;   // Exclusive read command
      gs::gs_param<bool> use_RDL;    // Read linked command
      gs::gs_param<bool> use_WRNP;   // Non-posted write command
      gs::gs_param<bool> use_WRC;    // Write conditional command
      gs::gs_param<bool> use_BCST;   // Broadcast command
      gs::gs_param<bool> use_error_codes;   // TODO
      gs::gs_param<bool> use_mID;       // if this module needs/uses the mID
      gs::gs_param<bool> use_trans_ID;  // if this module needs/uses the transaction ID
      // TODO future use: gs::gs_param<bool> use_burst_number;  // if this module needs/uses/understands the burst number (if false, the socket will handle)
      gs::gs_param<bool> use_wr_resp;           // if the user module needs(master)/sends(slave) a write response
      //gs::gs_param<bool> use_transact_time;     // TODO future use: if user module wants to handle tlm loosely-timed feature
      //gs::gs_param<bool> use_transact_no_time;  // TODO future use: 
      
      /// Constructor which does NOT register the gs_params at the database
      /**
       * Default user constructor
       */
      GSGPSocketConfig() 
      : m_socket_name("no_assigned_socket")
      //         name        val      array, top-level, register_at_db
      , use_RDEX("use_RDEX", "false", NULL,   false,    false)
      , use_RDL("use_RDL", "false"  , NULL,   false,    false)
      , use_WRNP("use_WRNP", "false", NULL,   false,    false)
      , use_WRC("use_WRC", "false"  , NULL,   false,    false)
      , use_BCST("use_BCST", "false", NULL,   false,    false)
      , use_error_codes("use_error_codes", "false", NULL, false, false)
      , use_mID("use_mID", "false", NULL, false, false)
      , use_trans_ID("use_trans_ID", "false", NULL, false, false)
      // future use: , use_burst_number("use_burst_number", "false", NULL, false, false)
      , use_wr_resp("use_wr_resp", "false", NULL, false, false)
      //, use_transact_time("use_transact_time", "false", NULL, false, false)
      //, use_transact_no_time("use_transact_no_time", "false", NULL, false, false)
      {
      }
      
      virtual ~GSGPSocketConfig() { }
      
    protected:
      
      /// Constructor which registers the gs_params at the database
      /**
       * May only be created by the friend class Socket
       */
      GSGPSocketConfig(const char* socket_name, const bool register_at_db) 
      : m_socket_name(socket_name)
      //         name        val      array, top-level, register_at_db
      , use_RDEX(std::string(socket_name) + ".use_RDEX", "false", NULL,   false,    register_at_db)
      , use_RDL(std::string(socket_name) + ".use_RDL", "false"  , NULL,   false,    register_at_db)
      , use_WRNP(std::string(socket_name) + ".use_WRNP", "false", NULL,   false,    register_at_db)
      , use_WRC(std::string(socket_name) + ".use_WRC", "false"  , NULL,   false,    register_at_db)
      , use_BCST(std::string(socket_name) + ".use_BCST", "false", NULL,   false,    register_at_db)
      , use_error_codes(std::string(socket_name) + ".use_error_codes", "false", NULL, false, register_at_db)
      , use_mID(std::string(socket_name) + ".use_mID", "false", NULL, false, register_at_db)
      , use_trans_ID(std::string(socket_name) + ".use_trans_ID", "false", NULL, false, register_at_db)
      // future use: , use_burst_number(std::string(socket_name) + ".use_burst_number", "false", NULL, false, register_at_db)
      , use_wr_resp(std::string(socket_name) + ".use_wr_resp", "false", NULL, false, register_at_db)
      //, use_transact_time(std::string(socket_name) + ".use_transact_time", "false", NULL, false, register_at_db)
      //, use_transact_no_time(std::string(socket_name) + ".use_transact_no_time", "false", NULL, false, register_at_db)
      {
      }
      
      /// Creates a gs::socket::config built of the config settings
      virtual gs::socket::config<tlm::tlm_base_protocol_types> convert_to_GreenSocket_conf() {
        gs::socket::config<tlm::tlm_base_protocol_types> cfg;
        // mandatory standard phases
        cfg.use_mandatory_phase(tlm::BEGIN_REQ);
        cfg.use_mandatory_phase(tlm::END_REQ);
        cfg.use_mandatory_phase(tlm::BEGIN_RESP);
        cfg.use_mandatory_phase(tlm::END_RESP);
        // always ignorable
        cfg.use_optional_extension<bytes_valid>();
        cfg.use_optional_phase(BEGIN_DATA);
        cfg.use_optional_phase(END_DATA);
        // ignorable or mandatory
        //TODO: distinguish between master or slave
        //      for the commands it is:
        //        for Masters: when used mandatory when not used optional
        //        for Slaves:  when used optional when not used rejected
        //        That allows masters that use command X, to only bind to slaves that support X
        //        and masters that do not use X, to bind to slaves that support X
        //      for error codes it will be mandatory when used and rejected when not used
        //        That will prevent binding a master and a slave when one of them is not setting the error codes
        //        because one of them will eveluate them
        //      for mID and transID it's:
        //        For masters: always optional because it is set automatically by the socket
        //        For slaves: it is mandatory when used, and optional when not used
        //        That will prevent binding a ID using slave to a non GSGPMaster socket that does not use IDs
        if (use_RDEX)  cfg.use_mandatory_extension<lock>();
        else           cfg.use_optional_extension<lock>();
        if (use_RDL || use_WRC)   cfg.use_mandatory_extension<semaphore>();
        else                      cfg.use_optional_extension<semaphore>();
        if (use_WRNP)  cfg.use_mandatory_extension<nonposted>();
        else           cfg.use_optional_extension<nonposted>();
        if (use_BCST)  cfg.use_mandatory_extension<broadcast>();
        else           cfg.use_optional_extension<broadcast>();
        if (use_error_codes)  cfg.use_mandatory_extension<error_code>();
        else                  cfg.use_optional_extension<error_code>();
        if (use_mID)   cfg.use_mandatory_extension<mID>();
        else           cfg.use_optional_extension<mID>();
        if (use_trans_ID)  cfg.use_mandatory_extension<transactionID>();
        else               cfg.use_optional_extension<transactionID>();
        // ignorable or mandatory, copied to GenericPhase
        // future use: if (use_burst_number)  cfg.use_mandatory_extension<burst_number>();
        // future use: else                   cfg.use_optional_extension<burst_number>();
        
        // future TODO: 
        if (use_wr_resp)  cfg.use_optional_extension<wr_resp_dummy>(); // if it stays optional after binding it has to be used
                                                                       // otherwise it will be rejected after binding
        else              cfg.reject_extension<wr_resp_dummy>();
        /* if (use_transact_time)  cfg.use_mandatory_extension<>();
         else                    cfg.use_optional_extension<>();
         if (use_transact_no_time)  cfg.use_mandatory_extension<>();
         else                       cfg.use_optional_extension<>();*/
        cfg.treat_unknown_as_rejected();
        return cfg;
      }
      
    };
    
    /// Proxy class allowing friend access from Socket to config
    class configure_gs_params {
    protected:
      /// Creates a GSGPSocketConfig object that has gs_params. Used by the Socket, not by the user!
      template <typename CONFIG>
      CONFIG* create_config_with_params(const char* socket_name) {
        CONFIG *conf = new CONFIG(socket_name, true);
        return conf;
      }
      /// Calls the protected convert_to_GreenSocket_conf function on the config object
      inline gs::socket::config<tlm::tlm_base_protocol_types> convert_to_GreenSocket_conf(GSGPSocketConfig &conf) {
        return conf.convert_to_GreenSocket_conf();
      }

    };

    
  } // end namespace gp
} // end namespace gs

#endif
