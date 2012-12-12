// LICENSETEXT
//
//   Copyright (C) 2005 - 2008 : GreenSocs Ltd
//       http://www.greensocs.com/ , email: info.com
//
//   Developed by :
//
//  Robert Guenzel, Christian Schroeder
//     Technical University of Braunschweig, Dept. E.I.S.
//     http://www.eis.cs.tu-bs.de
//
//   Mark Burton, Marcus Bartholomeu
//     GreenSocs Ltd
//
//
// The contents of this file are subject to the licensing terms specified
// in the file LICENSE. Please consult this file for restrictions and
// limitations that may apply.
// 
// ENDLICENSETEXT

#ifndef __GS_QUANTUMKEEPER_H__
#define __GS_QUANTUMKEEPER_H__

#include <tlm_utils/tlm_quantumkeeper.h>
#include "greencontrol/config.h"


namespace gs {

  class gs_quantumkeeper
    : tlm_utils::tlm_quantumkeeper
  {

  public:

    gs_quantumkeeper() :
      m_next_sync_point(sc_core::SC_ZERO_TIME),
      m_local_time(new sc_core::sc_time(sc_core::SC_ZERO_TIME)),
      own(true)
    {
      reset();
    }

    gs_quantumkeeper(sc_core::sc_time& t) :
      m_next_sync_point(sc_core::SC_ZERO_TIME),
      m_local_time(&t),
      own(false)
    {
      //does not reset if initializing from an external ref
      //  only initialize m_next_sync_point
      m_next_sync_point = sc_core::sc_time_stamp() + compute_local_quantum();
      if (t != sc_core::SC_ZERO_TIME) setTD(*m_local_time);
    }

    // copy constructor
    gs_quantumkeeper(gs_quantumkeeper& t) :
      m_next_sync_point(t.m_next_sync_point),
      m_local_time(t.m_local_time),
      own(false)
    {
    }

    virtual ~gs_quantumkeeper()
    {
      if (own) delete m_local_time;
    }
  
    virtual void inc(const sc_core::sc_time& t)
    {
      *m_local_time += t;
      setTD(*m_local_time);
    }

    virtual void set(const sc_core::sc_time& t)
    {
      *m_local_time = t;
      setTD(*m_local_time);
    }
  
    virtual bool need_sync() const
    {
      return sc_core::sc_time_stamp() + *m_local_time >= m_next_sync_point;
    }

    virtual void sync()
    {
      sc_core::wait(*m_local_time);
      reset();
    }
  
    virtual void reset()
    {
      *m_local_time = sc_core::SC_ZERO_TIME;
      m_next_sync_point = sc_core::sc_time_stamp() + compute_local_quantum();
    }
  
    virtual sc_core::sc_time get_current_time() const
    {
      return sc_core::sc_time_stamp() + *m_local_time;
    }
  
    virtual sc_core::sc_time get_local_time() const
    {
      return *m_local_time;
    }
  
  protected:
    virtual sc_core::sc_time compute_local_quantum()
    {
      return tlm::tlm_global_quantum::instance().compute_local_quantum();
    }
  
  protected:
    sc_core::sc_time m_next_sync_point;
    sc_core::sc_time* m_local_time;
    bool own;


  // Simulate sc_time
  public:

    friend std::ostream& operator<< (std::ostream& os, const gs_quantumkeeper& t);

    // implicit cast to sc_time
    // TODO: should be const to avoid change without a setTD
    operator sc_core::sc_time&()
    {
      return *m_local_time;
    }

    void operator= ( const sc_core::sc_time& t)
    {
      set(t);
    }

    void operator += ( const sc_core::sc_time& t)
    {
      inc(t);
    }


  // Static methods to handle the singleton TD gs_param
  public:

    static
    const sc_core::sc_time&
    getTD()
    {
      static sc_core::sc_time zero_time = sc_core::SC_ZERO_TIME;

      // return the TD time only if set by the current process
      if (pid_current() == td_set_pid())
        return td_instance();
      else {
        return zero_time;
      }
    }

    static
    void
    setTD(const sc_core::sc_time& t)
    {
      td_instance() = t;
      td_set_pid() = pid_current();
    }

    static
    sc_core::sc_process_b* const
    pid_current()
    {
      return sc_core::sc_get_curr_simcontext()->get_curr_proc_info()->process_handle;
    }

  private:

    static
    sc_core::sc_time&
    td_instance()
    {
      static sc_core::sc_time td;
      return td;
    }

    static
    sc_core::sc_process_b*&
    td_set_pid()
    {
      static sc_core::sc_process_b* pid = 0;
      return pid;
    }

  };

  inline std::ostream& operator<< (std::ostream& os, const gs_quantumkeeper& t)
  {
    return os << *(t.m_local_time);
  }


} // namespace gs

#endif // __GS_QUANTUMKEEPER_H__
