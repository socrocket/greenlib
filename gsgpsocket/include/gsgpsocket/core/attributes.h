// LICENSETEXT
// 
//   Copyright (C) 2007-2010 : GreenSocs Ltd
//       http://www.greensocs.com/ , email: info@greensocs.com
// 
//   Developed by :
// 
//   Wolfgang Klingauf, Robert Guenzel, Christian Schroeder
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

#ifndef __ATTRIBUTES_H__
#define __ATTRIBUTES_H__

#include "attributeBase.h"
#include <string.h>
#include <boost/config.hpp> // needed for SystemC 2.1
#include <systemc>
#include <map>


namespace gs {
  
  
//---------------------------------------------------------------------------
/**
 * The generic command set.
 */
//---------------------------------------------------------------------------
enum GenericMCmdType {
  Generic_MCMD_IDLE =0,   //Idle command
  Generic_MCMD_WR     ,   //Write command
  Generic_MCMD_RD     ,   //Read command
  Generic_MCMD_RDEX   ,   //Exclusive read command
  Generic_MCMD_RDL    ,   //Read linked command
  Generic_MCMD_WRNP   ,   //Non-posted write command
  Generic_MCMD_WRC    ,   //Write conditional command
  Generic_MCMD_BCST       //Broadcast command
};


//---------------------------------------------------------------------------
/**
 * The generic response set.
 */
//---------------------------------------------------------------------------
enum GenericSRespType {
  Generic_SRESP_NULL =0,    //Null response
  Generic_SRESP_DVA  ,      //Data valid/accept response
  Generic_SRESP_FAIL ,      //Request failed
  Generic_SRESP_ERR         //Error response
};


//---------------------------------------------------------------------------
/**
 * The generic burst sequence set.
 */
//---------------------------------------------------------------------------
enum GenericMBurstSeqType{
  Generic_MBURSTSEQ_INCR     =0,    //Incrementing
  Generic_MBURSTSEQ_DFLT1    ,      //Custom (packed)
  Generic_MBURSTSEQ_WRAP     ,      //Wrapping
  Generic_MBURSTSEQ_DFLT2    ,      //Custom (not packed)
  Generic_MBURSTSEQ_XOR      ,      //Exclusive OR
  Generic_MBURSTSEQ_STRM     ,      //Streaming
  Generic_MBURSTSEQ_UNKN     ,      //Unknown
  Generic_MBURSTSEQ_RESERVED        //Reserved
};


//---------------------------------------------------------------------------
/**
 * The generic error set. This set is defined in the namespace
 * GenericError to enable extensions with own error classes.
 */
//---------------------------------------------------------------------------
namespace GenericError {
enum GenericErrorType {
  Generic_Error_NoError=0,
  Generic_Error_TimeOut,
  Generic_Error_Collision,
  Generic_Error_AccessDenied,
  Generic_Error_UnknownAddress,
  First_User_Error
};
} // namespace GenericError


//---------------------------------------------------------------------------
/**
 * The simulation modes of the generic protocol
 */
//---------------------------------------------------------------------------
enum GenericSimulationModeType {
  /// BA mode (don't use update_event)
  MODE_BA = 0,
  /// CT mode (fire cycle-count accurate quark update notifications)
  MODE_CT
};


//---------------------------------------------------------------------------
  /**
   * The default data type.
   */
  //---------------------------------------------------------------------------
  // char MData_desc[] ="Transaction Data";
  
  /**
   * UPDATE Sep 25,2008: There is no 2 modes anymore. The transaction
   * always holds a char* and a size that represents the valid bytes that
   * can be read from that pointer. The data and pointer are always
   * syncronized to point to the same bytes.
   *
   *
   * This class is a wrapper for data that is to be transferred
   * by a GreenBus transaction.
   * The base type is std::vector<gs_uint8>*. 
   *
   * The data byte array resides either in GSDataTyep
   * or can  be allocated
   * by the initiator of a transaction (usually this 
   * should be done in the UserAPI implementation).
   *
   * Other data types then std::vectory<gs_uint8> 
   * that are to be transferred over GreenBus
   * can be transported by two means, and the chosen method
   * affects the compatibility class of a transaction.
   *
   *  1) Pointer-Mode: the GSDataType can carry a pointer to a complex
   *     data object. This pointer has to be casted
   *     into its original type in the target.
   *     To set the pointer, use the setPointer(void*) method.
   *     To get the pointer, use the getPointer() method.
   *     To find out whether a GSDataType carries a pointer,
   *     use the isPointer() method.
   *     The result will be a very fast simulation, but low
   *     compatibility, since the target port must be aware
   *     of the transmitted object type. Simulation accuracy
   *     is restricted to PV and BA.
   *
   *  2) Full-Data-Mode: in this mode, the GSDataType carries 
   *     a serialized array of bytes which is represented by
   *     the std::vector<gs_uint8>*. 
   *     To set the vector, use the set(std::vector<gs_uint8>*) method.
   *     To get the vector, use the get() method.
   *     To find out whether a GSDataType carries a byte-vector,
   *     use the isPointer() method.
   *     The result will be a slower simulation, but full
   *     compatibility, since the target port does not need
   *     to be aware of the orginal object type of the transmitted
   *     data to interpret them. CT simulation will provide full
   *     cycle-count accurate results, and heterogeneous UserAPIs
   *     can be connected to eachother.
   *
   * When the GSDataType is used with a byte-vector, it fully 
   * supports data introspection with DUST.
   */
class GSDataType : public AttributeRoot {
  
#ifdef USE_DUST
  friend class scv_extensions<GSDataType>;
#endif
  
  friend std::ostream& operator<<(std::ostream&, const GSDataType&);
  friend std::istream& operator>>(std::istream&, GSDataType&);
  
public:
  
  // /////////////////////////////////////////////////////////////////////// //
  // ////////////////////// CLASS GSDataData /////////////////////////////// //
  // /////////////////////////////////////////////////////////////////////// //

  /// Pseudo-vector which is used by the GSDataType and returned by getMData and getSData
  /**
   * This class is able to contain own data (if own_data != NULL).
   * But also not own data is allowed.
   */
  class GSDataData {
  protected:
    unsigned char *datptr;
    unsigned char *own_array;
    typedef unsigned int size_type;
    size_type m_size;
  public:

    /// not-own-data: Constructor without setting data ptr
    GSDataData() 
    : datptr(NULL)
    , own_array(NULL)
    , m_size(0)
    { }

    /// not-own-data: Constructor copying data ptr and size
    GSDataData(unsigned char* data_ptr, size_type _size) 
    : datptr(data_ptr)
    , own_array(NULL)
    , m_size(_size)
    { }
    
    //GSDataData( const std::vector<gs_uint8>& c );
    
    /// not-own-data: Contructor copies data pointer and size
    GSDataData( const GSDataData& c )
    : datptr(c.datptr)
    , own_array(NULL)
    , m_size(c.m_size)
    { }
    
    /// own-data: Contructor creating own data (optionally with default value)
    GSDataData( size_type num, const gs_uint8& val = gs_uint8() ) {
      own_array = new unsigned char[num];
      datptr = own_array;
      m_size = num;
      for (unsigned int i = 0; i < m_size; i++)
        datptr[i] = val;
    }
    
    //GSDataData( std::vector<gs_uint8>::input_iterator start, std::vector<gs_uint8>::input_iterator end );
    
    // Destructor: delete own data if this has
    ~GSDataData() {
      if (own_array)
        delete[] own_array; own_array = NULL;
    }
    
    /// set data pointer (remove own data if this has)
    void setData(unsigned char* ptr, size_type _size) {
      if (own_array) {
        delete[] own_array; own_array = NULL;
      }
      datptr = ptr;
      m_size = _size;
    }
    /// set data pointer (remove own data if this has)
    void setData(const GSDataData &d) {
      if (own_array) {
        delete[] own_array; own_array = NULL;
      }
      datptr = d.datptr;
      m_size = d.m_size;
    }
    
    // set data pointer (remove own data if this has)
    unsigned char* getData() const{
      return datptr;
    }
    
    // Access functions
    gs_uint8& at( size_type loc ) { return datptr[loc]; }
    
    const gs_uint8& at( size_type loc ) const { assert(loc >= m_size); return datptr[loc]; }
    
    size_type size() const { return m_size; }
    
    /// Resizes own data. Not allowed for not own data.
    void resize( size_type num, const gs_uint8& val = gs_uint8() ) {
      if (own_array && num > m_size) {
        delete[] own_array;
        own_array = new unsigned char[num];
        datptr = own_array;
        m_size = num;
      } else {
        assert(num <= m_size);
      }
      for (unsigned int i = 0; i < num; i++)
        datptr[i] = val;
    }
    
    // operators
    gs_uint8& operator[]( size_type index ) { return datptr[index]; }
    const gs_uint8& operator[]( size_type index ) const { return datptr[index]; }
    /*GSDataData operator=(const std::vector<gs_uint8>& c2);
    bool operator==(const std::vector<gs_uint8>& c1, const std::vector<gs_uint8>& c2);
    bool operator!=(const std::vector<gs_uint8>& c1, const std::vector<gs_uint8>& c2);
    bool operator<(const std::vector<gs_uint8>& c1, const std::vector<gs_uint8>& c2);
    bool operator>(const std::vector<gs_uint8>& c1, const std::vector<gs_uint8>& c2);
    bool operator<=(const std::vector<gs_uint8>& c1, const std::vector<gs_uint8>& c2);
    bool operator>=(const std::vector<gs_uint8>& c1, const std::vector<gs_uint8>& c2);
    GSDataData operator=(const GSDataData& c2);
    bool operator==(const GSDataData& c1, const std::vector<gs_uint8>::& c2);
    bool operator!=(const GSDataData& c1, const GSDataData& c2);
    bool operator<(const GSDataData& c1, const GSDataData& c2);
    bool operator>(const GSDataData& c1, const GSDataData& c2);
    bool operator<=(const GSDataData& c1, const GSDataData& c2);
    bool operator>=(const GSDataData& c1, const GSDataData& c2);*/
    
  };
  // /////////////////////////////////////////////////////////////////////// //
  // ////////////////// END CLASS GSDataData /////////////////////////////// //
  // /////////////////////////////////////////////////////////////////////// //
  
  typedef GSDataData dtype;
  typedef void ptype;

public:
  dtype *data;
private:
  ptype *pointer;
  bool m_is_pointer; // necessary because pointer may be NULL while still being a pointer
  typedef unsigned int size_type; // BARTHO: this should be size_t
  size_type size;
  std::string *name;
  bool own_data;
  
public:

  /**
   * Create an empty MasterDataType. Use set method to initialize.
   */
  GSDataType()
    : data(NULL), 
      pointer(NULL),
      m_is_pointer(false),
      name(NULL),
      own_data(false)
  {}

  /**
   * Create a MasterDataType that holds its own 'vector' (dtype=GSDataData), pre-sized to size.
   */
  GSDataType(int size)
    : data(new dtype(size)), 
      pointer(NULL),
      m_is_pointer(false),
      size(size),
      name(NULL),
      own_data(true)
  {}
  
  /**
   * Create a GSDataType which points to a GSDataData (which is NOT a std::vector<gs_uint8>).
   * @param d A data vector.
   */
  GSDataType(const dtype &d) 
    : data(const_cast<dtype*>(&d)), 
      pointer(NULL),
      m_is_pointer(false),
      size(data->size()),
      name(NULL),
      own_data(false)
  {
  }

  /**
   * Create a GSDataType which points to a void* pointer.
   *
   * If you want to set pointer data without being intereste in the size 
   * (e.g. legacy support), set the size to 0.
   *
   * @param p     A data pointer.
   * @param size  Pointer size, size_of(p), use setMBurstLength to set simulated data size!
   */
  GSDataType(const ptype *p, size_type size)  // Use size = 0 if you are not interested in the size (e.g. legacy support)
    : data(NULL),
      pointer(const_cast<ptype*>(p)),
      m_is_pointer(true),
      size(size),
      name(NULL),
      own_data(false)
  {
    if (size==0) SC_REPORT_WARNING("GSDataType", "setting pointer without providing the size of the object the pointer points to.");
    //if (p==NULL) SC_REPORT_WARNING("GSDataType", "setting NULL pointer will lead to a seg fault when reading from this because it won't be treated as a pointer.");
    // resolved with m_is_pointer
  }

  /// destructor
  ~GSDataType() {
    if (name!=NULL)
      delete name;
    if (own_data)
      delete data;
  }

private: // disabled  
  
  /**
   * Copy constructor is disabled to prevent bad access.
   */
  GSDataType(const GSDataType &m) {assert(0);}
  
  /**
   * Copy operator is disabled to prevent bad access.
   */
  GSDataType &operator= (const GSDataType &m) {
    return *this;
  }

public:  
  /**
   * Set byte vector of this GSDataType.
   * The byte vector is NOT copied. Only a pointer to it is stored.
   * @param d Reference to byte vector
   */
  void setData(const dtype &d) {
    if (own_data) { delete data; own_data = false; }
    data=const_cast<dtype*>(&d); 
    pointer=NULL; m_is_pointer = false;
    size=data->size();
  }

  /**
   * Set byte vector of this GSDataType.
   * The byte vector ptr is stored.
   * @param d Pointer to byte vector
   */
  /*void setDataPtr(dtype* d) {
    if (own_data) { delete data; own_data = false; }
    data=d; 
    pointer=NULL; m_is_pointer = false;
  }*/
  
  /// creates an own data member which does contain pointers but no own array
  void setOwnData(const dtype &d) {
    if (!own_data) { 
      data = new dtype(d);
      own_data = true;
    }
    else
      data->setData(d);

    pointer=NULL; m_is_pointer = false;
    size=data->size();
  }
  
  /**
   * Create a copy of a GSDataType. Note: does NOT copy the data, only the pointers to it!
   * @param m GSDataType to copy.
   */
  void set(const GSDataType &m) {
    if (own_data) { delete data; own_data = false; }
    data=m.data;
    pointer=m.pointer;
    m_is_pointer=m.m_is_pointer;
    size=m.size;
  }

  /**
   * Set the pointer of this GSDataType.
   * @param p      A data pointer.
   * @param _size  Pointer size, size_of(p), use setMBurstLength to set simulated data size!
   */
  void setPointer(const ptype *p, const size_type _size=0) {
    if (own_data) { delete data; own_data = false; }
    //data=new dtype((unsigned char*)p, _size); own_data = true;
    data = NULL;
    pointer=const_cast<ptype*>(p);
    m_is_pointer = true;
    size=_size;
    if (size==0) SC_REPORT_WARNING("GSDataType", "setting pointer without providing the size of the object the pointer points to.");
    //if (p==NULL) SC_REPORT_WARNING("GSDataType", "setting NULL pointer will lead to a seg fault when reading from this because it won't be treated as a pointer.");
    // solved with m_is_pointer
  }
  
  /**
   * Test if this GSDataTypes carries a pointer.
   */
  const bool isPointer() const {
    //return pointer!=NULL;
    return m_is_pointer;
    // there is no 2 modes anymore
  }  
  
  /**
   * Get byte vector carried by this GSDataType.
   * @return Reference to the data vector.
   */
  dtype &getData() const {
    return *data;
  }

  /**
   * Get byte vector pointer carried by this GSDataType.
   * @return Pointer to the data vector.
   */
  dtype* getDataPtr() const {
    return data;
  }
  
  /**
   * Get pointer carried by this GSDataType.
   */
  ptype *getPointer() const {
    return pointer;
  }
  
  /**
   * Overwrite this object's data with the data stored in another GSDataType.
   * @param src GSDataType from which the data shall be copied.
   */
  void deepcopyFrom(const GSDataType &src) {
    if (own_data) delete data;
    size=src.size;
    data = new dtype(size);
    own_data=true;
    std::memcpy(data->getData(), src.data->getData(), size);
    pointer=data->getData();
  }
  
  /**
   * Copy data from this object to another GSDataType's data array.
   * @param dst GSDataType to which the data shall be copied.
   */
  void deepcopyTo(GSDataType &dst) const {
    dst.deepcopyFrom(*this);
  }
  
  /**
   * Operator [] for data access in this GSDataType.
   * Please note that the operation will fail if the GSDataType carries a pointer (see isPointer).
   * Note also that this operation does no range-checking.
   */
  gs_uint8 & operator[] (gs_uint32 i) const {
    assert(data!=NULL);
    return (*data)[i];
  }
  
  /**
   * Append one byte to this GSDataType.
   * Note that this method overrides a previously call to setPointer.
   * The method returns false if the data vector in this GSDataType has not been initialized.
   * @param b add this byte to the vector in this GSDataType.
   * @return true on success, false if the data vector in this GSDataType has not been initialized.
   */
  const bool append(const gs_uint8 b) {
    SC_REPORT_WARNING("GSDataType", "The GSDataType (MData, MSData) does not support append(...) any longer!");
    return false;
    /*
    pointer=NULL;
    if (data==NULL)
      return false;
    
    data->push_back(b);
    return true;*/
  }

  /**
   * Return the size of the byte vector of this GSDataType.
   */
  inline virtual const gs_uint32 getSize() const { 
    return size;
  }

  /**
   * Get the name of this GSDataType.
   * @return A string containing the name.
   */
  virtual const std::string &getName() const { 
    assert(name!=NULL);
    return *name; 
  } 

  /**
   * Serialize to unsigned char array.
   */
  inline virtual const gs_uint8* serialize() {
    return data==NULL ? NULL : &(*data)[0];
  }
  
  /**
   * Set the name of this GSDataType.
   * @param _name The new name for this object.
   */
  virtual void setName(const std::string &_name)
  { 
    if (name!=NULL)
      delete name;
    name=new std::string(_name);
  }    
  
};

//-----------------------------------------------------------------------------
// stream operators for use of GSDataType with gb_param
//-----------------------------------------------------------------------------

inline
std::ostream& operator << (std::ostream &os, const GSDataType &obj) {
  if (obj.data != NULL)
    os << &(*(obj.data))[0];
  return os;
}

inline
std::istream& operator >> (std::istream &is, GSDataType &obj) {  
  if (obj.data != NULL)
    is >> &(*(obj.data))[0];
  return is;
}



  //---------------------------------------------------------------------------
  /** \addtogroup attributes Default attributes
   *  Default attributes that can be used in a user-defined protocol
   *  @{
   */
  //---------------------------------------------------------------------------

#define GSGB_ATTRIBUTE(name,type) typedef gs::AttributeBase<type> name
#define GSGB_ATTRIBUTESTACK(name,type) typedef gs::AttributeStackBase<type> name

//Request group
GSGB_ATTRIBUTE (MAddr, gs_uint64);            /// Master address
GSGB_ATTRIBUTE (MAddrSpace, gs_uint64);      /// Master address space
GSGB_ATTRIBUTE (MAtomicLength, gs_uint32);    /// "Length of atomic burst"
GSGB_ATTRIBUTE (MBurstLength, gs_uint32);     /// "Master burst length, set to '0' for no data at all (e.g. interrupt)."
GSGB_ATTRIBUTE (SBurstLength, gs_uint32);     /// "Slave burst length (for a write, this parameter identifies how much data has been accepted by the slave,
GSGB_ATTRIBUTE (MBurstPrecise, bool);         /// "Given burst length is precise (if false, arbitrary length burst is assumed)."
GSGB_ATTRIBUTE (MBurstSeq, GenericMBurstSeqType); /// "Address sequence of burst"
GSGB_ATTRIBUTE (MBurstSingleReq, bool);       /// "Burst uses single request/multiple data protocol"
GSGB_ATTRIBUTE (MByteEn, gs_uint32);          /// "Master byte enable"
GSGB_ATTRIBUTE (MCmd, GenericMCmdType);       /// Master command
GSGB_ATTRIBUTE (MConnID, gs_uint32);          /// "Master connection identifier"
GSGB_ATTRIBUTE (MDataWidth, gs_uint32);     /// depricated, see data_width parameter in tlm_port  /// "Master data bus width: # bytes that are transferred by the initiator port in one cycle."
GSGB_ATTRIBUTE (SDataWidth, gs_uint32);     /// depricated, see data_width parameter in tlm_port  /// "Slave data bus width: # bytes that are transferred by the targer port in one cylce. For busses, usually MDataWidth==SDataWidth holds. For NoCs including multi-bus systems, MDataWidth!=SDataWidth can occur."
GSGB_ATTRIBUTE (MID, gs_handle);              /// "Master identifier"
GSGB_ATTRIBUTE (MReqLast, bool);              /// "Last request in a burst"
GSGB_ATTRIBUTE (MTagID, gs_uint32);           /// "Master tag identifier "
GSGB_ATTRIBUTE (MTagInOrder, bool);           /// "Force tag-in-order"
GSGB_ATTRIBUTE (MThreadID, gs_uint32);        /// "Master thread identifier"
GSGB_ATTRIBUTE (MReqInfo, gs_uint64);         /// "Extra information sent with the request."
GSGB_ATTRIBUTE (TransID, gs_uint64);          /// "Unique transaction ID"

//Data group
typedef GSDataType MSData;                /// Transaction data (resides in master port)
typedef MSData MData;                     /// deprecated
GSGB_ATTRIBUTE (MDataInfo, gs_uint64);        /// "Extra information sent with the write data"
GSGB_ATTRIBUTE (SDataInfo, gs_uint64);        /// "Extra information sent with the read data"
GSGB_ATTRIBUTE (MSBytesValid, gs_uint64);       /// Number of valid bytes in MSData. Points to the first non-valid byte. Is set to MBurstLength upon request.

//Response group
GSGB_ATTRIBUTE (SResp, GenericSRespType);     /// "Slave response"
GSGB_ATTRIBUTE (SThreadID, gs_uint32);        /// "Slave thread identifier"
GSGB_ATTRIBUTE (STagID, gs_uint32);           /// "Slave tag identifier "
GSGB_ATTRIBUTE (STagInOrder, bool);           /// "Force tag-in-order "
GSGB_ATTRIBUTE (SRespInfo, gs_uint64);        /// "Extra information sent out with the response."
GSGB_ATTRIBUTE (SRespLast, bool);             /// "Last response in burst"
                                         ///  for a read, this parameter identifies how much data has been written by the slave."

GSGB_ATTRIBUTE (Error, gs_uint32);            /// "Error identifier"

GSGB_ATTRIBUTESTACK (MAddrSt, gs_uint64);            /// Master address stack
GSGB_ATTRIBUTESTACK (MIDSt, gs_handle);              /// "Master identifier" stack
/** @}
 */

typedef std::multimap<int,AttributeRoot*> attribute_map;
typedef std::multimap<int,AttributeRoot*>::iterator attribute_iterator;


//-----------------------------------------------------------------------------
// stream operators for use of attributes with gb_param.
// We need to define the enum operators first in order to make AttributeBase 
// stream operators work. Hence the AttributeBase stream operators cannot 
// be defined in attributeBase.h
//-----------------------------------------------------------------------------

#define ENUM_OPERATORS(type)                                            \
  inline std::istream& operator >> (std::istream &is, type &v) {        \
    gs_uint32 i;                                                        \
    is >> i;                                                            \
    v = static_cast<type>(i);                                           \
    return is;                                                          \
  }                                                                     \
  inline std::ostream& operator << (std::ostream &os, const type &v) {  \
    os << static_cast<gs_uint32>(v);                                    \
    return os;                                                          \
  }

ENUM_OPERATORS(GenericMCmdType)
ENUM_OPERATORS(GenericSRespType)
ENUM_OPERATORS(GenericMBurstSeqType)

// output stream operator for AttributeBase
template <class T>
inline std::ostream& operator << (std::ostream &os, const AttributeBase<T> &obj) {
  os << "0x" << std::hex << obj.value;
  return os;
}

// input stream operator for AttributeBase
template <class T>
inline std::istream& operator >> (std::istream &is, AttributeBase<T> &obj) {  
  // assume string to be plane number ("42") or hex number ("0x42")
  std::string::size_type idx;
  std::string s;
  std::stringstream ss;
  is >> s;
  idx = s.find("0x");
  if (idx == std::string::npos) 
    ss << s;
  else 
    ss << std::hex << s.substr(idx+2);
  ss >> obj.value;
  return is;
}

// output stream operator for AttributeStackBase
template <class T>
inline std::ostream& operator << (std::ostream &os, const AttributeStackBase<T> &obj) {
  os << "0x" << std::hex << *(obj.recent);
  return os;
}

// input stream operator for AttributeStackBase
template <class T>
inline std::istream& operator >> (std::istream &is, AttributeStackBase<T> &obj) {  
  // assume string to be plane number ("42") or hex number ("0x42")
  std::string::size_type idx;
  std::string s;
  std::stringstream ss;
  is >> s;
  idx = s.find("0x");
  if (idx == std::string::npos) 
    ss << s;
  else 
    ss << std::hex << s.substr(idx+2);
  ss >> *(obj.recent);
  return is;
}

 
} // end of namespace gs


#endif
