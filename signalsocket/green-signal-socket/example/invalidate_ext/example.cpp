#include "example.h"
int sc_main(int argc, char** argv) {

  Master m("m");
  Slave  s("s");
  
  m.socket(s.socket);
  std::string name = m.socket.name();
  //s.socket.set_source<bar>(name);
  sc_core::sc_start();

  return 0;
}

