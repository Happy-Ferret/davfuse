#ifndef LOGGING_H
#define LOGGING_H

#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "c_util.h"
#include "logging_types.h"
#include "log_printer.h"

#ifdef __cplusplus
extern "C" {
#endif

/* initializes to zero so logging must be explicitly enabled */
#ifndef _IS_LOGGING_C
extern log_level_t _logging_cur_level;
#endif

/* NB: perhaps this should just be a header function */
#define log(level, ...)				 \
  do {						 \
    if ((level) <= _logging_cur_level) {	 \
      log_printer_print(__FILE__, __LINE__, level, __VA_ARGS__);        \
    }						 \
  }						 \
  while (false)

#define log_debug(...) log(LOG_DEBUG, __VA_ARGS__)
#define log_info(...) log(LOG_INFO, __VA_ARGS__)
#define log_warning(...) log(LOG_WARNING, __VA_ARGS__)
#define log_error(...) log(LOG_ERROR, __VA_ARGS__)
#define log_critical(...) log(LOG_CRITICAL, __VA_ARGS__)

#define log_critical_errno(str) \
  log_critical(str ": %s", strerror(errno))
#define log_error_errno(str) \
  log_error(str ": %s", strerror(errno))

void
logging_set_global_level(log_level_t new_level);

#ifdef __cplusplus
}
#endif

#endif
