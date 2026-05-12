#ifndef RESULT_H
#define RESULT_H

typedef enum {
  r_ENONE = 0,
  r_DATA_READY,
  r_EENCODING,
  r_EMEM,
  r_ESYS,
  r_ETIMEOUT,
  r_EBOUNDS,
  r_EARGS,
  r_EDEVICE,
  r_ENOT_INIT,
  r_EDOUBLE_INIT,
  r_EUSER_CMD,
  r_EUSER_PARSE,
} result;

char* res_get_string(result res);

#endif
