#ifndef TARGET_SIGNAL_SOCKET_H_
#define TARGET_SIGNAL_SOCKET_H_

#include "greensocket/target/single_socket.h"
#include "greensignalsocket/green_signal_h/gs_generic_signal_ifs.h"
#include "greensignalsocket/green_signal_h/green_signal_socket/generic/wrapper_base.h"
#include <vector>
using namespace std;

namespace gs_generic_signal {
  typedef gs::socket::target_socket<32,gs_generic_signal_protocol_types,
    gs_generic_signal_bind_checker> target_signal_base_socket;
  
  typedef gs_generic_signal_protocol_types::tlm_payload_type transaction_type;
  typedef gs_generic_signal_protocol_types::tlm_phase_type phase_type;
  typedef tlm::tlm_sync_enum sync_enum_type;

/*!
* @detail The target_signal_socket basically derived from the target_signal_base_socket
* with gs_generic_signal_protocol_types as traits.
* This class basically checks the src_id of the payload passed along
* and if it is present in the list of supportable sources, then only the call is executed
* and it reaches to the user IP.
* Apart from that target socket keeps the value of the data passed 
* in the transaction payload.
*/
  template <typename MODULE>
  class target_signal_socket : public virtual target_signal_base_socket {
    
    public:
      target_signal_socket (const char* name) : target_signal_base_socket(name) {
        target_signal_base_socket::register_b_transport(this, &target_signal_socket::b_transport);
        target_signal_base_socket::register_nb_transport_fw(this, &target_signal_socket::nb_transport_fw);
      }    
  
      template<typename EXT>
      void set_source(std::string name) {
        supported_srcId_ext[wrapper_base::decode_name(name)].push_back(EXT::ID);
      }
  
      template<typename EXT>
      unsigned char get_last_value() {
        //std::cout << signal_map.size() << "<---size \n"; 
        if (signal_map.find(EXT::ID) == signal_map.end())
          std::cout << "This Extension is not supported by this target_signal_socket. Returning 0.\n";
        else
          return signal_map[EXT::ID];
        return 0;
      }
      /*
      void set_source (std::string& name ) { 
        supported_srcname_list.push_back(name);
      }
  
      bool is_src_supported (std::string name) {
        std::vector<std::string>::iterator it = supported_srcname_list.begin();
        std::vector<std::string>::iterator end_it = supported_srcname_list.end();
        for ( ; it != end_it; ++it) {
          if (*it == name)
            return true;
        }
        return false;
      }
      */
      void register_b_transport (MODULE* mod, void(MODULE::*cb)(transaction_type&, sc_core::sc_time&)) {
        parent_mod = mod;
        b_cb = cb;
      }
  
      void register_nb_transport_fw (MODULE* mod, 
        sync_enum_type(MODULE::*cb)(transaction_type&, phase_type&, sc_core::sc_time&)) {
        parent_mod = mod;
        nb_cb = cb;
      }
  
      void b_transport (transaction_type& trans, sc_core::sc_time& delay) {
        //check if the source id passed along with the payload is in the supported src list
        if (store_signal_value(trans))
          ((parent_mod->*b_cb)(trans, delay));
        //else
          //cout<<"Source ID of this txn not supported"<<endl;
        return;
      }
  
      sync_enum_type nb_transport_fw (transaction_type& trans, phase_type& phase, sc_core::sc_time& delay) {
        //check if the source id passed along with the payload is in the supported src list
        if (store_signal_value(trans))
          return ((parent_mod->*nb_cb)(trans, phase, delay));
        //else
          //cout<<"Source ID of this txn not supported"<<endl;
        return tlm::TLM_ACCEPTED;
      }
  
    private:
      void start_of_simulation() {
        gs::socket::config<gs_generic_signal_protocol_types> tmp_conf = get_recent_config();
        unsigned int maxExts = tmp_conf.get_upper_bound_for_extension_ids();
        // Create a map containing all the extensions used by this socket
        for (unsigned int j = 0; j < maxExts; ++j) {
          if (tmp_conf.has_extension(j) != gs::ext::gs_reject) {
            signal_map.insert(std::pair<unsigned int, unsigned char>(j,0));
          }
        }
      }
    
      bool is_src_supported (unsigned int srcId) {
        if (supported_srcId_ext.empty()) {
          return true; //by default listen from every source
        }
        else if (supported_srcId_ext.find(srcId) != supported_srcId_ext.end()) {
          return true;
        }
        return false;
      }
    
      bool is_ext_frm_src_supported (unsigned int extId, unsigned int srcId) {
        if (supported_srcId_ext.empty()) {
          return true; //by default listen from every source
        }
        else {
          assert(supported_srcId_ext.find(srcId) != supported_srcId_ext.end());
          unsigned int maxExts = supported_srcId_ext[srcId].size();
          for (unsigned int j = 0; j < maxExts; ++j) {
            if (supported_srcId_ext[srcId][j] == extId)
              return true;
          }
        }
        return false;
      }
  
      //return true iff the extension was valid and was expected from this source
      bool store_signal_value (gs_generic_signal_payload& trans) {
        unsigned int srcId = trans.get_src_id();
        if (!is_src_supported(srcId)) return false;

        //store the value for this extension
        unsigned int id = 0;
        tlm::tlm_extension_base* ext = 0;
        gs::ext::gs_extension_base* gs_ext = 0;
  
        std::map<unsigned int, unsigned char>::iterator it = signal_map.begin();
        std::map<unsigned int, unsigned char>::iterator end_it = signal_map.end();
  
        for (; it != end_it; ++it) {
          id = (*it).first;
          if ((ext = trans.get_extension(id))) { //check if the extension is present
            if (gs::ext::extension_cast()[id]==NULL)
              continue;
            gs_ext = gs::ext::extension_cast()[id](ext);
  
            if (gs_ext && gs_ext->is_valid()) { //check if the extension is valid one
  
              //check if the valid extension is one of the expected from this src
              if (is_ext_frm_src_supported(id, srcId)) {
                (*it).second = *(trans.get_data_ptr());
                //std::cout << "updated for " << id << " with " << (unsigned int)data << "\n";
                return true;
              }
              else return false;
            }
          }
        }
        return false;
      }

    private:
      MODULE* parent_mod;
      //KEY is extId and VALUE is value of this extension
      std::map< unsigned int , unsigned char > signal_map;
      //KEY is srcId and VALUE is list of extIds supported from this src
      std::map<unsigned int, std::vector<unsigned int> > supported_srcId_ext;
      //std::vector<unsigned int > supported_srcid_list;
      sync_enum_type (MODULE::*nb_cb)(transaction_type&, phase_type&, sc_core::sc_time&) ;
      void (MODULE::*b_cb)(transaction_type&, sc_core::sc_time&) ;
  };
}
#endif /*TARGET_SIGNAL_SOCKET_H_*/
