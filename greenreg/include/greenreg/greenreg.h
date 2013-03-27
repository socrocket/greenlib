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


#ifndef _GREENREG_H_
#define _GREENREG_H_

// global
#include "greenreg/gr_externs.h" ///! potential to be depricated

// sysc_ext / utils
#include "greenreg/sysc_ext/utils/gr_report.h" ///! replacements for SC_REPORT* with #IFDEF DEBUG wrappers built in

// framework / core
#include "greenreg/framework/core/gr_common.h" ///! some common structs and data shared throughout DRF
#include "greenreg/framework/core/I_gr_dump.h" ///! greenreg object dump interface
#include "greenreg/framework/core/gr_object.h" ///! base object type for greenreg, really just forces the use of some interfaces
#include "greenreg/framework/core/gr_dumper.h" ///! capabilities to dump out he platform or anything that implements I_gr_dump

// framework / core / analysis
#include "greenreg/framework/core/analysis/gr_analysis_t.h" ///! base analysis types
#include "greenreg/framework/core/analysis/gr_analyzer_composite_base.h" ///! potential to be depricated
#include "greenreg/framework/core/analysis/gr_analyzer_composite.h" ///! hierarchical exoskeleton implemented
#include "greenreg/framework/core/analysis/gr_analyzer.h" ///! the only class the user should interface with, analysis API
#include "greenreg/framework/core/analysis/I_gr_analyzer_user_callback.h" ///! interface for user callback definintion
#include "greenreg/framework/core/analysis/I_gr_analyzer.h" ///! shared interface for gr_analyzer (user API) and gr_analyzer_composite (exoskeleton)

// framework / devices
#include "greenreg/framework/devices/gr_device_container.h" ///! container for all top level gr_devices
#include "greenreg/framework/devices/gr_device.h" ///! definition of top level gr_device lots of cool stuff behind the scenes here, this would be your main sc_core::sc_module for your device or "unit"
#include "greenreg/framework/devices/gr_subdevice.h" ///! subdevice which can be the child of either a device or subdevice (refer to greenreg/internal headers and source files)

// framework / registers
#include "greenreg/internal/registers/register/I_register_container_bus_access.h" ///! bus access interface for register container (block of registers)
#include "greenreg/framework/registers/gr_register_container.h" ///! container for registers, also owns the register block
#include "greenreg/internal/registers/register/I_register.h" ///! top level register definition, this is what most user will interact with
#include "greenreg/internal/registers/register_data/I_register_data.h" ///! the data interface is the underlying storage mechanism for the register
#include "greenreg/framework/registers/gr_register_sharedio.h" ///! this is a sharedio register (1 register_data)
#include "greenreg/framework/registers/gr_register_splitio.h" ///! this is a split register (1 register_data)
#include "greenreg/internal/registers/bit_range/bit_range.h" ///! This bit range is really an overlay class to the data, thus it does not store any real data like other implementations
#include "greenreg/internal/registers/bit/bit.h" ///! This represents a bit
#include "greenreg/internal/registers/bit/bit_data.h" ///! The bit data is again separate from the bit to enable shared vs split setup
#include "greenreg/internal/registers/bit/bit_accessor.h" ///! The bit accessor does not declare bits, but will dynamically generate them when needed.

// sysc_ext / kernel
#include "greenreg/sysc_ext/kernel/gr_attribute_base.h" ///! base attribute type (it actually knows it's type!!)
#include "greenreg/sysc_ext/kernel/gr_attribute_type_container.h" ///! attribute type contianer (MANY nice accessability features)
#include "greenreg/sysc_ext/kernel/gr_attribute.h" ///! templated attribute CONTAINS and sc_attribute, but adds change event and accessability enhancements
#include "greenreg/sysc_ext/kernel/gr_switch_container.h" ///! container for switches
#include "greenreg/sysc_ext/kernel/gr_switch.h" ///! specialization of gr_attribute that enables switch positions with an event for each position
#include "greenreg/sysc_ext/kernel/I_gr_component.h" ///! the basic interface definition for greenreg components
#include "greenreg/sysc_ext/kernel/gr_component_container.h" ///! a container for greenreg components
#include "greenreg/sysc_ext/kernel/gr_component.h" ///! a component block that is not a module (such as a specialized fifo)
#include "greenreg/sysc_ext/kernel/gr_notification_rule_container.h" ///! container for notification rules
#include "greenreg/sysc_ext/kernel/I_notification_rule.h" ///! interface definition for notification rules, inherit from this to add new rule types.
#include "greenreg/sysc_ext/kernel/gr_notification_rules.h" ///! various implementations of notification rules

// sysc_ext / communication
#include "greenreg/sysc_ext/communication/gr_file_in.h" ///! a file input fifo
#include "greenreg/sysc_ext/communication/gr_file_out.h" ///! a file output fifo

// internal / device

// state_machine

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// GREENREG_UTILS needed for DRF platform API
#include "greenreg/utils/storage/properties_container.h"

//#include "commandline_args.h"

namespace gs {
namespace reg {

extern bool g_initialized;

extern gs::reg_utils::properties_container_t g_platform_properties;

class greenreg;

extern greenreg * g_greenreg;

////////////////////////////////////////////
/// greenreg
/// greenreg framework initialization class
/// the feature scope for this class is HUGE
/// current implementation is a fraction of a
/// fraction of the automation capability
/// intended..
/// @author 
/// @since 
////////////////////////////////////////////
class greenreg : public sc_core::sc_module
{
public:
	SC_HAS_PROCESS(greenreg);

	////////////////////////////////////////////
	/// greenreg
	/// constructor
	///
	/// @param _name ? name of greenreg module 
	/// @param _platform_config_path ? PATH not a file, for the configuration files
	/// @param _format ? format the configuration is stored in (currently forced to gs::reg::DR_CONFIG)
	////////////////////////////////////////////
	greenreg( sc_core::sc_module_name _name, std::string _platform_config_path = "", gr_dump_format_e _format = gs::reg::DR_CONFIG);
	virtual ~greenreg();

	/// @see sc_core::sc_module
	virtual void before_end_of_elaboration();

	/// @see sc_core::sc_module
	virtual void end_of_elaboration();

	/// @see sc_core::sc_module
	virtual void start_of_simulation();

	/// @see sc_core::sc_module
	virtual void end_of_simulation();

	/// initialize DRF parameters
	virtual void initialize( int argc, char ** argv);

	virtual void configure_platform( std::string _path, gr_dump_format_e _format);

protected:
	// These methods basically load the platform configuration up
	// and then initialize the platform once start_of_simulation executs
	virtual void load_platform_configuration( std::string _path, gs::reg::gr_dump_format_e _format);
	virtual void recursive_set_config( gs::reg_utils::properties_container_t * _p, gs::reg::I_device * _d);
	virtual void process_configuration();

	void show_args_help();
	void dump_platform( std::string _path);

protected:

	/// internally held configuration path
	std::string m_platform_config_path;

	/// format for gr_dump
	gr_dump_format_e m_format;

	/// holds command line arguments for startup and configuration
//	gs::reg_utils::commandline_args m_args;

};

} // end namespace gs:reg
} // end namespace gs:reg

/*
// undefine gr_report macros
#ifdef GR_GREENREG_SC_REPORT_PREFIX
# undef GR_GREENREG_SC_REPORT_PREFIX
#endif
#ifdef GR_REPORT_ERROR
# undef GR_REPORT_ERROR
#endif
#ifdef GR_ERROR
# undef GR_ERROR
#endif
#ifdef GR_REPORT_FATAL
# undef GR_REPORT_FATAL
#endif
#ifdef GR_FATAL
# undef GR_FATAL
#endif
#ifdef GR_REPORT_FORCE_WARNING
# undef GR_REPORT_FORCE_WARNING
#endif
#ifdef GR_FORCE_WARNING
# undef GR_FORCE_WARNING
#endif
#ifdef GR_REPORT_INFO
# undef GR_REPORT_INFO
#endif
#ifdef GR_INFO
# undef GR_INFO
#endif
#ifdef GR_REPORT_WARNING
# undef GR_REPORT_WARNING
#endif
#ifdef GR_WARNING
# undef GR_WARNING
#endif
*/

#endif // _GREENREG_H_
