// LICENSETEXT
//
//   Copyright (C) 2007 : GreenSocs Ltd
//       http://www.greensocs.com/ , email: info@greensocs.com
//
//   Developed by :
//
//   Robert Guenzel
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

#ifndef __genericRouter_h__
#define __genericRouter_h__

#include <boost/config.hpp> // needed for SystemC 2.1
#include <systemc>
#include "greensocket/initiator/multi_socket.h"
#include "greensocket/target/multi_socket.h"
#include "gsgpsocket/utils/gs_trace.h"
#include "greenrouter/genericRouter_if.h"
#include "greenrouter/genericProtocol_if.h"
#include "greenrouter/simpleAddressMap.h"
#include <sstream>
#include <set>

namespace gs{
namespace gp{

template <unsigned int BUSWIDTH, typename TRAITS, typename DEFAULT_PROTOCOL,
          unsigned int PORTMAX = 255,
          typename RESP_TYPE=tlm::tlm_response_status,
          RESP_TYPE ADDR_ERR_RESP=tlm::TLM_ADDRESS_ERROR_RESPONSE,
          void (TRAITS::tlm_payload_type::*SET_RESP_CALL)(const RESP_TYPE) =
              &(TRAITS::tlm_payload_type::set_response_status),
          typename ADDR_MAP = SimpleAddressMap<TRAITS, PORTMAX>
         >
class GenericRouterRaw:
public sc_core::sc_module,
public GenericRouter_if<BUSWIDTH, TRAITS>,
public GenericRouterBase<TRAITS, PORTMAX, ADDR_MAP>
{
private:
    class router_init_socket :
    public GenericRouter_if<BUSWIDTH, TRAITS>::init_socket_type
    {
    public:
        router_init_socket(const char* name, GenericRouterRaw& owner) :
        GenericRouter_if<BUSWIDTH, TRAITS>::init_socket_type(name),
        m_owner(owner)
        {
        }

        virtual void bound_to(const std::string& other_type,
                              gs::socket::bindability_base<TRAITS>* other,
                              unsigned int index)
        {
            m_owner.bound_to_target(other_type, other, index);
        }
    private:
        GenericRouterRaw& m_owner;
    };

    class router_target_socket :
    public GenericRouter_if<BUSWIDTH, TRAITS>::target_socket_type
    {
    public:
        router_target_socket(const char* name, GenericRouterRaw& owner):
        GenericRouter_if<BUSWIDTH, TRAITS>::target_socket_type(name),
        m_owner(owner)
        {
        }

        virtual void bound_to(const std::string& other_type,
                              gs::socket::bindability_base<TRAITS>* other,
                              unsigned int index)
        {
            m_owner.bound_to_initiator(other_type, other, index);
        }
    private:
        GenericRouterRaw& m_owner;
    };

public:
    typedef router_target_socket target_socket_type;
    typedef router_init_socket init_socket_type;
    typedef typename TRAITS::tlm_payload_type payload_type;
    typedef typename TRAITS::tlm_phase_type phase_type;
    typedef tlm::tlm_sync_enum sync_enum_type;

    //typedef std::multimap<unsigned int, unsigned int> mm_uint_uint;
    typedef std::multimap<unsigned int, Entry_t> mm_uint_Entry;

    /**
     * Input/Output sockets
     */

    /// target multiport
    target_socket_type target_socket;

    /// slave multiport
    init_socket_type init_socket;

    /// port to the protocol
    sc_core::sc_port<GenericProtocol_if<TRAITS>, 0> protocol_port;

    sc_core::sc_export<GenericRouter_if<BUSWIDTH, TRAITS> >
      protocol_router_target;

    /// slave address map
    ADDR_MAP* m_addressMap;

    virtual ADDR_MAP& getAddressMap()
    {
        return *m_addressMap;
    }

    virtual void do_eoe()
    {
        end_of_elaboration();
    }

    SC_HAS_PROCESS(GenericRouterRaw);

    /**
     * Constructor; bind ports and register SC_METHODs with the kernel.
     *
     * @param name_ module name
     */
    GenericRouterRaw(sc_core::sc_module_name name_):
    sc_core::sc_module(name_),
    target_socket("tsocket", *this),
    init_socket("isocket", *this),
    protocol_port("protocol"),
    protocol_router_target("protocol_router_target"),
    m_addressMap(0),
    m_EOEdone(false),
    my_id(id_counter++),
    mapCheckPending(false),
    m_DummyProtocol("DummyProtocol"),
    m_protocol_port_index(0)
    {
        GS_DUMP("I am a generic router.");

        init_socket.register_nb_transport_bw(this, &GenericRouterRaw::nb_bw);
        init_socket.register_invalidate_direct_mem_ptr(this,
            &GenericRouterRaw::inv_dmi);

        target_socket.register_nb_transport_fw(this, &GenericRouterRaw::nb_fw);
        target_socket.register_b_transport(this, &GenericRouterRaw::b_tr);
        target_socket.register_transport_dbg(this, &GenericRouterRaw::tr_dbg);
        target_socket.register_get_direct_mem_ptr(this,
            &GenericRouterRaw::get_dmi);

        // connect to dummy protocol (will only be used if no other
        // protocol is bound)
        protocol_port(m_DummyProtocol);
        m_DummyProtocol.router_port(*this);

        protocol_router_target(*this);
    }

    virtual void bound_to_target(const std::string& other_type,
                                 gs::socket::bindability_base<TRAITS>* other,
                                 unsigned int index) = 0;

    virtual void bound_to_initiator(const std::string& other_type,
                                    gs::socket::bindability_base<TRAITS>* other,
                                    unsigned int index)
    {
        GS_DUMP("Target socket index " << index
                                       << " is bound to a socket of type "
                                       << other_type);
        gs::socket::config<TRAITS> conf=target_socket.get_recent_config(0);
        for (unsigned int i=1; i<target_socket.size(); i++){
            std::stringstream s1, s2;
            s1 << "Merged Config of Initiators 0 to " << (i-1) << " of "
               <<target_socket.name();
            s2 << "Initiator at index " << i << " of " << target_socket.name();
            conf.merge_with(s1.str().c_str(), s2.str().c_str(),
                            target_socket.get_recent_config(i));
        }
        init_socket.set_config(conf);
        target_socket.set_config(conf);
    }

    void start_of_simulation()
    {
        GS_DUMP("Resolved configs are" << std::endl << "  For initiators:"
                    << std::endl
                    << target_socket.get_recent_config(0).to_string()
                    << std::endl << "  For targets:" << std::endl
                    << init_socket.get_recent_config(0).to_string());
    }


    void assign_address(sc_dt::uint64 baseAddress_, sc_dt::uint64 highAddress_,
                        unsigned int portNumber_)
    {
        if(!m_addressMap)
            createAddressMap();
        m_addressMap->insert(baseAddress_, highAddress_, portNumber_);
    }

    /**
     * This method can be made sensitive to an event in the bus protocol to
     * control the processing of transfers from master to slave.
     */
    void processMasterAccess()
    {
        // tell protocol that it's time to process the master accesses
        // this could include arbitrate between accesses that were enqueued
        // earlier with registerMasterAccess...
        protocol_port[m_protocol_port_index]->processMasterAccess();
    }

    /**
     * This method can be made sensitive to an event in the protocol to control
     * the processing of transfers from slave to master.
     */
    void processSlaveAccess()
    {
        // tell bus protocol that it's time to process the slave accesses
        // this could include arbitrate between accesses that were enqueued
        // earlier with registerSlaveAccess...
        protocol_port[m_protocol_port_index]->processSlaveAccess();
    }

    /**
     * Provide access to the initiator port (used by bus protocol).
     */
    virtual typename GenericRouter_if<BUSWIDTH, TRAITS>::init_socket_type*
        getInitPort()
    {
        return &init_socket;
    }

    /**
     * Provide access to the target port (used by bus protocol).
     */
    virtual typename GenericRouter_if<BUSWIDTH, TRAITS>::target_socket_type*
        getTargetPort()
    {
        return &target_socket;
    }

    virtual typename GenericRouter_if<BUSWIDTH, TRAITS>::bidir_socket_type*
        getBiDirSocket()
    {
        return 0;
    }

    /**
     * End of elab simulation kernel callback
     */
    virtual void end_of_elaboration()
    {
        if (m_EOEdone)
            return;
        GS_DUMP("end_of_elaboration called.");

        if (protocol_port.size() > 1) { // user protocol bound
            GS_DUMP("User protocol detected.");
            m_protocol_port_index = 1;
        } else {
          GS_DUMP("No user protocol detected. Using dummy protocol.");
        }


        //TODO: this has to be done each time the protocol is switched!
        {
            sc_core::sc_spawn_options opts;
            opts.spawn_method();
            opts.dont_initialize();
            if(protocol_port[m_protocol_port_index]->
                assignProcessMasterAccessSensitivity(opts))
            {
                sc_core::sc_spawn(sc_bind(&GenericRouterRaw::processMasterAccess,
                    this),
                    sc_core::sc_gen_unique_name("processMasterAccess"), &opts);
            }
        }
        {
            sc_core::sc_spawn_options opts;
            opts.spawn_method();
            opts.dont_initialize();
            if (protocol_port[m_protocol_port_index]->
                 assignProcessSlaveAccessSensitivity(opts))
            {
                sc_core::sc_spawn(sc_bind(&GenericRouterRaw::processSlaveAccess,
                    this),
                    sc_core::sc_gen_unique_name("processSlaveAccess"), &opts);
            }
        }

        // if m_address_map is not assigned a value yet then
        // calll create address map
        if (m_addressMap == 0) {
            createAddressMap();
        }

        // create the slave address map
        GS_DUMP_N(name(), "Creating address map...");
        m_addressMap->generateMap(init_socket, true);
        m_addressMap->dumpMap();

        m_EOEdone=true;
    }

    /**
     * Refresh Address Mapping.
     */
    void refreshAddressMap(bool checkOverlap = true)
    {
        if(m_addressMap)
        {
            delete m_addressMap;
        }
        createAddressMap();

        GS_DUMP_N(name(), "Creating address map...");
        m_addressMap->generateMap(init_socket, checkOverlap);
        m_addressMap->dumpMap();

        m_EOEdone=true;
    }

    /**
     * This is the PV TLM blocking if implementation
     */
    void b_tr(unsigned int from, payload_type& txn, sc_core::sc_time& time)
    {
        if(mapCheckPending) {
            refreshAddressMap(true);
            mapCheckPending = false;
        }

        protocol_port[m_protocol_port_index]->before_b_transport(from, txn,
                                                                 time);
        GS_DUMP("forwarding PV transaction from master index"
            << from << " to slave at address=" << txn.get_address());

        //get the config of initiator 'from' (through target socket of bus)
        gs::socket::config<TRAITS> tmp_conf =
            target_socket.get_recent_config(from);

        bool decode_ok = false;
        std::vector<unsigned int> targetIdVec =
            decodeAddress(txn, decode_ok, &tmp_conf, from);

        if(!decode_ok) {
            (txn.*SET_RESP_CALL)(ADDR_ERR_RESP);
        } else {
            for (unsigned int i = 0; i < targetIdVec.size(); ++i) {
                Port_id_t tar_port_num = targetIdVec[i];
                if (tar_port_num == m_addressMap->get_max_port()) {
                    (txn.*SET_RESP_CALL)(ADDR_ERR_RESP);
                } else {
                    init_socket[tar_port_num]->b_transport(txn, time);
                }
            }
        }
    }

    sync_enum_type nb_bw(unsigned int from, payload_type& txn, phase_type& ph,
                         sc_core::sc_time& time)
    {
        if(mapCheckPending) {
            refreshAddressMap(true);
            mapCheckPending = false;
        }

        return protocol_port[m_protocol_port_index]->
            registerSlaveAccess(from, txn, ph, time);
    }

    sync_enum_type nb_fw(unsigned int from, payload_type& txn, phase_type& ph,
                         sc_core::sc_time& time)
    {
        if(mapCheckPending) {
            refreshAddressMap(true);
            mapCheckPending = false;
        }

        //get the config of initiator 'from' (through target socket of bus)
        gs::socket::config<TRAITS> tmp_conf =
            target_socket.get_recent_config(from);

        bool decode_ok = false;
        std::vector<unsigned int> targetIdVec =
            decodeAddress(txn, decode_ok, &tmp_conf, from);

        if (decode_ok) {
            for (unsigned int i = 0; i < targetIdVec.size(); ++i) {
                Port_id_t tar_port_num = targetIdVec[i];
                if (tar_port_num == m_addressMap->get_max_port()) {
                    (txn.*SET_RESP_CALL)(ADDR_ERR_RESP);
                    ph = tlm::END_REQ;
                    return tlm::TLM_UPDATED;
                } else {
                    return protocol_port[m_protocol_port_index]
                        ->registerMasterAccess(from, txn, ph, time);
                }
            }
        }

        (txn.*SET_RESP_CALL)(ADDR_ERR_RESP);
        ph = tlm::END_REQ;
        return tlm::TLM_UPDATED;
    }

    void inv_dmi(unsigned int from, sc_dt::uint64 start_range,
                                    sc_dt::uint64 end_range)
    {
        pair<mm_uint_Entry::iterator, mm_uint_Entry::iterator> ret;
        mm_uint_Entry::iterator it;
        std::set<Port_id_t> port_set;
        protocol_port[m_protocol_port_index]->
            invalidate_direct_mem_ptr(from, start_range, end_range);

        if(end_range < start_range) {
            GS_DUMP("ERROR: inv_dmi: End Address is less then start address, "
                    "please check");
            return;
        }
        ret = m_dmi_tport_entry.equal_range(from);

        for(it=ret.first; it!=ret.second; ++it) {
            Entry_t &entry1 = (*it).second;
            if(port_set.count(entry1.port_id) == 0) {
                target_socket[entry1.port_id]->
                    invalidate_direct_mem_ptr(start_range, end_range);
                port_set.insert(entry1.port_id);
            }

            if(entry1.lower_bound == start_range
                && entry1.upper_bound == end_range)
            {
                m_dmi_tport_entry.erase(it);
            } else {
                // if the remove_range returns true then the given address
                // range is removd from the
                // current entry but if it returns false, that is because the
                // given address range
                // strictly lie in between the given entry
                // Thus the given entry needs to be split into two, which
                // does not cover
                // the given address range
                if(entry1.remove_range(start_range,end_range) == false) {
                    Entry_t entr(end_range+1, entry1.upper_bound,
                               entry1.port_id);
                    m_dmi_tport_entry.insert(
                        pair<unsigned int, Entry_t>(from,entr));
                    entry1.upper_bound = start_range - 1;
                }
            }
        }
    }

    unsigned int tr_dbg(unsigned int from, payload_type& trans)
    {
	if(mapCheckPending) {
            refreshAddressMap(true);
            mapCheckPending = false;
        }

        protocol_port[m_protocol_port_index]->transport_dbg(from, trans);

        GS_DUMP("forwarding DBG transaction from master index"
                        << from << " to slave at address=" << txn.get_address());

        //get the config of initiator 'from' (through target socket of bus)
        gs::socket::config<TRAITS> tmp_conf =
                target_socket.get_recent_config(from);

        bool decode_ok = false;
        std::vector<unsigned int> targetIdVec =
                decodeAddress(trans, decode_ok, &tmp_conf, from);

        unsigned int result = 0;

        if(!decode_ok) {
            (trans.*SET_RESP_CALL)(ADDR_ERR_RESP);
            return 0;
        } else {
            for (unsigned int i = 0; i < targetIdVec.size(); ++i) {
                Port_id_t tar_port_num = targetIdVec[i];
                if (tar_port_num == m_addressMap->get_max_port()) {
                    (trans.*SET_RESP_CALL)(ADDR_ERR_RESP);
                } else {
                    result += init_socket[tar_port_num]->transport_dbg(trans);
                }
            }
        }
        return result;
    }

    bool get_dmi(unsigned int from, payload_type& trans, tlm::tlm_dmi& dmi_data)
    {
        //return protocol_port[m_protocol_port_index]->get_direct_mem_ptr(from,
        //trans, dmi_data);

        Map_address_t start_addr;
        Map_address_t end_addr;
        pair<mm_uint_Entry::iterator, mm_uint_Entry::iterator> ret;
        mm_uint_Entry::iterator it;
        //get the config of initiator 'from' (through target socket of bus)
        gs::socket::config<TRAITS> tmp_conf =
            target_socket.get_recent_config(from);

        if(mapCheckPending) {
            refreshAddressMap(true);
            mapCheckPending = false;
        }

        bool decode_ok = false;
        Port_id_t tar_port_num = decodeAddress(trans, decode_ok, &tmp_conf,
                                               from)[0];
        if (!decode_ok)
        {
          return false;
        }

        // dont know what is this call for??
        protocol_port[m_protocol_port_index]->
            get_direct_mem_ptr(from, trans, dmi_data);
        GS_DUMP("Getting dmi pointer from master index" << from
                                                        << " to slave at port="
                                                        << tar_port_num);

        // if the original call to target returns true...
        if (init_socket[tar_port_num]->get_direct_mem_ptr(trans, dmi_data)) {
            start_addr = dmi_data.get_start_address();
            end_addr = dmi_data.get_end_address();
            Entry_t entr(start_addr, end_addr, from);

            if(end_addr < start_addr) {
                GS_DUMP("ERROR:get_dmi: End Address is less then start address,"
                        " please check");
                return false;
            }

            ret = m_dmi_tport_entry.equal_range(tar_port_num);
            // Check for all existing entries
            for(it=ret.first; it!=ret.second; ++it) {
                Entry_t &entry1 = (*it).second;
                // if port_it is no matching then move to the next entry
                if( entry1.port_id != from )
                        continue;
                else
                {
                  // if the given range gets merged with the current entry then
                  // there is no more to do ... so one can return safely
                  if (entry1.merge_with(start_addr,end_addr))
                       return true;
                }
            }
            // if the given address range can not be merged with any other entry
            // insert it as another entry
            m_dmi_tport_entry.insert(
                pair<unsigned int, Entry_t>(tar_port_num,entr));
            return true;
        } else
            return false;
    }

    /**
     * Create the addressmap, kept as virtual in case there is a need to
     * overload it
     */
    virtual ADDR_MAP* createAddressMap()
    {
        m_addressMap = new ADDR_MAP;
        return m_addressMap;
    }

    /**
     * GenericRouter_if's decodeAddress function.
     */
    virtual std::vector<unsigned int> decodeAddress(payload_type& txn,
        bool &decode_ok, gs::socket::config<TRAITS>* conf = 0,
        unsigned int from = 0)
    {
        return m_addressMap->decode(txn, decode_ok, conf, from);
    }

    virtual unsigned int getRouterID()
    {
        return my_id;
    }

    virtual unsigned int getCurrentNumRouters()
    {
        return id_counter;
    }

protected:
    bool m_EOEdone;
    unsigned int my_id;
    static unsigned int id_counter;
    //mm_uint_uint m_dmi_iport_tport;
    mm_uint_Entry m_dmi_tport_entry;
    bool mapCheckPending;
public:
    DEFAULT_PROTOCOL m_DummyProtocol;
    unsigned m_protocol_port_index; //TODO: change to gs_param
};

template <unsigned int BUSWIDTH, typename TRAITS, typename DEFAULT_PROTOCOL,
          unsigned int PORTMAX, typename RESP_TYPE, RESP_TYPE ADDR_ERR_RESP,
          void (TRAITS::tlm_payload_type::*SET_RESP_CALL)(const RESP_TYPE),
          typename ADDR_MAP >
unsigned int GenericRouterRaw<BUSWIDTH, TRAITS, DEFAULT_PROTOCOL, PORTMAX,
                            RESP_TYPE, ADDR_ERR_RESP, SET_RESP_CALL,
                            ADDR_MAP>::id_counter=0;

template <unsigned int BUSWIDTH, typename TRAITS, typename DEFAULT_PROTOCOL,
          unsigned int PORTMAX, typename RESP_TYPE, RESP_TYPE ADDR_ERR_RESP,
          void (TRAITS::tlm_payload_type::*SET_RESP_CALL)(const RESP_TYPE),
          typename ADDR_MAP >
class GenericRouter_b:
public GenericRouterRaw<BUSWIDTH, TRAITS, DEFAULT_PROTOCOL, PORTMAX,
                        RESP_TYPE, ADDR_ERR_RESP, SET_RESP_CALL,
                        ADDR_MAP>
{
protected:
    typedef GenericRouterRaw<BUSWIDTH, TRAITS, DEFAULT_PROTOCOL, PORTMAX,
                             RESP_TYPE, ADDR_ERR_RESP, SET_RESP_CALL,
                             ADDR_MAP >
            GenericRouterRawType;

public:
    GenericRouter_b(sc_core::sc_module_name name_):
    GenericRouterRawType(name_)
    {
    };

    virtual void bound_to_target(const std::string& other_type,
                                 gs::socket::bindability_base<TRAITS>* other,
                                 unsigned int index)
    {
        GS_DUMP("Initiator socket index " << index
                                          << " is bound to a socket of type "
                                          << other_type);
        gs::socket::config<TRAITS> conf=this->init_socket.get_recent_config(0);
        for (unsigned int i=1; i<this->init_socket.size(); i++){
            std::stringstream s1, s2;
            s1 << "Merged Config of Targets 0 to " << (i-1) << " of "
               << this->init_socket.name();
            s2 << "Target at index " << i << " of " << this->init_socket.name();
            conf.merge_with(s1.str().c_str(), s2.str().c_str(),
                            this->init_socket.get_recent_config(i));
        }
        this->target_socket.set_config(conf);
        this->init_socket.set_config(conf);
    }
};

template <unsigned int BUSWIDTH, typename TRAITS, typename DEFAULT_PROTOCOL,
          unsigned int PORTMAX, typename RESP_TYPE, RESP_TYPE ADDR_ERR_RESP,
          void (TRAITS::tlm_payload_type::*SET_RESP_CALL)(const RESP_TYPE)>
class GenericRouter_b<BUSWIDTH, TRAITS, DEFAULT_PROTOCOL, PORTMAX,
                      RESP_TYPE, ADDR_ERR_RESP, SET_RESP_CALL,
                      SimpleAddressMap<TRAITS, PORTMAX> >:
public GenericRouterRaw<BUSWIDTH, TRAITS, DEFAULT_PROTOCOL, PORTMAX,
                        RESP_TYPE, ADDR_ERR_RESP, SET_RESP_CALL,
                        SimpleAddressMap<TRAITS, PORTMAX> >
{
    GC_HAS_CALLBACKS();
protected:
    typedef GenericRouterRaw<BUSWIDTH, TRAITS, DEFAULT_PROTOCOL, PORTMAX,
                             RESP_TYPE, ADDR_ERR_RESP, SET_RESP_CALL,
                             SimpleAddressMap<TRAITS, PORTMAX> >
            GenericRouterRawType;

public:
    typedef typename GenericRouterRawType::payload_type payload_type;

    GenericRouter_b(sc_core::sc_module_name name_):
    GenericRouterRawType(name_)
    {
    };

    virtual void bound_to_target(const std::string& other_type,
                                 gs::socket::bindability_base<TRAITS>* other,
                                 unsigned int index)
    {
        GS_DUMP("Initiator socket index " << index
                                          << " is bound to a socket of type "
                                          << other_type);
        gs::socket::config<TRAITS> conf=this->init_socket.get_recent_config(0);
        for (unsigned int i=1; i<this->init_socket.size(); i++){
            std::stringstream s1, s2;
            s1 << "Merged Config of Targets 0 to " << (i-1) << " of "
               << this->init_socket.name();
            s2 << "Target at index " << i << " of " << this->init_socket.name();
            conf.merge_with(s1.str().c_str(), s2.str().c_str(),
                            this->init_socket.get_recent_config(i));
        }
        this->target_socket.set_config(conf);
        this->init_socket.set_config(conf);

        gs::socket::GreenSocketAddress_base*
                greenSocketAddress(
                    dynamic_cast<gs::socket::GreenSocketAddress_base*>(other));

        if (greenSocketAddress)
        {
            GC_REGISTER_TYPED_PARAM_CALLBACK(&greenSocketAddress->base_addr,
                                             gs::cnf::post_write,
                                             GenericRouter_b,
                                             addressChanged);
        }
    }

   /*
    * Callback on base_addr / high_addr of each device (post_write)
    * Update address map with new address and invalidate DMI
    * Only support simpleAddressMap
    *
    * @param param address (base or high)
    * @param type callback type
    * @return gs::cnf::return_nothing
    */
   gs::cnf::callback_return_type addressChanged(
       gs::gs_param_base& param,
       gs::cnf::callback_type type)
   {
       gs::gs_param<gs_uint64>& addr =
           static_cast<gs::gs_param<gs_uint64>&>(param);
       bool decodeResult = false;
       payload_type txn;
       std::vector<Port_id_t> portIDs;
       Port_id_t portID, portIDTmp;
       gs::socket::config<TRAITS> conf;
       gs::gs_param<gs_uint64> baseAddr;
       gs::gs_param<gs_uint64> highAddr;

       GS_DUMP_N(name(), "addressChanged : " << param.getName()
           << " = 0x" << std::hex << addr << std::dec);

       this->refreshAddressMap(false);

       txn.set_address(addr);

       for(unsigned int i = 0; i < this->target_socket.size(); i++) {
           portIDs = this->decodeAddress(txn, decodeResult, &conf, i);
           if (decodeResult) {
               for (std::vector<Port_id_t>::iterator it = portIDs.begin();
                    it != portIDs.end(); ++it)
               {
                   portID = *it;
                   gs::socket::bindability_base<TRAITS>*
                       otherSide(this->init_socket.get_other_side(portID,
                                 portIDTmp));
                   gs::socket::GreenSocketAddress_base*
                       greenSocketAddress(
                           dynamic_cast<gs::socket::GreenSocketAddress_base*>
                               (otherSide));
                   GenericRouterBase<TRAITS, PORTMAX,
                                     SimpleAddressMap<TRAITS, PORTMAX> >*
                       router(NULL);

                   if (otherSide) {
                       router  = dynamic_cast<GenericRouterBase<TRAITS,
                                              PORTMAX,
                                              SimpleAddressMap<TRAITS, PORTMAX>
                                             >*>
                                      (otherSide->get_parent());
                   }

                   if (router) {
                       baseAddr = router->getAddressMap().get_min();
                       highAddr = router->getAddressMap().get_max();
                   } else if (greenSocketAddress) {
                       baseAddr = greenSocketAddress->base_addr;
                       highAddr = greenSocketAddress->high_addr;
                   }
                   this->mapCheckPending = true;
                   this->target_socket[i]->invalidate_direct_mem_ptr(baseAddr,
                                                               highAddr);
               }
           }
       }

       return gs::cnf::return_nothing;
   }
};

#define __INCLUDED_BY_GENERIC_ROUTER_B_H__
#include "greenrouter/protocol/Dummy/DummyProtocol.h"
#undef __INCLUDED_BY_GENERIC_ROUTER_B_H__

template <unsigned int BUSWIDTH, unsigned int PORTMAX = 255,
          typename TRAITS=tlm::tlm_base_protocol_types,
          typename RESP_TYPE=tlm::tlm_response_status,
          RESP_TYPE ADDR_ERR_RESP=tlm::TLM_ADDRESS_ERROR_RESPONSE,
          void (TRAITS::tlm_payload_type::*SET_RESP_CALL)(const RESP_TYPE) =
              &TRAITS::tlm_payload_type::set_response_status,
          typename ADDR_MAP = SimpleAddressMap<TRAITS, PORTMAX>
         >
class GenericRouter : public GenericRouter_b<BUSWIDTH, TRAITS,
    DummyProtocol<BUSWIDTH, TRAITS>, PORTMAX, RESP_TYPE, ADDR_ERR_RESP,
    SET_RESP_CALL, ADDR_MAP >
{
public:
    GenericRouter(const char* name):
    GenericRouter_b<BUSWIDTH, TRAITS, DummyProtocol<BUSWIDTH, TRAITS>,
    PORTMAX, RESP_TYPE, ADDR_ERR_RESP, SET_RESP_CALL, ADDR_MAP >(name)
    {
    }
};


}
}
#endif

