//   GreenReg framework
//
// LICENSETEXT
//
//   Copyright (C) 2009 : GreenSocs Ltd
// 	 http://www.greensocs.com/ , email: info@greensocs.com
//
//   Developed by :
//   
//   Christian Schroeder <schroeder@eis.cs.tu-bs.de>,
//     Technical University of Braunschweig, Dept. E.I.S.
//     http://www.eis.cs.tu-bs.de
//
//
//   This program is free software.
// 
//   If you have no applicable agreement with GreenSocs Ltd, this software
//   is licensed to you, and you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
// 
//   If you have a applicable agreement with GreenSocs Ltd, the terms of that
//   agreement prevail.
// 
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
// 
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
//   02110-1301  USA 
// 
// ENDLICENSETEXT

//
// Simple GreenReg example 
//  showing GR_FUNCTION, GR_FUNCTION_PARAMS,
//  GR_SENSITIVE, GR_DELAYED_SENSITIVE
//
// This is more a code example than a runnable example that
// does meaningful things.
//


#include <boost/config.hpp>
#include <systemc>
#include <greenreg/greenreg.h>
#include <greenreg/greenreg_socket.h>

#include "greencontrol/all.h"

#define SHOW(name, msg) \
{ std::printf("@%s /%d (%s): ", sc_core::sc_time_stamp().to_string().c_str(), (unsigned)sc_core::sc_delta_count(), name); std::cout << msg << std::endl; }



/// Device sending register accesses over the bus
class MasterDevice 
: public gs::reg::gr_device 
{
public:
  
  gs::reg::greenreg_socket< gs::gp::generic_master> m_master_socket;	// TLM Master Port declaration
  
  
  
	SC_HAS_PROCESS( MasterDevice );
	
  MasterDevice(sc_core::sc_module_name name) 
  : gr_device(name, gs::reg::INDEXED_ADDRESS, 2, NULL)
  , m_master_socket( "master_socket" )							// TLM bus master socket
    {
    SC_THREAD( send_thread );
  }
  
  void send_thread() {
    std::cout << std::endl;
    SHOW(name(), "Start sending bus register accesses");
    
    unsigned int cw;
    
    std::cout << "------------------------------------------------------------------------------------------" << std::endl;
    // Bus access Reg1
    
    wait(5, sc_core::SC_NS);

    // @5ns
    cw = 0xFF;
    SHOW(name(), "---- SC_THREAD Bus Write 0x"<<std::hex<<cw<<std::dec<<" to reg [0x01]");
    m_master_socket->write( 0x01, cw, 4 );						// write 4 bytes

    wait(5, sc_core::SC_NS);
    
    // @10ns
    std::cout << std::endl << std::endl;
    cw = 0xFFAD00BB;
    SHOW(name(), "---- SC_THREAD Bus Write 0x"<<std::hex<<cw<<std::dec<<" to reg [0x01]");
    m_master_socket->write( 0x01, cw, 4 );						// write 4 bytes
    
    wait(5, sc_core::SC_NS);

    // @15ns
    std::cout << std::endl << std::endl;
    cw = 0x0D;
    SHOW(name(), "---- SC_THREAD Bus Write 0x"<<std::hex<<cw<<std::dec<<" to reg [0x01]");
    m_master_socket->write( 0x01, cw, 4 );						// write 4 bytes
    std::cout << std::endl << std::endl;
    
    wait(5, sc_core::SC_NS);
    
    // @20ns
    std::cout << std::endl << std::endl;
    cw = 0x000000;
    SHOW(name(), "---- SC_THREAD Bus Write 0x"<<std::hex<<cw<<std::dec<<" to reg [0x01]");
    m_master_socket->write( 0x01, cw, 4 );						// write 4 bytes
    std::cout << std::endl << std::endl;
    
    wait(5, sc_core::SC_NS);
    
    std::cout << "------------------------------------------------------------------------------------------" << std::endl;
  }
  
};

