/*
 * tlm_serial_phase.h
 *
 * Copyright (C) 2014, GreenSocs Ltd.
 *
 * Developped by Manish Aggarwal, Ruchir Bharti
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

#ifndef __TLM_SERIAL_PHASE_H__
#define __TLM_SERIAL_PHASE_H__

#include <string>
#include <iostream>
#include <vector>

namespace tlm_serial {
  
enum serial_phase_enum { UNINITIALIZED_PHASE=0, BEG_TRANSMISSION=1};

inline unsigned int create_phase_number(){
  static unsigned int number=BEG_TRANSMISSION+1;
  return number++;
}

inline std::vector<const char*>& get_phase_name_vec(){
  static std::vector<const char*> phase_name_vec(BEG_TRANSMISSION+1, (const char*)NULL);
  return phase_name_vec;
}

class tlm_serial_phase{
public:
  tlm_serial_phase(): m_id(0) {}
  tlm_serial_phase(unsigned int id): m_id(id){}
  tlm_serial_phase(const serial_phase_enum& standard): m_id((unsigned int) standard){}
  tlm_serial_phase& operator=(const serial_phase_enum& standard){m_id=(unsigned int)standard; return *this;}
  operator unsigned int() const{return m_id;}
  
private:
  unsigned int m_id;  
};

inline
std::ostream& operator<<(std::ostream& s, const tlm_serial_phase& p){
  switch ((unsigned int)p){
    case UNINITIALIZED_PHASE:   s<<"UNINITIALIZED_PHASE"; break;
    case BEG_TRANSMISSION:  	s<<"BEG_TRANSMISSION"; break;
    default:
      s<<get_phase_name_vec()[(unsigned int)p]; return s;      
  }
  return s;
}
  
#define TLM_SERIAL_DECLARE_EXTENDED_PHASE(name_arg) \
class tlm_serial_phase_##name_arg:public serial::tlm_serial_phase{ \
public:\
static const tlm_serial_phase_##name_arg& get_phase(){static tlm_serial_phase_##name_arg tmp; return tmp;}\
private:\
tlm_serial_phase_##name_arg():serial::tlm_serial_phase(serial::create_phase_number()){serial::get_phase_name_vec().push_back(get_char_##name_arg());};\
tlm_serial_phase_##name_arg(const tlm_serial_phase_##name_arg&); \
tlm_serial_phase_##name_arg& operator=(const tlm_serial_phase_##name_arg&); \
static inline const char* get_char_##name_arg(){static const char* tmp=#name_arg; return tmp;} \
}; \
static const tlm_serial_phase_##name_arg& name_arg=tlm_serial_phase_##name_arg::get_phase()
  
} // namespace tlm_serial

#endif /* TLM_SERIAL_PHASE_HEADER */
