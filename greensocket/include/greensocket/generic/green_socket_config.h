//   GreenSocket
//
// LICENSETEXT
//
//   Copyright (C) 2007 : GreenSocs Ltd
// 	 http://www.greensocs.com/ , email: info@greensocs.com
//
//   Developed by :
//   
//   Robert Guenzel <guenzel@eis.cs.tu-bs.de>,
//     Technical University of Braunschweig, Dept. E.I.S.
//     http://www.eis.cs.tu-bs.de
//
//
// The contents of this file are subject to the licensing terms specified
// in the file LICENSE. Please consult this file for restrictions and
// limitations that may apply.
// 
// ENDLICENSETEXT
#ifndef __GREEN_SOCKET_CONFIG_H__
#define __GREEN_SOCKET_CONFIG_H__

#include "tlm.h"
#include <vector>
#include <string>
#include <map>
#include <set>
#include "greensocket/generic/gs_extension.h"

#define END_OF_PHASE_LIST tlm::UNINITIALIZED_PHASE
#define EXTENDED_PHASE(ph) ((unsigned int)ph)

namespace gs{
namespace socket{

template <typename TRAITS>
struct osci_config;

//This structure represents the configuration for a GreenSocket
// it should be set up during construction or before_end_of_elaboration
// and be assigned to a GreenSocket
template <typename TRAITS>
struct config
{
  friend struct osci_config<TRAITS>;

  typedef typename TRAITS::tlm_payload_type  payload_type;
  typedef typename TRAITS::tlm_phase_type  phase_type;

  //empty constructor
  config();

  //copy constructor
  config(const config&);

  //destructor
  ~config();

  //add a mandatory extension to the configuration
  //Example:
  //  gs::socket::config conf;
  //  conf.use_manadatory_extension<cacheable>();
  //
  // This will add the cacheable extension as mandatory to the configuration
  template <typename T>
  void use_mandatory_extension();

  //add a ignorable extension to the configuration
  //Example:
  //  gs::socket::config conf;
  //  conf.use_optional_extension<cacheable>();
  //
  // This will add the cacheable extension as ignorable to the configuration
  template <typename T>
  void use_optional_extension();

  //add a rejected extension to the configuration
  //Example:
  //  gs::socket::config conf;
  //  conf.reject_extension<cacheable>();
  //
  // This will add the cacheable extension as rejected to the configuration
  template <typename T>
  void reject_extension();

  //remove an extension from the configuration
  //Example:
  //  gs::socket::config conf;
  //  conf.remove_extension<cacheable>();
  //
  // This will remove the cacheable extension from the configuration
  template <typename T>
  void remove_extension();
  
  //add a phase to the configuration as mandatory
  //Example:
  //  gs::socket::config conf;
  //  conf.use_mandatory_phase(tlm::BEGIN_REQ);
  //
  // This will add the osci phases BEGIN_REQ as mandatory to the configuration
  void use_mandatory_phase(unsigned int ph1);

  //add a phase to the configuration as ignorable
  //Example:
  //  gs::socket::config conf;
  //  conf.use_optional_phase(tlm::BEGIN_REQ);
  //
  // This will add the osci phases BEGIN_REQ as ignorable to the configuration
  void use_optional_phase(unsigned int ph1);
  
  //add a phase to the configuration as rejected
  //Example:
  //  gs::socket::config conf;
  //  conf.use_optional_phase(tlm::BEGIN_RESP);
  //
  // This will add the osci phases BEGIN_RESP as rejected to the configuration  
  void reject_phase(unsigned int ph1);

  //remove an phase from the configuration
  //Example:
  //  gs::socket::config conf;
  //  conf.remove_phase(tlm::BEGIN_REQ);
  //
  // This will remove the BEGIN_REQ phase from the configuration
  void remove_phase(unsigned int ph1);

  //set how the socket will handle unknown extensions/phases (i.e. extensions/phases
  // that are not part of its configuration)
  //
  // they can be either treated as ignoreable (so after binding they will be part of the configuration)
  // or as rejected
  void treat_unknown_as_optional();
  void treat_unknown_as_rejected();
  
  //find out how unknown extensions are handled
  bool unknowns_are_optional();

  //merge a configuration with another
  // the first const char* shall identify the owner of the configuration
  //  on which the function is called, the second const char* shall
  //  identify the owner of the configuration that is passed as the 
  //  third argument (these const chars* will be used in the error
  //  messages)
  //
  //The merge A.merge_with("A","B", B) will only be successful if:
  //  * if all mandatory extension and phases of A are mandatory or ignorable in B
  //  * if no rejected extension or phase of A is mandatory in B
  //  * if A treats unknowns as rejected but there are no unknowns in B
  //
  //After the merge configuration A changes:
  //  * mandatory extensions stay mandatory (no change)
  //  * rejected extensions stay rejected (no change)
  //  * ignorable extensions that were rejected in B are removed from A
  //  * ignorable extension that were mandatory in B are now mandatory in A
  //  * ignorable extension that are ignorable in B stay ignorable
  //  * if B treats unknowns as rejected, ignorables unknown to B are removed from A
  //  * if B treats unknowns as ignorable, ignorables unknown to B stay ignorable in A
  //  * if A treats unknowns as ignorable, unknowns in B are added as ignorable to A
  //  * all the above points for phases
  //  * if B treats unknowns as rejected A will do so now
  bool merge_with(const char*, const char*, config&, bool abort_at_mismatch=true);
  
  
  //this functions converts the configuration into a string to be dumped to files or std::out
  // it is for debug only
  std::string to_string() const;
  
  //test if a certain extension is part of the configuration
  // if a pointer to a bindability_enum is provided, it will point to the
  //  binability state of the extension
  template <typename T>
  gs::ext::gs_extension_bindability_enum has_extension() const;

  //test if a certain phase is part of the configuration
  // if a pointer to a bindability_enum is provided, it will point to the
  //  binability state of the extension  
  gs::ext::gs_extension_bindability_enum has_phase(unsigned int) const;
  
  //When a configuration is changed during elaboration, the socket will reevaluate the binding only if the new configuration differs from the old one. 
  //However, customized sockets may use the callbacks that are generated due to a binding check to transmit information on top of the actual GreenSocket 
  //configuration. In this case you can use this function to enforce GreenSocket to reevaluate the binding and do all callbacks again, 
  //even if the GreenSocket configuration did not change.  
  //NOTE: this setter only expresses a wish. If force_reeval is already true it cannot be reset using this setter
  // this allows to call a sequence like
  // config.set_force_reeval(conditions_a_is_met());
  // config.set_force_reeval(conditions_b_is_met());
  // config.set_force_reeval(conditions_c_is_met());
  // and if any one of these is true, force_reeval becomes true, even if one of them is false
  void set_force_reeval(bool val);

  //this function can be used to check if a configuration is invalid
  // it shall only be used on socket configurations after construction time
  bool get_invalid()const;  
  
  //may only be called after or at start_of_simulation
  unsigned int get_upper_bound_for_extension_ids(){
    return m_used_extensions.size();
  }

  //test of an extension with a certain ID is in the config
  gs::ext::gs_extension_bindability_enum has_extension(unsigned int index) const{
    if (m_used_extensions.size()<=index) {
      if (m_treat_unknown_as_rejected)
        return gs::ext::gs_reject;
      else
        return gs::ext::gs_optional;
    }
    if (m_used_extensions.at(index)!=gs::ext::gs_unknown){
      return m_used_extensions.at(index);
    }
    else
    if (m_treat_unknown_as_rejected)
      return gs::ext::gs_reject;
    else
      return gs::ext::gs_optional;
  }
  
  //assignment operator
  config& operator=(const config&);
protected:

  //internal helpers. Look into implementation comments
  void set_string_ptr(const std::string* name_);
  void set_invalid(bool value);
  void add_to_phase_map(std::vector<unsigned int>& phs, gs::ext::gs_extension_bindability_enum mandatory);

  std::vector<gs::ext::gs_extension_bindability_enum> m_used_extensions; //the vector of known extensions
  std::map<std::string, gs::ext::gs_extension_bindability_enum> m_used_phases; //the map of known phases
  
public:
  bool diff(const config&); //test if two configs have a difference
  const std::string* m_type_string; //a pointer to a string that identifies the type of the socket to which this config was assigned
protected:
  bool m_treat_unknown_as_rejected; //remember how to treat unknown extensions
public:
  bool invalid; //once assigned this bool will change from false to true
  bool force_reeval; //this bool can be used to enforce a bound_to callback, even if the config was not changed
  
  static std::vector<std::string>& get_ext_name_vect(); //this vector holds the names of the extensions. It allows to get the name of an extension
                                                        //if all you have is its ID
  static std::string& get_ext_name(unsigned int index); //this function get's the name of the extension that has ID=index
};

} //end ns socket
} //ens ns gs

#include "greensocket/generic/green_socket_config.tpp"


#endif
