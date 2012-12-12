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

#ifndef __GS_BIND_MACROS_H__
#define __GS_BIND_MACROS_H__

#ifdef GS_USE_EXTERNAL_BOOST

#include <boost/typeof/typeof.hpp>

#ifdef USE_GS_MONITOR
  #define GS_BIND_AND_OBSERVE(init, target, o_plugin) \
    { \
      BOOST_TYPEOF(gs::socket::connect_with_monitor(init, target)) \
        mon(gs::socket::connect_with_monitor(init, target)); \
      o_plugin->observe(mon->get_param()); \
      gs::socket::monitor_base* monbase = dynamic_cast<gs::socket::monitor_base*>(mon); \
      boost::shared_ptr<gs::socket::monitor_base> monbaseptr(monbase);  \
      gs::socket::monitor_base::get_list().push_back(monbaseptr); \
    }
#else
  #define GS_BIND_AND_OBSERVE(init, target, o_plugin) \
    init(target);
#endif

#ifdef USE_GS_MONITOR
  #define GS_BIND_AND_DEFINE_MONITOR_PARAM(init, target, param) \
    gs::gs_param_base* param##_p; \
    { \
      BOOST_TYPEOF(gs::socket::connect_with_monitor(init, target)) \
        mon(gs::socket::connect_with_monitor(init, target)); \
      param##_p = &mon->get_param(); \
      gs::socket::monitor_base* monbase = dynamic_cast<gs::socket::monitor_base*>(mon); \
      boost::shared_ptr<gs::socket::monitor_base> monbaseptr(monbase);  \
      gs::socket::monitor_base::get_list().push_back(monbaseptr); \
    } \
    gs::gs_param_base& param = *param##_p;
#else
  #define GS_BIND_AND_DEFINE_MONITOR_PARAM(init, target, param) \
     init(target);
#endif

#endif

#endif // __GS_BIND_MACROS_H__
