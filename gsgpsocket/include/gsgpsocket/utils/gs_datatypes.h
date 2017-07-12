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

#include <stdint.h>

namespace gs {

typedef int8_t    gs_int8;
typedef int16_t   gs_int16;
typedef int32_t   gs_int32;
typedef int64_t   gs_int64;

typedef uint8_t   gs_uint8;
typedef uint16_t  gs_uint16;
typedef uint32_t  gs_uint32;
typedef uint64_t  gs_uint64;
typedef uintptr_t gs_handle;

} // end namespace tlm

#endif
