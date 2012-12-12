//   GreenSocket
//
// LICENSETEXT
//
//   Copyright (C) 2007 : GreenSocs Ltd
// 	 http://www.greensocs.com/ , email: info@greensocs.com
//
//   Developed by :
//   
//   Ashwani Singh, Circuitsutra Technologies 
//
//
// The contents of this file are subject to the licensing terms specified
// in the file LICENSE. Please consult this file for restrictions and
// limitations that may apply.
// 
// ENDLICENSETEXT

#ifndef __GREEN_SOCKET_TIMING_SUPPORT_H__
#define __GREEN_SOCKET_TIMING_SUPPORT_H__

#include "tlm.h"
#include <vector>
#include <map>
#include <stdarg.h>
#include "greensocket/generic/gs_callbacks.h"
#include "greensocket/utils/gs_msg_output.h"

namespace gs{
namespace socket{

//This class handles the timing support.
//For the lower abstraction level, the phase of a transaction can begin with a delay from the 
//clock edge and we in that case tranfer such delays from master to slave and vice versa using this class
class timing_info
{
public:
//set the time after the clock at which this phase will start (latest possible start)
 inline void set_start_time(const tlm::tlm_phase &, const sc_core::sc_time &); 
 
//get the start time of the provided phase
 inline sc_core::sc_time& get_start_time(const tlm::tlm_phase & );
 
//compare two timing info structs
 inline bool operator ==(timing_info& rhs);
 inline bool operator !=(timing_info& rhs);
 
//returns true if there is at least one phase that does not start
// at the clock edge (i.e. its start time is larger than SC_ZERO_TIME
 inline bool is_non_default();
 
//the container to store the start times
 std::map<tlm::tlm_phase , sc_core::sc_time> m_timing_map;

};

//the callback functor object declaration
#define GS_FUNC_RETURN void
#define GS_FUNC_ARGS timing_info mst_timing
#define GS_FUNC_ARGS_WITHOUT_TYPES mst_timing
GS_FUNC_WRAPPER(set_timing_callback);
GS_FUNC_FUNCTOR(set_timing_callback);
#undef GS_FUNC_RETURN
#undef GS_FUNC_ARGS
#undef GS_FUNC_ARGS_WITHOUT_TYPES  

//this base class is used to detect combinatorial loops
//  a combinatorial loop will lead to an infinite recursion in calls to set_timing (see below)
//  with every recurion a class internal counter is incremented. If it hits a predefined threshhold
//  an error will be reported. The default threshold is a 10000 (which would mean a combinatorial path
//  that goes through 9999 modules is acceptable
class timing_support_base
{
public:

  //use this functions in case you really have a combinatorial path that is longer than the defaul of 9999 modules
  static void set_recursion_limit(sc_dt::uint64 value){get_recursion_limit()=value;}
private:

  //this project had the requirement to be header-only, so we cannot use a static member of the class (requires a library)
  // so we use an implicitly inlined function that contains a static counter 
  static sc_dt::uint64& get_recursion_count()
  {
    static sc_dt::uint64 s_cnt=0;
    return s_cnt;
  }

  //for the same reason as above we need to store the recurions limit in a static function member
  static sc_dt::uint64& get_recursion_limit()
  {
    static sc_dt::uint64 s_limit=10000;
    return s_limit;
  }

protected:

  //this function is used by derived classes to increment the recursion counter
  // will raise an error if recursion limit is hit
  void inc_recursion_count(){
    get_recursion_count()++;
    if (get_recursion_count()==get_recursion_limit()){
      GS_MSG_OUTPUT(GS_MSG_FATAL, "Timing information distribution recursion limit reached (currently "<<get_recursion_limit()<<"). Check your code or increase the limit with gs::socket::timing_support_base::set_recursion_limit(value)");
    }
  }
  
  //this function decerements the recursion counter
  void dec_recursion_count(){get_recursion_count()--;}
};

//a fwd declaration
template<typename TRAITS, typename BIND_BASE>
class target_timing_support_base;


//this class is used by initiator sockets and contains the timing info distribution facilities
//we need to know the TRAITS because the functors we use internally require knowledge of the TRAITS
//we need to know the BIND_BASE to know the bindability base class type used in the bound_to function
template<typename TRAITS, typename BIND_BASE>
class initiator_timing_support_base : protected timing_support_base
{
public:
    typedef BIND_BASE bindability_base_type;
    typedef typename bindability_base_type::bind_base_type bind_base_types;
    typedef target_timing_support_base<TRAITS,BIND_BASE> other_socket_type;
	
    //default CTOR
    initiator_timing_support_base();
	
    //function that allows for registering a timing listener callback
    template<typename MODULE>
    inline void set_timing_listener_callback(MODULE* owner, void(MODULE::*timing_cb) (timing_info));
	
    //pass a timing information to a socket that shall be used for all the bindings of the socket
    inline void set_initiator_timing(timing_info&);
    
    //passs a timing information to a socket that shall be used for a specific binding
    // NOTE: this call can only be used when the number of bindings is fixed, i.e. at end_of_elaboration or later
    inline void set_initiator_timing(timing_info&, unsigned int );
    
    //this function is called by the connected target socket if it signals that the target has changed
    // its timing info
    inline void set_target_timing(timing_info&);
      
        
protected:

    //this function has the same signature and name as the bound_to function that is called by the bind checker
    // when a binding was successful. since the bound_to fn of the bind checker is virtual it will end up at
    // the derived socket, so this derived socket than has to call this bound_to function to make the timing info
    // distribution work
    inline void bound_to(const std::string& other_type, typename BIND_BASE::bind_base_type* other, unsigned int index);
	
    //an internal helper function that actually transmits the timing info from this socket to the connected one(s)
    inline void transmit_timing(unsigned int);
    
    //the functor that holds the callback (this is a member function container that does not need to know
    // the class type of the actual owner of the member function)
    set_timing_callback_functor<TRAITS> m_time_cb;
    
    //the timing info that is used to store the default timing info (before any binding did take place)
    timing_info m_one_for_all_timing;
    
    //a vector to remember which bindings were already successful
    std::vector<bool> m_bound;
    
    //a vector to store individual timing infos for each binding (once they have been established)
    std::vector<timing_info> m_timing_list;
    
    //a vector to store the connected targets for each binding
    std::vector<bind_base_types*> other_socket;

};


//this class does the same stuff as the class above but for target sockets
// documentation will be ommited as this class is symetrical to the one above
template<typename TRAITS, typename BIND_BASE>
class target_timing_support_base : protected timing_support_base
{
public:
    typedef BIND_BASE bindability_base_type;
    typedef typename bindability_base_type::bind_base_type bind_base_types;
    typedef initiator_timing_support_base <TRAITS,BIND_BASE> other_socket_type;
    target_timing_support_base();
    //function that allows for registering a timing listener callback
    template<typename MODULE>
    inline void set_timing_listener_callback(MODULE* owner, void(MODULE::*timing_cb) (timing_info));
    inline void set_target_timing(timing_info&);
    inline void set_target_timing(timing_info&, unsigned int);
    inline void set_initiator_timing(timing_info&);
       
protected:
	
    inline void bound_to(const std::string& other_type, typename BIND_BASE::bind_base_type* other, unsigned int index);
    inline void transmit_timing(unsigned int);
    set_timing_callback_functor<TRAITS> m_time_cb;
    timing_info m_one_for_all_timing;
    std::vector<bool> m_bound;
    std::vector<timing_info> m_timing_list;
    std::vector<bind_base_types *> other_socket;
};


} //end ns socket
} //end ns gs

#include "greensocket/generic/green_socket_timing_support_base.tpp"
#endif
