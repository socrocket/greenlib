//   GreenControl framework
//
// LICENSETEXT
//
//   Copyright (C) 2010 : GreenSocs Ltd
// 	 http://www.greensocs.com/ , email: info@greensocs.com
//
//   Developed by :
//   
//   Christian Schroeder <schroeder@eis.cs.tu-bs.de>,
//     Technical University of Braunschweig, Dept. E.I.S.
//     http://www.eis.cs.tu-bs.de
//
//
// The contents of this file are subject to the licensing terms specified
// in the file LICENSE. Please consult this file for restrictions and
// limitations that may apply.
// 
// ENDLICENSETEXT


#ifndef __test_plugin_h__
#define __test_plugin_h__

// GreenControl Core
#include "greencontrol/core.h"

// Service related stuff
#include "test_globals.h"
#include "test_command_if.h"


namespace gs {
namespace test {

/// Test Service Plugin.
/**
 * This is the GreenControl Test_Plugin.
 * The plugin provides the service TEST_SERVICE (service extension class test_service_ID).
 *
 * The Plugin derives from test_command_if to get the command_if methods implemented
 * in the same way for the Plugin and the User API.
 *
 * The Plugin derives from test_service_ID to be able to specify the Service ID when
 * constructing the gc_port. Alternatively this could have a member of type test_service_ID;
 * it does not matter how much object of test_service_ID exist in the system, they will all
 * map to the same Service ID.
 */
class Test_Plugin
  : public gs::ctr::gc_port_if
  , public test_command_if
  , public test_service_ID
{

public:

  /// Constructor
  /**
   * The service name is fixed.
   */
  Test_Plugin()
  : m_gc_port(*this/*test_service_ID*/, test_service_ID::get_Service_ID_string(), true, this)
  { 
    std::cout << "This is Service Plugin '" << test_service_ID::get_Service_ID_string() << "' (ID=" << test_service_ID::get_Service_ID() << ")" << std::endl;
  }

  /// Destructor
  ~Test_Plugin() {
  }
  
  /// Called by gc_port through gc_port_if when transaction arrives (gc_port_if).
  /**
   * Implements gc_port_if. 
   * This method starts whenever an API sent a transaction to this Service Plugin
   */
  void transport(gs::ctr::ControlTransactionHandle &tr)
  {
    static unsigned int return_value = 100; // some value to return
    TEST_DUMP_N(name(), " Got transaction, command="<<getCommandName(tr->get_mCmd()));
    // show received Transaction
    //GAV_DUMP_N(name(), "  received transaction: "<<tr->toString().c_str());      

    // According to the command do stuff, e.g. fill in transaction atoms to be returned
    switch (tr->get_mCmd()) {
        
        // ////////////   Command CMD_TEST_1   /////////////////////////////// //
      case CMD_TEST_1:
      {
        TEST_DUMP_N(name(), "received transaction");
        test_extension* ext;
        tr->get_extension(ext);
        assert(ext != NULL && "Sent by wrong API?");
        TEST_DUMP_N(name(), "Plugin got data: " << (*ext).getValueLog());

        test_return_extension* ret_ext;
        tr->get_extension(ret_ext);
        assert(ret_ext != NULL && "Sent by wrong API?");
        ret_ext->m_extension_data = ++return_value;
        
        break;
      }

        // ////////////   Command CMD_TEST_2   /////////////////////////////// //
      case CMD_TEST_2:
      {
        TEST_DUMP_N(name(), "TODO");
        break;
      }
        
      // ////////////   No Command (error)   ////////////////////////////////////// //
    default:
      {
        SC_REPORT_WARNING(name(), "transport: unknown command!");
        tr->set_mError(1);
      }
    }
  }

  /// Returns the name of the plugin (implements command_if and test_command_if).
  const std::string getName() { return test_service_ID::get_Service_ID_string(); }

  /// Returns the name of the plugin
  const char* name() const { return test_service_ID::get_Service_ID_string().c_str(); }

protected:
    
  /// Port to connect to the Core
  gs::ctr::gc_port m_gc_port;
  
};
  
} // end namespace test
} // end namespace gs

#endif
