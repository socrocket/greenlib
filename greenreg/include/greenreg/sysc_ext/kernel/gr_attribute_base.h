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


//#define EXCLUDE_FROM_BUILD
#ifndef EXCLUDE_FROM_BUILD
#undef EXCLUDE_FROM_BUILD

#ifndef _GR_ATTRIBUTE_BASE_H_
#define _GR_ATTRIBUTE_BASE_H_

#include <string>

#include <boost/config.hpp>
#include <systemc>

#include "greenreg/utils/stl_ext/base_types.h"

#include "greenreg/framework/core/gr_object.h"
#include "greenreg/framework/core/gr_common.h"
#include "greenreg/framework/core/I_gr_config.h"

#include "greenreg/sysc_ext/kernel/gr_notification_rule_container.h"

#include "greenreg/internal/registers/register/I_register.h"

#include "greenreg/sysc_ext/kernel/gr_event.h"

namespace gs {
namespace reg {

////////////////////////////////////////////
/// gr_attrib_rule_container_e
/// enumeration of notification rule containers for add_rule capability
/// ATTRIB_PRE_READ - rules will execute prior to a read request to gr_attribute< unsigned int>
/// ATTRIB_POST_WRITE - rules will execute after a write request to gr_attribute< unsigned int>
/// @author 
/// @since 
////////////////////////////////////////////
enum gr_attrib_rule_container_e
{
	ATTRIB_PRE_READ = 0,
	ATTRIB_POST_WRITE
};

class gr_attribute_container;

////////////////////////////////////////////
/// gr_attrib_base
/// base class for gr_attribute which is
/// templated
/// @author 
/// @since 
////////////////////////////////////////////
class gr_attrib_base : public gr_object, public I_gr_config
{
public:

	////////////////////////////////////////////
	/// constructor
	///
	/// @param _type ? forces user to specify type
	////////////////////////////////////////////
	gr_attrib_base( std::string _name, const std::string _description, gr_attribute_container & _a);

	virtual ~gr_attrib_base();

	////////////////////////////////////////////
	/// get_type
	///
	/// @return type specified at construction
	////////////////////////////////////////////
	inline gs::reg_utils::base_type_e get_type()
	{ return m_type; }

	////////////////////////////////////////////
	/// operator = ( const double & _value)
	/// converts double to a gr_attribute<T>
	/// unless T is of type double.
	///
	/// If T = double gr_attribute<T> overload
	/// will take precedence
	///
	/// Only works with C++ base types.
	///
	/// @return reference to this class
	////////////////////////////////////////////
	virtual gr_attrib_base& operator = ( const double& _value);

	////////////////////////////////////////////
	/// operator = ( const int & _value)
	/// converts int to a gr_attribute<T>
	/// unless T is of type int.
	///
	/// If T = int gr_attribute<T> overload
	/// will take precedence
	///
	/// Only works with C++ base types.
	///
	/// @return reference to this class
	////////////////////////////////////////////
	virtual gr_attrib_base& operator = ( const int& _value);

	////////////////////////////////////////////
	/// operator = ( const unsigned int & _value)
	/// converts unsigned int to a gr_attribute<T>
	/// unless T is of type unsigned int.
	///
	/// If T = unsigned int gr_attribute<T> overload
	/// will take precedence
	///
	/// Only works with C++ base types.
	///
	/// @return reference to this class
	////////////////////////////////////////////
	virtual gr_attrib_base& operator = ( const unsigned int& _value);

	////////////////////////////////////////////
	/// default_event
	///
	/// @return default event fired when attrib changes
	////////////////////////////////////////////
	gr_event & default_event()
	{
		return m_change_event;
	}

	////////////////////////////////////////////
	/// enable_event
	/// enables the internal event to fire
	/// must be called to enable event from start
	////////////////////////////////////////////
	void enable_event() { m_has_event = true; }

	////////////////////////////////////////////
	/// disable_event
	/// disables the internal event, this should
	/// only be necessary if you wish to disable
	/// the event if it has been enabled.  The
	/// default setup for gr_attribute is disabled.
	////////////////////////////////////////////
	void disable_event() { m_has_event = false; }

	////////////////////////////////////////////
	/// has_event
	/// @return returns the current state of the
	/// 	event firing mechanism..
	////////////////////////////////////////////
	bool has_event() { return m_has_event; }

	////////////////////////////////////////////
	/// get_pre_read_rules
	///
	/// @return gr_notification_rule_container for PRE_READ rules
	///
	/// @see gr_notification_rule_container
	////////////////////////////////////////////
	gr_notification_rule_container & get_pre_read_rules();

	////////////////////////////////////////////
	/// get_post_write_rules
	///
	/// @return gr_notification_rule_container for POST_WRITE rules
	///
	/// @see gr_notification_rule_container
	////////////////////////////////////////////
	gr_notification_rule_container & get_post_write_rules();

	////////////////////////////////////////////
	/// add_rule
	/// adds a rule based on content provided, each
	/// rule has a dynamic set of parameters
	///
	/// @param _container ? enumeration id of which container to add the rule to
	/// @param _name ? unique string name identifying the rule
	/// @param _rule_type ? enumeration id of which rule to add
	/// @param ... ? param list based on rule requirements
	/// @return sc_core::sc_event of rule or dummy if rule already existed
	////////////////////////////////////////////
	gr_event & add_rule(
		gr_attrib_rule_container_e _container,
		std::string _name,
		gr_reg_rule_type_e _rule_type,
		...);

// may only be set during construction
protected:

	/// event fired when data change occurs (if enabled)
	gr_event m_change_event;

	/// user modifiable boolean to enable event firing
	/// when data change occurs
	bool m_has_event;

	/// type of the attribute
	gs::reg_utils::base_type_e m_type;

	/// description
	std::string m_description;

	gr_notification_rule_container * m_read_rules;

	gr_notification_rule_container * m_write_rules;
};

} // end namespace gs::reg
} // end namespace gs::reg

#endif // _GR_ATTRIBUTE_BASE_H_

#endif // EXCLUDE_FROM_BUILD
#undef EXCLUDE_FROM_BUILD
