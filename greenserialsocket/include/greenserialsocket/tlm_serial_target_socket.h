/*
 * tlm_serial_target_socket.h
 *
 * Copyright (C) 2014, GreenSocs Ltd.
 *
 * Developped by Manish Aggarwal
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses>.
 *
 * Linking GreenSocs code, statically or dynamically with other modules
 * is making a combined work based on GreenSocs code. Thus, the terms and
 * conditions of the GNU General Public License cover the whole
 * combination.
 *
 * In addition, as a special exception, the copyright holders, GreenSocs
 * Ltd, give you permission to combine GreenSocs code with free software
 * programs or libraries that are released under the GNU LGPL, under the
 * OSCI license, under the OCP TLM Kit Research License Agreement or
 * under the OVP evaluation license.You may copy and distribute such a
 * system following the terms of the GNU GPL and the licenses of the
 * other code concerned.
 *
 * Note that people who make modified versions of GreenSocs code are not
 * obligated to grant this special exception for their modified versions;
 * it is their choice whether to do so. The GNU General Public License
 * gives permission to release a modified version without this exception;
 * this exception also makes it possible to release a modified version
 * which carries forward this exception.
 *
 */

#ifndef __TLM_SERIAL_TARGET_SOCKET_H__
#define __TLM_SERIAL_TARGET_SOCKET_H__

#include "greensocket/target/green_target_socket_base.h"
#include "tlm_serial_protocol.h"
#include "tlm_serial_payload.h"

namespace tlm_serial
{

template<unsigned int NUMBINDS=1,
	typename BASE_SOCKET=gs::socket::target_socket_base<32,tlm_serial_protocol_types,
        NUMBINDS,gs::socket::bind_checker<tlm_serial_protocol_types> > >
class tlm_serial_target_socket:public BASE_SOCKET
{
public:
	typedef BASE_SOCKET base_type;
	typedef tlm_serial::tlm_serial_protocol_types traits_type;

	tlm_serial_target_socket(const char* name):gs::socket::target_socket_base<32,tlm_serial_protocol_types, NUMBINDS,
                                                     gs::socket::bind_checker<tlm_serial_protocol_types> >(name)
        {
          configure_socket();
        }

	~tlm_serial_target_socket()
        { }

protected:
	void configure_socket()
        {
            gs::socket::config<traits_type> cfg;
            cfg.treat_unknown_as_rejected();
            //the base protocol phases are always mandatory
            cfg.use_mandatory_phase(tlm_serial::BEG_TRANSMISSION);
            BASE_SOCKET::set_config(cfg);
        }
};

}

#endif /* __TLM_SERIAL_INITIATOR_SOCKET_H__ */
