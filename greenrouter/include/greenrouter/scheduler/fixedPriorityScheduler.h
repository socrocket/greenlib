// LICENSETEXT
//
//   Copyright (C) 2007 : GreenSocs Ltd
//       http://www.greensocs.com/ , email: info@greensocs.com
//
//   Developed by :
//
//   Robert Guenzel
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

#ifndef __fixedPriorityScheduler_h__
#define __fixedPriorityScheduler_h__


#include "tlm.h"
#include "greenrouter/genericScheduler_if.h"
#include "gsgpsocket/utils/gs_trace.h"
#include <set>

namespace gs{
namespace gp{

//--------------------------------------------------------------------------
/**
 * This is the comparator used for inserting new requests into the queue.
 */
//--------------------------------------------------------------------------
template <class T>
class fixedPrioritySchedulerCmp
{
public:
  fixedPrioritySchedulerCmp() {}

  bool operator()(const T& x, const T& y)
  {
    // sorting criterium is the master connection ordering
    return x.second<y.second;
  }
};


//--------------------------------------------------------------------------
/**
 * A fixed priority queue. Priorities are assigned in descending order, i.e.
 * the first masterport connected to the router is assigned the highest priority.
 *
 * The queue provides a gb_param queueSize with which to observe the current queue size.
 * Enable observation feature by #define QUEUE_DEBUG
 */
//--------------------------------------------------------------------------

template <typename TRAITS>
class fixedPriorityScheduler_b
: public GenericScheduler_if<TRAITS>,
  public sc_core::sc_module
{
public:
  typedef typename GenericScheduler_if<TRAITS>::pair_type pair_type;
  typedef std::pair<pair_type, unsigned int> set_pair_type;
  typedef fixedPrioritySchedulerCmp<set_pair_type> transactionCmp;
  typedef std::multiset<set_pair_type, transactionCmp> transactionSet;

  sc_core::sc_export<GenericScheduler_if<TRAITS> > protocol_scheduler_target;

  /**
   * Default constructor.
   */
  fixedPriorityScheduler_b(sc_core::sc_module_name name_)
    : sc_core::sc_module(name_), protocol_scheduler_target("protocol_scheduler_target")
  {
    init();
    protocol_scheduler_target(*this);
  }

  /**
   * This constructor is for compatibility reasons only.
   */
  fixedPriorityScheduler_b(sc_core::sc_module_name name_, sc_core::sc_time &t)
    : sc_core::sc_module(name_), protocol_scheduler_target("protocol_scheduler_target")
  {
    init();
    protocol_scheduler_target(*this);
  }


  virtual void enqueue(typename TRAITS::tlm_payload_type* txn, const typename TRAITS::tlm_phase_type& ph, unsigned int m_id){
    m_queue.insert(set_pair_type(pair_type(txn, ph), m_id));

    GS_DUMP("Queuing a RequestValid txn from master id="<<m_id);
    GS_DUMP("Queue size now is "<<(int)m_queue.size());
  }


  virtual pair_type& dequeue(bool remove=true){
    if (m_queue.size()==0) {
      SC_REPORT_ERROR(name(), "Dequeue was called on empty queue.");
      m_last = set_pair_type(pair_type(), 0);
      return m_last.first;
    }
    m_pos = m_queue.begin();
    m_last = (*m_pos);
    if (remove)
      m_queue.erase(m_pos);

    GS_DUMP("Popping a request from master id="<<m_last.second);
    GS_DUMP("Queue size now is "<<m_queue.size());

    return m_last.first;
  }


  virtual bool isPending() {
    return m_queue.size()>0;
  }

  virtual bool isEmpty() {
    return m_queue.size()==0;
  }

protected:
  transactionSet m_queue;
  typename transactionSet::iterator m_pos;
  transactionCmp m_cmp;
  set_pair_type m_last;

private:
  void init() {
    GS_DUMP("I am a fixed-priority scheduler.");
  }
};

class fixedPriorityScheduler : public fixedPriorityScheduler_b<tlm::tlm_base_protocol_types>{
public:
  fixedPriorityScheduler(const char* name)
    : fixedPriorityScheduler_b<tlm::tlm_base_protocol_types>(name)
  {
  }

  /**
   * This constructor is for compatibility reasons only.
   */
  fixedPriorityScheduler(const char* name, sc_core::sc_time &t)
    : fixedPriorityScheduler_b<tlm::tlm_base_protocol_types>(name, t)
  {
  }
};

}
}

#endif
