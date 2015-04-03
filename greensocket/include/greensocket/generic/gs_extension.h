//   GreenSocket
//
// LICENSETEXT
//
//   Copyright (C) 2007 : GreenSocs Ltd
// 	 http://www.greensocs.com/ , email: info@greensocs.com
//
//   Developed by :
//   
//   Robert Guenzel <guenzel@eis.cs.tu-bs.de>,
//     Technical University of Braunschweig, Dept. E.I.S.
//     http://www.eis.cs.tu-bs.de
//
//
// The contents of this file are subject to the licensing terms specified
// in the file LICENSE. Please consult this file for restrictions and
// limitations that may apply.
// 
// ENDLICENSETEXT

#ifndef __GS_EXTENSION_H__
#define __GS_EXTENSION_H__

// Some MSVC DLL handling
// NOTE: _DLL is defined when /MD or /MDd is specified.
#if defined(_DLL) && !defined(__MINGW32__) && !defined(__MINGW64__)
  #ifdef GREENSOCKET_EXPORTS
    #define GREENSOCKET_API __declspec(dllexport)
    #define GREENSOCKET_DECL_VISIBLE 1
  #else
    #define GREENSOCKET_API __declspec(dllimport)
    #define GREENSOCKET_DECL_VISIBLE 0
  #endif

  #ifndef GREENSOCKET_EXT_API
    #define GREENSOCKET_EXT_API GREENSOCKET_API
  #endif
  // DLL has problem with inline functions
  #define INLINE

#else
// Or just ignore the macros for other platforms
  #define GREENSOCKET_API
  #define GREENSOCKET_DECL_VISIBLE 1
  #define GREENSOCKET_EXT_API
  #define INLINE inline
#endif

// The macro bellow is to fix compatibility issue between MSVC and
// GCC, as GCC not not permit template specialization outside of the
// namespace where it is defined, while MSVC requires the template
// specialization in order to export them from the DLL.
#if defined(_MSC_VER) && !defined(__GCCXML__)
  #define IF_MSVC_1(a) a
  #define IF_MSVC_4(a,b,c,d) a,b,c,d
#else
  #define IF_MSVC_1(a)
  #define IF_MSVC_4(a,b,c,d)
#endif

// Fix GCC pedantic issue: define dummy class to "eat" extra ';' from macro call
#ifdef __GNUC__
  #define FIX_GCC_PEDANTIC_ISSUE(name) FIX_GCC_PEDANTIC_ISSUE2(name,__LINE__)
  #define FIX_GCC_PEDANTIC_ISSUE2(name,line) FIX_GCC_PEDANTIC_ISSUE3(name,line)
  #define FIX_GCC_PEDANTIC_ISSUE3(name,line) class dummy_pedantic_##name##line {}
#else
  #define FIX_GCC_PEDANTIC_ISSUE(name)
#endif

#if defined(_MSC_VER) && !defined(NO_DEMANGLE)
#define NO_DEMANGLE
#endif

#ifndef NO_DEMANGLE
#include <cxxabi.h>
#endif

#if defined SYSTEMC_VERSION & SYSTEMC_VERSION <= 20050714
// Use newer boost (fix for a buggy SystemC 2.1 boost)
#include <boost/config.hpp>
#endif

#include "tlm.h"
#include "greensocket/utils/greensocket_extension_pool.h"
#include "greensocket/utils/gs_bind_macros.h"
#include "greensocket/utils/gs_msg_output.h"
#include <map>
#include <sstream>

namespace gs{
namespace ext{

//struct bindability_proxy;

//This enum defines the bindability state of a phase or extension
// It is evaluated when two sockets are bound. 
//  gs_mandatory means: 
//    If the connected socket does not know or reject this extension/phase
//    the binding cannot be done.
//  gs_optional means:
//    If the connected socket rejects this extension/phase it will not be used.
//    If the connected socket requires use of this extension/phase it will be used.
//    If the connected socket does also treat this extension/phase as ignorable the user
//     may or may not use it
//  gs_reject means:
//    The connected socket may not use this extension/phase.
enum GREENSOCKET_API gs_extension_bindability_enum{
  gs_optional=0, gs_mandatory=1, gs_reject=2, gs_unknown=3
};

enum GREENSOCKET_API gs_extension_type_enum{
  gs_array_guard=0, gs_data=1, gs_guarded_data=2//, gs_tlm_array_overflow_protector_guard=3
};


//This is the base class for all gs_extensions
//  It carries 
//  the name of the extension and some virtual functions.
//  It's only purpose to allow for introspecting extension through a template-free base class
//
// The user shall never directly use this class, it is for GreenSocket internal use only
struct GREENSOCKET_API gs_extension_base
{
  gs_extension_base( const char* _name)
    : 
#if (defined(NO_DEMANGLE) || defined(_MSC_VER))
      name(_name)
#else
      name(abi::__cxa_demangle(_name,0,0,&dummy_status_int))
#endif
  {
  }
  
  virtual ~gs_extension_base(){}
  virtual std::string dump() const=0; //dump the extension content to string for introspection
  virtual gs_extension_type_enum get_type(unsigned int&) const=0; //get the type of etxension
  virtual bool is_valid(){return false;} //check if the extension's valid flag is set

  const std::string& get_name()const{return name;} //get the name of the extension type
protected:
  int dummy_status_int; //we need a dummy from time to time
  std::string name; //storage for the type name (to avoid runtime demangling)
}; 

//the following four structs allow for compile time differentiation
// between the four different types of gs_extensions
// That saves a lot of runtime if-else statements
struct GREENSOCKET_API gs_array_guard_id{gs_array_guard_id(){}};
struct GREENSOCKET_API gs_data_id{gs_data_id() {}};
struct GREENSOCKET_API gs_guarded_data_id{gs_guarded_data_id() {}};
struct GREENSOCKET_API gs_real_guard_id{gs_real_guard_id(){}};


//this class is used as the dummy guard "extension" in case no guard is needed
// we need to make it a template to avoid having a library
template<typename T>
struct GREENSOCKET_API dummy_guard{static T ID;};
#if GREENSOCKET_DECL_VISIBLE
template<typename T>
T dummy_guard<T>::ID=0xdeadbeef;
#endif


//this is the template base for all the greensocket TLM extensions
template<
  typename T, //the typename of the extension
  typename GUARD, //the typename of the guard
  typename ID_TYPE, //the type of the ID class
  gs_extension_type_enum ENUM_VAL> //finally the enum value to return when the type is to be checked
struct gs_extension_t : public tlm::tlm_extension<T>, public gs_extension_base
{
  typedef GUARD guard; //allow to get the guard from an extebsion
  
  //CTOR
  gs_extension_t()
    :gs_extension_base(typeid(T).name())//pass the name to the base class
    , my_pool(NULL){} //by default we have no pool
  
  //DTOR has no job
  ~gs_extension_t(){}
  
  //default free implementation: 
  //  if ENUM_VAL is array_guard (the guard needed to avoid the auto_extension_stack_overflow, see green_socket_extension_support_base.tpp)
  //   we don't do anything. The if is calculated by the compiler so free() is really empty
  //  otherwise we recycle an extension in a pool if we have one. If not, we just delete ourself
  virtual void free(){
    if (ENUM_VAL!=gs_array_guard){
      if(my_pool) my_pool->recycle(static_cast<T*>(this)); else delete this;
    }
  }
  
  //get the type enum as return value, and the ID of the guard extension by reference
  virtual gs_extension_type_enum get_type(unsigned int& g_id)const {g_id=guard::ID; return ENUM_VAL;}
  
  //the following two are from the tlm_extension
  virtual void copy_from(tlm::tlm_extension_base const &)=0;
  virtual tlm::tlm_extension_base* clone() const=0;
  
  //dump shall convert the extension into a string
  virtual std::string dump() const=0;
  
  //return our valid flag
  virtual bool is_valid(){return m_valid;}
  
  //this static member is used for compile time differntiation of the different extension types (guard, guarded data, data only)
  static const ID_TYPE _ID;
  
  //the ptr to the pool for this extension
  gs::socket::extension_pool<T>* my_pool;
  
  //the valid flag
  bool m_valid;
};

//instantiate that static member _ID of the extension class
template<typename T, typename GUARD, typename ID_TYPE, gs_extension_type_enum ENUM_VAL>
const ID_TYPE gs_extension_t<T, GUARD, ID_TYPE, ENUM_VAL>::_ID;


/*this is how the different extensions are to be defined:
  gs_extension_t<T, dummy_guard<unsigned int>, gs_array_guard_id,  gs_array_guard>  == gs_array_guard
  gs_extension_t<T, dummy_guard<unsigned int>, gs_data_id,         gs_data>         == gs_data_extension
  gs_extension_t<T, GUARD,                     gs_guarded_data_id, gs_guarded_data> == gs_guarded_data_extension
  gs_extension_t<T, GUARD,                     gs_real_guard_id,   gs_guarded_data> == gs_real_guard_extension

Note that that guard extensions and guarded data extension both use the gs_guarded_data type enum value,
because a guard extensions is basically a guarded data bool.
*/

/*
The following MACRO defines a memory management guard extensions that allows to protect the auto_extension
stack within the generic payload from overflow.

It is a singleton extension. Only its existence matters. So, no pool. No real clone, no free.

*/
#define GS_ARRAY_GUARD_ONLY_EXTENSION(name)\
struct GREENSOCKET_EXT_API name; \
IF_MSVC_1(template class GREENSOCKET_EXT_API tlm::tlm_extension<struct name>;) \
IF_MSVC_4(template struct GREENSOCKET_EXT_API gs::ext::gs_extension_t<name, gs::ext::dummy_guard<unsigned int>, gs::ext::gs_array_guard_id, gs::ext::gs_array_guard>;) \
struct GREENSOCKET_EXT_API name : public gs::ext::gs_extension_t<name, gs::ext::dummy_guard<unsigned int>, gs::ext::gs_array_guard_id, gs::ext::gs_array_guard>{\
  virtual std::string dump() const{return "</extension name=\"" #name "\" type=\"guard\" value=\"set\">";}\
  virtual tlm::tlm_extension_base* clone() const {\
    return const_cast<tlm::tlm_extension_base*>((const tlm::tlm_extension_base*)this);\
  } \
  virtual void copy_from(tlm::tlm_extension_base const &){}  \
}; \
IF_MSVC_1(template class GREENSOCKET_EXT_API std::allocator<name*>;) \
IF_MSVC_1(template class GREENSOCKET_EXT_API std::vector<name*>;) \
IF_MSVC_1(template class GREENSOCKET_EXT_API gs::socket::extension_pool<name>;) \
IF_MSVC_1(template GREENSOCKET_EXT_API gs::ext::gs_extension_base* gs::ext::convert_ext<name>(tlm::tlm_extension_base*);) \
IF_MSVC_1(template GREENSOCKET_EXT_API name* gs::ext::create_extension<name>(const gs::ext::gs_array_guard_id&);) \
FIX_GCC_PEDANTIC_ISSUE(array_guard_ext)

//this macro shall be used to declare a guard only extension
// Example: GS_GUARD_ONLY_EXTENSION(cacheable); will define
//  a guard extension type named cacheable
//
//When using this MACRO it first creates a mm guard extension
// then uses this mm guard as guard for the real guard extension.
//The dump function will show 1 or 0 depending whether the valid flag is set or not.
// note that this does not mean the whole guard is valid. That also depends on the mm guard's existence
//clone and copy from just exchange the valid flags
#define GS_GUARD_ONLY_EXTENSION(name)\
GS_ARRAY_GUARD_ONLY_EXTENSION(name##_array_guard); \
struct GREENSOCKET_EXT_API name; \
IF_MSVC_1(template class GREENSOCKET_EXT_API tlm::tlm_extension<struct name>;) \
IF_MSVC_4(template struct GREENSOCKET_EXT_API gs::ext::gs_extension_t<name, name##_array_guard, gs::ext::gs_real_guard_id, gs::ext::gs_guarded_data>;) \
struct GREENSOCKET_EXT_API name : public gs::ext::gs_extension_t<name, name##_array_guard, gs::ext::gs_real_guard_id, gs::ext::gs_guarded_data>{\
  virtual std::string dump() const{ \
    std::stringstream s; \
    s<<"</extension name=\"" #name "\" type=\"guard\" value=\""<<gs::ext::gs_extension_t<name, name##_array_guard, gs::ext::gs_real_guard_id, gs::ext::gs_guarded_data>::m_valid<<"\">";\
    return s.str();\
  }\
  void copy_from(tlm::tlm_extension_base const & ext){ \
    const name* tmp=static_cast<const name*>(&ext); \
    this->m_valid=tmp->m_valid; \
  } \
  tlm::tlm_extension_base* clone() const { \
    name* tmp; \
    if (gs::ext::gs_extension_t<name, name##_array_guard, gs::ext::gs_real_guard_id, gs::ext::gs_guarded_data>::my_pool){ \
      tmp=gs::ext::gs_extension_t<name, name##_array_guard, gs::ext::gs_real_guard_id, gs::ext::gs_guarded_data>::my_pool->create(); \
      tmp->my_pool=gs::ext::gs_extension_t<name, name##_array_guard, gs::ext::gs_real_guard_id, gs::ext::gs_guarded_data>::my_pool; \
    }\
    else \
      tmp=new name(); \
    tmp->m_valid=this->m_valid; return tmp;\
  } \
}; \
IF_MSVC_1(template class GREENSOCKET_EXT_API std::allocator<name*>;) \
IF_MSVC_1(template class GREENSOCKET_EXT_API std::vector<name*>;) \
IF_MSVC_1(template class GREENSOCKET_EXT_API gs::socket::extension_pool<name>;) \
IF_MSVC_1(template GREENSOCKET_EXT_API gs::ext::gs_extension_base* gs::ext::convert_ext<name>(tlm::tlm_extension_base*);) \
IF_MSVC_1(template GREENSOCKET_EXT_API name* gs::ext::create_extension<name>(const gs::ext::gs_real_guard_id&);) \
FIX_GCC_PEDANTIC_ISSUE(guard_only_ext)



//this macro shall be used to declare a data only extension
// Example: 
/*  GS_DATA_ONLY_EXTENSION(priority){
      void copy_from(tlm::tlm_extension_base const & ext){
        const priority* tmp=static_cast<const name*>(&ext);
        this->prio=tmp->prio;
      }
      tlm::tlm_extension_base* clone() const {
        priority* tmp=new name(); 
        tmp->prio=this->prio; 
        return tmp;
      }
      std::string dump() const{
        std::stringstream s;
        s<<"Priority is "<<prio;
        return s.str();
      }
      unsigned int prio;
    };
*/
// This will declare a data extension that contains a single
//  unsigned integer and is called priority
// Note that you have to implement copy_from, clone (just like for
//  any normal OSCI extension) plus a call named construct
//  that shall always return a new empty extension object.
#define GS_DATA_ONLY_EXTENSION(name)\
struct GREENSOCKET_EXT_API name; \
IF_MSVC_1(template class GREENSOCKET_EXT_API tlm::tlm_extension<struct name>;) \
IF_MSVC_4(template struct GREENSOCKET_EXT_API gs::ext::gs_extension_t<name, gs::ext::dummy_guard<unsigned int>, gs::ext::gs_data_id, gs::ext::gs_data>;) \
struct GREENSOCKET_EXT_API name : public gs::ext::gs_extension_t<name, gs::ext::dummy_guard<unsigned int>, gs::ext::gs_data_id, gs::ext::gs_data>

#define GS_DATA_ONLY_EXTENSION_TAIL(name)\
IF_MSVC_1(template class GREENSOCKET_EXT_API std::allocator<name*>;) \
IF_MSVC_1(template class GREENSOCKET_EXT_API std::vector<name*>;) \
IF_MSVC_1(template class GREENSOCKET_EXT_API gs::socket::extension_pool<name>;) \
IF_MSVC_1(template GREENSOCKET_EXT_API gs::ext::gs_extension_base* gs::ext::convert_ext<name>(tlm::tlm_extension_base*);) \
IF_MSVC_1(template GREENSOCKET_EXT_API name* gs::ext::create_extension<name>(const gs::ext::gs_data_id&);) \
FIX_GCC_PEDANTIC_ISSUE(data_only_ext)


//this macro shall be used to declare a guared data extension
// Example: 
/*  GS_GUARDED_DATA_EXTENSION(burstsequence){
      void copy_from(tlm::tlm_extension_base const & ext){
        const burstsequence* tmp=static_cast<const name*>(&ext);
        this->seq=tmp->seq;
      }
      tlm::tlm_extension_base* clone() const {
        burstsequence* tmp=new name(); 
        tmp->seq=this->seq; 
        return tmp;
      }
      std::string dump() const{
        std::stringstream s;
        s<<"Burst sequence is "<<seq;
        return s.str();
      }      
      unsigned int seq;
    };
*/
// This will declare a guared data extension that contains a single
//  unsigned integer and is called burstsequence.
// Note that you have to implement copy_from, clone (just like for
//  any normal OSCI extension) plus a call named construct
//  that shall always return a new empty extension object.
#define GS_GUARDED_DATA_EXTENSION(name)\
GS_ARRAY_GUARD_ONLY_EXTENSION(name##_guard); \
struct GREENSOCKET_EXT_API name; \
IF_MSVC_1(template class GREENSOCKET_EXT_API tlm::tlm_extension<struct name>;) \
IF_MSVC_4(template struct GREENSOCKET_EXT_API gs::ext::gs_extension_t<name, name##_guard, gs::ext::gs_guarded_data_id, gs::ext::gs_guarded_data>;) \
struct GREENSOCKET_EXT_API name : public gs::ext::gs_extension_t<name, name##_guard, gs::ext::gs_guarded_data_id, gs::ext::gs_guarded_data>

#define GS_GUARDED_DATA_EXTENSION_TAIL(name)\
IF_MSVC_1(template class GREENSOCKET_EXT_API std::allocator<name*>;) \
IF_MSVC_1(template class GREENSOCKET_EXT_API std::vector<name*>;) \
IF_MSVC_1(template class GREENSOCKET_EXT_API gs::socket::extension_pool<name>;) \
IF_MSVC_1(template GREENSOCKET_EXT_API gs::ext::gs_extension_base* gs::ext::convert_ext<name>(tlm::tlm_extension_base*);) \
IF_MSVC_1(template GREENSOCKET_EXT_API name* gs::ext::create_extension<name>(const gs::ext::gs_guarded_data_id&);) \
FIX_GCC_PEDANTIC_ISSUE(guarded_data_ext)


//the following macro can be used to set up guared data extensions 
//  that contain a single member.
//  This is a very common thing, so we provide this macro.
// Example: SINGLE_MEMBER_GUARDED_DATA(burstsequence, unsigned int);
// does just the same as the example for GS_GUARDED_DATA_EXTENSION
// (although the member is now named value instead of seq)
#define SINGLE_MEMBER_GUARDED_DATA(name, type) \
GS_GUARDED_DATA_EXTENSION(name){ \
  void copy_from(tlm::tlm_extension_base const & ext){ \
    const name* tmp=static_cast<const name*>(&ext); \
    this->m_valid=tmp->m_valid;\
    this->value=tmp->value; \
  } \
  tlm::tlm_extension_base* clone() const { \
    name* tmp; \
    if (gs::ext::gs_extension_t<name, name##_guard, gs::ext::gs_guarded_data_id, gs::ext::gs_guarded_data>::my_pool){ \
      tmp=gs::ext::gs_extension_t<name, name##_guard, gs::ext::gs_guarded_data_id, gs::ext::gs_guarded_data>::my_pool->create(); \
      tmp->my_pool=gs::ext::gs_extension_t<name, name##_guard, gs::ext::gs_guarded_data_id, gs::ext::gs_guarded_data>::my_pool; \
    }\
    else \
      tmp=new name(); \
    tmp->m_valid=this->m_valid;\
    tmp->value=this->value; return tmp;\
  } \
  std::string dump() const{ std::stringstream s; s<<"</extension name=\"" #name "\" type=\"guarded_data\" value=\""<<value<<"\">"; return s.str();} \
  type value; \
}; \
GS_GUARDED_DATA_EXTENSION_TAIL(name)

//the following macro can be used to set up simple data extensions 
//  that contain a single member.
//  This is a very common thing, so we provide this macro.
// Example: SINGLE_MEMBER_DATA(priority, unsigned int);
// does just the same as the example for GS_DATA_ONLY_EXTENSION
// (although the member is now named value instead of prio)
#define SINGLE_MEMBER_DATA(name, type) \
GS_DATA_ONLY_EXTENSION(name){ \
  void copy_from(tlm::tlm_extension_base const & ext){ \
    const name* tmp=static_cast<const name*>(&ext); \
    this->value=tmp->value; \
  } \
  tlm::tlm_extension_base* clone() const { \
    name* tmp; \
    if (gs::ext::gs_extension_t<name, gs::ext::dummy_guard<unsigned int>, gs::ext::gs_data_id, gs::ext::gs_data>::my_pool){ \
      tmp=gs::ext::gs_extension_t<name, gs::ext::dummy_guard<unsigned int>, gs::ext::gs_data_id, gs::ext::gs_data>::my_pool->create(); \
      tmp->my_pool=gs::ext::gs_extension_t<name, gs::ext::dummy_guard<unsigned int>, gs::ext::gs_data_id, gs::ext::gs_data>::my_pool; \
    }\
    else \
      tmp=new name(); \
    tmp->value=this->value; return tmp;\
  } \
  std::string dump() const{ std::stringstream s; s<<"</extension name=\"" #name "\" type=\"data\" value=\""<<value<<"\">"; return s.str();} \
  type value; \
}; \
GS_DATA_ONLY_EXTENSION_TAIL(name)

typedef GREENSOCKET_API gs_extension_base* (*conv_fn)(tlm::tlm_extension_base*);

//this vector is used as a cast replacement. In principle you can try a dyn cast
// on every tlm_extension into gs_extension_base. However that is creepy slow.
//So whenever an extension is created for the very first time, we add a static
// function with the signature 
// gs_extension_base* convert(tlm::tlm_extension_base*)
// into this vector (at the index of tlm::tlm_extension_base::ID) 
INLINE GREENSOCKET_API std::vector<conv_fn>& extension_cast()
#if GREENSOCKET_DECL_VISIBLE
{
  static std::vector<conv_fn> s_vec;
  return s_vec;
}
#else
;
#endif


//this is the template function whose explicit form for a certain type is then what we put into the vector
template <typename T>
INLINE gs_extension_base* convert_ext(tlm::tlm_extension_base* other){
  return static_cast<gs_extension_base*>(static_cast<T*>(other));
}

//with the two things above a "cast" looks then like this:
/*
unsigned int some_extension_ID;
tlm_extension_base* foo=generic_payload.get_extension(some_extension_ID);
if (foo){
  gs::ext::conv_fn my_cast=extension_cast()[some_extension_ID];
  if (my_cast) //the existence of a conversion function ensures that the extension is actually a greensocket extension
    std::cout<<"the content of the extension with ID="<<some_extension_ID<<" is "<<my_cast(foo)->dump()<<std::endl;
  else
    std::cout<<"extension with ID="<<some_extension_ID<<" is no greensocket extension. No type agnostic introspection available."<<std::endl;
}
*/

//with this aproach you can iterate over the whole extension array and introspect all the (greensocket) extensions, without knowing their types
//the speed up compared to a dynamic cast is about factor 100


//this is just a wrapper around a vector. Often we need vectors in extensions. And the dump function makes use of the << operator.
// so we specifiy a <<operator for this vector wrapper and then we use the wrapper instead of a vector.
//Then you can do something like SINGLE_MEMBER_GUARDED_DATA(some_vec_extension, vector_container<int>)
// SINGLE_MEMBER_GUARDED_DATA(some_vec_extension, vector<int>) would not work because of the missing <<operator.
template<typename T>
struct vector_container : public std::vector<T>{
};
template <typename T>
inline std::ostream& operator<< (std::ostream & os, const vector_container<T> & vec){
  os<<"supressed";
  return os;
}


/*
The create_extension function for mm guards. Create just a singleton object.
*/
template <typename T>
INLINE T* create_extension(const gs::ext::gs_array_guard_id&)
{
  static T* retVal=NULL; //singleton pointer
  if (!retVal) { //if not yet there
    extension_cast().resize(tlm::max_num_extensions()); //add the conversion function
    extension_cast()[T::ID]=convert_ext<T>;
    retVal=new T(); //create the extension
  }
  return retVal; //return it
}

/*
The create_extension function for data only extensions. 
*/
template <typename T>
INLINE T* create_extension(const gs::ext::gs_data_id&)
{
  static gs::socket::extension_pool<T>* p_pool=NULL; //singleton pool
  if (!p_pool) { 
    p_pool=new gs::socket::extension_pool<T>(10); //if not yet there create the pool
    extension_cast().resize(tlm::max_num_extensions()); //add the conversion function
    extension_cast()[T::ID]=convert_ext<T>;
  }
  T* retVal=p_pool->create(); //get an extension from the pool
  retVal->my_pool=p_pool; //store the pool ptr inside the extension
  return retVal; //return it
}

//same as data extension
template <typename T>
INLINE T* create_extension(const gs::ext::gs_guarded_data_id&)
{
  static gs::socket::extension_pool<T>* p_pool=NULL;
  if (!p_pool) {
    p_pool=new gs::socket::extension_pool<T>(10);
    extension_cast().resize(tlm::max_num_extensions());
    extension_cast()[T::ID]=convert_ext<T>;
  }
  T* retVal=p_pool->create();
  retVal->my_pool=p_pool;
  return retVal;
}

//same as data extension
template <typename T>
INLINE T* create_extension(const gs::ext::gs_real_guard_id&)
{
  static gs::socket::extension_pool<T>* p_pool=NULL;
  if (!p_pool) {
    p_pool=new gs::socket::extension_pool<T>(10);
    extension_cast().resize(tlm::max_num_extensions());
    extension_cast()[T::ID]=convert_ext<T>;
  }
  T* retVal=p_pool->create();
  retVal->my_pool=p_pool;
  return retVal;
}

/*
universal create_extension function
use compile time differentiation to choose which implementation to use.
*/
template <typename T>
inline T* create_extension(){
  return create_extension<T>(T::_ID);
}


} //end ns ext
} //end ns gs

#endif
