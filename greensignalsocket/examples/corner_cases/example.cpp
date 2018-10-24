#include "example.h"

/*
 * This check some odd corner cases
 */

int sc_main(int argc, char** argv) {

  Master m("m");
  Slave  s("s");
  SlaveMulti s2("s2");
  
  assert(argc == 2);
  m.test_id = atoi(argv[1]);
  s.test_id = atoi(argv[1]);
  s2.test_id = atoi(argv[1]);

  m.socket(s.socket);
  m.socket2(s2.socket);
  sc_core::sc_start();

  return 0;
}

