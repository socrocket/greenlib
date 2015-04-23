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


#ifndef _GR_FILE_IN_H_
#define _GR_FILE_IN_H_

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
/// Component which writes file intput to a
/// fifo, and is useful in debugging.
/// @author 
/// @since 
////////////////////////////////////////////
class gr_file_in : public gr_component< gr_file_in>
{
public:

	////////////////////////////////////////////
	/// Constructor
	///
	/// @param _name ? unique sc_core::sc_module_name for this component
	/// @param _component_container ? container the component will register with
	/// @param _file_name ? filename the input will be read to
	/// @param _continuous ? continuous file input or not
	///
	/// @see sc_core::sc_module_name, gr_component_container
	////////////////////////////////////////////
	gr_file_in( sc_core::sc_module_name _name, gr_component_container & _component_container, std::string _file_name, bool _continious)
	: gr_component< gr_file_in>( _name, _component_container),
	m_file_name( "file_name", "file input", a, _file_name),
	m_continious( _continious)
	{
		m_file_handle = fopen( _file_name.c_str(), "rb");
		if( m_file_handle == NULL)
		{
			std::stringstream ss;
			ss << "gr_file_in : " << (const char *)(_name) << " failed to open " << m_file_name.get_value().c_str() << ". \nExiting";
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
		SC_METHOD( read_data_process);
		sensitive << run_event << m_fifo_out.data_read_event();
		dont_initialize();

		SC_METHOD( file_name_changed_process);
		sensitive << m_file_name.default_event();
		dont_initialize();
	}

	////////////////////////////////////////////
	/// Destructor
	////////////////////////////////////////////
	virtual ~gr_file_in()
	{
		try{
			if( m_file_handle != NULL)
				fclose( m_file_handle);
		} catch( ...)
		{
			GR_INFO("gr_file_in failure on file handle close");
		}
	}

	/// @see gr_module, sc_core::sc_module
	virtual const char* kind() const
	{ return( "gr_file_in"); }

    ////////////////////////////////////////////
    /// read_data_process
    /// read data from the file when a pull comes from
    /// from the fifo.
    ////////////////////////////////////////////
    void read_data_process()
    {
        unsigned int val;
        GR_INFO("reading data");
        if(feof(m_file_handle))
        {
            if(m_continious)
            {
                fclose(m_file_handle);
                m_file_handle = fopen(m_file_name.get_value().c_str(), "rb");
                if(m_file_handle == NULL)
                {
                    std::stringstream ss;
                    ss << "gr_file_in : " << basename() << " failed to open "
                       << m_file_name.get_value().c_str() << ". \nExiting";
                    GR_ERROR(ss.str().c_str());
                }
            }
            else
            {
                fclose(m_file_handle);
                m_file_handle = NULL;
            }
        }
        else
        {
            // fill up the fifo
            while(!feof(m_file_handle) && m_fifo_out.num_free() > 0)
            {
                size_t sz;
                sz = fread(&val, 4, 1, m_file_handle);
                if (sz != 4)
                {
                    std::stringstream ss;
                    ss << "gr_file_in : " << basename() << " cannot read "
                       << m_file_name.get_value().c_str() << ". \nExiting";
                    GR_ERROR(ss.str().c_str());
                }
                m_fifo_out.nb_write(val);
            }
        }
        next_trigger(m_fifo_out.data_read_event());
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
		m_file_handle = fopen( m_file_name.get_value().c_str(), "rb");

		if( m_file_handle == NULL)
		{
			std::stringstream ss;
			ss<< "gr_file_in : " << basename() << " failed to open " << m_file_name.get_value().c_str() << ". \nExiting";
			GR_ERROR( ss.str().c_str());
		}
		next_trigger();
	}

	/// output fifo that is filled with data read from file
  sc_core::sc_fifo_out< unsigned int> m_fifo_out;

	/// an event that can be fired by the user to kickoff the read process
	sc_core::sc_event run_event;

	/// gr_attribute which will cause stimulus to exectue file_name_changed_proccess when changed
	gr_attribute< std::string> m_file_name;

protected:

	/// file handle this module will read from
	FILE * m_file_handle;

	/// boolean flag set at initialization to determine if this is a continuous feed or not.
	bool m_continious;
};

} // end namespace gs:reg
} // end namespace gs:reg

#endif /*_GR_FILE_IN_H_*/
