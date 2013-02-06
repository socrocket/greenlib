/*****************************************************************************/

//@file gs_generic_signal_phase.h
//
// @brief This file creates a new phases for the greensignal protocol.
//
// 
//
//==================================================================
// Author:
// Ashwani Singh, CircuitSutra Technologies
//
/*****************************************************************************/
#ifndef GS_GENERIC_SIGNAL_PHASE_H_
#define GS_GENERIC_SIGNAL_PHASE_H_

#include <string>
#include <iostream>
#include <vector>
namespace gs_generic_signal
{
enum gs_generic_signal_phase_enum{REQ,ACK};

inline unsigned int create_phase_number(){
  static unsigned int number=ACK+1;
  return number++;
}

inline std::vector<const char*>& get_phase_name_vec(){
  static std::vector<const char*> phase_name_vec(ACK+1, (const char*)NULL);
  return phase_name_vec;
}

class signal_phase{
public:
  signal_phase(): m_id(0) {}
  signal_phase(unsigned int id): m_id(id){}
  signal_phase(const gs_generic_signal_phase_enum & standard): m_id((unsigned int)standard){}
  signal_phase& operator=(const gs_generic_signal_phase_enum & standard){m_id=(unsigned int)standard; return *this;}
  operator unsigned int()const {return m_id;}
  
private:
	unsigned int m_id;  
};

inline
std::ostream& operator<<(std::ostream& s, const signal_phase& p){
  switch ((unsigned int)p){
    case REQ: s<<"REQ"; break;
    case ACK:  s<<"ACK"; break;
   default:
      s<<get_phase_name_vec()[(unsigned int)p]; return s;      
  }
  return s;
}


#define DECLARE_EXTENDED_SIGNAL_PHASE(name_arg) \
class signal_phase_##name_arg:public gs_generic_signal::signal_phase{ \
public:\
static const signal_phase_##name_arg& get_phase(){static signal_phase_##name_arg tmp; return tmp;}\
private:\
signal_phase_##name_arg():gs_generic_signal::signal_phase(gs_generic_signal::create_phase_number()){gs_generic_signal::get_phase_name_vec().push_back(get_char_##name_arg());};\
signal_phase_##name_arg(const signal_phase_##name_arg&); \
signal_phase_##name_arg& operator=(const signal_phase_##name_arg&); \
static inline const char* get_char_##name_arg(){static const char* tmp=#name_arg; return tmp;} \
}; \
static const signal_phase_##name_arg& name_arg=signal_phase_##name_arg::get_phase()
}
#endif /*GS_GENERIC_SIGNAL_PHASE_H_*/
