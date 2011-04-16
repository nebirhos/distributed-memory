/**
 * @file logger.h
 *
 * @author Francesco Disperati
 *
 * Released under GNU GPLv3 (http://www.gnu.org/licenses/gpl-3.0.txt)
 */

#ifndef _DM_LOGGER_H_
#define _DM_LOGGER_H_

#include <dm/type.h>
#include <iostream>
using namespace std;


namespace DM {

/**
 * Simple logger utility.
 * Writes stuff to std::clog stream, according to their priority.
 * Log entries are in this form:
 *
 * @verbatim [timestamp] [level info] ... message ... @endverbatim
 *
 * Priority levels are defined in DM::LogLevel.
 * Even if operations on POSIX streams should be atomic, this class is
 * NOT thread-safe.
 */
class Logger {
public:
  /** Writes with debug priority */
  static ostream& debug();
  /** Writes with info priority */
  static ostream& info();
  /** Writes with error priority */
  static ostream& error();

private:
  /** Writes stream */
  static ostream& log(LogLevel);
};

} // namespace DM

#endif /* _DM_LOGGER_H_ */
