#include <windows.h>

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "c_util.h"
#include "log_printer_outputdebugstring.h"

bool
log_printer_outputdebugstring_default_init(void) {
  return true;
}

void
log_printer_outputdebugstring_shutdown(void) {
}

void
log_printer_outputdebugstring_print(const char *filename, int lineno,
                                    log_level_t level,
                                    const char *format, ...) {
  /* figure out how many chacters are needed */
  va_list ap;

  va_start(ap, format);
  const int chars_needed =
    _vscprintf(format, ap);
  va_end(ap);

  if (chars_needed < 0) {
    /* TODO: convert string using utf8_to_mb in fs_win32.c,
       and use OutputDebugStringW */
    OutputDebugStringA("Error while getting necessary length for log");
    return;
  }

  char *const buf = malloc(chars_needed + 1);
  if (!buf) {
    OutputDebugStringA("Error while malloc'ing buf for log");
    return;
  }

  UNUSED(filename);
  UNUSED(lineno);
  UNUSED(level);

  va_start(ap, format);
  const int chars_printed =
    vsnprintf(buf, chars_needed + 1, format, ap);
  va_end(ap);

  if (chars_printed >= 0) {
    assert(chars_printed == chars_needed);
    OutputDebugStringA(buf);
  }
  else {
    OutputDebugStringA("Couldn't print string");
  }

  free(buf);
}
