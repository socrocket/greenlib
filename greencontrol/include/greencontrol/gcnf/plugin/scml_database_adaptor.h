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

// doygen comments

#ifndef __SCML_DATABASE_APAPTOR_H__
#define __SCML_DATABASE_APAPTOR_H__

#include <string>
#include <iostream>
#include <exception>

#include "greencontrol/gcnf/plugin/config_globals.h"
#include "greencontrol/gcnf/plugin/utils.h"
#include "greencontrol/gcnf/plugin/param_db_if.h"
#include "greencontrol/gcnf/plugin/configdatabase.h"

#include <scml.h>


namespace gs {
namespace cnf {


/// Database for the Config Plugin, which connects to the CoWare scml registry.
/**
 * Parameter database which extends the default GreenConfig database with
 * scml parameters being read from the CoWare scml registry to set initial
 * values.
 *
 * This implements param_db_if to be used in the ConfigPlugin.
 */
template<typename gs_param_base_T>
class Scml_database_adaptor_T
: public ConfigDatabase
{

public:

  /// Constructor SCML database adaptor
  /**
   * @param name  Name of this object.
   * @param overwrite_initial_values_with_scml_value If an scml database value shall overwrite another GreenConfig initial value (default=true)
   */
  Scml_database_adaptor_T(const char* name, bool overwrite_initial_values_with_scml_value = true) 
  : ConfigDatabase(name)
  , scml_reg(scml_property_registry::inst()) // Get the scml registry instance
  , m_overwrite_initial_values_with_scml_value(overwrite_initial_values_with_scml_value)
  {  }
  
private:
  /// Constructor without parameter must not be used!
  Scml_database_adaptor_T() { sc_assert(false && "Constructor not allowed"); }
  
public:

  /// @see gs::cnf::param_db_if::addParam
  bool addParam(gs_param_base_T* par) {
    assert(par != NULL);
    GCNF_DUMP_N(name(), "addParam("<<par->getName()<<")");
    // get initial values from scml database if existing
    processSCMLinitValue(par->getName());
    return ConfigDatabase::addParam(par);
  }
  
 /*
  
  /// @see gs::cnf::param_db_if::removeParam
  bool removeParam(gs_param_base_T* param) {
    GCNF_DUMP_N(name(), "removeParam("<<gs_param_base_T.getName()<<")");      
    return ConfigDatabase::removeParam(param);
  }
  
  /// @see gs::cnf::param_db_if::setInitValue
  bool setInitValue(const std::string &hier_parname, const std::string &value) {
    GCNF_DUMP_N(name(), "setInitValue("<<hier_parname<<", "<<value<<")");
    return ConfigDatabase::setInitValue(hier_parname, value);
  }
  */
  /// @see gs::cnf::param_db_if::getValue
  std::string getValue(const std::string &hier_parname) {
    GCNF_DUMP_N(name(), "getValue("<<hier_parname<<")");
    processSCMLinitValue(hier_parname);
    return ConfigDatabase::getValue(hier_parname);
  }
/*
  /// @see gs::cnf::param_db_if::getParam
  gs_param_base_T* getParam(const std::string &hier_parname) {
    GCNF_DUMP_N(name(), "getParam("<<hier_parname.c_str()<<")");
    return ConfigDatabase::getParam(hier_parname);
  }
*/  
  /// @see gs::cnf::param_db_if::existsParam
  bool existsParam(const std::string &hier_parname) {
    GCNF_DUMP_N(name(), "existsParam("<<hier_parname.c_str()<<")");
    processSCMLinitValue(hier_parname);
    return ConfigDatabase::existsParam(hier_parname);
  }
/*
  /// @see gs::cnf::param_db_if::is_explicit
  bool is_explicit(const std::string &hier_parname) {
    GCNF_DUMP_N(name(), "is_explicit("<<hier_parname.c_str()<<")");
    return ConfigDatabase::is_explicit(hier_parname);
  }
*/
  /// @see gs::cnf::param_db_if::getParametersVector
  const std::vector<std::string> getParametersVector() {
    GCNF_DUMP_N(name(), "getParameters()");
    SC_REPORT_INFO(name(), "The scml database adaptor getParametersVector() does not provide implicit parameters from the scml database because it is not supported there!");
    return ConfigDatabase::getParametersVector();
  }
/*
  /// @see gs::cnf::param_db_if::getParameters
  const std::string getParameters() {
    GCNF_DUMP_N(name(), "getParameters()");      
    return ConfigDatabase::getParameters();
  }*/

  const char* name() const { return ConfigDatabase::m_name.c_str(); }
  
protected:

  /// Searches in the scml database for an initial value for the given parameter and applies it (if needed)
  /**
   * This automatically checks if an init value shall be written to the parameter.
   * The value is NOT applied
   * 1/ if the constructor parameter overwrite_initial_values_with_scml_value is false or
   * 2/ of the parameter is already existing explicitely or
   * 3/ there is already an initial value in the GreenConfig database (which might be because 
   *    this function has already been called for this parameter and applied an init value, 
   *    or another init value has been set using the GreenConfig mechanisms, which has precedence)
   */
  const bool processSCMLinitValue(const std::string &nam) {
    GCNF_DUMP_N(name(), "processSCMLinitValue("<<nam.c_str()<<")");
    #ifdef DEBUG
      if( !isHierarchicalParameterName(nam) ) {
        SC_REPORT_WARNING(name(), "getValue: Tried to get a parameter which is not hierarchical given.");
      }
    #endif
    
    std::string value;

    const std::string moduleName = nam.substr(0, nam.find_last_of(SC_NAME_DELIMITER));
    const std::string lname = nam.substr(nam.find_last_of(SC_NAME_DELIMITER)+1, nam.length()-nam.find_last_of(SC_NAME_DELIMITER)-1);

    if (!check_if_overwrite(nam)) return false;
    if (ConfigDatabase::existsParam(nam)) return false; // Do not overwrite an already existing implicit value
    // get parameter out of registry (Property arbitrary)
    // Try all types
    // Try int
    if (scml_reg.hasProperty(scml_property_registry::MODULE, scml_property_registry::INTEGER, moduleName, lname)) {
      int tmp = scml_reg.getIntProperty(scml_property_registry::MODULE, moduleName, lname);
      std::stringstream s; s << tmp; value = s.str();
      GCNF_DUMP_N(name(), "processSCMLinitValue: got value '"<<value<<"' from scml database for param '"<<nam<<"'");
      if (setInitValue(nam, value)) {
        std::stringstream ss;
        ss << "Parameter '"<<nam<<"' init value from scml registry overwrites existing init value!";
        GCNF_DUMP_N(name(), ss.str());
        SC_REPORT_INFO(name(), ss.str().c_str());
      }
      return true;
    }
    // Try bool
    else if (scml_reg.hasProperty(scml_property_registry::MODULE, scml_property_registry::BOOL, moduleName, lname)) {
      bool tmp = scml_reg.getBoolProperty(scml_property_registry::MODULE, moduleName, lname);
      std::stringstream s; s << tmp; value = s.str();
      GCNF_DUMP_N(name(), "processSCMLinitValue: got value '"<<value<<"' from scml database for param '"<<nam<<"'");
      if (setInitValue(nam, value)) {
        std::stringstream ss;
        ss << "Parameter '"<<nam<<"' init value from scml registry overwrites existing init value!";
        GCNF_DUMP_N(name(), ss.str());
        SC_REPORT_INFO(name(), ss.str().c_str());
      }
      return true;
    }
    // Try double
    else if (scml_reg.hasProperty(scml_property_registry::MODULE, scml_property_registry::DOUBLE, moduleName, lname)) {
      double tmp = scml_reg.getDoubleProperty(scml_property_registry::MODULE, moduleName, lname);
      std::stringstream s; s << tmp; value = s.str();
      GCNF_DUMP_N(name(), "processSCMLinitValue: got value '"<<value<<"' from scml database for param '"<<nam<<"'");
      if (setInitValue(nam, value)) {
        std::stringstream ss;
        ss << "Parameter '"<<nam<<"' init value from scml registry overwrites existing init value!";
        GCNF_DUMP_N(name(), ss.str());
        SC_REPORT_INFO(name(), ss.str().c_str());
      }
      return true;
    }
    // Try string
    else if (scml_reg.hasProperty(scml_property_registry::MODULE, scml_property_registry::STRING, moduleName, lname)) {
      value = scml_reg.getStringProperty(scml_property_registry::MODULE, moduleName, lname);
      GCNF_DUMP_N(name(), "processSCMLinitValue: got value '"<<value<<"' from scml database for param '"<<nam<<"'");
      if (setInitValue(nam, value)) {
        std::stringstream ss;
        ss << "Parameter '"<<nam<<"' init value from scml registry overwrites existing init value!";
        GCNF_DUMP_N(name(), ss.str());
        SC_REPORT_INFO(name(), ss.str().c_str());
      }
      return true;
    }
    else {
      GCNF_DUMP_N(name(), "processSCMLinitValue: Param '"<<nam<<"' does not exist in scml database.");
      return false;
    }
  }
  
  /// Returns if the initial value from the scml database shall be written as the initial value to the parameter
  /**
   * This decision is based on the constructor option overwrite_initial_values_with_scml_value
   * and is never applied when the param is already existing explicitly.
   *
   * @param nam Parameter name
   */
  inline bool check_if_overwrite(const std::string& nam) {
    if (!m_overwrite_initial_values_with_scml_value) {
      if (existsParam(nam)) {
        std::stringstream ss;
        ss << "Parameter '"<<nam<<"' has already an init value, which will NOT be overwritten by an scml database value!";
        GCNF_DUMP_N(name(), ss.str());
        SC_REPORT_INFO(name(), ss.str().c_str());
        return false; // Do not overwrite it
      }
    }
    return true; // Do overwrite it
  }
  
  /// scml registry
  scml_property_registry &scml_reg;
  
  /// If a previously set initial value shall be overwritten by the scml registry initial value
  bool m_overwrite_initial_values_with_scml_value;
};

class gs_param_base;
template<class T> class gs_param;
/// Typedef for Scml_database_adaptor_T with the only allowed template specialization
typedef Scml_database_adaptor_T<gs_param_base> Scml_database_adaptor;

} // end namespace cnf
} // end namespace gs

#endif
