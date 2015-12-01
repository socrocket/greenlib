// LICENSETEXT
//
//   Copyright (C) 2007 - 2009 : GreenSocs Ltd
//       http://www.greensocs.com/ , email: info@greensocs.com
//
//   Developed by :
//
//   Puneet Arora, CircuitSutra Technologies
//   puneet@circuitsutra.com
//
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

#ifndef __MACAddressMap_h__
#define __MACAddressMap_h__
#define CLASS_NAME MACAddressMap

#include "tlm.h"
#include "greenrouter/genericRouter_if.h"
#include <list>

/**
 * Simple address map implementation for the generic protocol.
 */

namespace gs {
namespace gp {

using std::string;
using std::cout;
using std::cerr;
using std::endl;
using std::dec;
using std::hex;
using std::ostream;

//typedef sc_dt::uint64 Map_address_t;                  ///< address type
typedef uint64_t Map_address_t;                         ///< address type
//typedef unsigned int Map_address_t;                   ///< address type
typedef unsigned int  Port_id_t;                        ///< port ID type
///< address map type
typedef std::map <unsigned int, Map_address_t> Ethernet_Map_t;
///< address map iterator type
typedef Ethernet_Map_t::iterator Ethernet_Map_iterator_t;

template < typename TRAITS, unsigned int PORTMAX = 255>
class MACAddressMap {

protected:
    //KEY is PortId and VALUE is MAC address of this
    //device/ethernetController/backendInterface
    Ethernet_Map_t m_address_map;

private:

    typedef typename TRAITS::tlm_payload_type payload_type;
    // class constants

    const Port_id_t m_max_port;                           ///< maximum port ID
    const char *    m_routine_name;                       ///< routine name

    // class member variables
    Port_id_t m_extWrldPortId;

public:

    /**
     * Constructor
     */
    MACAddressMap():
    m_max_port(PORTMAX),
    m_routine_name("MACAddressMap"),
    m_extWrldPortId (PORTMAX)
    {
    }

    virtual ~MACAddressMap(void)
    {
    }

    /**
     * Generate the MAC map
     *
     * @param socket map socket
     * @param checkOverlap (Unused) enable or disable overlap check
     */
    template <typename SOCKET>
    void generateMap(SOCKET& socket, bool checkOverlap = true)
    {
        GS_DUMP_N(m_routine_name, "Generating address map:");
        GS_DUMP_N(m_routine_name, "Number of target connections: "
                                  << socket.size());

        for (Port_id_t port_id = 0; port_id < socket.size(); port_id++) {
            // get target_port
            Port_id_t tmp;

            gs::socket::bindability_base < TRAITS > *
                other_side(socket.get_other_side(port_id, tmp));
            gs::socket::GreenSocketAddress_base *
                other_gp(dynamic_cast<
                    gs::socket::GreenSocketAddress_base*>(other_side));
            GenericRouterBase < TRAITS, PORTMAX,
                    MACAddressMap<TRAITS, PORTMAX> > *
                other_router(NULL);
            string other_name("generic OSCI socket");

            if(other_side) {
                other_router = dynamic_cast < GenericRouterBase < TRAITS, PORTMAX, MACAddressMap<TRAITS, PORTMAX>  > * >(other_side->get_parent_object());
                other_name = other_side->get_name();
            }

            Map_address_t address_low; ///< address lower bound
            bool mapped(false); ///< port mapped flag

            if (other_router) {
                other_router->do_eoe();

                //address_low = other_router->getAddressMap().get_min();

                m_address_map[port_id] = address_low;
            } else if (other_gp) {
                address_low = other_gp->base_addr;

                m_address_map[port_id] = address_low;
                if (address_low == 0xffffffffffffull) {
                    //std::cout << "Setting m_extWrldPortId to " << port_id
                    // << "\n";
                    m_extWrldPortId = port_id;
                }
#ifdef GS_SOCKET_ADDRESS_ARRAY
                assert(other_gp->num_address_range==0);
#endif
            } else {
                // verify port mapping
                for (Ethernet_Map_iterator_t i = m_address_map.begin();
                    i != m_address_map.end(); i++)
                {
                    if (port_id == i->first) {
                        mapped = true;
                        break;
                    }
                }

                if (!mapped) {
                    cerr << "WARNING: Target ( " << other_name << " ) on port "
                         << port_id << " of " << socket.name ()
                         << " is not derived from GSGPSlaveSocket_base, so the "
                            "addresses cannot be determined automatically. Use "
                            "router_instance.assign_address ( lower, upper, "
                         << port_id << ")" << endl;
                }
            }

            // get address parameters
            if (mapped) {
                for (Ethernet_Map_iterator_t i = m_address_map.begin();
                     i != m_address_map.end(); i++)
                {
                    if (port_id == i->first) {
                        GS_DUMP_N (m_routine_name, "Target ( "
                                           << other_name
                                           << " ) connected to port " << port_id
                                           << " range: " << "[ 0x" << hex
                                           << i->lower_bound << ", 0x" << hex
                                           << i->upper_bound << " ]"
                                           << dec);
                    }
                }
            }
        }
    }

// Define GS_VERBOSE_DUMPMAP if not defined in the compilation command line
#if !defined(GS_VERBOSE_DUMPMAP)
    #if defined(GS_VERBOSE)
        #define GS_VERBOSE_DUMPMAP 1
    #else /* GS_VERBOSE */
        #define GS_VERBOSE_DUMPMAP 0
    #endif /*GS_VERBOSE  */
#endif /* GS_VERBOSE_DUMPMAP */

    /**
     * Dump map
     *
     * @param verbose Only show the map if value is true. Default to true only
     * when macro GS_VERBOSE is defined.
     */
    void dumpMap(bool verbose = GS_VERBOSE_DUMPMAP)
    {
        if (verbose) {
            cout << m_routine_name << " address map:" << endl;

            for (Ethernet_Map_iterator_t i = m_address_map.begin();
                 i != m_address_map.end(); i++)
            {
                cout << "Port id : " << i->first << ", MAC address : "
                     << i->second << endl;
            }
        }
    }

    /*
     * Parse the ethernet payload. Get the source and destination MAC addresses
     * from the first 12 bytes of data field. Return the portId of destination
     * MAC address from the m_address_map and update the MAC address of source
     * portId if required.
     *
     * @param txn address to decode
     * @param conf conf of the socket from which the trnx is coming.
     * This value is ignored here, and used when address map is extension based
     * @param from portId of the socket from which the trnx is coming
     *
     * @return target port number
     *
     * @note on error, m_max_port is returned
     */
    virtual std::vector<Port_id_t>& decode(payload_type& txn,
                                           gs::socket::config<TRAITS>* conf,
                                           unsigned int from)
    {

        static std::vector<Port_id_t> targetId(1);

        //Update the MAC address of the source if required
        unsigned char* data = txn.get_data_ptr();
        //printf("In decode DataPtr=%p, %d\n", data, from);
        //for (int i=0; i<14; ++i)
        //  std::cout << (unsigned int) (*(data+i)) << "\n";

        //First six bytes contain destination address and following six bytes
        //contain src address
        Map_address_t srcAdd = 0, destAdd = 0;
        memcpy(&destAdd, data, 6);
        memcpy(&srcAdd, data+6, 6);
        //std::cout << std::hex << "destAdd = " << destAdd << ", srcAdd = " <<
        //srcAdd << std::dec << "\n";
        m_address_map[from] = srcAdd;

        Port_id_t port_id (m_max_port);
        bool success (false);
        for (Ethernet_Map_iterator_t i = m_address_map.begin();
             i != m_address_map.end(); i++)
        {
            if (i->second == destAdd) {
                success = true;
                port_id = i->first;
                break;
            }
        }

        if (!success) {
            if (from == m_extWrldPortId) {
                //port_id was initialized with m_max_port so nothing to do
             } else {
                 port_id = m_extWrldPortId;
             }
        }

        targetId[0] = port_id;
        return targetId;
    }

    /**
     * Get max port
     *
     * @return the maximum port associated with this simple_address_map
     */
    Port_id_t get_max_port(void)
    {
        return m_max_port;
    }

    /**
     * Get address map
     *
     * @retuen returns the default address map
     */
    const Ethernet_Map_t & get_address_map() const {
        return m_address_map;
    }
};

}
}

#undef CLASS_NAME
#endif  /* __simpleAddressMap_h__ */
