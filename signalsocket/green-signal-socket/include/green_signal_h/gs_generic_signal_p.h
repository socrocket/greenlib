#ifndef GS_GENERIC_SIGNAL_P_H_
#define GS_GENERIC_SIGNAL_P_H_

//******************************************************************
//@file gs_generic_signal_p.h
//
// @brief This file creates a new payload gs_generic_signal_payload and new
// protocol type class for Green Signal Interface, which is a generic
// signal bus
//
// 
//
//==================================================================
// Author:
// Ashwani Singh, CircuitSutra Technologies
//
//==================================================================

#include "tlm.h"

namespace gs_generic_signal {

class gs_generic_signal_payload;

struct gs_generic_signal_mm_interface{
  virtual void free(gs_generic_signal_payload*)=0;
  virtual ~gs_generic_signal_mm_interface(){}
};
/*
//---------------------------------------------------------------------------
// Classes and helper functions for the extension mechanism
//---------------------------------------------------------------------------
// Helper function:
inline unsigned int max_num_extensions(bool increment=false)
{
    static unsigned int max_num = 0;
    if (increment) ++max_num;
    return max_num;
}

// This class can be used for storing pointers to the extension classes, used
// in gs_generic_signal_payload:
class gs_generic_signal_extension_base
{
public:
    virtual gs_generic_signal_extension_base* clone() const = 0;
    virtual void free() { delete this; }
    virtual void copy_from(gs_generic_signal_extension_base const &) = 0;
protected:
    virtual ~gs_generic_signal_extension_base() {}
    static unsigned int register_extension()
    {
        return (max_num_extensions(true) - 1);
    };
};

// Base class for all extension classes, derive your extension class in
// the following way:
// class my_extension : public gs_generic_signal_extension<my_extension> { ...
// This triggers proper extension registration during C++ static
// contruction time. my_extension::ID will hold the unique index in the
// gs_generic__signal_payload::m_extensions array.
template <typename T>
class gs_generic_signal_extension : public gs_generic_signal_extension_base
{
public:
    virtual gs_generic_signal_extension_base* clone() const = 0;
    virtual void copy_from(gs_generic_signal_extension_base const &ext) = 0; //{assert(typeid(this)==typeid(ext)); assert(ID === ext.ID); assert(0);}
    virtual ~gs_generic_signal_extension() {}
    const static unsigned int ID;
};

template <typename T>
const
unsigned int gs_generic_signal_extension<T>::ID = gs_generic_signal_extension_base::register_extension();
*/
//---------------------------------------------------------------------------
// enumeration types
//---------------------------------------------------------------------------
class gs_generic_signal_payload {

public:
    //---------------
    // Constructors
    //---------------

    // Default constructor
	gs_generic_signal_payload():
		 m_data_ptr(NULL),
		 m_src_id(0),
	    m_ack_required(false),
        m_extensions(tlm::max_num_extensions()),
        m_mm(0),
        m_ref_count(0){}
    
    explicit gs_generic_signal_payload(gs_generic_signal_mm_interface *mm)
        :m_data_ptr(NULL)
        ,m_src_id(0)
		 ,m_ack_required(false)
         ,m_extensions(tlm::max_num_extensions())
        , m_mm(mm)
        , m_ref_count(0)
    {
    }

    void acquire(){assert(m_mm != 0); m_ref_count++;}
    void release(){assert(m_mm != 0); if (--m_ref_count==0) m_mm->free(this);}
    int get_ref_count() const {return m_ref_count;}
    void set_mm(gs_generic_signal_mm_interface* mm) { m_mm = mm; }
    bool has_mm() const { return m_mm != 0; }

    void reset(){
      //should the other members be reset too?
      m_extensions.free_entire_cache();
    };

         void set_response_status(const tlm::tlm_response_status)
        {
        //m_response_status = status;
        }


private:
    //disabled copy ctor 
    // Copy constructor
	gs_generic_signal_payload(const gs_generic_signal_payload& x):
	m_data_ptr(x.m_data_ptr),
	m_src_id(x.m_src_id),
	m_ack_required(x.m_ack_required)
	
    {
		//copy all extensions
		for(unsigned int i=0; i<m_extensions.size(); i++)
        {
            m_extensions[i] = x.get_extension(i);
        }
    }

    // Assignment operator
	gs_generic_signal_payload& operator= (const gs_generic_signal_payload& x)
    {
		if(&x!= this)
		{
		  m_data_ptr=x.m_data_ptr;
		  m_src_id=x.m_src_id;
		  m_ack_required= x.m_ack_required;
		      // extension copy: all extension arrays must be of equal size by
              // construction (i.e. it must either be constructed after C++
              // static construction time, or the resize_extensions() method must
              // have been called prior to using the object)
  			for(unsigned int i=0; i<m_extensions.size(); i++)
  			{
  				m_extensions[i] = x.get_extension(i);
  			}
		}
    	return(*this);
    }
public:
    // non-virtual deep-copying of the object
    void deep_copy_from(const gs_generic_signal_payload & other)
    {
        // deep copy extensions (sticky and non-sticky)
        for(unsigned int i=0; i<other.m_extensions.size(); i++)
        {
            if(other.m_extensions[i])
            {                       //original has extension i
                if(!m_extensions[i])
                {                   //We don't: clone.
                	tlm::tlm_extension_base *ext = other.m_extensions[i]->clone();
                    if(ext)     //extension may not be clonable.
                    {
                        if(has_mm())
                        {           //mm can take care of removing cloned extensions
                            set_auto_extension(i, ext);
                        }
                        else
                        {           // no mm, user will call free_all_extensions().
                            set_extension(i, ext);
                        }
                    }
                }
                else
                {                   //We already have such extension. Copy original over it.
                    m_extensions[i]->copy_from(*other.m_extensions[i]);
                }
            }
        }
    }

    void update_extensions_from(const gs_generic_signal_payload & other)
    {
        // deep copy extensions that are already present
        for(unsigned int i=0; i<other.m_extensions.size(); i++)
        {
            if(other.m_extensions[i])
            {                       //original has extension i
                if(m_extensions[i])
                {                   //We have it too. copy.
                    m_extensions[i]->copy_from(*other.m_extensions[i]);
                }
            }
        }
    }

    // Free all extensions. Useful when reusing a cloned transaction that doesn't have memory manager.
    // normal and sticky extensions are freed and extension array cleared.
    void free_all_extensions()
    {
        m_extensions.free_entire_cache();
        for(unsigned int i=0; i<m_extensions.size(); i++)
        {
            if(m_extensions[i])
            {
                m_extensions[i]->free();
                m_extensions[i] = 0;
            }
        }
    }
    //--------------
    // Destructor
    //--------------
    virtual ~gs_generic_signal_payload() {
        for(unsigned int i=0; i<m_extensions.size(); i++)
            if(m_extensions[i]) m_extensions[i]->free();
    }
    //----------------
    // API (including setters & getters)
    //---------------
    void set_src_id(unsigned int &id){ m_src_id=id;}
    unsigned int get_src_id ()const{return m_src_id;}
    
    bool is_ack_required()const{return m_ack_required;}
    void set_ack_requirement(bool & flag){m_ack_required=flag;}
    bool get_ack_requirement()const{return m_ack_required;}
    
    void set_data_ptr(unsigned char *data){m_data_ptr =data;}
    unsigned char* get_data_ptr()const{return m_data_ptr;}

private:
	unsigned char *m_data_ptr;
	unsigned int m_src_id;
	bool m_ack_required;
	
public:
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
        m_extensions.expand(tlm::max_num_extensions());
    }

private:
    tlm::tlm_array<tlm::tlm_extension_base*> m_extensions;
    gs_generic_signal_mm_interface*              m_mm;
    unsigned int                   m_ref_count;
};

}
#endif /*GREEN_SIGNAL_P_H_*/
