// LICENSETEXT
// 
//   Copyright (C) 2008-2009 : GreenSocs Ltd
//       http://www.greensocs.com/ , email: info@greensocs.com
// 
//   Developed by :
// 
//   Robert Guenzel, Christian Schroeder
//     Technical University of Braunschweig, Dept. E.I.S.
//     http://www.eis.cs.tu-bs.de
// 
// 
// The contents of this file are subject to the licensing terms specified
// in the file LICENSE. Please consult this file for restrictions and
// limitations that may apply.
// 
// ENDLICENSETEXT

#ifndef __GENERIC_TRANSACTION_H__
#define __GENERIC_TRANSACTION_H__

#include <stdarg.h>
#include "../utils/gs_datatypes.h"
#include "../core/attributes.h"
#include "../utils/unevenpair.h"

#include "greensocket/generic/gs_extension.h"
#include "gsgpsocket/transport/generic_phase.h"
#include "greensocket/initiator/single_socket.h"


namespace gs {

  GS_GUARD_ONLY_EXTENSION(lock);
  GS_GUARD_ONLY_EXTENSION(semaphore);
  GS_GUARD_ONLY_EXTENSION(nonposted);
  GS_GUARD_ONLY_EXTENSION(broadcast);
  GS_GUARD_ONLY_EXTENSION(wr_resp_dummy); // dummy (behaviour) extension only needed for binding information, does not transport any data
  
  SINGLE_MEMBER_DATA(mID, MID);
  SINGLE_MEMBER_DATA(error_code, Error);
  SINGLE_MEMBER_DATA(transactionID, TransID);

  // The following fields are TLM2.0 transaction members that are copied by the GSGP Socket
  // to/from the GenericProtocol phase during receiving/sending. 
  SINGLE_MEMBER_DATA(bytes_valid, gs_uint64/*MSBytesValid*/); // This is not accessible via the GP transaction but via the GP phase!
  SINGLE_MEMBER_DATA(burst_number, gs_uint32); // This is not accessible via the GP transaction but via the GP phase!
  //SINGLE_MEMBER_DATA(burst_number_vec, ...); // TODO: This is not accessible via the GP transaction but via the GP phase!

  SINGLE_MEMBER_GUARDED_DATA(simulated_length, gs_uint32);
  
  /// Handle class that simply converts the -> operator to . operations
  template <typename T>
  class Handle: public T {
    typedef T base_type;
  public:
    Handle() 
    : base_type(NULL, NULL) 
    { 
    }

    Handle(socket::extension_support_base<tlm::tlm_base_protocol_types> *socket, tlm::tlm_generic_payload *tlmtr)
    : base_type(socket, tlmtr) 
    {
      base_type::m_ext_support=socket;
      base_type::m_tb_txn=tlmtr;
    }

    base_type* operator->(){ return this; }
    
    base_type& get() { return *this; }
  };

  
  namespace gp {

    /*class GenericTransaction;
    class GenericRouterAccess;
    class GenericSlaveAccess;
    class GenericMasterAccess;*/
    
    /*typedef Handle<GenericTransaction>  GenericTransactionHandle;
    typedef Handle<GenericRouterAccess> GenericRouterAccessHandle;
    typedef Handle<GenericSlaveAccess>  GenericSlaveAccessHandle;
    typedef Handle<GenericMasterAccess> GenericMasterAccessHandle;*/
    
  
    /// Transaction class
    /**
     * A generic transaction class.
     * This is the GreenBus transaction container.
     * Access with GenericMasterAcessHandle, GenericSlaveAccessHandle, 
     * and GenericRouterAccessHandle
     *
     * How GreenBus MData is mapped to this (and the tlm) transaction:
     *  The tlm transaction has a DATA_ONLY_EXTENSION voidPtr which is used only in the case of
     *  the GreenBus pointer mode.
     *  If the pointer mode is used, the tlm data_ptr is set to NULL (which is illeagal in tlm2.0).
     *  In this case the receiving GPSocket/GenericTransaction knows that the pointer in the extension
     *  is being used.
     *  If the normal mode is used, the pointer to the vector is submitted within the tlm data_ptr.
     *  The data_length is equivalent to the GreenBus burst_length.
     *
     */
    class GenericTransaction
    {
      
    protected:
      
      // socket
      socket::extension_support_base<tlm::tlm_base_protocol_types> *m_ext_support;
      
      // quarks
      tlm::tlm_generic_payload *m_tb_txn;
      
      /// member to be able to return a reference in getMData
      MData tmpData;
      /// member to be able to set this pseudo-vector as data to a GSDataType
      GSDataType::dtype tmpDataData;
      
    public:

      /// If this transaction contains a not released tlm transaction. This must not be used if alive==false!
      bool alive;
     
    public: // legacy protected: setMID, setMCmd, setMAddr, setMData, setMBurstLength, setSResp, setSData, setTransID,
            //                   getMAddr, getMID, getMCmd, getMData, getMBurstLength, getSBurstLength, getSResp, getSData, getTransID,
            //                   setMError, getMError, setSError, getSError

      // //////////////////// quark access //////////////////////////////////////////////// //
      
      /// set master id
      inline void setMID(const MID& _mID) { m_ext_support->get_extension<mID>(*m_tb_txn)->value = _mID; }
      /// get master id
      inline const MID& getMID() { return m_ext_support->get_extension<mID>(*m_tb_txn)->value; }

      /// set master command (see GenericMCmdType)
      inline void setMCmd(const MCmd& _mCmd) { 
        switch ((unsigned int)_mCmd) {
          case Generic_MCMD_WR: // tlm write
            m_tb_txn->set_command(tlm::TLM_WRITE_COMMAND);
            break;
          case Generic_MCMD_RD: // tlm read
            m_tb_txn->set_command(tlm::TLM_READ_COMMAND);
            break;
          case Generic_MCMD_RDEX: // tlm read + lock
            m_tb_txn->set_command(tlm::TLM_READ_COMMAND);
            m_ext_support->validate_extension<lock>(*m_tb_txn); // set lock guard
            break;
          case Generic_MCMD_RDL: // tlm read + semaphore
            m_tb_txn->set_command(tlm::TLM_READ_COMMAND);
            m_ext_support->validate_extension<semaphore>(*m_tb_txn);
            break;
          case Generic_MCMD_WRNP: // tlm write + nonposted
            m_tb_txn->set_command(tlm::TLM_WRITE_COMMAND);
            m_ext_support->validate_extension<nonposted>(*m_tb_txn);
            break;
          case Generic_MCMD_WRC: // tlm write + semaphore
            m_tb_txn->set_command(tlm::TLM_WRITE_COMMAND);
            m_ext_support->validate_extension<semaphore>(*m_tb_txn);
            break;
          case Generic_MCMD_BCST: // tlm read + broadcast
            m_tb_txn->set_command(tlm::TLM_READ_COMMAND);
            m_ext_support->validate_extension<broadcast>(*m_tb_txn);
            break;
          default:
            m_tb_txn->set_command(tlm::TLM_IGNORE_COMMAND);
            //SC_REPORT_WARNING("GenericTransaction", "Called setMCmd(...) with unknown command.");
            break;
        }
      }
      /// get master cmd
      inline const MCmd getMCmd() {
        switch (m_tb_txn->get_command()) {
          case tlm::TLM_READ_COMMAND: {
            if (m_ext_support->get_extension<lock>(*m_tb_txn))
              return Generic_MCMD_RDEX;
            else if (m_ext_support->get_extension<semaphore>(*m_tb_txn))
              return Generic_MCMD_WRC;
            else if (m_ext_support->get_extension<broadcast>(*m_tb_txn))
              return Generic_MCMD_BCST;
            else 
              return Generic_MCMD_RD;
            break;
          }
          case tlm::TLM_WRITE_COMMAND: {
            if (m_ext_support->get_extension<nonposted>(*m_tb_txn))
              return Generic_MCMD_WRNP;
            else if(m_ext_support->get_extension<semaphore>(*m_tb_txn))
              return Generic_MCMD_WRC;
            else
              return Generic_MCMD_WR;
            break;
          }
          default:
            return Generic_MCMD_IDLE;
        }
        m_ext_support->get_extension<lock >(*m_tb_txn);
      }
      
      /// set response information
      inline void setSResp(const SResp& _sResp){ 
        // Convert GreenBus GenericSRespType -> TLM response_status
        GenericSRespType resp = _sResp;
        switch (resp) {
          case gs::Generic_SRESP_DVA: // Data valid/accept response
            m_tb_txn->set_response_status(tlm::TLM_OK_RESPONSE);
            break;
          case gs::Generic_SRESP_NULL: // Null response
            m_tb_txn->set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);
            break;
          case gs::Generic_SRESP_ERR: // Error response
            m_tb_txn->set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
            break;
          case gs::Generic_SRESP_FAIL: // Request failed
            m_tb_txn->set_response_status(tlm::TLM_COMMAND_ERROR_RESPONSE);
            break;
            // not mapped: tlm::TLM_ADDRESS_ERROR_RESPONSE, tlm::TLM_BURST_ERROR_RESPONSE, tlm::TLM_BYTE_ENABLE_ERROR_RESPONSE:
          default:
            m_tb_txn->set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);
            break;
        }      
      }
      /// get the slave response (see GenericSRespType)
      inline const SResp getSResp() const { 
        // Convert TLM response_status -> GreenBus GenericSRespType
        switch (m_tb_txn->get_response_status()) {
          case tlm::TLM_OK_RESPONSE:
            return gs::Generic_SRESP_DVA;       //Data valid/accept response
          case tlm::TLM_INCOMPLETE_RESPONSE:
            return gs::Generic_SRESP_NULL;      //Null response
          case tlm::TLM_COMMAND_ERROR_RESPONSE:
            return gs::Generic_SRESP_FAIL;      //Request failed
          case tlm::TLM_GENERIC_ERROR_RESPONSE:
          case tlm::TLM_ADDRESS_ERROR_RESPONSE:
          case tlm::TLM_BURST_ERROR_RESPONSE:
          case tlm::TLM_BYTE_ENABLE_ERROR_RESPONSE:
            return gs::Generic_SRESP_ERR;       //Error response
          default:
            return Generic_SRESP_NULL;
        }      
      }
      
      /// set transaction ID (only use if you want to overwrite default value, which is generated by the port) 
      inline void setTransID(const TransID& _transID) { m_ext_support->get_extension<transactionID>(*m_tb_txn)->value = _transID; }
      /// get the transaction ID (set by create_transaction)
      /*virtual*/ const TransID& getTransID() { return m_ext_support->get_extension<transactionID>(*m_tb_txn)->value; }
      
      
      /// set target address
      inline void setMAddr(const MAddr& _mAddr) { m_tb_txn->set_address(_mAddr); }
      /// get target address
      inline const MAddr getMAddr() const { return m_tb_txn->get_address(); }

      /// set master burst length ('0' implies variable length burst, then you need to set MBurstLength to the overall transaction length with the last data/response atom)
      inline void setMBurstLength(const MBurstLength& _mBurstLength) {  
        //mBurstLength=_mBurstLength; 
        //msBytesValid=_mBurstLength;
        simulated_length* sim_len;
        if (m_ext_support->get_extension<simulated_length>(sim_len, *m_tb_txn)){ //setMData called prior to setBurstLength
          sim_len->value=_mBurstLength; //so data length has already been set
        }
        else
          m_tb_txn->set_data_length(_mBurstLength);
      }
      /// get the master burst length
      inline const MBurstLength getMBurstLength() const { 
        simulated_length* sim_len;
        if (m_ext_support->get_extension<simulated_length>(sim_len, *m_tb_txn)){
          return sim_len->value;
        }
        else
          return m_tb_txn->get_data_length(); 
      }
      /// get slave burst length
      inline const SBurstLength getSBurstLength() const {
        simulated_length* sim_len;
        if (m_ext_support->get_extension<simulated_length>(sim_len, *m_tb_txn)){
          return sim_len->value;
        }
        else
          return m_tb_txn->get_data_length(); 
      }
      
      /// set the master error (see GenericError)
      inline void setMError(const Error& _mError) { m_ext_support->get_extension<error_code>(*m_tb_txn)->value = _mError; }
      /// get the master error (see GenericError)
      inline const Error getMError() { return m_ext_support->get_extension<error_code>(*m_tb_txn)->value; }

      /// set the slave error (see GenericError)
      inline void setSError(const Error& _sError) { m_ext_support->get_extension<error_code>(*m_tb_txn)->value = _sError; }
      /// get the slave error (see GenericError)
      inline const Error getSError() { return m_ext_support->get_extension<error_code>(*m_tb_txn)->value; }

      /// set number of valid bytes with this atom
      /* BUGFIX: MOVED TO PHASE
       inline void setMSBytesValid(const MSBytesValid& _msBytesValid) {
       m_ext_support->get_extension<msBytesValid>(*m_tb_txn)->value = _msBytesValid; 
       }*/
      /// get the number of valid data bytes in this atom
      /* BUGFIX: MOVED TO PHASE
       inline const MSBytesValid getMSBytesValid() { return m_ext_support->get_extension<msBytesValid>(*m_tb_txn)->value; }
       */
      
      // //////////////// Data quark access functions ///////////////////// //
      
      /// set master data (i.e. set the data pointer in the transaction container to the given GSDataType)
      inline void setMData(const GSDataType& _mData) {
        // BARTHO: GSDataType always synchronizes the char* and void*
        if (_mData.isPointer()){
          m_tb_txn->set_data_ptr((unsigned char*)_mData.getPointer());
          simulated_length* sim_len;
          m_ext_support->get_extension<simulated_length>(sim_len, *m_tb_txn);
          if (m_tb_txn->get_data_length()){ //burst length set prior to setMData
            
            
            sim_len->value=m_tb_txn->get_data_length();
          }
          m_tb_txn->set_data_length(_mData.getSize()); 
          m_ext_support->validate_extension<simulated_length>(*m_tb_txn);
        }
        else{
          m_tb_txn->set_data_ptr(_mData.getData().getData());
          // Data length will be set by user by calling setMBurstLength()
          m_ext_support->invalidate_extension<simulated_length>(*m_tb_txn);
        }
      }
      /// get access to the transaction data (see GSDataType)
      inline MData& getMData() /*const*/ {
        this->getMData(tmpData);
        return tmpData;
      }
      inline void getMData(MData & _dst ) { 
        // BARTHO: GSDataType always synchronizes the char* and void*
        simulated_length* sim_len;
        if (m_ext_support->get_extension<simulated_length>(sim_len, *m_tb_txn)){
          _dst.setPointer(m_tb_txn->get_data_ptr(), m_tb_txn->get_data_length());
        }
        else{
          GSDataType::dtype tmpDat;
          tmpDat.setData(m_tb_txn->get_data_ptr(), m_tb_txn->get_data_length());
          _dst.setOwnData(tmpDat);
        }
      }
      
      /// set slave read data
      inline void setSData(MData& _sData) { 
        // check if target size is big enough
        assert(m_tb_txn->get_data_length() >= _sData.getData().size());
        // copy the vector data to the tlm unsigned char array (should work)
        memcpy(m_tb_txn->get_data_ptr(), _sData.getData().getData(), _sData.getData().size());
      }
      /// get access to the transaction data (see GSDataType)
      inline MData& getSData() { 
        return getMData(); 
      }
      
    public:
          
      // construction /////////////////////////////////////////////////////////////
      
      /** 
       * Create a generic transaction. You should not use this constructor
       * directly. Always use master_port::createTransaction().
       */  
      GenericTransaction(socket::extension_support_base<tlm::tlm_base_protocol_types> *socket, tlm::tlm_generic_payload *tlmtr)
      : m_ext_support(socket)
      , m_tb_txn(tlmtr)
      , alive(true)
      /*: mBurstLength(0),  // only init important flags
      sBurstLength(0),
      mError(0), 
      sError(0),
      is_extended(false), 
      extendedType(NULL),
      attribs(NULL),
      extended_attribs(NULL)*/
      {
        if (tlmtr == NULL) alive = false;
      }
      

      /** 
       * Create an empty generic transaction. You should not use this constructor
       * directly. Always use master_port::createTransaction().
       */  
      explicit GenericTransaction()
      : m_ext_support(NULL)
      , m_tb_txn(NULL)
      , alive(false)
      {
      }
      
      /// copy constructor
      GenericTransaction(const GenericTransaction &t) {
        *this = t; // use copy-operator
      }
      
      void reset(){
        m_tb_txn->set_data_length(0);
        m_tb_txn->set_data_ptr(NULL);
        m_ext_support->invalidate_extension<lock>(*m_tb_txn);
        m_ext_support->invalidate_extension<semaphore>(*m_tb_txn);
        m_ext_support->invalidate_extension<nonposted>(*m_tb_txn);
        m_ext_support->invalidate_extension<broadcast>(*m_tb_txn);
        setSError(gs::GenericError::Generic_Error_NoError);
        setMError(gs::GenericError::Generic_Error_NoError);
        // do not reset alive!
        
        setSResp(gs::Generic_SRESP_NULL);
        /*mBurstLength=0;
        sBurstLength=0;
        mError=0;
        sError=0;*/
      }
      
      /// destructor
      ~GenericTransaction() {
        // TODO?
      }
      
      /// copy operator
      GenericTransaction& operator=(const GenericTransaction &t) {
        if (&t==this)
          return *this;
        
        m_ext_support = t.m_ext_support;
        m_tb_txn = t.m_tb_txn;
        alive = t.alive;
        
        return *this;
      }
      
      
      
      // get an access handle to this transaction /////////////////////////////////
      // ATTENTION: implemented in TAIL section
      /// Get the router access set to this transaction
      /*inline GenericRouterAccessHandle &getRouterAccess();
      /// Get the master access set to this transaction
      inline GenericMasterAccessHandle &getMasterAccess();
      /// Get the target access set to this transaction
      inline GenericSlaveAccessHandle &getTargetAccess();*/
      
      /// return the undelying tlm transaction
      inline tlm::tlm_generic_payload* get_tlm_transaction() {
        return m_tb_txn;
      }
      
      /// Returns the extension support base of this transaction
      inline socket::extension_support_base<tlm::tlm_base_protocol_types>* get_extension_support_base() const {
        return m_ext_support;
      }

    };
    
    
    /// Interface to access the generic transaction through the master.
    /**
     * A master can get all  attributes, but can set only master attributes.
     */
    class GenericMasterAccess 
    : public GenericTransaction
    {
    public:
      GenericMasterAccess() : GenericTransaction(NULL, NULL) { }
      //GenericMasterAccessHandle(tlm::tlm_mm_interface* mm, socket::extension_support_base<tlm::tlm_base_protocol_types> *socket)
      //: GenericTransaction(mm, socket) { }
      GenericMasterAccess(socket::extension_support_base<tlm::tlm_base_protocol_types> *socket, tlm::tlm_generic_payload *tlmtr)
      : GenericTransaction(socket, tlmtr) { }
      /*GenericMasterAccess(socket::initiator_socket<> *socket, tlm::tlm_generic_payload *tlmtr)
      : GenericTransaction(socket, tlmtr) { }*/
      
      /// Deprecated (dummy): Get the master access set to this transaction
      inline GenericMasterAccess& getMasterAccess() { return *this; }
      // Deprecated: Imitates the boost shared ptr get() function
      inline GenericMasterAccess* get() { return this; }
      
      using GenericTransaction::setMAddr;
      using GenericTransaction::setMBurstLength;
      using GenericTransaction::setMCmd;
      using GenericTransaction::setMData;
      using GenericTransaction::setMID;
      using GenericTransaction::setMError;
      using GenericTransaction::setTransID;
      
      using GenericTransaction::getMAddr;
      using GenericTransaction::getMBurstLength;
      using GenericTransaction::getMCmd;
      using GenericTransaction::getMData;
      using GenericTransaction::getMID;
      using GenericTransaction::getSResp;
      using GenericTransaction::getSData;
      using GenericTransaction::getMError;
      using GenericTransaction::getSError;
      using GenericTransaction::getTransID;
      
    protected:
      using GenericTransaction::setSResp;
      using GenericTransaction::setSData;
      using GenericTransaction::getSBurstLength;
      using GenericTransaction::setSError;
    };
    
    
    /// Interface to access the generic transaction through the slave.
    /**
     * A slave can get all attributes, but can set only slave attributes.
     */
    class GenericSlaveAccess
    : public GenericTransaction
    {
    public:
      GenericSlaveAccess() : GenericTransaction(NULL, NULL) { }
      GenericSlaveAccess(socket::extension_support_base<tlm::tlm_base_protocol_types> *socket, tlm::tlm_generic_payload *tlmtr)
      : GenericTransaction(socket, tlmtr) { }
      /*GenericSlaveAccess(socket::initiator_socket<> *socket, tlm::tlm_generic_payload *tlmtr)
      : GenericTransaction(socket, tlmtr) { }*/
      
      /// Deprecated (dummy): Get the target access set to this transaction
      inline GenericSlaveAccess& getTargetAccess() { return *this; }
      // Deprecated: Imitates the boost shared ptr get() function
      inline GenericSlaveAccess* get() { return this; }
      
      using GenericTransaction::setSData;
      using GenericTransaction::setSResp;
      using GenericTransaction::setSError;
      
      using GenericTransaction::getMAddr;
      using GenericTransaction::getMBurstLength;
      using GenericTransaction::getMCmd;
      using GenericTransaction::getMData;
      using GenericTransaction::getMID;
      using GenericTransaction::getSResp;
      using GenericTransaction::getSData;
      using GenericTransaction::getMError;
      using GenericTransaction::getSError;
      using GenericTransaction::getTransID;
      
    protected:
      using GenericTransaction::setMID;
      using GenericTransaction::setMCmd;
      using GenericTransaction::setMAddr;
      using GenericTransaction::setMData;
      using GenericTransaction::setMBurstLength;
      using GenericTransaction::setTransID;
      using GenericTransaction::getSBurstLength;
      using GenericTransaction::setMError;
    };
    
    
    /// Interface to access the generic transaction through a bidirectional socket.
    /**
     * A bidirectional device can get all attributes, and can set master and slave attributes.
     */
    class GenericBidirectionalAccess 
    : public GenericTransaction
    {
    public:
      GenericBidirectionalAccess() : GenericTransaction(NULL, NULL) { }
      //GenericBidirectionalAccessHandle(tlm::tlm_mm_interface* mm, socket::extension_support_base<tlm::tlm_base_protocol_types> *socket)
      //: GenericTransaction(mm, socket) { }
      GenericBidirectionalAccess(socket::extension_support_base<tlm::tlm_base_protocol_types> *socket, tlm::tlm_generic_payload *tlmtr)
      : GenericTransaction(socket, tlmtr) { }
      /*GenericBidirectionalAccess(socket::initiator_socket<> *socket, tlm::tlm_generic_payload *tlmtr)
       : GenericTransaction(socket, tlmtr) { }*/
      
      /// Deprecated (dummy): Get the master access set to this transaction
      inline GenericBidirectionalAccess& getMasterAccess() { return *this; }
      // Deprecated: Imitates the boost shared ptr get() function
      inline GenericBidirectionalAccess* get() { return this; }
      
      using GenericTransaction::setMAddr;
      using GenericTransaction::setMBurstLength;
      using GenericTransaction::setMCmd;
      using GenericTransaction::setMData;
      using GenericTransaction::setMID;
      using GenericTransaction::setMError;
      using GenericTransaction::setTransID;
      
      using GenericTransaction::getMAddr;
      using GenericTransaction::getMBurstLength;
      using GenericTransaction::getMCmd;
      using GenericTransaction::getMData;
      using GenericTransaction::getMID;
      using GenericTransaction::getSResp;
      using GenericTransaction::getSData;
      using GenericTransaction::getMError;
      using GenericTransaction::getSError;
      using GenericTransaction::getTransID;
      
      using GenericTransaction::setSResp;
      using GenericTransaction::setSData;
      using GenericTransaction::setSError;

    protected:
      using GenericTransaction::getSBurstLength;
    };

    //---------------------------------------------------------------------------
    /**
     * Interface to access the generic transaction through the router.
     * A router can only "see" the protocol attributes. It is not authorized
     * to access the transaction data.
     */
    //---------------------------------------------------------------------------
    class GenericRouterAccess
    : public GenericTransaction
    {
    public:
      GenericRouterAccess(socket::extension_support_base<tlm::tlm_base_protocol_types> *socket, tlm::tlm_generic_payload *tlmtr)
      : GenericTransaction(socket, tlmtr) { }

      using GenericTransaction::getMID;
      using GenericTransaction::getMCmd;
      using GenericTransaction::getMAddr;
      using GenericTransaction::getMBurstLength;
      using GenericTransaction::getMError;
      using GenericTransaction::getSError;
      
      using GenericTransaction::setMError;
      using GenericTransaction::setSError;
      using GenericTransaction::getTransID;
      
    protected:
      using GenericTransaction::setSResp;
      using GenericTransaction::setMID;
      using GenericTransaction::setMCmd;
      using GenericTransaction::setMAddr;
      using GenericTransaction::setMData;
      using GenericTransaction::setMBurstLength;
      using GenericTransaction::setSData;
      using GenericTransaction::setTransID;
      using GenericTransaction::getMData;
      using GenericTransaction::getSResp;
      using GenericTransaction::getSBurstLength;
      using GenericTransaction::getSData;
    };

    typedef Handle<GenericTransaction>  GenericTransactionHandle;
    typedef Handle<GenericRouterAccess> GenericRouterAccessHandle;
    typedef Handle<GenericSlaveAccess>  GenericSlaveAccessHandle;
    typedef Handle<GenericMasterAccess> GenericMasterAccessHandle;
    typedef Handle<GenericBidirectionalAccess> GenericBidirectionalAccessHandle;
    
// has been moved to the socket include files and renamed to master_atom and slave_atom
//    typedef unevenpair<GenericTransactionHandle, GenericPhase> GS_ATOM;
//#ifdef __GNUC__
//    typedef GS_ATOM ATOM;
//#endif

    /**
     * Helper function to get access to the transaction from an access handle.
     */
    template <class T>
    inline GenericTransactionHandle _getTransactionAccess(const T &a) { return a; }

  } // end namespace gp

} // end namespace gs

#endif
