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


// The following code is a copy of:
//  #include "greenbus/utils/gs_datatypes.h"
// <copy>

#ifndef __gs_datatypes_h__
#ifndef __gs_gs_datatypes__
#define __gs_datatypes_h__

/** 
 * \file gs_datatypes.h Default POD datatypes for GreenBus models
 */

#include <stdint.h>

namespace tlm {
  
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
#endif

// </copy>


#ifndef __gc_datatypes_h__
#define __gc_datatypes_h__
namespace gs {
#ifdef __gs_datatypes_h__
  using tlm::gs_int8;
  using tlm::gs_int16;
  using tlm::gs_int32;
  using tlm::gs_int64;
  using tlm::gs_uint8;
  using tlm::gs_uint16;
  using tlm::gs_uint32;
  using tlm::gs_uint64;
#endif
} // end namespace gs
#endif
