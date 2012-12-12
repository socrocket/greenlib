// LICENSETEXT
// 
//   Copyright (C) 2008 : GreenSocs Ltd
//       http://www.greensocs.com/ , email: info@greensocs.com
// 
//   Developed by :
// 
//   Christian Schröder, Robert Guenzel
//     Technical University of Braunschweig, Dept. E.I.S.
//     http://www.eis.cs.tu-bs.de
// 
// 
// The contents of this file are subject to the licensing terms specified
// in the file LICENSE. Please consult this file for restrictions and
// limitations that may apply.
// 
// ENDLICENSETEXT
 

template <unsigned int BUSWIDTH, typename TRANSACTION, typename CONFIG, typename PHASE>
inline void GSGPBidirectionalSocket<BUSWIDTH, TRANSACTION, CONFIG, PHASE>::
start_of_simulation() {
  master_socket_type::start_of_simulation();
  slave_socket_type::start_of_simulation();
}
