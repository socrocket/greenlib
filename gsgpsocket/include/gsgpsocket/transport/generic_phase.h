// LICENSETEXT
// 
//   Copyright (C) 2008 : GreenSocs Ltd
//       http://www.greensocs.com/ , email: info@greensocs.com
// 
//   Developed by :
// 
//   Robert Guenzel, Christian Schröder
//     Technical University of Braunschweig, Dept. E.I.S.
//     http://www.eis.cs.tu-bs.de
// 
// 
// The contents of this file are subject to the licensing terms specified
// in the file LICENSE. Please consult this file for restrictions and
// limitations that may apply.
// 
// ENDLICENSETEXT


#ifndef __GENERIC_PHASE_H__
#define __GENERIC_PHASE_H__


namespace gs {
  
  
  DECLARE_EXTENDED_PHASE(BEGIN_DATA);
  DECLARE_EXTENDED_PHASE(END_DATA);

  
  namespace gp {
  
    
    // The phase class keeps the same as in GreenBus
    // but MSBytesValid has been moved from the GenericTransaction
    // to the phase (fixes conceptual bug in GreenBus).
    //
    // The phase members (except the state which is the tlm 2.0 phase)
    // are transmitted within the tlm 2.0 transaction but are visible to
    // the GreenBus user in the transaction.
    //
    // The phase members are set during reception within the GB Socket
    // and during sending the phase members are put into the transaction.
    
    //---------------------------------------------------------------------------
    /**
     * A generic phase class. 
     * This is a set of phases and access functions 
     * to form the GreenBus generic protocol.
     */
    //---------------------------------------------------------------------------
    class GenericPhase  // "Phases for the Generic Protocol";
      {
      protected:

        /// Number of data atoms sent in this transaction
        // future use: gs_uint32 BurstNumber;
        
        /// valid bytes (use alternative to BurstNumber)
        gs_uint64 BytesValid;
        
      public:
        
        /// the phases of the generic protocol
        enum {
          Idle = 0,
          RequestValid,RequestAccepted,RequestError,
          DataValid, DataAccepted, DataError,
          ResponseValid,ResponseAccepted, ResponseError,
          LAST_GENERIC_PHASE
        };

        /// DEPRECATED, use to_string() instead!
        inline std::string toString() {
          return to_string();
        }
        
        /// Returns the string represenation of this phase
        std::string to_string() {
          std::string ret;
          switch (state) {
            case Idle: ret = "Idle"; break;
            case RequestValid: ret = "RequestValid"; break;
            case RequestAccepted: ret = "RequestAccepted"; break;
            case RequestError: ret = "RequestError"; break;
            case DataValid: ret = "DataValid"; break;
            case DataAccepted: ret = "DataAccepted"; break;
            case DataError: ret = "DataError"; break;
            case ResponseValid: ret = "ResponseValid"; break;
            case ResponseAccepted: ret = "ResponseAccepted"; break;
            case ResponseError: ret = "ResponseError"; break;
            default:
              ret = "UNKNOWN PHASE";
          }
          return ret;
        }
        
        /// phase
        mutable gs_uint32 state;
        
        
        /**
         * Create phase with default state (Idle).
         */
        GenericPhase() 
        : /* future use: BurstNumber(0),*/ BytesValid(0), state(Idle)
        {}
        
        GenericPhase(sc_core::sc_event &_initiator_update_event, sc_core::sc_event &_target_update_event) 
        : /* future use: BurstNumber(0),*/ BytesValid(0), state(Idle)
        {}
        
        
        /**
         * Create phase.
         * @param s Phase state
         */
        GenericPhase(gs_uint32 s)
        : /* future use: BurstNumber(0),*/ BytesValid(0), state(s)
        {}
        
        /**
         * Create phase.
         * @param s Phase state
         * @param b Burst number
         */
        GenericPhase(gs_uint32 s, gs_uint32 b)
        : /* future use: BurstNumber(b),*/ BytesValid(0), state(s)
        {}
        
        /// copy constructor
        GenericPhase(const GenericPhase &p) {
          *this = p; // use copy-operator
        }
        
        /// copy operator
        GenericPhase & operator=(const GenericPhase &p) {
          if (&p==this)
            return *this;
          state=p.state;
          // future use: BurstNumber=p.BurstNumber;
          BytesValid=p.BytesValid;
          return *this;
        }
        
        /// has a request atom been sent by the master?
        inline bool isRequestValid() { return state == RequestValid; }
        /// has a request atom been accepted by the slave?
        inline bool isRequestAccepted() { return state == RequestAccepted; }
        /// has the slave replied an error on a master's request atom?
        inline bool isRequestError() { return state == RequestError; }
        /// has a data atom been sent by the master?
        inline bool isDataValid() { return state == DataValid; }
        /// has a data atom been accepted by the slave?
        inline bool isDataAccepted() { return state == DataAccepted; }
        /// has the slave replied an error on a master's data atom?
        inline bool isDataError() { return state == DataError; }
        /// has a response atom been sent by the slave?
        inline bool isResponseValid() { return state == ResponseValid; }
        /// has a response atom been accepted by the master?
        inline bool isResponseAccepted() { return state == ResponseAccepted; }
        /// has the master replied an error on a slave's response atom?
        inline bool isResponseError() { return state == ResponseError; }
        
        inline void setBytesValid(const gs_uint64 bv) { BytesValid = bv; }
        inline gs_uint64 getBytesValid() const { return BytesValid; }
        
      };
    
    
    
#define REQUESTPHASE gs::gp::GenericPhase::RequestValid, gs::gp::GenericPhase::RequestAccepted, gs::gp::GenericPhase::RequestError
#define DATAPHASE gs::gp::GenericPhase::DataValid, gs::gp::GenericPhase::DataAccepted, gs::gp::GenericPhase::DataError
#define RESPONSEPHASE gs::gp::GenericPhase::ResponseValid, gs::gp::GenericPhase::ResponseAccepted, gs::gp::GenericPhase::ResponseError
    
#define INITATTRIBUTE(name)                     \
  name.setName(#name)
    
    
  } // end namespace gp
  
} // end namespace gs

#endif
