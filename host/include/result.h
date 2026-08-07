#ifndef RESULT_H
#define RESULT_H

#include <threads.h>
#define RES_RETURN(RESULT, VALUE)         do { _res = RESULT; return VALUE; } while (0)

#define RESULTS \
  X(r_ENONE) \
  X(r_EUNKNOWN) \
  X(r_DATA_READY) \
  X(r_ENO_DATA) \
  X(r_ENULL_PTR) \
  X(r_EENCODING) \
  X(r_EMEM) \
  X(r_ESYS) \
  X(r_ETIMEOUT) \
  X(r_EBOUNDS) \
  X(r_EARGS) \
  X(r_EDEVICE) \
  X(r_EPAYLOAD_SIZE) \
  X(r_ENOT_INIT) \
  X(r_ENOT_CONNECTED) \
  X(r_EDOUBLE_INIT) \
  X(r_EINVALID_CMD) \
  X(r_EPARSE) \
  X(r_EMISSING_DATA) \
  X(r_ECOMPATIBILITY) \


typedef enum {
#define X(RESULT) \
  RESULT,

  RESULTS

#undef X
} result;

extern thread_local result _res;

char* res_get_string(result res);

#endif
