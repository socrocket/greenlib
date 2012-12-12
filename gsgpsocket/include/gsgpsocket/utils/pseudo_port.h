// LICENSETEXT
//
//   Copyright (C) 2005 - 2008 : GreenSocs Ltd
//       http://www.greensocs.com/ , email: info.com
//
//   Developed by :
//
//  Robert Guenzel, Christian Schroeder
//     Technical University of Braunschweig, Dept. E.I.S.
//     http://www.eis.cs.tu-bs.de
//
//   Mark Burton, Marcus Bartholomeu
//     GreenSocs Ltd
//
//
// The contents of this file are subject to the licensing terms specified
// in the file LICENSE. Please consult this file for restrictions and
// limitations that may apply.
// 
// ENDLICENSETEXT


#ifndef __PSEUDO_PORT_H__
#define __PSEUDO_PORT_H__

namespace gs{
namespace gp{

template<typename INTERFACE>
struct pseudo_port{
public:
  pseudo_port(): m_interface(NULL){};
  void bind(INTERFACE& interface_){
    m_interface=&interface_;
  }
  void operator()(INTERFACE& interface_){
    bind(interface_);
  }
  INTERFACE* operator->(){return m_interface;}
protected:
  INTERFACE* m_interface;
};

}
}

#endif
