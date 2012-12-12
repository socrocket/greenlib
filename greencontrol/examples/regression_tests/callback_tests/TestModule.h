//   GreenControl framework
//
// LICENSETEXT
//
//   Copyright (C) 2009 : GreenSocs Ltd
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

// GreenConfig
#include "greencontrol/config.h" 

class TestModule
: public sc_core::sc_module
{
  
  GC_HAS_CALLBACKS();
  
public:
  SC_HAS_PROCESS(TestModule);
  TestModule(sc_core::sc_module_name name) 
  : sc_core::sc_module(name) 
  {
    m_cnf_api = gs::cnf::GCnf_Api::getApiInstance(this);
    SC_THREAD(main_action);
    
    m_cnf_api->REGISTER_NEW_PARAM_CALLBACK(TestModule, new_param_callback_function);
  }
  
  ~TestModule() {
    GC_UNREGISTER_CALLBACKS();
  }
  
  void main_action() {
    GC_REGISTER_TYPED_PARAM_CALLBACK(m_cnf_api->getPar("OM1.int_param"), gs::cnf::pre_read,      TestModule, callback_function);
    GC_REGISTER_TYPED_PARAM_CALLBACK(m_cnf_api->getPar("OM1.int_param"), gs::cnf::post_read,  TestModule, callback_function);
    GC_REGISTER_TYPED_PARAM_CALLBACK(m_cnf_api->getPar("OM1.int_param"), gs::cnf::pre_write,     TestModule, callback_function);
    GC_REGISTER_TYPED_PARAM_CALLBACK(m_cnf_api->getPar("OM1.int_param"), gs::cnf::post_write,    TestModule, callback_function);
    GC_REGISTER_TYPED_PARAM_CALLBACK(m_cnf_api->getPar("OM1.int_param"), gs::cnf::destroy_param, TestModule, callback_function);
    
    GC_REGISTER_TYPED_PARAM_CALLBACK(m_cnf_api->getPar("OM2.int_param"), gs::cnf::pre_read,      TestModule, callback_function);
    GC_REGISTER_TYPED_PARAM_CALLBACK(m_cnf_api->getPar("OM2.int_param"), gs::cnf::post_read,  TestModule, callback_function);
    GC_REGISTER_TYPED_PARAM_CALLBACK(m_cnf_api->getPar("OM2.int_param"), gs::cnf::pre_write,     TestModule, callback_function);
    GC_REGISTER_TYPED_PARAM_CALLBACK(m_cnf_api->getPar("OM2.int_param"), gs::cnf::post_write,    TestModule, callback_function);
    GC_REGISTER_TYPED_PARAM_CALLBACK(m_cnf_api->getPar("OM2.int_param"), gs::cnf::destroy_param, TestModule, callback_function);

    // deprecated
    GC_REGISTER_PARAM_CALLBACK(m_cnf_api->getPar("OM2.int_param"), TestModule, simple_callback_function);
    
    m_cnf_api->getPar("OM1.int_param")->setString("501");
    m_cnf_api->getPar("OM2.int_param")->setString("502");
  }
  
  gs::cnf::callback_return_type callback_function(gs::gs_param_base& changed_param, gs::cnf::callback_type reason) {
    std::cout << " callback function got: "<< callback_type_to_string(reason) <<" callback for param "<<changed_param.getName() << std::endl; 
    std::string val = changed_param.getString();
    bool reject = false;
    gs::cnf::callback_return_type cb_return = gs::cnf::return_nothing;
    switch (reason) {
      case gs::cnf::pre_read:
        // Here you could update the parameters value to whatever the reading object should see
        break;
      case gs::cnf::reject_write:
        // Here you could reject the write if it does not match some requirements, e.g. parameter dependencies
        if (reject) cb_return = gs::cnf::return_value_change_rejected;
        break;
      case gs::cnf::pre_write:
        // Here you could log the old value
        std::cout << "   callback function: (old value: "<<changed_param.getString()<<")" << std::endl;
        break;
      case gs::cnf::post_write:
        // Here you could log the new value
        std::cout << "   callback function: (new value: "<<changed_param.getString()<<")" << std::endl;
        break;
      case gs::cnf::destroy_param:
        // Here you should remove your local reference/pointer to this parameter to prevent from using it later (seg faults!)
        break;
      default:
        // nothing else possible
        // Note: create_param is called to a different method with different signature
        break;
    }
    return cb_return;
  }

  void simple_callback_function(gs::gs_param_base& changed_param) {
    std::cout << " simple callback function got: callback for param "<<changed_param.getName() << std::endl; 
  }
  
  void new_param_callback_function(const std::string parname, const std::string val) {
    std::cout << "New parameter callback method called:" << std::endl;
    std::cout << "  Parameter '" << parname << "'"<< std::endl;
    gs::gs_param_base *par = m_cnf_api->getPar(parname);
    if (par == NULL) {
      std::cout << "  implicit" << std::endl;
    } else {
      std::cout << "  exlicit of type '" << par->getTypeString() << "'" << std::endl;
    }
  }
  
  gs::cnf::cnf_api_if *m_cnf_api;
  
};
