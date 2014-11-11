/*
 * tlm_serial_payload.h
 *
 * Copyright (C) 2014, GreenSocs Ltd.
 *
 * Developped by Manish Aggarwal, Ruchir Bharti
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses>.
 *
 * Linking GreenSocs code, statically or dynamically with other modules
 * is making a combined work based on GreenSocs code. Thus, the terms and
 * conditions of the GNU General Public License cover the whole
 * combination.
 *
 * In addition, as a special exception, the copyright holders, GreenSocs
 * Ltd, give you permission to combine GreenSocs code with free software
 * programs or libraries that are released under the GNU LGPL, under the
 * OSCI license, under the OCP TLM Kit Research License Agreement or
 * under the OVP evaluation license.You may copy and distribute such a
 * system following the terms of the GNU GPL and the licenses of the
 * other code concerned.
 *
 * Note that people who make modified versions of GreenSocs code are not
 * obligated to grant this special exception for their modified versions;
 * it is their choice whether to do so. The GNU General Public License
 * gives permission to release a modified version without this exception;
 * this exception also makes it possible to release a modified version
 * which carries forward this exception.
 *
 */

#ifndef __TLM_SERIAL_GP_H__
#define __TLM_SERIAL_GP_H__

#include <systemc>

using namespace tlm;

namespace tlm_serial {

class
tlm_serial_payload;

class tlm_serial_mm_interface {
public:
  virtual void free(tlm_serial_payload*) = 0;
  virtual ~tlm_serial_mm_interface() {}
};

//---------------------------------------------------------------------------
// enumeration types
//---------------------------------------------------------------------------
enum serial_command {
    SERIAL_SEND_CHAR_COMMAND,
    SERIAL_BREAK_CHAR_COMMAND
};

enum serial_response_status {
    SERIAL_OK_RESPONSE = 1,
    SERIAL_ERROR_RESPONSE = 0,
};

enum serial_enable_flag {
    SERIAL_ENABLE_NONE=0x0,
    SERIAL_PARITY_ENABLED=0x1,
    SERIAL_VALID_BITS_ENABLED=0x2,
    SERIAL_STOP_BITS_ENABLED=0x4,
    SERIAL_BAUD_RATE_ENABLED=0x8
};

//---------------------------------------------------------------------------
// The tlm serial payload class:
//---------------------------------------------------------------------------
class tlm_serial_payload {

public:
    //---------------
    // Constructors
    //---------------

    // Default constructor
    tlm_serial_payload()
        : m_command(SERIAL_SEND_CHAR_COMMAND)
        , m_data(0)
        , m_length(0)
        , m_response_status(SERIAL_OK_RESPONSE)
        , m_parity_bit(0)
        , m_valid_bits(0)
        , m_stop_bits(0)
        , m_enable_bits(SERIAL_ENABLE_NONE)
        , m_baudrate(0)
        , m_mm(0)
        , m_ref_count(0)
    {
    }

    explicit tlm_serial_payload(tlm_serial_mm_interface* mm)
        : m_command(SERIAL_SEND_CHAR_COMMAND)
        , m_data(0)
        , m_length(0)
        , m_response_status(SERIAL_OK_RESPONSE)
        , m_parity_bit(0)
        , m_valid_bits(0)
        , m_stop_bits(0)
        , m_enable_bits(SERIAL_ENABLE_NONE)
        , m_baudrate(0)
        , m_mm(mm)
        , m_ref_count(0)
    {
    }

    void acquire(){assert(m_mm != 0); m_ref_count++;}
    void release(){assert(m_mm != 0); if (--m_ref_count==0) m_mm->free(this);}
    int get_ref_count(){return m_ref_count;}
    void set_mm(tlm_serial_mm_interface* mm) { m_mm = mm; }
    bool has_mm() { return m_mm != 0; }

    void reset(){
    };


private:
    //disabled copy ctor and assignment operator.
    // Copy constructor
    tlm_serial_payload(const tlm_serial_payload& x)
        : m_command(x.get_command())
        , m_data(x.get_data_ptr())
        , m_length(x.get_data_length())
        , m_response_status(x.get_response_status())
        , m_parity_bit(x.get_parity_bits())
        , m_valid_bits(x.get_valid_bits())
        , m_stop_bits(x.get_num_stop_bits_in_end())
        , m_enable_bits(x.get_enable_bits())
        , m_baudrate(x.get_baudrate())
    {}

    // Assignment operator
    tlm_serial_payload& operator= (const tlm_serial_payload& x)
    {
        m_command =            x.get_command();
        m_data =               x.get_data_ptr();
        m_length =             x.get_data_length();
        m_response_status =    x.get_response_status();
	m_enable_bits= 		x.get_enable_bits();
	m_parity_bit =        x.get_parity_bits();
	m_valid_bits =         x.get_valid_bits();
	m_stop_bits =          x.get_num_stop_bits_in_end();
	m_baudrate =          x.get_baudrate();

        return (*this);
    }
public:
    // non-virtual deep-copying of the object
    void deep_copy_from(const tlm_serial_payload & other)
    {
        m_command =            other.get_command();
        m_data =            other.get_data_ptr();
        m_length =             other.get_data_length();
        m_response_status =    other.get_response_status();
	m_enable_bits= 		other.get_enable_bits();
	m_parity_bit =        other.get_parity_bits();
	m_valid_bits =         other.get_valid_bits();
	m_stop_bits =          other.get_num_stop_bits_in_end();
	m_baudrate =          other.get_baudrate();

        // deep copy data
        // there must be enough space in the target transaction!
        if(m_data && other.m_data)
        {
            memcpy(m_data, other.m_data, m_length);
        }
        // deep copy parity bits
        // there must be enough space in the target transaction!
        if((m_enable_bits&SERIAL_PARITY_ENABLED) && (other.m_enable_bits&SERIAL_PARITY_ENABLED))
        {
            memcpy(m_parity_bit, other.m_parity_bit, m_length);
        }
    }

    //--------------
    // Destructor
    //--------------
    virtual ~tlm_serial_payload() {
    }

    //----------------
    // API (including setters & getters)
    //---------------

    // Command related methods
    serial_command        get_command() const {return m_command;}
    void                  set_command(const serial_command command) {m_command = command;}

    // Data related methods
    unsigned char*       get_data_ptr() const {return m_data;}
    void                 set_data_ptr(unsigned char* data) {m_data = data;}

    // Transaction length (in bytes) related methods
    unsigned int         get_data_length() const {return m_length;}
    void                 set_data_length(const unsigned int length) {m_length = length;}

    // Response status related methods
    bool                 is_response_ok() const {return (m_response_status > 0);}
    bool                 is_response_error() const {return (m_response_status <= 0);}
    serial_response_status get_response_status() const {return m_response_status;}
    void                 set_response_status(const serial_response_status response_status)
			  {m_response_status = response_status;}
    std::string          get_response_string() const
    {

        switch(m_response_status)
        {
        case SERIAL_OK_RESPONSE:            return "SERIAL_OK_RESPONSE";
        case SERIAL_ERROR_RESPONSE:	    return "SERIAL_ERROR_RESPONSE";
        }
        return "SERIAL_UNKNOWN_RESPONSE";
    }

    unsigned char        get_num_stop_bits_in_end() const {return m_stop_bits;}
    void                 set_num_stop_bits_in_end(const unsigned char stop_bits) {m_stop_bits = stop_bits; }

    unsigned char        get_valid_bits() const {return m_valid_bits;}
    void                 set_valid_bits(const unsigned char valid_bits) {m_valid_bits = valid_bits; }

    uint16_t             get_enable_bits() const { return m_enable_bits; }
    void                 set_enable_bits(uint16_t enable_bits) { m_enable_bits = enable_bits; }
 
    bool		isParityEnable()  { return (m_enable_bits & SERIAL_PARITY_ENABLED); }
    bool		isValidBitEnable(){ return (m_enable_bits & SERIAL_VALID_BITS_ENABLED); }
    bool		isStopBitEnable() { return (m_enable_bits & SERIAL_STOP_BITS_ENABLED); }
    bool		isBaudRateEnable(){ return (m_enable_bits & SERIAL_BAUD_RATE_ENABLED); }

    bool*                get_parity_bits() const {return m_parity_bit;}
    void                 set_parity_bits(bool* parity_bits) {m_parity_bit = parity_bits; }

    // BaudRate update method
    unsigned int         get_baudrate() const {return m_baudrate;}
    void                 set_baudrate(unsigned int baudrate) {m_baudrate = baudrate; }

private:

    serial_command        m_command;
    unsigned char*        m_data;
    unsigned int          m_length;
    serial_response_status  m_response_status;
    bool*                 m_parity_bit;
    unsigned int          m_valid_bits;	// can be varied from 5 to 8
    unsigned char         m_stop_bits;
    uint16_t     	  m_enable_bits;
    unsigned int	  m_baudrate;

    // For memory Management
    tlm_serial_mm_interface*           m_mm;
    unsigned int                   m_ref_count;

public:

    /* --------------------------------------------------------------------- */
    /* Dynamic extension mechanism:                                          */
    /* --------------------------------------------------------------------- */
    /* The extension mechanism is intended to enable initiator modules to    */
    /* optionally and transparently add data fields to the                   */
    /* tlm_generic_payload. Target modules are free to check for extensions  */
    /* and may or may not react to the data in the extension fields. The     */
    /* definition of the extensions' semantics is solely in the              */
    /* responsibility of the user.                                           */
    /*                                                                       */
    /* The following rules apply:                                            */
    /*                                                                       */
    /* - Every extension class must be derived from tlm_extension, e.g.:     */
    /*     class my_extension : public tlm_extension<my_extension> { ... }   */
    /*                                                                       */
    /* - A tlm_generic_payload object should be constructed after C++        */
    /*   static initialization time. This way it is guaranteed that the      */
    /*   extension array is of sufficient size to hold all possible          */
    /*   extensions. Alternatively, the initiator module can enforce a valid */
    /*   extension array size by calling the resize_extensions() method      */
    /*   once before the first transaction with the payload object is        */
    /*   initiated.                                                          */
    /*                                                                       */
    /* - Initiators should use the the set_extension(e) or clear_extension(e)*/
    /*   methods for manipulating the extension array. The type of the       */
    /*   argument must be a pointer to the specific registered extension     */
    /*   type (my_extension in the above example) and is used to             */
    /*   automatically locate the appropriate index in the array.            */
    /*                                                                       */
    /* - Targets can check for a specific extension by calling               */
    /*   get_extension(e). e will point to zero if the extension is not      */
    /*   present.                                                            */
    /*                                                                       */
    /* --------------------------------------------------------------------- */

    // Stick the pointer to an extension into the vector, return the
    // previous value:
    template <typename T> T* set_extension(T* ext)
    {
        return static_cast<T*>(set_extension(T::ID, ext));
    }

    // non-templatized version with manual index:
    tlm::tlm_extension_base* set_extension(unsigned int index,
                                      tlm::tlm_extension_base* ext)
    {
        tlm::tlm_extension_base* tmp = m_extensions[index];
        m_extensions[index] = ext;
        return tmp;
    }

    // Stick the pointer to an extension into the vector, return the
    // previous value and schedule its release
    template <typename T> T* set_auto_extension(T* ext)
    {
        return static_cast<T*>(set_auto_extension(T::ID, ext));
    }

    // non-templatized version with manual index:
    tlm::tlm_extension_base* set_auto_extension(unsigned int index,
                                           tlm::tlm_extension_base* ext)
    {
        tlm::tlm_extension_base* tmp = m_extensions[index];
        m_extensions[index] = ext;
        if (!tmp) m_extensions.insert_in_cache(&m_extensions[index]);
        assert(m_mm != 0);
        return tmp;
    }

    // Check for an extension, ext will point to 0 if not present
    template <typename T> void get_extension(T*& ext) const
    {
        ext = get_extension<T>();
    }
    template <typename T> T* get_extension() const
    {
        return static_cast<T*>(get_extension(T::ID));
    }
    // Non-templatized version with manual index:
    tlm::tlm_extension_base* get_extension(unsigned int index) const
    {
        return m_extensions[index];
    }

    //this call just removes the extension from the txn but does not
    // call free() or tells the MM to do so
    // it return false if there was active MM so you are now in an unsafe situation
    // recommended use: when 100% sure there is no MM
    template <typename T> void clear_extension(const T* ext)
    {
        clear_extension<T>();
    }

    //this call just removes the extension from the txn but does not
    // call free() or tells the MM to do so
    // it return false if there was active MM so you are now in an unsafe situation
    // recommended use: when 100% sure there is no MM
    template <typename T> void clear_extension()
    {
        clear_extension(T::ID);
    }

    //this call removes the extension from the txn and does
    // call free() or tells the MM to do so when the txn is finally done
    // recommended use: when not sure there is no MM
    template <typename T> void release_extension(T* ext)
    {
        release_extension<T>();
    }

    //this call removes the extension from the txn and does
    // call free() or tells the MM to do so when the txn is finally done
    // recommended use: when not sure there is no MM
    template <typename T> void release_extension()
    {
        release_extension(T::ID);
    }

private:
    // Non-templatized version with manual index
    void clear_extension(unsigned int index)
    {
        m_extensions[index] = static_cast<tlm::tlm_extension_base*>(0);
    }
    // Non-templatized version with manual index
    void release_extension(unsigned int index)
    {
        if (m_mm)
        {
            m_extensions.insert_in_cache(&m_extensions[index]);
        }
        else
        {
            m_extensions[index]->free();
            m_extensions[index] = static_cast<tlm::tlm_extension_base*>(0);
        }
    }

public:
    // Make sure the extension array is large enough. Can be called once by
    // an initiator module (before issuing the first transaction) to make
    // sure that the extension array is of correct size. This is only needed
    // if the initiator cannot guarantee that the generic payload object is
    // allocated after C++ static construction time.
    void resize_extensions()
    {
        m_extensions.expand(max_num_extensions());
    }

private:
    tlm::tlm_array<tlm::tlm_extension_base*> m_extensions;

};

} // namespace tlm_serial

#endif 
