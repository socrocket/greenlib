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

//get the vector that associates extension IDs with extension names
template <typename TRAITS>
std::vector<std::string>& gs::socket::config<TRAITS>::get_ext_name_vect(){
  static std::vector<std::string>* p_name_vec=NULL;
  //initialize the vector so that it has enough space to hold all the extensions. Initially we do not know the names
  if (!p_name_vec) p_name_vec=new std::vector<std::string>(tlm::max_num_extensions(), "not known yet.");
  return *p_name_vec;
}

//get the reference to one of the names of the ID-name-association vector
// if the ID is larger than what we expect we do not raise an error
// The extension names are used within debug output only, so whoever
// reads the extension name as "not existing" will know something is wrong
template <typename TRAITS>
std::string& gs::socket::config<TRAITS>::get_ext_name(unsigned int index){
  static std::string out_of_bounds("Extension not existing.");
  if (index>=tlm::max_num_extensions()) return out_of_bounds;
  return get_ext_name_vect()[index];
}

//test if the provided config is different from the config the function is called on
template <typename TRAITS>
bool gs::socket::config<TRAITS>::diff(const gs::socket::config<TRAITS>& other){
  if(m_treat_unknown_as_rejected!=other.m_treat_unknown_as_rejected) {
    return true;
  }

  if(m_used_phases.size()!=other.m_used_phases.size()) {
    return true;
  } //there is a diff if phase maps are unequal
  
  if(m_used_extensions.size()!=other.m_used_extensions.size()) {
    return true;
  } //there is a diff if extension vectors are of inequal size
  
  //there is a diff if extensions differ in their bindability qualification
  for (unsigned int i=0;i<m_used_extensions.size(); i++){
    if (other.m_used_extensions.at(i)!=m_used_extensions[i]) {
      return true;
    }
  }
  
  //go through all the phases that are within this config
  for (std::map<std::string, gs::ext::gs_extension_bindability_enum>::iterator i=m_used_phases.begin(); i!=m_used_phases.end(); i++){
    std::map<std::string, gs::ext::gs_extension_bindability_enum>::const_iterator finder=other.m_used_phases.find(i->first);
    if (finder==other.m_used_phases.end()) {
      //there is a diff if a phase cannot be found in the provided config
      return true;
    }
    else 
    if (finder->second!= i->second) {
      //there is a diff if a phase is part of the provided config but has a different bindability qualification
      return true;
    }
  }

  //go through all the phases that are within the provided config
  for (std::map<std::string, gs::ext::gs_extension_bindability_enum>::const_iterator i=other.m_used_phases.begin(); i!=other.m_used_phases.end(); i++){
    std::map<std::string, gs::ext::gs_extension_bindability_enum>::iterator finder=m_used_phases.find(i->first);
    if (finder==m_used_phases.end()) {
      //there is a diff if a phase cannot be found in this config
      return true;
    }
    else 
    if (finder->second!= i->second) {
      //there is a diff if a phase is part of this config but has a different bindability qualification
      return true;
    }
  }
  return false;  
}

//copy CTOR uses assignment operator implementation
template <typename TRAITS>
gs::socket::config<TRAITS>::config(const config& other){
  *this=other;
}

//assignment operator implementation
template <typename TRAITS>
gs::socket::config<TRAITS>& gs::socket::config<TRAITS>::operator=(const gs::socket::config<TRAITS>& other){
  m_used_phases=other.m_used_phases;
  m_type_string=other.m_type_string;
  m_treat_unknown_as_rejected=other.m_treat_unknown_as_rejected;
  invalid=other.invalid;
  force_reeval=other.force_reeval;
  m_used_extensions=other.m_used_extensions;
  return *this;
}

//DTOR has no job
template <typename TRAITS>
gs::socket::config<TRAITS>::~config(){
}

//set but never reset force_reeval
template <typename TRAITS>
void gs::socket::config<TRAITS>::set_force_reeval(bool val){force_reeval|=val;}

//set the pointer to the string
template <typename TRAITS>
void gs::socket::config<TRAITS>::set_string_ptr(const std::string* name_){m_type_string=name_;}

//getter to test if config is invalid
template <typename TRAITS>
bool gs::socket::config<TRAITS>::get_invalid()const{return invalid;}

//setter for invalid
template <typename TRAITS>
void gs::socket::config<TRAITS>::set_invalid(bool value){invalid=value;}

/*
Test how a certain extension is treated. Test is type based
*/
template <typename TRAITS>
template <typename T>
gs::ext::gs_extension_bindability_enum gs::socket::config<TRAITS>::has_extension()const{
  //if the ID of the extension is not within our vector of known extensions
  // we return optional or rejected, depending on whether we treat unknowns as
  // optional or rejected
  if (m_used_extensions.size()<=T::ID) { 
    if (m_treat_unknown_as_rejected)
      return gs::ext::gs_reject;
    else
      return gs::ext::gs_optional;
  }
  
  //if the extension is not unkown, we return what we know
  if (m_used_extensions.at(T::ID)!=gs::ext::gs_unknown){
    return m_used_extensions.at(T::ID);
  }
  else
  //otherwise we return optional or rejected, depending on whether we treat unknowns as
  // optional or rejected
  if (m_treat_unknown_as_rejected)
    return gs::ext::gs_reject;
  else
    return gs::ext::gs_optional;
}


//test how a certain phase is treated
template <typename TRAITS>
gs::ext::gs_extension_bindability_enum gs::socket::config<TRAITS>::has_phase(unsigned int ph)const{
  //First convert phase into string. We use the string here, because the TLM-2.0 LRM does not 
  // explicitly state that the uints used for phases are unique.
  phase_type tmp=ph; 
  std::stringstream s; 
  s<<tmp;
  
  //if the phase is in our map, we return what we know
  if (m_used_phases.find(s.str())!=m_used_phases.end()){
    return m_used_phases.find(s.str())->second;
  }
  else
  //otherwise we return optional or rejected, depending on whether we treat unknowns as
  // optional or rejected
  if (m_treat_unknown_as_rejected)
    return gs::ext::gs_reject;
  else
    return gs::ext::gs_optional;
}

//the CTOR: initially the config is invalid and the forced reevaluation is turned off
template <typename TRAITS>
gs::socket::config<TRAITS>::config(): invalid(true), force_reeval(false){}

//this function performs the merge of two configurations
template <typename TRAITS>
bool gs::socket::config<TRAITS>::merge_with(const char* my_name, const char* other_name, config<TRAITS>& other_conf, bool abort_at_mismatch){
  //first, let's make sure the extension vectors are big enough
  m_used_extensions.resize(tlm::max_num_extensions(), gs::ext::gs_unknown);
  other_conf.m_used_extensions.resize(tlm::max_num_extensions(), gs::ext::gs_unknown);
  
  //we have a bool that remembers whether or not to abort
  // the reason is that we wanna see all problems between two configs and not just the first one detected
  bool do_abort=false; //initially we do not expect a problem
  
  //go through the extension vectors (remember both are of equal size now)
  for (unsigned int i=0; i<m_used_extensions.size(); i++){
    gs::ext::gs_extension_bindability_enum& ext1=m_used_extensions[i]; //NOTE: ext1 is a reference! and it is from our own config
    gs::ext::gs_extension_bindability_enum  ext2=other_conf.m_used_extensions[i]; //while ext2 is a copy
    
    //test for compatibility
    switch(ext1){
      case gs::ext::gs_mandatory:
        if (ext2==gs::ext::gs_unknown){ //ext1 is mandatory and ext2 is unknown
          if(other_conf.m_treat_unknown_as_rejected){ //if the other config treats unknown as rejected, we need to print an error an remember to abort.
            GS_MSG_OUTPUT(GS_MSG_WARN, my_name<<" requires use of extension "<<get_ext_name(i)<<" but "<<other_name<<" doesn't know that extension and reject unknown extensions");
            do_abort=true;
          }
        }
        else{
          if (ext2==gs::ext::gs_reject){ //ext1 is mandatory and ext2 is rejected. Print an error and remember to abort
            GS_MSG_OUTPUT(GS_MSG_WARN, my_name<<" requires use of extension "<<get_ext_name(i)<<" but "<<other_name<<" rejects that extension");
            do_abort=true;
          }
          //else nothing. The extension stays mandatory
        }
        break;
      case gs::ext::gs_optional:
        switch(ext2){
          case gs::ext::gs_unknown: //ext1 is optional and ext2 is unknown
            if (other_conf.m_treat_unknown_as_rejected) ext1=gs::ext::gs_reject; //if other treats unknown as rejected we will now mark the optional thing as rejected
            break;            
          case gs::ext::gs_reject: //ext1 is optional and ext2 is rejected
            ext1=gs::ext::gs_reject; //we will mark ext1 as rejected
            break;
          case gs::ext::gs_mandatory: //ext1 is optional and ext2 is mandatory
            ext1=gs::ext::gs_mandatory; //change ext1 to mandatory
            break;
          case gs::ext::gs_optional: //both are optional. Stay optional.
            break;
        }
        break;
      case gs::ext::gs_reject:
        //the other side will check this (both sides check the configs)
        break;
      case gs::ext::gs_unknown:
        if (ext2!=gs::ext::gs_unknown){ //ext1 is unkown, but ext2 is not unknown
          if(m_treat_unknown_as_rejected){ //we treat unknown as rejected
            if (ext2==gs::ext::gs_mandatory){ //and ext2 is mandatory so we need to abort
              GS_MSG_OUTPUT(GS_MSG_WARN, my_name<<" treats unknown extensions as rejected but "<<other_name<<" uses the extension "<<get_ext_name(i)<<" which is unknown to "<<my_name);
              do_abort=true;
            }
          }
          else{ //ext1 is unkown, ext2 is not unknown, we treat unknown as optional, so we just adopt
            ext1=ext2;
          }
        }
    }    
  }
    
  //we go through our map of phases
  for (std::map<std::string, gs::ext::gs_extension_bindability_enum>::iterator i=m_used_phases.begin(); i!=m_used_phases.end(); i++){
    std::map<std::string, gs::ext::gs_extension_bindability_enum>::iterator finder=other_conf.m_used_phases.find(i->first);
    switch (i->second){
      case gs::ext::gs_optional: //we treat the phase as optional
        if (finder!=other_conf.m_used_phases.end()) 
          switch(finder->second){
            case gs::ext::gs_mandatory: i->second=gs::ext::gs_mandatory; break; //the other treats it as mandatory so we adopt that
            case gs::ext::gs_reject: i->second=gs::ext::gs_reject; break; //the other treats it as rejected so we adopt that
            case gs::ext::gs_optional: break; //the other treats it as optional as well, so we keep it optional
            
            //the phase map never store unknown phases explicitly, because it only stores known phases
            case gs::ext::gs_unknown: GS_MSG_OUTPUT(GS_MSG_FATAL, "green_socket_config.tpp (line "<<__LINE__<<"): This should NEVER happen!"); break;
          }
        else //we treat the phase as optional and the other doesn't know it 
          if (other_conf.m_treat_unknown_as_rejected) i->second=gs::ext::gs_reject;//so if the other treat unknown as rejected we will reject the phase now
        break;
      case gs::ext::gs_mandatory: //we treat the phase a mandatory
        if (finder==other_conf.m_used_phases.end()){
          if(other_conf.m_treat_unknown_as_rejected){ //the other doesn't know it and treats unknwon as rejected so we raise and error and abort
            GS_MSG_OUTPUT(GS_MSG_WARN, my_name<<" requires use of phase "<<i->first<<" but "<<other_name<<" doesn't know that phase and rejects unknown phases");
            do_abort=true;
          }
        }
        else
        if (finder->second==gs::ext::gs_reject){ //the other knows it but rejects it so we raise and error and abort
          GS_MSG_OUTPUT(GS_MSG_WARN, my_name<<" requires use of phase "<<i->first<<" but "<<other_name<<" rejects that phase");
          do_abort=true;
        }
        break;
      case gs::ext::gs_reject: //we reject it. That will be covered by the other side
        break;
      case gs::ext::gs_unknown:
        //the phase map never store unknown phases explicitly, because it only stores known phases
        GS_MSG_OUTPUT(GS_MSG_FATAL, "green_socket_config.tpp (line "<<__LINE__<<"): This should NEVER happen!");
        break;
    }
  }
  
  //we go through the other map of phases
  for (std::map<std::string, gs::ext::gs_extension_bindability_enum>::iterator i=other_conf.m_used_phases.begin(); i!=other_conf.m_used_phases.end(); i++){
    std::map<std::string, gs::ext::gs_extension_bindability_enum>::iterator finder=m_used_phases.find(i->first);
    if (finder==m_used_phases.end()){ // we do not know the phase the other knows
      if (m_treat_unknown_as_rejected){ //and we treat unknown as rejected
        if (i->second==gs::ext::gs_mandatory){ //and the other treats it as mandatory. So we abort
          GS_MSG_OUTPUT(GS_MSG_WARN, my_name<<" treats unknown phases as rejected but "<<other_name<<" uses the phase "<<i->first<<" which is unknown to "<<my_name);
          do_abort=true;
        }
      }
      else{ //we treat unkown as optional, so we adopt the thing
        m_used_phases[i->first]=i->second;
      }
    }
  }
  
  //merge the treatment of unknown extensions
  m_treat_unknown_as_rejected=m_treat_unknown_as_rejected | other_conf.m_treat_unknown_as_rejected; //reject is stronger than ignore
  
  //if we have to abort we do it now
  if (abort_at_mismatch && do_abort)
    GS_MSG_OUTPUT(GS_MSG_FATAL, "binding check failed");
  
  return do_abort; //otherwise we will at least return the info that the check would have failed
}

//set how we treat unkown extensions and phases
template <typename TRAITS>
void gs::socket::config<TRAITS>::treat_unknown_as_optional(){
  m_treat_unknown_as_rejected=false;
}

//set how we treat unkown extensions and phases
template <typename TRAITS>
void gs::socket::config<TRAITS>::treat_unknown_as_rejected(){
  m_treat_unknown_as_rejected=true;
}

//test how we treat unkown extensions and phases
template <typename TRAITS>
bool gs::socket::config<TRAITS>::unknowns_are_optional(){
  return !m_treat_unknown_as_rejected;
}

//store a phase as mandatory in the config
template <typename TRAITS>
void gs::socket::config<TRAITS>::use_mandatory_phase(unsigned int ph1){
  //in earlier versions you could add a vector of phases at once. The helper is still from that time
  // so we build a temporary vector to use it.
  std::vector<unsigned int> phs;
  phs.push_back(ph1);
  add_to_phase_map(phs, gs::ext::gs_mandatory); 
}

//store a phase as optional in the config
template <typename TRAITS>
void gs::socket::config<TRAITS>::use_optional_phase(unsigned int ph1){
  //in earlier versions you could add a vector of phases at once. The helper is still from that time
  // so we build a temporary vector to use it.
  std::vector<unsigned int> phs;
  phs.push_back(ph1);
  add_to_phase_map(phs, gs::ext::gs_optional);
}

//store a phase as rejected in the config
template <typename TRAITS>
void gs::socket::config<TRAITS>::reject_phase(unsigned int ph1){
  //in earlier versions you could add a vector of phases at once. The helper is still from that time
  // so we build a temporary vector to use it.
  std::vector<unsigned int> phs;
  phs.push_back(ph1);
  add_to_phase_map(phs, gs::ext::gs_reject);
}

//remove a phase from the config. i.e. make it unkown
template <typename TRAITS>
void gs::socket::config<TRAITS>::remove_phase(unsigned int ph1){
  phase_type tmp=ph1;
  std::stringstream s;
  s<<tmp;
  m_used_phases.erase(s.str());
}

//remove an extension from the config. i.e. make it unkown
template <typename TRAITS>
template <typename T>
void gs::socket::config<TRAITS>::remove_extension(){
  T tmp; //create a temporary extension.
  get_ext_name(T::ID)=tmp.get_name(); //get the name and store it in our name vector
  m_used_extensions.resize(tlm::max_num_extensions(), gs::ext::gs_unknown); //make sure the vector is large enough
  m_used_extensions[T::ID]=gs::ext::gs_unknown; //set the state to unkown
}

template <typename TRAITS>
template <typename T>
void gs::socket::config<TRAITS>::use_mandatory_extension(){
  T tmp; //create a temporary extension.
  get_ext_name(T::ID)=tmp.get_name(); //get the name and store it in our name vector
  m_used_extensions.resize(tlm::max_num_extensions(), gs::ext::gs_unknown); //make sure the vector is large enough
  m_used_extensions[T::ID]=gs::ext::gs_mandatory;//set the state to mandatory
}

template <typename TRAITS>
template <typename T>
void gs::socket::config<TRAITS>::reject_extension(){
  T tmp; //create a temporary extension.
  get_ext_name(T::ID)=tmp.get_name(); //get the name and store it in our name vector
  m_used_extensions.resize(tlm::max_num_extensions(), gs::ext::gs_unknown);//make sure the vector is large enough
  m_used_extensions[T::ID]=gs::ext::gs_reject;//set the state to rejected
}

template <typename TRAITS>
template <typename T>
void gs::socket::config<TRAITS>::use_optional_extension(){
  T tmp; //create a temporary extension.
  get_ext_name(T::ID)=tmp.get_name(); //get the name and store it in our name vector
  m_used_extensions.resize(tlm::max_num_extensions(), gs::ext::gs_unknown); //make sure the vector is large enough
  m_used_extensions[T::ID]=gs::ext::gs_optional; //set the state to rejected
}

template <typename TRAITS>
void gs::socket::config<TRAITS>::add_to_phase_map(std::vector<unsigned int>& phs, gs::ext::gs_extension_bindability_enum mandatory){
  //add a vector of phases to our config
  for (unsigned int i=0; i<phs.size(); i++){
    phase_type tmp=phs[i];
    std::stringstream s;
    s<<tmp;
    //if phase is already there just the bindability state is changed
    m_used_phases[s.str()]=mandatory;
  }
}

//dump the config as text into a string
template <typename TRAITS>
std::string gs::socket::config<TRAITS>::to_string() const{
  std::stringstream s;
  if (invalid) {s<<" Note: Unassigned configuration."<<std::endl;}
  s<<"  Treats unknown as "<<((m_treat_unknown_as_rejected)?"rejected.":"optional.")<<std::endl;
  s<<"  Used extensions:"<<std::endl;
  for (unsigned int i=0; i<m_used_extensions.size(); i++){
    if (m_used_extensions.at(i)!=gs::ext::gs_unknown){
      s<<"    "<<get_ext_name(i)<<" used as ";
      switch(m_used_extensions.at(i)){
        case gs::ext::gs_mandatory: s<<"mandatory"; break;
        case gs::ext::gs_optional: s<<"optional"; break;
        case gs::ext::gs_reject: s<<"rejected"; break;
        case gs::ext::gs_unknown: ;
      }
      s<<"."<<std::endl;
    }
  }
  s<<"  Used phases:"<<std::endl;
  for (std::map<std::string,gs::ext::gs_extension_bindability_enum>::const_iterator i=m_used_phases.begin(); i!=m_used_phases.end(); i++){
    s<<"    "<<i->first<<" used as ";
    switch(i->second){
      case gs::ext::gs_mandatory: s<<"mandatory"; break;
      case gs::ext::gs_optional: s<<"optional"; break;
      case gs::ext::gs_reject: s<<"rejected"; break;
      case gs::ext::gs_unknown: ;
    }
    s<<"."<<std::endl;

  }
  return s.str();
}
