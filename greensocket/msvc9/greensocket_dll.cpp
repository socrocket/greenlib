// LICENSETEXT
//
//   Copyright (C) 2007 : GreenSocs Ltd
// 	 http://www.greensocs.com/ , email: info@greensocs.com
//
//   Developed by :
//   
//   Marcus Bartholomeu <bartho@greensocs.com>
//     GreenSocs
//     http://www.greensocs.com
//
//
// The contents of this file are subject to the licensing terms specified
// in the file LICENSE. Please consult this file for restrictions and
// limitations that may apply.
// 
// ENDLICENSETEXT

#include "greensocket/generic/gs_extension.h"

#include "greensocket/initiator/single_socket.h"
#include "greensocket/target/single_socket.h"
#include "greensocket/bidirectional/single_socket.h"

#include "greensocket/initiator/multi_socket.h"
#include "greensocket/target/multi_socket.h"
#include "greensocket/bidirectional/multi_socket.h"

#include "greensocket/examples/common/pseudo_extension_lib.h"

#ifdef SOCD
// for SoC Designer probes
#include "greensocket/monitor/green_socket_monitor.h"
#endif
