
#include "systemc.h"
#include "tlm.h"
#include "master.h"
#include "slave.h"

int sc_main(int argc, char** argv)
{
   
   SerialMaster master("master1");
   SerialSlave slave("slave1");

   master.master_sock(slave.slave_sock);

   sc_core::sc_start();
   return(0);
}




