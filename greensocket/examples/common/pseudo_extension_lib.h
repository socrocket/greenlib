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


//This file represents a pseudo extension library for the greensocket examples
// The runtime bindability features of greensocket can only be exploited if
// sockets (in the world?) are greensockets and use extensions from a global
// library. Then (and only then) interoperability can be enhanced

#ifndef __PSEUDO_EXTENSION_LIB_H__
#define __PSEUDO_EXTENSION_LIB_H__

#include "greensocket/generic/gs_extension.h"

SINGLE_MEMBER_GUARDED_DATA(priority, unsigned int);
SINGLE_MEMBER_GUARDED_DATA(mid, unsigned int);

GS_GUARD_ONLY_EXTENSION(cacheable);
GS_GUARD_ONLY_EXTENSION(secure);


// Make this file be the "owner" of the phases defined here
// NOTE: the macro bellow is only used when compiling in MSVC and creating a DLL.
//       It requires a special version of the TLM library compiled as DLL
#undef TLM_PHASE_API
#define TLM_PHASE_API GREENSOCKET_API

DECLARE_EXTENDED_PHASE(FOO);
DECLARE_EXTENDED_PHASE(BAR);

// Return the "ownership" of the phases to TLM
// NOTE: _DLL is defined when /MD or /MDd is specified in MSVC
#ifdef _DLL
  #undef TLM_PHASE_API
  #define TLM_PHASE_API TLM_API
#endif

#endif
