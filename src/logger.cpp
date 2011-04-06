#include "logger.h"
#include <time.h>
using namespace std;


namespace DM {

ostream& Logger::debug() { return Logger::log(LOG_DEBUG); }

ostream& Logger::info() { return Logger::log(LOG_INFO); }

ostream& Logger::error() { return Logger::log(LOG_ERROR); }

ostream& Logger::log(LogLevel level) {
  static ostream logstream( clog.rdbuf() );
  logstream.rdbuf( (level >= LOG_LEVEL)? clog.rdbuf() : 0 );

  time_t now = time(NULL);
  string time_string = ctime(&now);
  string level_name;
  switch (level) {
  case LOG_DEBUG:
    level_name = "debug";
    break;
  case LOG_INFO:
    level_name = "info";
    break;
  case LOG_ERROR:
    level_name = "error";
    break;
  }
  time_string.erase( time_string.find('\n', 0), 1 );
  logstream << "[" << time_string << "] " << "[" + level_name + "] ";
  return logstream;
}

} // namespace DM
