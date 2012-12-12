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


//the CTOR just initializes the base classes
template <unsigned int BUSWIDTH,typename TRAITS, unsigned int N, typename BIND_BASE>
gs::socket::target_socket_base<BUSWIDTH,TRAITS,N,BIND_BASE>::target_socket_base(const char* name)
                                     : base_type(name,static_cast<typename BIND_BASE::bind_base_type*>(this))
                                     , bind_checker_type(base_type::name(), (base_type*)this, BUSWIDTH)
//                                    , ext_support_type(BUSWIDTH)
                                     , base_eoe_done(false)
                                     
{
}


//execute end of elab only if not disabled in base
template <unsigned int BUSWIDTH,typename TRAITS, unsigned int N, typename BIND_BASE>
void gs::socket::target_socket_base<BUSWIDTH,TRAITS,N,BIND_BASE>::end_of_elaboration(){
  if (base_type::m_eoe_disabled) {
    return;
  }

  base_type::end_of_elaboration(); base_eoe_done=true; //execute base end_of_elab and remember that it is done
  for (unsigned int i=0; i<base_type::size(); i++) //the check all the bindings
    bind_checker_type::check_binding(i);
}
 
//the bound_to callback that happens when a binding was successful 
template <unsigned int BUSWIDTH,typename TRAITS, unsigned int N, typename BIND_BASE>
void gs::socket::target_socket_base<BUSWIDTH,TRAITS,N,BIND_BASE>::bound_to(const std::string& other_socket, typename BIND_BASE::bind_base_type* type, unsigned int index)
{
  //inform timing support class that binding is okay
  target_timing_support_base<TRAITS,BIND_BASE >::bound_to(other_socket, type, index);
}

//the string a required by the bind checker
template <unsigned int BUSWIDTH,typename TRAITS, unsigned int N, typename BIND_BASE>
const std::string& gs::socket::target_socket_base<BUSWIDTH,TRAITS,N,BIND_BASE>::get_type_string(){return s_kind;}

template <unsigned int BUSWIDTH, typename TRAITS, unsigned int N, typename BIND_BASE>
const std::string gs::socket::target_socket_base<BUSWIDTH, TRAITS,N, BIND_BASE>::s_kind="basic_green_socket";
