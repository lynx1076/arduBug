#ifndef RESULT_H
#define RESULT_H

typedef enum {
  r_ENONE = 0,
  r_DATA_READY,
  r_EMEM,
  r_ESYS,
  r_ETIMEOUT,
  r_EBOUNDS,
  r_EARGS,
  r_EDEVICE
} result;

#endif
