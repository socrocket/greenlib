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

#ifndef __test_globals_h__
#define __test_globals_h__

#include <iostream>

#define TEST_VERBOSE

#ifdef TEST_VERBOSE
 #include <stdio.h>
 #define TEST_DUMP_N(name, msg)     { std::cout<<"@"<<sc_core::sc_time_stamp().to_string()<<" /"<<(unsigned)sc_core::sc_delta_count()<<" ("<<name<<"): " <<msg<<std::endl; } 
 #define TEST_PRINT_SPACE           { std::cout << "." << std::endl; }
 #define IF_TEST_VERBOSE(arg) arg
#else
 #define TEST_DUMP_N(name, msg)
 #define TEST_PRINT_SPACE
 #define IF_TEST_VERBOSE(arg)
#endif

namespace gs {
namespace test {

  /// Commands for the Test Service
  enum TestCommand {
    CMD_TEST_NONE=0,
    CMD_TEST_1=1,
    CMD_TEST_2=2
  };
  
  /// Service Extension: the Test Service ID
  /**
   * Shall derive from gs::ctr::gc_service_ID_extension<my_service_ID> to be used
   * for identification of this service
   */
  struct test_service_ID
  : public gs::ctr::gc_service_ID_extension<test_service_ID> 
  { 
    /// Constructor defining the Service ID string for debugging and logging
    test_service_ID() : gs::ctr::gc_service_ID_extension<test_service_ID>("TEST_SERVICE") { }
  };

  /// Transaction Extension for Test_Service
  class test_extension
  : public gs::ctr::gc_transaction_extension<test_extension>
  {
  public:
    test_extension() : m_type("test_extension"), m_name("test_extension_name") { }

    /// Extension Data
    std::vector<std::string> m_extension_data;
    
    /// log_if implementation
    virtual std::string toString() {
      std::stringstream ss_ret;
      ss_ret << getNameLog() << " (" << getTypeLog() << ") = " << getValueLog();
      return ss_ret.str();
    }
    /// log_if implementation
    virtual std::string getValueLog() {
      std::stringstream ss_ret;
      std::vector<std::string>::iterator it;
      for ( it=m_extension_data.begin() ; it < m_extension_data.end(); it++ )
        ss_ret << *it << " ";
      return ss_ret.str();
    };
    /// log_if implementation
    virtual std::string getTypeLog() { return m_type; }
    /// log_if implementation
    virtual std::string getNameLog() { return m_name; }
    
  protected:
    const std::string m_type; // for log_if
    const std::string m_name; // for log_if
  };
  
  /// Transaction Extension for Test_Service
  class test_return_extension
  : public gs::ctr::gc_transaction_extension<test_return_extension>
  {
  public:
    test_return_extension() { }
    
    /// log_if implementation
    virtual std::string toString()    { return "not implemented"; }
    /// log_if implementation
    virtual std::string getValueLog() { return "not implemented"; }
    /// log_if implementation
    virtual std::string getTypeLog()  { return "not implemented"; }
    /// log_if implementation
    virtual std::string getNameLog()  { return "not implemented"; }

    /// Extension Data
    unsigned int m_extension_data;
  };    
  


} // end namespace
} // end namespace

#endif
