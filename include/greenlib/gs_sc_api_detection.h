// LICENSETEXT
//
//   Copyright (C) 2007 : GreenSocs Ltd
//       http://www.greensocs.com/ , email: info@greensocs.com
//
//   Developed by :
//
//   Marcus Bartholomeu
//     GreenSocs Ltd
//
//
// The contents of this file are subject to the licensing terms specified
// in the file LICENSE. Please consult this file for restrictions and
// limitations that may apply.
//
// ENDLICENSETEXT


////////////////////////////////////////////////////////
//
// Detect SystemC API and define the macro SYSTEMC_API
// (include detection for other SystemC vendors if necessary)
//
// 2008-10-02: Detect OSCI SystemC versions 2.1 and 2.2
//
////////////////////////////////////////////////////////


#ifndef GS_SC_API_DETECTION_H
#define GS_SC_API_DETECTION_H

#include <systemc>

// Respect if the macro is set externally (to 230 or 231 only)
#ifdef SYSTEMC_API

#if SYSTEMC_API != 210 && SYSTEMC_API != 220 && SYSTEMC_API != 230 && \
SYSTEMC_API != 231
#error The SYSTEMC_API macro is set to an unknown value. Accepted values are \
210, 220, 230 or 231. Please, see the source code gs_sc_api_detection.h for more \
information.
#endif

// So it is not defined externally, detect it!
#else

// OSCI SystemC 2.1.0, 2.2.0 and 2.2.05jun06 (Unsupported)
#if SYSTEMC_VERSION == 20050714 || SYSTEMC_VERSION == 20070314 \
                                || SYSTEMC_VERSION == 20060505
#error GreenLib only supports SystemC 2.3.0 and 2.3.1
#endif

// OSCI SystemC 2.3.0
#if SYSTEMC_VERSION == 20111121 || SYSTEMC_VERSION == 20120701
#define SYSTEMC_API 230
#endif

// OSCI SystemC 2.3.1
#if SYSTEMC_VERSION == 20140417
#define SYSTEMC_API 231
#endif

// SystemC API to use must be defined at this point
// include the detection above for other SystemC vendors to
// define the SYSTEMC_API macro
#ifndef SYSTEMC_API
#error The SYSTEMC_API macro is not set and could not be detected. Please, \
see the source code sc_api_detection.h for more information.
#endif

#endif // def SYSTEMC_API

#endif // ndef GS_SC_API_DETECTION_H
