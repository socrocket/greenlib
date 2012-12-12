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

#ifndef __GREEN_SOCKET_DEFAULT_POOL_H__
#define __GREEN_SOCKET_DEFAULT_POOL_H__

#include "tlm.h"

namespace gs{
namespace socket{


//This is the pool of transactions used within
// green_initiator_sockets.
// It is a simple pool.
template<typename TRAITS,
        typename EXT_FILLER,
        unsigned int N=20
        >
class default_pool{
public:
  typedef typename TRAITS::tlm_payload_type payload_type;

protected:
  //a simple struct we can use to build linked lists of pointers to the pooled objects
  struct entry{
    public:
    entry(payload_type* content);
    
    payload_type* that;
    entry* next;
  };
    
public:
  default_pool(void* filler);
  
  ~default_pool();
  
  //test if a given transaction (pointer) belongs to this pool
  // ATTENION: for debug only. It includes a pretty slow search operation
  bool is_from(payload_type* cont);
  
  //get a transaction from the pool
  inline payload_type* construct();

  //return a transaction to the pool
  inline void free (payload_type* cont);
  
protected:
  entry* unused; //list of available objects in the pool
  entry* used;   //list of available entry objects in the pool. one for each transaction that is currently "used", i.e. outside the pool
  std::vector<payload_type*> mine; //just for clean up and is_from
  EXT_FILLER* m_filler; //a pointer to an object used to initialize new pool members
};

} //end ns socket
} //end ns gs

#include "greensocket/generic/green_socket_default_pool.tpp"

#endif
