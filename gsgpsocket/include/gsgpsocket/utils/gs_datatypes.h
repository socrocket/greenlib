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

#ifndef __gs_gs_datatypes_h__
#define __gs_gs_datatypes_h__

/** 
 * \file gs_datatypes.h Default POD datatypes for GreenBus models
 */

namespace gs {

typedef char                 gs_int8;
typedef short                gs_int16;
typedef int                  gs_int32;
typedef long long            gs_int64;

typedef unsigned char        gs_uint8;
typedef unsigned short       gs_uint16;
typedef unsigned int         gs_uint32;
typedef unsigned long long   gs_uint64;
typedef long                 gs_handle;

} // end namespace tlm

#endif
