#include "config.h"
#include <iostream>


using namespace std;


int main(int argc, char *argv[]) {
  if (argc != 2) {
    return -1;
  }
  else {
    DM::Config my_conf = DM::Config( argv[1] );
    cout << my_conf;
  }

  return 0;
}
