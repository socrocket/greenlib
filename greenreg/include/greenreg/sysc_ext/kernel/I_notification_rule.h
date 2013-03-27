/*
Copyright (c) 2008, Intel Corporation
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.
    * Neither the name of Intel Corporation nor the names of its contributors
      may be used to endorse or promote products derived from this software
      without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

// ChangeLog GreenSocs
// 2009-01-21 Christian Schroeder: changed namespace
// 2009-01-29 Christian Schroeder: renamings
// 2009-01-29 FURTHER CHANGES SEE SVN LOG!


#ifndef I_NOTIFICATION_RULE_H_
#define I_NOTIFICATION_RULE_H_

#include <boost/config.hpp>
#include <systemc>

#include "greenreg/gr_externs.h"

#include "greenreg/sysc_ext/kernel/gr_event.h"


//#define GR_NOTIF_RULE_DBG

#ifdef GR_NOTIF_RULE_DBG
# define GR_NOTIF_RULE_DUMP(message) {std::cout << message << std::endl;}
#else
# define GR_NOTIF_RULE_DUMP(message) {}
#endif


namespace gs {
namespace reg {

////////////////////////////////////////////
/// I_notification_rule
/// Interface definition of a notification rule,
/// the user would desend from this and create their
/// own rule or use one of the many notification
/// rules already in DRF
/// @author 
/// @since 
////////////////////////////////////////////
class I_notification_rule
{
protected:

	/// default event to be fired when rule becomes true
	gr_event m_default_event;

public:

	////////////////////////////////////////////
	/// Constructor
	////////////////////////////////////////////
	I_notification_rule()
	{;}

	////////////////////////////////////////////
	/// Destructor
	////////////////////////////////////////////
	virtual ~I_notification_rule()
	{;}

	////////////////////////////////////////////
	/// process_rule
	/// Method will implement some rule which, which true, will fire m_default_event
	///
	/// @param _old_value ? original value of the calling entity (register, attribute, etc.)
	/// @param _new_value ? new value of the calling entity (register, attribute, etc.)
  /// @param _transaction ? transaction initiating this call
  /// @param _delayed ? if the notifications shall be delayed or called directly
	////////////////////////////////////////////
	virtual void process_rule( uint_gr_t _old_value, uint_gr_t & _new_value, transaction_type* _transaction, bool _delayed) = 0;

	////////////////////////////////////////////
	/// default_event
	///
	/// @return sc_core::sc_event that will be fired if and when this rule becomes true.
	///
	/// @see sc_core::sc_event
	////////////////////////////////////////////
	inline gr_event & default_event()
	{ return m_default_event; }

  /// Setter for delay
  void set_delay(const sc_core::sc_time &delay) {
    m_default_event.set_delay(delay);
  }

  /// Getter for delay
  const sc_core::sc_time& get_delay() const {
    return m_default_event.get_delay();
  }

};

} // end namespace gs:reg
} // end namespace gs:reg

#endif /*I_NOTIFICATION_RULE_H_*/
