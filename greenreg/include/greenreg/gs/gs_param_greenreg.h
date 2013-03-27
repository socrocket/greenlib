//   GreenReg framework
//
// LICENSETEXT
//
//   Copyright (C) 2008-2009 : GreenSocs Ltd
//      http://www.greensocs.com/ , email: info@greensocs.com
//
//   Developed by :
//   
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

#include "greencontrol/config.h"
#include "greenreg/sysc_ext/utils/gr_report.h"

// /////////////////////////////////////////////////////////////////////////////// //
// /////////////////// gs_param_greenreg< T > ///////////////////////////////////////// //
// /////////////////////////////////////////////////////////////////////////////// //


//
// How-to apply this adapter class to a GreenReg class:
//
// 1/ Derive from this class: my_class : gs::gs_param_greenreg<unsigned int>(param_name)
//    The class needs to implement the add_rule(...) function to register
//    notification rules.
// 2/ Call init_param() from the lowest constructor (when the object is full completed)
// 3/ Implement the two virtual functions set_greenreg_value() and get_greenreg_value()
//    and use them to give access to the data.
//

namespace gs {
  
/// Template specialization for gs_param_greenreg< T > adapter.
/**
 * This class works as an adapter to the greenreg classes representing any
 * named object of bool or unsigned int, like register, bit, bit_range
 *
 * The class using this adapter needs to derive from this class
 * and implement the pure virtual functions to specialize the 
 * behavior concerning the greenreg object type.
 *
 * Enables rules when first callback is registered,
 * disables again when last callback unregistered.
 */
template<typename T>
class gs_param_greenreg
: public gs_param_base
{
protected:
  /// Typedef for the value.
  typedef T val_type;
  /// Typedef for this specialized class.
  typedef gs_param_greenreg<val_type> my_type;
  
  /// String whose reference can be returned as string value
  mutable std::string return_string;
  
  using gs_param_base::m_api;
  using gs_param_base::m_par_name;
  
public:
  // Explicit constructors to avoid implicit construction of parameters.
  
  /// Constructor with (local/hierarchical) name
  explicit gs_param_greenreg(const char *nam,                                      const bool force_top_level_name = false) : gs_param_base(std::string(nam), true, NULL,         force_top_level_name ), m_has_been_initialized(false), m_notification_rules_active(false) { add_param_attribute(gs::cnf::param_attributes::state); } 
  explicit gs_param_greenreg(const std::string &nam,                               const bool force_top_level_name = false) : gs_param_base(nam             , true, NULL,         force_top_level_name ), m_has_been_initialized(false), m_notification_rules_active(false) { add_param_attribute(gs::cnf::param_attributes::state); } 
  
  // Constructors with parent array
  explicit gs_param_greenreg(const std::string &nam, gs_param_array* parent_array, const bool force_top_level_name = false) : gs_param_base(nam             , true, parent_array, force_top_level_name ), m_has_been_initialized(false), m_notification_rules_active(false) { add_param_attribute(gs::cnf::param_attributes::state); } 
  explicit gs_param_greenreg(const char *nam,        gs_param_array* parent_array, const bool force_top_level_name = false) : gs_param_base(std::string(nam), true, parent_array, force_top_level_name ), m_has_been_initialized(false), m_notification_rules_active(false) { add_param_attribute(gs::cnf::param_attributes::state); } 
  
  using gs_param_base::name;
  
  /// Init method without value
  /**
   * Register for callbacks (notifications).
   * make_callbacks() is called by the greenreg callback handling function.
   */
  void init_param() {
    GS_PARAM_DUMP("Init gs_param_greenreg "<< m_par_name.c_str());
    
    // add to plugin database
    if (m_register_at_db) {
      m_api->addPar(this);
    }
    
    // *** Prepare notifications for callbacks
    //std::cout << m_par_name << " Add notification rules to greenreg object for param callbacks." << std::endl; // TODO CS: remove
    m_notification_rule_vec = add_post_read_param_rules();
    // callback to be called
    gs::reg_utils::void_cpp_callback_0<gs_param_greenreg<T> > *cb = new gs::reg_utils::void_cpp_callback_0<gs_param_greenreg<T> >(); // TODO: Memory leak
    cb->bind(this, &gs_param_greenreg<T>::post_write_callback_func);
    // add callback and disable event for notification rule's event
    std::vector<gs::reg::gr_notification_rule_container*> par_rules = get_param_rules();
    for (std::vector<std::string>::iterator it = m_notification_rule_vec.begin(); it != m_notification_rule_vec.end(); it++) {
      for (std::vector<gs::reg::gr_notification_rule_container*>::iterator cit = par_rules.begin(); cit != par_rules.end(); cit++) {
        //std::cout << " activate rule "<<*it << std::endl; // TODO CS: remove
        if ((*cit)->has_rule(*it)) {
          (*cit)->get_rule_event(*it).add_callback(*cb);
          //(*cit)->get_rule_event(*it).disable_timing(); // not needed, will automatically never notify event
          break; // break the inner for if this rule was found
        }
      }
    }
    // prosumably disable rules
    activate_my_notification_rules(has_callbacks());
    
    m_has_been_initialized = true;
  }
  
  /// Call parameter make_callbacks() when getting a post change greenreg callback
  void post_write_callback_func() {
    GS_PARAM_DUMP("gs_param_greenreg "<<m_par_name<<" GOT greenreg notification CALLBACK!")
    make_post_write_callbacks();
  }

  /// Destructor
  virtual ~gs_param_greenreg() {
    assert(m_has_been_initialized && "You need to add the call to init_param() to the contructor of the lowest class deriving from this adapter!");
    gs_param_base::destruct_gs_param();
  }
  
  
  // ///////////////////////
  //  overloaded callback/notification handling

  /// Calls base function and (de)activates the notification rules according the existent callbacks
  virtual boost::shared_ptr<gs::cnf::ParamCallbAdapt_b> registerParamCallback(boost::shared_ptr<gs::cnf::ParamCallbAdapt_b> callb, gs::cnf::callback_type type = gs::cnf::post_write_and_destroy/*DEPRECATED, TODO: remove this default!*/) {
    boost::shared_ptr<gs::cnf::ParamCallbAdapt_b> ret = gs_param_base::registerParamCallback(callb, type);
    activate_my_notification_rules(has_callbacks());
    return ret;
  }
  
  /// Calls base function and (de)activates the notification rules according the existent callbacks
  virtual bool unregisterParamCallback(gs::cnf::ParamCallbAdapt_b *callb) {
    bool ret = gs_param_base::unregisterParamCallback(callb);
    activate_my_notification_rules(has_callbacks());
    return ret;
  }
  
  /// Calls base function and (de)activates the notification rules according the existent callbacks
  virtual bool unregisterParamCallbacks(void* observer) {
    bool ret = gs_param_base::unregisterParamCallbacks(observer);
    activate_my_notification_rules(has_callbacks());
    return ret;
  }


  // ///////////////////////
  //  additional functions
  
  
  // ///////////////////////
  //  overloaded functions
  
  /// Overloads gs_param_base::getTypeString
  const std::string getTypeString() const {
    return std::string("gs_param_greenreg<T>");
  }
  
  /* /// Overloads gs_param_base::getType
  const Param_type getType() const {
    return PARTYPE_NOT_AVAILABLE;
  }*/
  
  /// Overloads gs_param_t<T>::serialize, uses the convertion function of gs_param<T>
  std::string serialize(const T &val) const{
    return gs_param<T>::static_serialize(val);
  }  
  /// Overloads gs_param_t<T>::deserialize in gs_param_t<T>
  const bool deserialize(T &target_val, const std::string& str) {
    return static_deserialize(target_val, str);
  }
  /// Static convertion function, uses the convertion function of gs_param<T>
  inline static bool static_deserialize(T &target_val, const std::string& str) {
    return gs_param<T>::static_deserialize(target_val, str);
  }   
  
  
  // //////////////////////////////////////////////////////////////////// //
  // ///////   set and get with value   ///////////////////////////////// //
  
  /// Set the value of this parameter to the value of another gs_param.
  my_type& operator = (const my_type& v) { 
    setValue(v.getValue());
    return *this;
  }
  
  /// @see gs::cnf::gs_param_base::get_value_pointer
  const void* get_value_pointer() const {
    assert(false); exit(1);
    return NULL;
  }

  /// Set the value of this parameter.
  void setValue(const val_type &val) {
    set_greenreg_value(val); // calls virtual function in greenreg class
    //makeCallbacks(); // moved to greenreg callback
  }
  
  /// Returns the value of this parameter.
  const val_type getValue() const {
    return const_cast<gs_param_greenreg<T>*>(this)->get_greenreg_value(); // calls virtual function in greenreg class
  }

  
  // //////////////////////////////////////////////////////////////////// //
  // ///////   set and get with string representation   ///////////////// //
  
  /// Set the value of this parameter with a string.
  bool setString(const std::string &str) {
    val_type v = get_greenreg_value();
    bool success = deserialize(v, str);
    setValue(v);
    //if (success)
    //  makeCallbacks(); // moved to greenreg callback
    return success;
  }
  
  /// Get the value of this parameter as a string.
  const std::string& getString() const {
    return_string = serialize(getValue());
    return return_string;
  }
 
  // /////////////////////////////////////////////////////////////////////////////////// //
  // ///////   virtual functions to be implemented by deriving class   ///////////////// //

public:
  /// Adds a notification rule to the object, needs to be implemented by the deriving class
  /*virtual gs::reg::gr_event & add_rule(
                                   gs::reg::gr_reg_rule_container_e _container,
                                   std::string _name,
                                   gs::reg::gr_reg_rule_type_e _rule_type,
                                   ...) = 0;*/
  
protected:
  /// Shall add all notification rules needed for (post read) parameter callback mapping
  /**
   * This function is implemented by the register/others that
   * shall cause parameter callbacks. The functions returns names of the
   * used notification rules
   * @TODO: rename to add_post_write_param_rules(), Also change in User Guide!
   *
   * @return Vector of string names of rules being added.
   */
  virtual std::vector<std::string> add_post_read_param_rules() = 0;
  
  /// Shall return the notification rule container(s) which contain(s) the rule(s) being 
  /// added by add_post_read_param_rules, needs to be implemented by the deriving class
  /**
   * A secure way is to return all notification rule containers
   * the derived object has. Optionally only return the ones being
   * used by the add_post_read_param_rules() function
   *
   * @return Vector of pointers to notification rule container(s).
   */
  virtual std::vector<gs::reg::gr_notification_rule_container*> get_param_rules() = 0;

  /// Virtual function setting the value of the greenreg object, needs to be implemented by the object
  virtual void set_greenreg_value(const val_type &val) = 0;
  
  /// Virtual function getting the value from the greenreg object, needs to be implemented by the object
  virtual val_type get_greenreg_value() = 0;

  
  // //////////////////////////////////////////////// //
  // ///////   internal functions   ///////////////// //

protected:
  /// Activates / deactivates the notification rules
  /**
   * Activates / deactivates the notification rules which have been
   * added by this parameter for callback generation.<br>
   * Forces applying if not yet initialized (m_has_been_initialized).
   *
   * @param activ  If to activate (true) or deactivate (false) the rules.
   */
  void activate_my_notification_rules(bool activ) {
    if (activ && (!m_notification_rules_active || !m_has_been_initialized)) {
      bool activated;
      //std::cout << m_par_name << " Activate notification rules for param callbacks."<< std::endl;
      // go through all my (gs_param's) notification rules
      for (std::vector<std::string>::iterator it = m_notification_rule_vec.begin(); it != m_notification_rule_vec.end(); it++) {
        activated = false;
        std::vector<gs::reg::gr_notification_rule_container*> par_rules = get_param_rules();
        // go through all rule containers the parent has
        for (std::vector<gs::reg::gr_notification_rule_container*>::iterator cit = par_rules.begin(); cit != par_rules.end(); cit++) {
          if ((*cit)->has_rule(*it)) {
            //std::cout << " activate rule "<<*it << std::endl; // TODO CS: remove
            (*cit)->activate_rule(*it);
            activated = true;
          }
        }
        if (!activated) {
          GR_WARNING("The notification rule could not been activated!"); // Should never reach here!, maybe the returned get_param_rules() are not complete
        }
      }
      m_notification_rules_active = true;
    } else if (!activ && (m_notification_rules_active || !m_has_been_initialized)) {
      bool deactivated;
      //std::cout << m_par_name << " Deactivate notification rules for param callbacks."<< std::endl;
      // go through all my (gs_param's) notification rules
      for (std::vector<std::string>::iterator it = m_notification_rule_vec.begin(); it != m_notification_rule_vec.end(); it++) {
        deactivated = false;
        std::vector<gs::reg::gr_notification_rule_container*> par_rules = get_param_rules();
        // go through all rule containers the parent has
        for (std::vector<gs::reg::gr_notification_rule_container*>::iterator cit = par_rules.begin(); cit != par_rules.end(); cit++) {
          //std::cout << " search in container 0x"<< std::hex << (*cit) << std::dec << " for rule "<<*it << std::endl; // TODO CS: remove
          if ((*cit)->has_rule(*it)) {
            //std::cout << " deactivate rule "<<*it << std::endl; // TODO CS: remove
            (*cit)->deactivate_rule(*it);
            deactivated = true;
          }
        }
        if (!deactivated) {
          GR_WARNING("The notification rule could not been deactivated!"); // Should never reach here!, maybe the returned get_param_rules() are not complete
        }
      }
      m_notification_rules_active = false;
    } else {
      // nothing needs to be done
    }
  }
  
protected:
  /// If this param has been initialized, for debug purpose
  bool m_has_been_initialized;
  
  /// Vector containing the names of the notification rules owned by this param
  std::vector<std::string> m_notification_rule_vec;
  /// If the rules are currently activated
  bool m_notification_rules_active;
};

} // end namespace gs
