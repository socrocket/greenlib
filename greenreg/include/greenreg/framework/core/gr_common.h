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

// ChangeLog GreenSocs
// 2009-01-21 Christian Schroeder: changed namespace
// 2009-01-29 Christian Schroeder: renamings
// 2009-01-29 FURTHER CHANGES SEE SVN LOG!


//#define EXCLUDE_FROM_BUILD
#ifndef EXCLUDE_FROM_BUILD
#undef EXCLUDE_FROM_BUILD

#ifndef _GR_COMMON_H_
#define _GR_COMMON_H_

#include <string>

#include <boost/config.hpp> // needed for SystemC 2.1
#include <systemc>

#include "greenreg/utils/storage/option_structs.h"
#include "greenreg/utils/storage/option_list.h"

#include "greenreg/gr_settings.h"

// for transaction typedef
#include "gsgpsocket/transport/GSGPSlaveSocket.h"
#include "greenreg/utils/greenreg_utils.h"


namespace gs {
namespace reg {

/// Transaction type which is used in the notification call chain
typedef gs::gp::GenericSlavePort<32>::accessHandle transaction_type;

////////////////////////////////////////////
/// gr_bool_toggle_e
/// simple boolean toggle
/// @author 
/// @since 
////////////////////////////////////////////
enum gr_bool_toggle_e
{
	TOGGLE_ERROR = 0,
	ON,
	OFF
};

class gr_options_bool_toggle_t : public gs::reg_utils::option_list_t
{
public:
	gr_options_bool_toggle_t()
	{
		error_value = 0;
		_( "TOGGLE_ERROR", TOGGLE_ERROR);
		_( "ON", ON);
		_( "OFF", OFF);
	}
};
static gr_options_bool_toggle_t gr_options_bool_toggle;

/*
static ::gs::reg_utils::option_list_t gr_options_bool_toggle =
{ 2, 0, 
	{	
		{"On",ON}, 	
		{"Off",OFF} 
	}
};
*/
////////////////////////////////////////////
/// gr_options_format
/// supported file formats
/// @author 
/// @since 
////////////////////////////////////////////
enum gr_dump_format_e
{
	XML = 1,
	DR_CONFIG = 2,
	SC_DUMP=3
};
class gr_options_format_t : public gs::reg_utils::option_list_t
{
public:
	gr_options_format_t()
	{
		error_value = 0;
		_( "XML", XML);
		_( "DR_CONFIG", DR_CONFIG);
		_( "SC_DUMP", SC_DUMP);
	}
};
static gr_options_format_t gr_options_format;

////////////////////////////////////////////
/// gr_options_time_mode
/// various timing modes
/// @author 
/// @since 
////////////////////////////////////////////
enum gr_time_mode_e
{
	TIME_ERROR = 0,
	UNTIMED,
	TIMED, // not available in pv
	SIM_TIMED
};

class gr_options_time_mode_t : public gs::reg_utils::option_list_t
{
public:
	gr_options_time_mode_t()
	{
		error_value = 0;
		_( "TIME_ERROR", TIME_ERROR);
		_( "UNTIMED", UNTIMED);
		_( "TIMED", TIMED);
		_( "SIM_TIMED", SIM_TIMED);
	}
};
static gr_options_time_mode_t gr_options_time_mode;

////////////////////////////////////////////
/// gr_options_bus_mode
/// runtime switchable bus modes
/// @author 
/// @since 
////////////////////////////////////////////
enum gr_bus_mode_e
{
	IMPLICIT = 1,
	EXPLICIT,
	PV,
	BACKDOOR 
};
class gr_options_bus_mode_t : public gs::reg_utils::option_list_t
{
public:
	gr_options_bus_mode_t()
	{
		error_value = 0;
		_( "IMPLICIT", IMPLICIT);
		_( "EXPLICIT", EXPLICIT);
		_( "PV", PV);
		_( "BACKDOOR", BACKDOOR);
	}
};
static gr_options_bus_mode_t gr_options_bus_mode;

////////////////////////////////////////////
/// gr_defined_switches_e
/// defines known static switches
/// @author 
/// @since 
////////////////////////////////////////////
enum gr_defined_switches_e
{
	INVALID_SWITCH = 0,
	BOOL_TOGGLE,
	BUS_MODE,
	TIME_MODE
};

extern gs::reg_utils::lookup_table< int, gs::reg_utils::option_list_t *> g_defined_switches;

} // end namespace gs:reg
} // end namespace gs:reg

#endif // _GR_COMMON_H_

#endif // EXCLUDE_FROM_BUILD 
#undef EXCLUDE_FROM_BUILD
