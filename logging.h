#ifndef LOGGING_H
#define LOGGING_H

#include <stdio.h>

typedef enum {
  LOG_NOTHING,
  LOG_CRITICAL,
  LOG_ERROR,
  LOG_WARNING,
  LOG_INFO,
  LOG_DEBUG,
} log_level_t;

/* initializes to zero so logging must be explicitly enabled */
#ifndef _IS_LOGGING_C
extern FILE *_logging_dest;
extern log_level_t _logging_cur_level;
#endif

#define log(level, ...)                         \
  do {                                          \
    if ((level) > _logging_cur_level) {         \
      break;                                    \
    }                                           \
    assert(_logging_dest);                      \
    fprintf(_logging_dest, __VA_ARGS__);        \
  }                                             \
  while (0)

#define log_critical(...) log(LOG_CRITICAL, __VA_ARGS__)
#define log_warning(...) log(LOG_WARNING, __VA_ARGS__)

bool
init_logging(FILE *log_destination, log_level_t level);

#endif
