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
// 2009-01-29 Christian Schroeder: renamings
// 2009-01-29 FURTHER CHANGES SEE SVN LOG!
//
//


#include "greenreg/sysc_ext/kernel/gr_notification_rule_container.h"

#include <sstream>

#include "greenreg/sysc_ext/utils/gr_report.h"

#include "greenreg/sysc_ext/kernel/I_notification_rule.h"
#include "greenreg/sysc_ext/kernel/gr_notification_rules.h"

namespace gs {
namespace reg {

gr_notification_rule_container::gr_notification_rule_container()
{
}

gr_notification_rule_container::~gr_notification_rule_container()
{
}

void gr_notification_rule_container::move_rule_to_front (std::string _name) {
  m_it = find_in_container(m_active_rules, _name);
  if (m_it == m_active_rules.begin()) {
     std::string msg = _name + " is already at the front";
     GR_INFO(msg.c_str());
  }
  else if (m_it != m_active_rules.end()) {
    I_notification_rule* _rule = m_it->second;
    m_active_rules.erase(m_it);
    m_active_rules.insert(m_active_rules.begin(), std::pair<std::string, I_notification_rule*>(_name, _rule));
  }
  else {
    std::string msg = _name + " cannot be moved to the front as it is not an active rule";
    GR_INFO(msg.c_str());
  }
}

void gr_notification_rule_container::move_rule_to_back (std::string _name) {
  m_it = find_in_container(m_active_rules, _name);
  if (m_it == m_active_rules.end()) {
    std::string msg = _name + " cannot be moved to the back as it is not an active rule";
    GR_INFO(msg.c_str());
  }
  else if (++m_it == m_active_rules.end()) {
     std::string msg = _name + " is already at the back";
     GR_INFO(msg.c_str());
  }
  else {
    --m_it;
    I_notification_rule* _rule = m_it->second;
    m_active_rules.erase(m_it);
    m_active_rules.push_back(std::pair<std::string, I_notification_rule*>(_name, _rule));
  }
}
bool gr_notification_rule_container::add_notification_rule( std::string _name, I_notification_rule * _rule)
{
  if( find_rule( _name) == NULL)
  {
    m_active_rules.push_back(std::pair<std::string, I_notification_rule*>(_name, _rule));
    GR_NOTIF_RULE_DUMP("Added notification rule "<< _name << " (to container 0x"<<(std::hex) << this << ")" << (std::dec))
#ifdef GR_NOTIF_RULE_DBG
    show_active_rules();
#endif
    return( true);
  }
  std::stringstream ss;
  ss << "Adding of notification rule "<< _name<< " FAILED!";
  GR_INFO( ss.str().c_str());
  return( false);
}

void gr_notification_rule_container::deactivate_rule( std::string _name)
{
  if( find_inactive_rule( _name) == NULL)
  {
    m_it = find_in_container(m_active_rules,_name);
    if( m_it != m_active_rules.end())
    {
      I_notification_rule * rule = m_it->second;
      m_active_rules.erase(m_it);
      m_inactive_rules.push_back(std::pair<std::string, I_notification_rule*>(_name, rule));
      if( find_in_container(m_active_rules, _name) != m_active_rules.end())
        GR_ERROR( "deactivate did not work!!! Should never reach here!");
      GR_NOTIF_RULE_DUMP("rule deactivated");
    }
    GR_NOTIF_RULE_DUMP(" rule "<<_name<<" you want to deactivate is not existing here!")
  }  
  else {
    //GR_INFO( "notification rule is already deactivated!");
  }
}

void gr_notification_rule_container::activate_rule( std::string _name)
{
  if( find_active_rule( _name) == NULL)
  {
    m_it = find_in_container(m_inactive_rules,_name);
    if( m_it != m_inactive_rules.end())
    {
      I_notification_rule * rule = m_it->second;
      m_inactive_rules.erase(m_it);
      m_active_rules.push_back(std::pair<std::string, I_notification_rule*>(_name, rule));
      GR_NOTIF_RULE_DUMP( "rule activated");
    }
  }  
}
  
void gr_notification_rule_container::process_active_rules( uint_gr_t _old_value, uint_gr_t _new_value, transaction_type* _transaction, bool _delayed)
{
  for( m_it = m_active_rules.begin(); m_it != m_active_rules.end(); m_it++)
  {
    GR_NOTIF_RULE_DUMP("  process rule "<<m_it->first)
    m_it->second->process_rule( _old_value, _new_value, _transaction, _delayed);
  }
}

gr_event & gr_notification_rule_container::get_rule_event( std::string _name)
{
  I_notification_rule * rule = find_rule( _name);
  if( rule != NULL)
    return( rule->default_event());
  std::stringstream ss;
  ss << "ERROR IN CODE, gr_notification_rule_container returning dummy event for: '" << _name << "'";
  GR_INFO( ss.str().c_str());
  return( dummy_event);
}

I_notification_rule * gr_notification_rule_container::find_rule( std::string _name)
{
  I_notification_rule * rule = find_active_rule( _name);
  if( rule != NULL)
    return( rule);
  return( find_inactive_rule( _name));
}

I_notification_rule * gr_notification_rule_container::find_active_rule( std::string _name)
{
  m_it = find_in_container(m_active_rules, _name);
  if( m_it != m_active_rules.end())
    return( m_it->second);
  return( NULL);
}

I_notification_rule * gr_notification_rule_container::find_inactive_rule( std::string _name)
{
  m_it = find_in_container(m_inactive_rules, _name);
  if( m_it != m_inactive_rules.end())
    return( m_it->second);
  return( NULL);
}

gr_notification_rule_container::container_iterator gr_notification_rule_container::find_in_container(rule_container& container, std::string _name) {
  m_it = container.begin();
  while (m_it != container.end()) {
    if (m_it->first.compare(_name) == 0)
      return m_it;
    m_it++;
  }
  return container.end();
}
  
void gr_notification_rule_container::disable_events() {
  switch_enable_events(false);
}

void gr_notification_rule_container::enable_events() {
  switch_enable_events(true);
}
  
void gr_notification_rule_container::switch_enable_events(bool enable_events) {
  // go through all notification rules and set the switches
  GR_NOTIF_RULE_DUMP("gr_notification_rule_container: switch all notification rules")
  for (container_iterator it = m_active_rules.begin(); it != m_active_rules.end(); it++) {
    GR_NOTIF_RULE_DUMP("  switch active rule "<<it->first)
    if (enable_events)
      it->second->default_event().enable_timing();
    else
      it->second->default_event().disable_timing();
  }
  for (container_iterator it = m_inactive_rules.begin(); it != m_inactive_rules.end(); it++) {
    GR_NOTIF_RULE_DUMP("  switch inactive rule "<<it->first)
    if (enable_events)
      it->second->default_event().enable_timing();
    else
      it->second->default_event().disable_timing();
  }
}

bool gr_notification_rule_container::has_rule(const std::string _name) {
  return (find_rule(_name) != NULL);
}

bool gr_notification_rule_container::is_empty() {
    return ((m_inactive_rules.size() == 0) && (m_active_rules.size() == 0));
}

#ifdef GR_NOTIF_RULE_DBG
void gr_notification_rule_container::show_active_rules() {
  std::cout << "active rules (container 0x"<<(std::hex) << this << "):" << std::endl;
  for (container_iterator it = m_active_rules.begin(); it != m_active_rules.end(); it++) {
    std::cout << "  " << it->first << std::endl;
  }
  std::cout << std::endl;
}
void gr_notification_rule_container::show_inactive_rules() {
  std::cout << "inactive rules (container 0x"<<(std::hex) << this << "):" << std::endl;
  for (container_iterator it = m_inactive_rules.begin(); it != m_inactive_rules.end(); it++) {
    std::cout << "  " << it->first << std::endl;
  }
  std::cout << std::endl;
}
#endif
  
} // end namespace gs::reg
} // end namespace gs::reg
