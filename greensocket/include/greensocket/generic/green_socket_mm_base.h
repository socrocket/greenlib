//   GreenSocket
//
// LICENSETEXT
//
//   Copyright (C) 2007 : GreenSocs Ltd
// 	 http://www.greensocs.com/ , email: info@greensocs.com
//
//   Developed by :
//   
//   Robert Guenzel <guenzel@eis.cs.tu-bs.de>,
//     Technical University of Braunschweig, Dept. E.I.S.
//     http://www.eis.cs.tu-bs.de
//
//
// The contents of this file are subject to the licensing terms specified
// in the file LICENSE. Please consult this file for restrictions and
// limitations that may apply.
// 
// ENDLICENSETEXT


#ifndef __GREEN_SOCKET_MM_BASE_H__
#define __GREEN_SOCKET_MM_BASE_H__

#include "tlm.h"
#include <vector>

namespace gs{
namespace socket{

//an enum for our allocation schemes
enum allocation_scheme{
  GS_TXN_ONLY, //only allocate transaction
  GS_TXN_WITH_DATA,  //allocate transactions and enable data array allocation support
  GS_TXN_WITH_BE,  //allocate transactions and enable byte enable array allocation support
  GS_TXN_WITH_BE_AND_DATA  //allocate transactions and enable both byte enable and data array allocation support
};

//a container for two vectors. one for the data array, one for the byte enable array
struct per_txn_data_and_be_struct{
  std::vector<unsigned char> data;
  std::vector<unsigned char> be;
};

//this is not a functional extension, but a mere MM extension, hence we will not create it using a greensocket extension MACRO
struct per_txn_data_and_be
  : public tlm::tlm_extension<per_txn_data_and_be>
{
  void copy_from(tlm::tlm_extension_base const & ext){} //copy from, because we do not clone
  tlm::tlm_extension_base* clone() const {return NULL;} //no clone of per txn data and be
  per_txn_data_and_be_struct value;
};

//This function is a base class for the memory management features of the
// green_initiator socket.
template <typename TRAITS, typename MM_INTERFACE>
class mm_base:
    public MM_INTERFACE //this class MUST ONLY contain a function 'virtual void free(typename TRAITS::tlm_payload_type*)'
{
public:
  typedef typename TRAITS::tlm_payload_type  payload_type;
  typedef typename TRAITS::tlm_phase_type  phase_type;
  
  inline payload_type* fill_txn(payload_type*);  //the default transaction filler function (see gsock_tech.pdf)
  virtual void free(payload_type*)=0; //the memory management interface function (see gsock_tech.pdf)
  mm_base(allocation_scheme); //the ctor, taking the allocation scheme
  
  /*reserve the requested amount of data array bytes
    - works only if the allocation scheme is GS_TXN_WITH_DATA or GS_TXN_WITH_BE_AND_DATA
    - will automatically set the data_length to the allocated size. IF you intentionally over-allocate, set the data_length manually afterwards
  */
  void reserve_data_size(payload_type&, unsigned int);

  /*reserve the requested amount of byte enable array bytes
    - works only if the allocation scheme is GS_TXN_WITH_BE or GS_TXN_WITH_BE_AND_DATA
    - will automatically set the byte_enable_length to the allocated size. IF you intentionally over-allocate, set the byte_enable_length manually afterwards
  */
  void reserve_be_size(payload_type&, unsigned int);
  
  //get the number of the currently allocated data size
  // may be different from what data_length is
  // for debug only
  unsigned int get_reserved_data_size(payload_type&);

  //get the number of the currently allocated data size
  // may be different from what byte_enable_length is
  // for debug only
  unsigned int get_reserved_be_size(payload_type&);
  
protected:
  allocation_scheme m_scheme; //the allocation scheme to use by this socket
};

} //end ns socket
} //end ns gs

#include "greensocket/generic/green_socket_mm_base.tpp"

#endif
