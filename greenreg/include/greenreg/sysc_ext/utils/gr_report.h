/*
Copyright (c) 2008, Intel Corporation
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.
    * Neither the name of Intel Corporation nor the names of its contributors
      may be used to endorse or promote products derived from this software
      without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

//
// ChangeLog GreenSocs
//
// 2009-01-20 Christian Schroeder: renamed DEBUG -> GS_DEBUG
// 2009-01-21 Christian Schroeder: changed namespace
// 2009-01-28 Christian Schroeder: renamed DR_*  -> GR_*
//             added warning not to use the macros outside GreenReg
//             changed report macros to use file and line only once
//             introduced report scheme "GreenSocs/GreenReg" or "GreenSocs/GreenReg/other"
// 2009-01-29 Christian Schroeder: renamings
// 2009-01-29 FURTHER CHANGES SEE SVN LOG!
//


//#define EXCLUDE_FROM_BUILD
#ifndef EXCLUDE_FROM_BUILD
#undef EXCLUDE_FROM_BUILD

#include <string>

#include <boost/config.hpp>
#include <systemc>

#include "greenreg/utils/stl_ext/string_utils.h"

#ifndef __GR_REPORT_H__
#define __GR_REPORT_H__

//
//
//     THESE ARE GREENREG INTERNAL MACROS
//     
//     DO NOT USE THEM IN USER CODE !!!!
//
//

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// Defines a string for the compiler with active file and line number
//#define GR_FILE_LINE std::string( std::string(__FILE__) + std::string(":") + gs::reg_utils::string::to_string(__LINE__) + std::string(" ")).c_str()
//#define GR_FILE_LINE std::string( std::string(__FILE__) + std::string(":") + std::string( "__LINE__") + std::string(" ")).c_str()
#define GR_FILE_LINE std::string( std::string(__FILE__) + std::string(":") + gs::reg_utils::string::to_string< unsigned int>(__LINE__) + std::string(" ")).c_str()

#define GR_GREENREG_SC_REPORT_PREFIX "/GreenSocs/GreenReg"

	////////////////////////////////////////////
	/// GR_REPORT_ERROR
	/// MACRO - replaces SC_REPORT_ERROR for consistancy
	///
	/// @param _file ? char * name of file
	/// @param _message char * message to report
	/// @return N/A
	///
	/// @see SC_REPORT_ERROR
	////////////////////////////////////////////
//#define GR_REPORT_ERROR( _file, _message) SC_REPORT_ERROR( _file, _message); SC_REPORT does output the file and line anyway!
#define GR_REPORT_ERROR(_postfix, _message) SC_REPORT_ERROR( (std::string(GR_GREENREG_SC_REPORT_PREFIX) + "/" + _postfix).c_str(), _message);
#define GR_ERROR( _message)			SC_REPORT_ERROR( GR_GREENREG_SC_REPORT_PREFIX, _message);

	////////////////////////////////////////////
	/// GR_REPORT_FATAL
	/// MACRO - replaces SC_REPORT_FATAL for consistancy
	///
	/// @param _file ? char * name of file
	/// @param _message char * message to report
	/// @return N/A
	///
	/// @see SC_REPORT_FATAL
	////////////////////////////////////////////
#define GR_REPORT_FATAL(_postfix, _message) SC_REPORT_FATAL( (std::string(GR_GREENREG_SC_REPORT_PREFIX) + "/" + _postfix).c_str(), _message);
#define GR_FATAL( _message) SC_REPORT_FATAL(GR_GREENREG_SC_REPORT_PREFIX, _message);

/// To report a warning indepent from GS_DEBUG macro (which disable the usual GR_WARNING) 
#define GR_REPORT_FORCE_WARNING( _postfix, _message) SC_REPORT_WARNING( (std::string(GR_GREENREG_SC_REPORT_PREFIX) + "/" + _postfix).c_str(), _message);
#define GR_FORCE_WARNING( _message) SC_REPORT_WARNING(GR_GREENREG_SC_REPORT_PREFIX, _message);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef GS_DEBUG

  ////////////////////////////////////////////
	/// GR_REPORT_INFO
	/// MACRO - replaces SC_REPORT_INFO to enable
	///         debug only reporting
	///
	/// @param _file ? char * name of file
	/// @param _message char * message to report
	/// @return N/A
	///
	/// @see SC_REPORT_INFO
	////////////////////////////////////////////
	#define GR_REPORT_INFO( _postfix, _message) SC_REPORT_INFO( (std::string(GR_GREENREG_SC_REPORT_PREFIX) + "/" + _postfix).c_str(), _message);
	#define GR_INFO( _message) SC_REPORT_INFO(GR_GREENREG_SC_REPORT_PREFIX, _message);
	
	////////////////////////////////////////////
	/// GR_REPORT_WARNING
	/// MACRO - replaces SC_REPORT_WARNING to enable
	///         debug only reporting
	///
	/// @param _file ? char * name of file
	/// @param _message char * message to report
	/// @return N/A
	///
	/// @see SC_REPORT_WARNING
	////////////////////////////////////////////
  #define GR_REPORT_WARNING( _postfix, _message) SC_REPORT_WARNING( (std::string(GR_GREENREG_SC_REPORT_PREFIX) + "/" + _postfix).c_str(), _message);
	#define GR_WARNING( _message) SC_REPORT_WARNING(GR_GREENREG_SC_REPORT_PREFIX, _message);
	
#else // not GS_DEBUG
	#define GR_REPORT_INFO( _file, _message)	while(0);
	#define GR_INFO( _message)			while(0);
	
	#define GR_REPORT_WARNING( _file, _message)	while(0);
	#define GR_WARNING( _message)		while(0);
#endif // GS_DEBUG

#endif /*__GR_REPORT_H__*/

#endif // EXCLUDE_FROM_BUILD 
#undef EXCLUDE_FROM_BUILD
