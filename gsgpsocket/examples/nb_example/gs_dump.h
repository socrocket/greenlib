// LICENSETEXT
// 
//   Copyright (C) 2007 : GreenSocs Ltd
//       http://www.greensocs.com/ , email: info@greensocs.com
// 
//   Developed by :
// 
//   Wolfgang Klingauf, Robert Guenzel
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

#ifndef GS_DUMP // normally already included by GSGPSocket
# ifdef GS_VERBOSE
#   define GS_DUMP(msg)         std::cout<<"@ "<<sc_core::sc_time_stamp()<<" : "<<name()<<" "<<msg<<std::endl
#   define GS_DUMP_N(name, msg) std::cout<<"@ "<<sc_core::sc_time_stamp()<<" : "<<name  <<" "<<msg<<std::endl
# else
#  define GS_DUMP(msg)
#  define GS_DUMP_N(name, msg)
# endif
#endif
