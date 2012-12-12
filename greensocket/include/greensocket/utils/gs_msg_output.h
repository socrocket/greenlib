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

#ifndef __GS_MSG_OUTPUT_H__
#define __GS_MSG_OUTPUT_H__


// Define the macro GS_MSG_OUTPUT for each situation
//
// notes:
// 1) the macro should expand to one statement or be a block
// 2) the symbols GS_MSG_INFO, GS_MSG_WARN and GS_MSG_FATAL should be defined here, normally as macros
// 3) to convert to printf-like syntax from the stream, see the Carbon example

// for Carbon
#ifdef SOCD
  #include <sstream>
  #include "MxOutput.h"
  #define GS_MSG_INFO eslapi::CASI_MSG_INFO
  #define GS_MSG_WARN eslapi::CASI_MSG_WARNING
  #define GS_MSG_FATAL eslapi::CASI_MSG_FATAL_ERROR
  #define GS_MSG_OUTPUT(level, stream) \
    { \
      std::stringstream ss; \
      ss << stream; \
      eslapi::getOutputListener()->message(level, "%s", ss.str().c_str()); \
    }

// for other vendor
#elif USE_YOUR_OWN_DEFINE_HERE
  #define GS_MSG_INFO 
  #define GS_MSG_WARN 
  #define GS_MSG_FATAL 
  #define GS_MSG_OUTPUT(level, stream) \
  { \
    \ // implement your own here
  }

// GreenSocs default: send all to cerr
#else
  #define GS_MSG_INFO 0
  #define GS_MSG_WARN 1
  #define GS_MSG_FATAL 2
  #define GS_MSG_OUTPUT(level, stream) \
    { \
      std::cerr << stream << "." << std::endl; \
      if (level == GS_MSG_FATAL) abort(); \
    }

#endif


#endif // __GS_MSG_OUTPUT_H__
