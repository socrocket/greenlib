#ifndef __SIMPLE_TLM_SERIAL_SLAVE__
#define __SIMPLE_TLM_SERIAL_SLAVE__

#include "greenserialsocket/tlm_serial_target_socket.h"

using namespace tlm_serial;

class SerialSlave:public sc_module
{

 public:
   tlm_serial_target_socket<> slave_sock;

   SerialSlave (sc_module_name nm):sc_module(nm),slave_sock("slave_sock")
   {
     slave_sock.register_b_transport(this, &SerialSlave::b_transport);
     slave_sock.register_nb_transport_fw(this, &SerialSlave::nb_transport_fw);
   }

   void b_transport(tlm_serial::tlm_serial_payload& gp, sc_core::sc_time& time);
   tlm::tlm_sync_enum nb_transport_fw(tlm_serial::tlm_serial_payload& gp,tlm::tlm_phase& ph, sc_core::sc_time& time);
};


void SerialSlave::b_transport(tlm_serial::tlm_serial_payload& gp, sc_core::sc_time& time)
{
  cout<< "Inside b_transport"<< endl;

  unsigned char *data;
  uint32_t len;
  bool *ser_par;

  data = gp.get_data_ptr();
  len = gp.get_data_length();
  ser_par = gp.get_parity_bits();
  
  std::cout<< "At Serial Receiver" << endl;
  std::cout<< "Total number of charaters recieved is " << len <<endl;

  if (gp.isParityEnable())
      std::cout<< "Parity Bits are enabled" <<endl;
  else
      std::cout<< "Parity Bits are not enabled" <<endl;

  if (gp.isValidBitEnable())
      std::cout<< "Number of Valid bits in the char are "<< static_cast<uint32_t> (gp.get_valid_bits())  <<endl;
  else
      std::cout<< "Valid Bit option is not enable thus assuming 8 number of valid bits in a character." <<endl;

  if (gp.isStopBitEnable())
      std::cout<< "Number of stop bits that follows each character is "<< static_cast<uint32_t>(gp.get_num_stop_bits_in_end()) <<endl;
  else
      std::cout<< "Stop Bit option is not enabled" <<endl;

  if (gp.isBaudRateEnable())
      std::cout<< "Baud rate is set to " << gp.get_baudrate() <<endl;
  else
      std::cout<< "Baud rate is not set" <<endl;


  std::cout<< "Total " << len << " characters are received. They are: " <<endl;

  for (size_t i = 0 ; i < len; i++)
    std::cout<< "  " << data[i] << " with parity " << ser_par[i]  << endl;

  
}

tlm::tlm_sync_enum SerialSlave::nb_transport_fw(tlm_serial::tlm_serial_payload& gp,tlm::tlm_phase& ph, sc_core::sc_time& time)
{
  cout<< "Inside nb_transport_fw"<< endl;

  unsigned char *data;
  uint32_t len;
  bool *ser_par;

  if (ph == tlm_serial::BEG_TRANSMISSION)
     std::cout<< "BEG_TRANSMISSION phase received" << endl;
  else
  {
     std::cout<< "UNKNOWN phase received" << endl;
     gp.set_response_status(tlm_serial::SERIAL_ERROR_RESPONSE);
     return TLM_COMPLETED;
  }

  data = gp.get_data_ptr();
  len = gp.get_data_length();
  ser_par = gp.get_parity_bits();
  
  std::cout<< "At Serial Receiver" << endl;
  std::cout<< "Total number of charaters recieved is " << len <<endl;

  if (gp.isParityEnable())
      std::cout<< "Parity Bits are enabled" <<endl;
  else
      std::cout<< "Parity Bits are not enabled" <<endl;
  if (gp.isValidBitEnable())
      std::cout<< "Number of Valid bits in the char are "<< static_cast<uint32_t> (gp.get_valid_bits())  <<endl;
  else
      std::cout<< "Valid Bit option is not enable thus assuming 8 number of valid bits in a character." <<endl;
  if (gp.isStopBitEnable())
      std::cout<< "Number of stop bits that follows each character is "<< static_cast<uint32_t>(gp.get_num_stop_bits_in_end()) <<endl;
  else
      std::cout<< "Stop Bit option is not enabled" <<endl;
  if (gp.isBaudRateEnable())
      std::cout<< "Baud rate is set to " << gp.get_baudrate() <<endl;
  else
      std::cout<< "Baud rate is not set" <<endl;

  std::cout<< "Total " << len << " characters are received. They are: " <<endl;

  for (size_t i = 0 ; i < len; i++)
    std::cout<< "  " << data[i] << " with parity " << ser_par[i]  << endl;

  return TLM_COMPLETED;
}





#endif
