#ifndef _DM_LOGGER_H_
#define _DM_LOGGER_H_

#include <dm/type.h>
#include <iostream>
using namespace std;


namespace DM {

class Logger {
public:
  static ostream& debug();
  static ostream& info();
  static ostream& error();

private:
  static ostream& log(LogLevel);
};

} // namespace DM

#endif /* _DM_LOGGER_H_ */
