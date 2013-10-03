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

//
// ChangeLog GreenSocs
//
// 2009-01-08 Puneet Arora: changed the container of notification rules to be vector from map, to 
//   be able to provide support for ordering of rules. Added functions move_rule_to_front and move_rule_to_back
// 2009-01-20 Christian Schroeder: add callback/event switch for notification rules
// 2009-01-21 Christian Schroeder: changed namespace
// 2009-01-26 Christian Schroeder: added has_rule()
// 2009-01-29 Christian Schroeder: renamings
// 2009-01-29 FURTHER CHANGES SEE SVN LOG!
//


#ifndef _GR_NOTIFICATION_RULE_CONTAINER_H_
#define _GR_NOTIFICATION_RULE_CONTAINER_H_

#include <string>
#include <map>

#include <boost/config.hpp>
#include <systemc>

#include "greenreg/gr_externs.h"
#include "greenreg/framework/core/gr_common.h"

#include "greenreg/sysc_ext/kernel/gr_event.h"

namespace gs {
namespace reg {

class I_notification_rule;

////////////////////////////////////////////
/// gr_notification_rule_container
/// Container for storing notification rules, however
/// some key capabilities were required.  Primarily the
/// ability to track active and inactive rules in such
/// a way that when a stimulus is triggered only the
/// active rules are ran without the need to runtime filter
/// (basically using two storage maps)
/// @author 
/// @since 
////////////////////////////////////////////
class gr_notification_rule_container
{
protected:
  typedef std::vector<std::pair<std::string, I_notification_rule*> > rule_container;
  typedef rule_container::iterator container_iterator;

public:

  ////////////////////////////////////////////
  /// Constructor
  ////////////////////////////////////////////
  gr_notification_rule_container();

  ////////////////////////////////////////////
  /// Destructor
  ////////////////////////////////////////////
  virtual ~gr_notification_rule_container();

  ////////////////////////////////////////////
  /// add_notification_rule
  /// adds a notification rule to the container
  ///
  /// @param _name ? unique name of rule
  /// @param _rule ? pointer to rule, the container now owns this memory
  /// @return true if add was successful (no conflicts)
  ///
  /// @see I notification_rule
  ////////////////////////////////////////////
  bool add_notification_rule( std::string _name, I_notification_rule * _rule);

  ////////////////////////////////////////////
  /// deactivate_rule
  /// Moves the rule from the active list to the
  /// inactive list if it exists in the active list,
  /// otherwise no action is performed.
  ///
  /// @param _name ? unique name of rule
  ////////////////////////////////////////////
  void deactivate_rule( std::string _name);

  ////////////////////////////////////////////
  /// activate_rule
  /// Moves the rule from the inactive list to the
  /// active list if it exists in the inactive list,
  /// otherwise no action is performed.
  ///
  /// @param _name ? unique name of rule
  ////////////////////////////////////////////
  void activate_rule( std::string _name);

  ////////////////////////////////////////////
  /// process_active_rules
  /// Walks throught active rules and executs them
  /// in whatever order the iterrator for the map
  /// runs in, passing both the old value and new
  /// value for evaluation.
  ///
  /// @param _old_value ? original value of the calling entity (register, attribute, etc.)
  /// @param _new_value ? new value of the calling entity (register, attribute, etc.)
  /// @param _transaction ? transaction initiating this call
  /// @param _delayed ? if the notifications shall be delayed or called directly
  ////////////////////////////////////////////
  void process_active_rules( uint_gr_t _old_value, uint_gr_t _new_value, transaction_type* _transaction = NULL, bool _delayed = false);

  ////////////////////////////////////////////
  /// get_rule_event
  /// returns the rule event for the specified
  /// rule name.
  ///
  /// @param _name ? unique name of rule
  /// @return sc_core::sc_event associated with rule, dummy event if rule does not exist
  ///
  /// @see sc_core::sc_event
  ////////////////////////////////////////////
  gr_event & get_rule_event( std::string _name);

  ////////////////////////////////////////////
  /// move_rule_to_front
  /// moves the rule to be notified first of 
  /// all.
  ///
  /// @param _name ? unique name of rule
  ////////////////////////////////////////////
  void move_rule_to_front (std::string _name);

  ////////////////////////////////////////////
  /// move_rule_to_back
  /// moves the rule to be notified at last.
  ///
  /// @param _name ? unique name of rule
  ////////////////////////////////////////////
  void move_rule_to_back (std::string _name);

  ////////////////////////////////////////////
  /// disable_events
  /// Switches the behavior of all gr_events managed by
  /// the rules of this container not to use events.
  /// Called by the classes implementing the switch interface
  /// and own notification_rule_containers. E.g. I_register,
  /// gr_attribute.
  ////////////////////////////////////////////
  void disable_events();
  
  ////////////////////////////////////////////
  /// enable_events
  /// Switches the behavior of all gr_events managed by
  /// the rules of this container to use sc_events.
  /// Called by the classes implementing the switch interface
  /// and own notification_rule_containers. E.g. I_register,
  /// gr_attribute.
  ////////////////////////////////////////////
  void enable_events();
  
  ////////////////////////////////////////////
  /// has_rule
  /// Returns if this container contains the specified rule.
  ////////////////////////////////////////////
  bool has_rule(const std::string _name);

  ////////////////////////////////////////////
  /// is_empty
  /// Returns if this container contains no rules.
  ////////////////////////////////////////////
  bool is_empty();
  
protected:
  
  /// map of active rules
  rule_container m_active_rules;

  /// map of inactive rules
  rule_container m_inactive_rules;

  /// dummy event to return if a request is made for an invalid notification rule
  gr_event dummy_event;

  /// notification rule map iterator
  container_iterator m_it;

  ////////////////////////////////////////////
  /// find_rule
  /// Searched through both active and inactive rules.
  ///
  /// @param _name ? unique name of rule
  /// @return notification rule if found, NULL otherwise
  ///
  /// @see I_notification_rule
  ////////////////////////////////////////////
  I_notification_rule * find_rule( std::string _name);

  ////////////////////////////////////////////
  /// find_active_rule
  /// Finds only active rules.
  ///
  /// @param _name ? unique name of rule
  /// @return notification rule if found, NULL otherwise
  ///
  /// @see I_notification_rule
  ////////////////////////////////////////////
  I_notification_rule * find_active_rule( std::string _name);

  ////////////////////////////////////////////
  /// find_inactive_rule
  /// Finds only inactive rules.
  ///
  /// @param _name ? unique name of rule
  /// @return notification rule if found, NULL otherwise
  ///
  /// @see I_notification_rule
  ////////////////////////////////////////////
  I_notification_rule * find_inactive_rule( std::string _name);
  
  ////////////////////////////////////////////
  /// find_in_container
  /// Help function returning a rule chosen out of the container
  ///
  /// @param container ? Rule container that should be searched in
  /// @param _name ? name of rule that should be searched for
  /// @return Iterator containing rule
  ////////////////////////////////////////////
  container_iterator find_in_container(rule_container& container, std::string _name);
  
  ////////////////////////////////////////////
  /// switch_enable_events
  /// Help function performing the switching for all 
  /// notification rules managed by this container
  ///
  /// @param enable_events ? If to enable the events (true) or not (false)
  ////////////////////////////////////////////
  void switch_enable_events(bool enable_events);
  
#ifdef GS_DEBUG
  void show_active_rules();
  void show_inactive_rules();
#endif

};

} // end namespace gs:reg
} // end namespace gs:reg

#endif /*_GR_NOTIFICATION_RULE_CONTAINER_H_*/
