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


#ifndef _GR_FILE_OUT_H_
#define _GR_FILE_OUT_H_

#include <stdio.h>
#include <fcntl.h>
#include <sstream>

#include <boost/config.hpp>
#include <systemc>

#include "greenreg/sysc_ext/kernel/gr_component.h"
#include "greenreg/sysc_ext/kernel/gr_attribute.h"

namespace gs {
namespace reg {

////////////////////////////////////////////
/// gr_file_out
/// Component which writes fifo output to a
/// file, and is useful in debugging.
/// @author 
/// @since 
////////////////////////////////////////////
class gr_file_out : public gr_component< gr_file_out>
{
public:

	////////////////////////////////////////////
	/// Constructor
	///
	/// @param _name ? unique sc_core::sc_module_name for this component
	/// @param _component_container ? container the component will register with
	/// @param _file_name ? filename the output will be written to
	/// @param _continuous ? continuous file output or not
	///
	/// @see sc_core::sc_module_name, gr_component_container
	////////////////////////////////////////////
	gr_file_out( sc_core::sc_module_name _name, gr_component_container & _component_container, std::string _file_name, bool _continious)
	: gr_component< gr_file_out>( _name, _component_container),
	m_file_name( "file_name", "file output", a, _file_name),
	m_continious( _continious)
	{
		m_file_handle = fopen( m_file_name.get_value().c_str(), "wb");
		if( m_file_handle == NULL)
		{
			std::stringstream ss;
			ss<< "gr_file_out : " << (const char *)( _name) << " failed to open " << m_file_name.get_value().c_str() << ". \nExiting";
			GR_ERROR( ss.str().c_str());
		} else {
			m_file_name.enable_event();
		}

	}

	////////////////////////////////////////////
	/// end_of_elaboration
	/// initialize processes
	////////////////////////////////////////////
	void end_of_elaboration()
	{
		SC_METHOD( write_data_process);
		sensitive << m_fifo_in.data_written_event();
		dont_initialize();

		SC_METHOD( file_name_changed_process);
		sensitive << m_file_name.default_event();
		dont_initialize();
	}

	////////////////////////////////////////////
	/// Destructor
	////////////////////////////////////////////
	virtual ~gr_file_out()
	{
		if( m_file_handle != NULL)
			fclose( m_file_handle);
	}

	/// @see gr_module, sc_core::sc_module
	virtual const char* kind() const
	{ return( "gr_file_out"); }

	////////////////////////////////////////////
	/// write_data_process
	/// writes data out to the file handle on notification
	/// from the fifo.
	////////////////////////////////////////////
	void write_data_process()
	{
		GR_INFO( "writing data");
		unsigned int val;

		// fill up the fifo
		while( m_fifo_in.num_available() > 0)
		{
			if( m_fifo_in.nb_read( val))
			{
				fwrite( &val, 4, 1, m_file_handle);
//				fflush( m_file_handle);
			}
		}
		next_trigger( m_fifo_in.data_written_event());

	}

	////////////////////////////////////////////
	/// file_name_changed_process
	/// Closes the old file handle and opens a new
	/// file handle with the new filename.
	////////////////////////////////////////////
	void file_name_changed_process()
	{
		GR_INFO( "file name changed");

		fclose( m_file_handle);
		m_file_handle = fopen( m_file_name.get_value().c_str(), "wb");

		if( m_file_handle == NULL)
		{
			std::stringstream ss;
			ss<< "gr_file_out : " << basename() << " failed to open " << m_file_name.get_value().c_str() << ". \nExiting";
			GR_ERROR( ss.str().c_str());
		}
		next_trigger();

	}

	/// input fifo that is filled with data
  sc_core::sc_fifo_in< unsigned int> m_fifo_in;

	/// gr_attribute which will cause stimulus to exectue file_name_changed_proccess when changed
	gr_attribute< std::string> m_file_name;

protected:

	/// file handle this module will write out too
	FILE * m_file_handle;

	/// boolean flag set at initialization to determine if this is a continuous feed or not.
	bool m_continious;
};

} // end namespace gs:reg
} // end namespace gs:reg

#endif /*_GR_FILE_OUT_H_*/
