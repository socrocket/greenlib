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





/// Device owning registers being accessed by the Master device over the bus 
class ReceiverSlaveDevice 
: public gs::reg::gr_device 
{
public:
  
  gs::reg::greenreg_socket< gs::gp::generic_slave> m_slave_socket; // Slave socket with delayed switch
  GC_HAS_CALLBACKS();
  SC_HAS_PROCESS( ReceiverSlaveDevice );
	
  ReceiverSlaveDevice(sc_core::sc_module_name name) 
  : gr_device(name, gs::reg::INDEXED_ADDRESS, 2, NULL)
  , m_slave_socket( "slave_socket", r, 0x0, 0xFFFFFFFF)		// TLM Slave socket
  {
    SC_THREAD(delayed_switch_demo);
    
		// create register
    r.create_register( "Reg5", "Test Register5", /*offset*/0x01,
                      /*config*/gs::reg::STANDARD_REG | gs::reg::SINGLE_IO | gs::reg::SINGLE_BUFFER | gs::reg::FULL_WIDTH,
                      /*init value*/0xaaaaaaaa, /*write mask*/0xFFFFFFFF, /*reg width*/32, /*lock mask*/0x00 );
		std::cout << "created single IO register Reg5 = 0x" << std::hex << r[0x01] << std::dec<< std::endl<< std::endl;
    
    std::cout << std::endl;
  }
	~ReceiverSlaveDevice () {
    GC_UNREGISTER_CALLBACKS();
	}
  
	void end_of_elaboration() {
    std::cout << "Entered ReceiverSlaveDevice::end_of_elaboration" << std::endl;
    
    std::cout << "register not delayed GR_FUNCTION without params for post write" << std::endl;
    GR_FUNCTION(ReceiverSlaveDevice, show_notification_reg1_not_delayed_no_params);
    GR_SENSITIVE(r[0x01].add_rule( gs::reg::POST_WRITE, "post_write_1", gs::reg::NOTIFY));
    
    std::cout << "register not delayed GR_FUNCTION with params for post write" << std::endl;
    GR_FUNCTION_PARAMS(ReceiverSlaveDevice, show_notification_reg1_not_delayed_params);
    GR_SENSITIVE(r[0x01].add_rule( gs::reg::POST_WRITE, "post_write_3", gs::reg::NOTIFY));
    
    std::cout << "register delayed GR_FUNCTION without params for post write" << std::endl;
    GR_FUNCTION(ReceiverSlaveDevice, show_notification_reg1_delayed_no_params);
    GR_DELAYED_SENSITIVE(r[0x01].add_rule( gs::reg::POST_WRITE, "post_write_2", gs::reg::NOTIFY),
                         sc_core::sc_time(1,sc_core::SC_NS));
    
    std::cout << "register delayed GR_FUNCTION with params for post write" << std::endl;
    GR_FUNCTION_PARAMS(ReceiverSlaveDevice, show_notification_reg1_delayed_params);
    GR_DELAYED_SENSITIVE(r[0x01].add_rule( gs::reg::POST_WRITE, "post_write_4", gs::reg::NOTIFY),
                         sc_core::sc_time(2, sc_core::SC_NS));
    
    std::cout << std::endl;
	}
  
  // SC_THREAD which demonstrates how the delayed switch can be switched
  void delayed_switch_demo() {

    wait(11, sc_core::SC_NS);
    SHOW(name(), "************************************************* \n   disable delayed switch now")
    m_slave_socket.disable_delay();

    SHOW(name(), "************************************************* \n   enable delayed switch now")
    wait(10, sc_core::SC_NS);
    m_slave_socket.enable_delay();
  }
  
  void show_notification_reg1_not_delayed_no_params() {
    SHOW("cb_not_delayed", "****** got NOT DELAYED register notification (GR_FUNCTION callback) for Reg1 post write");
    std::cout << "              value = " << r[0x01] <<"=0x"<<std::hex << r[0x01] <<std::dec<< std::endl;
	}
	void show_notification_reg1_delayed_no_params() {
    SHOW("cb_delayed", "****** got DELAYED register notification (GR_FUNCTION callback) for Reg1 post write");
    std::cout << "              value = " << r[0x01] <<"=0x"<<std::hex << r[0x01] <<std::dec<< std::endl;
	}
	void show_notification_reg1_not_delayed_params(gs::reg::transaction_type* &tr, const sc_core::sc_time& delay) {
    SHOW("cb_not_delayedP", "****** got NOT DELAYED (transaction, delay) register notification (GR_FUNCTION callback) for Reg1 post write");
    SHOW("cb_not_delayedP", "       transaction ID = " << tr->getTransID());
    std::cout << "   value = " << r[0x01] <<"=0x"<<std::hex << r[0x01] <<std::dec<< std::endl;
	}
	void show_notification_reg1_delayed_params(gs::reg::transaction_type* &tr, const sc_core::sc_time& delay) {
    SHOW("cb_delayedP", "****** got DELAYED (transaction, delay) register notification (GR_FUNCTION callback) for Reg1 post write");
    SHOW("cb_delayedP", "       " << "delayed for "<< delay.to_string());
    SHOW("cb_delayedP", "       transaction ID = " << tr->getTransID());
    std::cout << "                      value = " << r[0x01] <<"=0x"<<std::hex << r[0x01] <<std::dec<< std::endl;
	}
  
  
  void reg2_split_synchronizer_write_GR() {
    std::cout << "sync usr reg2" << std::endl;
    r[0x02].i.set(r[0x02].o, false);
	}
  void reg2_split_synchronizer_bus_write_GR() {
    std::cout << "sync bus reg2" << std::endl;
    r[0x02].o.set(r[0x02].i, false);
	}
  
};

