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


#include "greenreg/framework/core/gr_dumper.h"

#include <stdio.h>
#include <fstream>
#include <sstream>

#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/fstream.hpp"

#include "greenreg/sysc_ext/utils/gr_report.h"
#include "greenreg/framework/devices/gr_device_container.h"

using namespace boost::filesystem;

namespace gs {
namespace reg {

gr_dumper::gr_dumper()
{
}

gr_dumper::~gr_dumper()
{
}


bool gr_dumper::dump( std::string _file_name, gr_dump_format_e _format, I_gr_dump & _gr_object)
{
		std::basic_ofstream<char> file( _file_name.c_str(), std::ios_base::out);
		bool retval = false;
		
		try {
			if( file.is_open() )
			{    
				GR_INFO("Writing Attributes...");        
				_gr_object.gr_dump( _format, file, 0);
				retval = true;
				file.close();
			} else {
				std::stringstream ss;
				ss << "File " << _file_name << " not found.\n";
				GR_FORCE_WARNING( ss.str().c_str());
			}
		} catch( ... ) {
			std::stringstream ss;
			ss << "File " << _file_name << " is missing or has invalid structure.\n";
			GR_FORCE_WARNING( ss.str().c_str());
		}
		
		return( retval);
}

bool gr_dumper::dump_platform( std::string _path, gr_dump_format_e _format)
{
	bool retval = false;
	unsigned int i = 0;
	
	boost::filesystem::path _boost_path( _path.c_str());
	boost::filesystem::create_directory( _boost_path);
	
	std::vector< unsigned int> ids = g_gr_device_container.get_all_data_ids();

	std::stringstream ss;
	ss << _path << "/platform.txt";
	
	std::basic_ofstream<char> file( ss.str().c_str(), std::ios_base::out);
	
	try {
		if( file.is_open() )
		{    
			file << "##########\n"
				<< "# This file lists configuration files for the entire platform.\n"
				<< "# Each file represents a single device in the platform.\n"
				<< "# This allows a flexible, auto-generated system, that is easy to maintain.\n"
				<< "##########\n";
			
			for( i = 0; i < ids.size(); i++)
			{
				ss.str("");
				ss << g_gr_device_container.get_data_key( ids[i]) << " = "
					<< g_gr_device_container.get_data_key( ids[i]) << ".txt\n";
				file << ss.str().c_str();
			}
			file << "\n";
			file.close();
			
		} else {
			std::stringstream ss;
			ss << "File not found.\n";
			GR_FORCE_WARNING( ss.str().c_str());
		}
	} catch( ... ) {
		std::stringstream ss;
		ss << "File is missing or has invalid structure.\n";
		GR_FORCE_WARNING( ss.str().c_str());
	}

	for( i = 0; i < ids.size(); i++)
	{
		ss.str("");
		ss << _path << "/" << g_gr_device_container.get_data_key( ids[i]) << ".txt";
		std::basic_ofstream<char> config_file( ss.str().c_str(), std::ios_base::out);
		try {
			if( config_file.is_open() )
			{    
				static_cast< gs::reg::device *>(g_gr_device_container.get_data( ids[i]))->gr_dump( _format, config_file, 0);
				retval = true;
				config_file.close();
			} else {
				std::stringstream ss;
				ss << "File not found.\n";
				GR_FORCE_WARNING( ss.str().c_str());
			}
		} catch( ... ) {
			std::stringstream ss;
			ss << "File is missing or has invalid structure.\n";
			GR_FORCE_WARNING( ss.str().c_str());
		}
	}

	return( retval);
}

} // end namespace gs:reg
} // end namespace gs:reg

