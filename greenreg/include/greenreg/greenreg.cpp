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


#include "greenreg/greenreg.h"

#include <vector>
#include <string>
#include <sstream>

#include "greenreg/utils/greenreg_utils.h"

#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/fstream.hpp"

//#include "properties_file_format.h"

using namespace boost::filesystem;

bool gs::reg::g_initialized = false;

gs::reg_utils::properties_container_t gs::reg::g_platform_properties( true);

gs::reg::greenreg * gs::reg::g_greenreg = NULL;

gs::reg::greenreg::greenreg( sc_core::sc_module_name _name, std::string _platform_config_path, gr_dump_format_e _format)
: sc_core::sc_module( _name),
m_platform_config_path( _platform_config_path),
m_format( _format)
{
	g_greenreg = this;
}

gs::reg::greenreg::~greenreg()
{
}

void gs::reg::greenreg::before_end_of_elaboration()
{
}

void gs::reg::greenreg::end_of_elaboration()
{
}

void gs::reg::greenreg::start_of_simulation()
{
/*	
	if( m_args.args.lookup_key( "greenreg_dump_platform") != m_args.args.get_error_value())
	{
		gr_dumper::dump_platform( m_args.args.lookup_key( "greenreg_dump_platform"), gs::reg::DR_CONFIG);
		sc_stop();
	} else if( m_args.args.lookup_key( "greenreg_configure_platform") != m_args.args.get_error_value())
	{
		configure_platform( m_args.args.lookup_key( "greenreg_configure_platform"), gs::reg::DR_CONFIG);

		if( m_args.args.lookup_key( "greenreg_debug") == "true")
		{
			gr_dumper::dump_platform( "greenreg_debug_platform_dump", gs::reg::DR_CONFIG);
		}
	}
//	if( m_platform_config_path != "")
//		configure_platform( m_platform_config_path, m_format);

// */// -  the world famous multi-comment...
	
}

void gs::reg::greenreg::end_of_simulation()
{
}

void gs::reg::greenreg::initialize( int argc, char ** argv)
{
	if( !g_initialized) {
		g_defined_switches.define_error_key( INVALID_SWITCH);
		g_defined_switches.define_error_value( NULL);
		g_defined_switches.add_entry( BOOL_TOGGLE, &gr_options_bool_toggle);
		g_defined_switches.add_entry( BUS_MODE, &gr_options_bus_mode);
		g_defined_switches.add_entry( TIME_MODE, &gr_options_time_mode);
	}

/*	
	m_args.parse_args( argc, argv);

	if( m_args.args.lookup_key( "greenreg_help") == "true")
	{
		show_args_help();
		exit(0);
	}
//*/
}

void gs::reg::greenreg::load_platform_configuration( std::string _path, gs::reg::gr_dump_format_e _format)
{
/*	
	gs::reg_utils::properties_container_t * module_files = new gs::reg_utils::properties_container_t( true);
	gs::reg_utils::properties_file_format platform_file;
	std::stringstream ss;
	ss << _path << "/platform.txt";

	platform_file.load_from_file( ss.str().c_str(), *module_files);

	std::vector< unsigned int>::iterator vit;
	std::vector< unsigned int> ids = module_files->get_all_data_ids();

	for( vit = ids.begin(); vit != ids.end(); vit++)
	{
		ss.str("");
		ss << _path << "/" << module_files->get_data(*vit)->c_str();
		platform_file.load_from_file( ss.str().c_str(), gs::reg::g_platform_properties);
	}

	delete( module_files);
//*/
}

void gs::reg::greenreg::recursive_set_config( gs::reg_utils::properties_container_t * _p, gs::reg::I_device * _d)
{
/*	
	if( _p != NULL && _d != NULL)
	{
		std::string reg_cont_name = "";
		gr_register_container * reg_container;
		std::vector< std::string>::iterator dev_it;
		std::vector< std::string> dev_names = _p->get_container_names();
		std::vector< unsigned int>::iterator prop_it;
		std::vector< unsigned int> prop_names = _p->get_all_data_ids();

		// first set the properties
		for( prop_it = prop_names.begin(); prop_it != prop_names.end(); prop_it++)
		{
			std::string attrib_name = _p->get_data_key( *prop_it);
			gs::reg::gr_attrib_base * attrib = _d->a.get_data( attrib_name);

			if( attrib != NULL)
			{
				attrib->parse_config( *(_p->get_data( *prop_it)));
			} else if( gs::reg_utils::string::does_pattern_match( attrib_name, "rc[*]", "*")) {
				attrib_name = gs::reg_utils::string::trim_leading( attrib_name, "r");
				attrib_name = gs::reg_utils::string::trim_leading( attrib_name, "c");
				attrib_name = gs::reg_utils::string::trim_all( attrib_name, "[]");
				reg_cont_name = attrib_name;
			} else if( gs::reg_utils::string::does_pattern_match( attrib_name, "r[*]", "*")) {
				attrib_name = gs::reg_utils::string::trim_leading( attrib_name, "r");
				attrib_name = gs::reg_utils::string::trim_all( attrib_name, "[]");
				reg_container = &(_d->get_device_register_container_reference_container()[reg_cont_name]);
				if( reg_container != NULL)
				{
					(*reg_container)[ gs::reg_utils::string::to_hex_value(attrib_name)].parse_config( *(_p->get_data( *prop_it)));
				} else {
					std::stringstream ss;
					ss << "r[" << attrib_name << "] does not belong to a container, please check with model owner.";
					GR_FORCE_WARNING( ss.str().c_str());
				}
			} else if( gs::reg_utils::string::does_pattern_match( attrib_name, "r[*].i", "*")) {
				attrib_name = gs::reg_utils::string::trim_leading( attrib_name, "r");
				attrib_name = gs::reg_utils::string::trim_trailing( attrib_name, "i");
				attrib_name = gs::reg_utils::string::trim_trailing( attrib_name, ".");
				attrib_name = gs::reg_utils::string::trim_all( attrib_name, "[]");
				reg_container = &(_d->get_device_register_container_reference_container()[reg_cont_name]);
				if( reg_container != NULL)
				{
					(*reg_container)[ gs::reg_utils::string::to_hex_value(attrib_name)].i.parse_config( *(_p->get_data( *prop_it)));
				} else {
					std::stringstream ss;
					ss << "r[" << attrib_name << "] does not belong to a container, please check with model owner.";
					GR_FORCE_WARNING( ss.str().c_str());
				}
			} else if( gs::reg_utils::string::does_pattern_match( attrib_name, "r[*].o", "*")) {
				attrib_name = gs::reg_utils::string::trim_leading( attrib_name, "r");
				attrib_name = gs::reg_utils::string::trim_trailing( attrib_name, "o");
				attrib_name = gs::reg_utils::string::trim_trailing( attrib_name, ".");
				attrib_name = gs::reg_utils::string::trim_all( attrib_name, "[]");
				reg_container = &(_d->get_device_register_container_reference_container()[reg_cont_name]);
				if( reg_container != NULL)
				{
					(*reg_container)[ gs::reg_utils::string::to_hex_value(attrib_name)].o.parse_config( *(_p->get_data( *prop_it)));
				} else {
					std::stringstream ss;
					ss << "r[" << attrib_name << "] does not belong to a container, please check with model owner.";
					GR_FORCE_WARNING( ss.str().c_str());
				}
			} else {
				std::stringstream ss;
				ss << attrib_name << " is invalid.";
				GR_FORCE_WARNING( ss.str().c_str());
			}
		}

		gs::reg_utils::properties_container_t * p = NULL;
		gs::reg::I_device * d = NULL;
		gs::reg::device * dev = NULL;
		// then descend
		for( dev_it = dev_names.begin(); dev_it != dev_names.end(); dev_it++)
		{
			p = _p->get_container( (*dev_it));

			// if the current I_device is a device
			dev = dynamic_cast< device *> (_d);
			if( dev != NULL)
			{
				// try to obtain a device by the specified name
				d = dev->d.get_data( (*dev_it));
			}

			// if the current I_device is not a device, or does not have a device by specified name
			if( d == NULL)
			{
				// try to obtain a subdevice by the specified name
				d = _d->sd.get_data( (*dev_it));
			}

			recursive_set_config( p, d);
		}
	}
*/
}

void gs::reg::greenreg::process_configuration()
{
	gs::reg_utils::properties_container_t * p = NULL;
	gs::reg::device * d = NULL;

	std::vector< std::string>::iterator dev_it;
	std::vector< std::string> dev_names = gs::reg::g_gr_device_container.get_all_data_keys();

	std::vector< unsigned int>::iterator prop_it;
	std::vector< unsigned int> prop_names = gs::reg::g_platform_properties.get_all_data_ids();

	if( prop_names.size() > 0)
	{
		// global properties, none for now
	}

	for( dev_it = dev_names.begin(); dev_it != dev_names.end(); dev_it++)
	{
		std::string name = *dev_it;
		p = gs::reg::g_platform_properties.get_container( name);
		d = gs::reg::g_gr_device_container.get_data( name);
		recursive_set_config( p, d);
	}
}

void gs::reg::greenreg::configure_platform( std::string _path, gs::reg::gr_dump_format_e _format)
{
	if( boost::filesystem::exists( _path))
	{
		load_platform_configuration( _path, _format);
		process_configuration();
	} else {
		std::stringstream ss;
		ss << _path << " is invalid or does not exist, please check your launch params!";
		GR_FORCE_WARNING( ss.str().c_str());
		sc_core::sc_stop();
	}
}

void gs::reg::greenreg::show_args_help()
{
	std::stringstream ss;
	ss << "Device & Register Framework Command Line Help\n"
	   << "---------------------------------------------\n\n"
	   << "-greenreg_help\n"
	   << "\tlist command line help for DRF\n"
	   << "-greenreg_dump_platform=path\n"
	   << "\tdumps all platform configuration files to path\n"
	   << "-greenreg_configure_platform=path\n"
	   << "\toverrides default platform values with config files\n"
	   << "-greenreg_debug\n"
	   << "\tenable greenreg debug output (debug.txt) and capabilities\n"
	   << "\n";
	GR_INFO( ss.str().c_str());
}

void gs::reg::greenreg::dump_platform( std::string _path)
{
	if( boost::filesystem::exists( _path))
	{
		gs::reg::gr_dumper::dump_platform( _path, gs::reg::DR_CONFIG);
	} else {
		std::stringstream ss;
		ss << _path << " is invalid or does not exist, please check your launch params!";
		GR_FORCE_WARNING( ss.str().c_str());
	}
	sc_core::sc_stop();
}
