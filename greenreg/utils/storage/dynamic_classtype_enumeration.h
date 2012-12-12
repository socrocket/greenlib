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

#ifndef _DYNAMIC_CLASSTYPE_ENUMERATION_H_
#define _DYNAMIC_CLASSTYPE_ENUMERATION_H_

#include <string>
#include <map>

namespace gs {
namespace reg_utils {
	
// forward declaration
class I_dynamic_enum_type;
	
////////////////////////////////////////////
/// I_dynamic_classtype_enumeration
/// interface describing general methods
/// @author 
/// @since 
////////////////////////////////////////////
class I_dynamic_classtype_enumeration
{
public:

	I_dynamic_classtype_enumeration();
	virtual ~I_dynamic_classtype_enumeration();
	
	////////////////////////////////////////////
	/// has_type
	/// determines if the enumeration contains
	/// a definition for the type
	///
	/// @param _type ? based on type string obtained
	/// from I_dynamic_enum_type::get_rtti_type_id()
	/// or using typeid(*this).raw_name()
	///
	/// @return true if registered, false if not
	///
	/// @see I_dynamic_enum_type
	////////////////////////////////////////////
	bool has_type( std::string _type);

	////////////////////////////////////////////
	/// get_type_id
	/// returns the internal id for a defined type
	///
	/// @param _type ? type string (refer to has_type)
	/// @return internal id for type or 0 if not found
	///
	/// @see I_dynamic_classtype_enumeration::has_type
	////////////////////////////////////////////
	unsigned int get_type_id( std::string _type);

	////////////////////////////////////////////
	/// get_type
	/// returns the associated type name for
	/// defined type id
	///
	/// @param _id ? id of registered type
	/// @return registered type name or "" if not found
	////////////////////////////////////////////
	std::string get_type( unsigned int _id);

	////////////////////////////////////////////
	/// get_printable_type
	/// returns a printable (human readable) 
	/// string representing the unique type
	///
	/// @param _id ? id of registered type
	/// @return printable string or "" if not found
	///
	/// @see reference
	////////////////////////////////////////////
	std::string get_printable_type( unsigned int _id);

protected:

	unsigned int create_enumeration_entry( I_dynamic_enum_type * _enum_type);

	/// struct for storing type and printable name
	/// so that RTTI calls are not required
	typedef struct typename_t{
		std::string m_type;
		std::string m_printable_name;
	} typename_t;

	/// map from typename (rtti now, possibly user defined in future) to id
	std::map< std::string, unsigned int> m_typename_to_ids;

	/// map from id to rtti typenames
	std::map< unsigned int, typename_t> m_ids_to_typename;

	/// current unique id counter
	unsigned int m_id_counter;
};

////////////////////////////////////////////
/// I_dynamic_enum_type
/// interface which implements and describes
/// some basic methods for retreiving type
/// name, id.
///
/// @author 
/// @since 
////////////////////////////////////////////
class I_dynamic_enum_type
{
public:
	I_dynamic_enum_type() : m_type_id(0) {;}
  virtual ~I_dynamic_enum_type() {}
  
	////////////////////////////////////////////
	/// get_printable_type
	/// returns a printable (human readable) 
	/// string representing the unique type
	///
	/// @return printable string or "" if not registered
	////////////////////////////////////////////
	virtual std::string get_printable_type() { return( _enumeration->get_printable_type( m_type_id)); }

	////////////////////////////////////////////
	/// get_type
	/// returns the associated type name
	///
	/// @return type name or "" if not registered
	////////////////////////////////////////////
	virtual std::string get_type() 
#ifdef _WINDOWS
	{ return( _enumeration->get_type( m_type_id)); }
#else
	{ return( _enumeration->get_printable_type( m_type_id)); }
#endif

	////////////////////////////////////////////
	/// get_type_id
	/// returns associated type id
	///
	/// @return type id or 0 if not registered
	////////////////////////////////////////////
	virtual unsigned int get_type_id() { return m_type_id;}

protected:
	/// an subclass of this interface will call
	/// this interfaces protected members
	friend class I_dynamic_classtype_enumeration;

#ifdef _WINDOWS
	////////////////////////////////////////////
	/// get_rtti_type_id
	/// MUST BE IMPLEMENTED AT LEAF CLASS
	/// RESULT NOT SAME ON ALL COMPILERS BUT ALWAYS UNIQUE
	/// returns value of typeid(*this).raw_name()
	///
	/// @return compiler link type name
	////////////////////////////////////////////
	virtual std::string get_rtti_type_id() = 0;
#endif // _WINDOWS

	////////////////////////////////////////////
	/// get_rtti_printable_type_id
	/// MUST BE IMPLEMENTED AT LEAF CLASS
	/// RESULT NOT SAME ON ALL COMPILERS BUT ALWAYS UNIQUE
	/// returns value of typeid(*this).printable_name()
	///
	/// @return compiler expansion type name
	////////////////////////////////////////////
	virtual std::string get_rtti_printable_type_id() = 0;

	/// type id assigned when registered
	unsigned int m_type_id;

	/// reference to owning enumeration
	I_dynamic_classtype_enumeration * _enumeration;
};

////////////////////////////////////////////
/// dynamic_classtype_enumeration
/// templated base line implementation of 
/// interface with the exception of method:
/// register_enumeration which requires template
/// type and is sole reason for existance of
/// I_dynamic_classtype_enumeration
///
/// @author 
/// @since 
/// @see I_dynamic_classtype_enumeration
////////////////////////////////////////////
template< class T>
class dynamic_classtype_enumeration : public I_dynamic_classtype_enumeration
{
public:
	dynamic_classtype_enumeration() {;}
	
	virtual ~dynamic_classtype_enumeration() {;}

	////////////////////////////////////////////
	/// register_enumeration
	/// Registers parent reference into the
	/// enumeration assuming that it inherits
	/// from I_dynamic_enum_type.  additional
	/// compile time checks should be added.
	///
	/// conceptually this type is not required.
	/// but it provides one the ability to define
	/// base types for enumerations so that one
	/// does not combine completely separate classes
	/// into the same enumeration.
	///
	/// @param _enum_childclass_type ? type of enumeration
	/// @return unique enumeration id
	////////////////////////////////////////////
	unsigned int register_enumeration( T & _enum_childclass_type) {
		I_dynamic_enum_type * enum_type = reinterpret_cast< I_dynamic_enum_type *>( &(_enum_childclass_type));
		return( create_enumeration_entry( enum_type));
	}
};

////////////////////////////////////////////
/// dynamic_enum_type
/// exists for the sole purpose of providing
/// a templated method to register a decendent
/// with it associated container
///
/// @author 
/// @since 
/// @see I_dynamic_enum_type
////////////////////////////////////////////
class dynamic_enum_type : I_dynamic_enum_type
{
public:
  virtual ~dynamic_enum_type() {}

	////////////////////////////////////////////
	/// _register
	/// registers a LEAF child (this) of type LEAF_T
	/// with its enumeration container of type ENUM_T
	///
	/// @param _dynamic_class_enum - reference to 
	///		dynamic enumeration which LEAF child (this)
	///		will register with.
	/// @return unique enumeration id
	///
	/// @see dynamic_classtype_enumeration::register_enumeration
	////////////////////////////////////////////
	template< class ENUM_T, class LEAF_T>
	unsigned int _register( dynamic_classtype_enumeration< ENUM_T> & _dynamic_class_enum)
	{
		return( _dynamic_class_enum.register_enumeration( *(static_cast< LEAF_T *>(this))));
	}
};

} // end namespace gs::reg_utils
} // end namespace gs::reg_utils

#endif // _DYNAMIC_CLASSTYPE_ENUMERATION

#endif // EXCLUDE_FROM_BUILD 
#undef EXCLUDE_FROM_BUILD
