#include "result.h"

char* res_get_string(result res) {
#define CASE(RESULT) case RESULT: return #RESULT
  switch (res) {
    CASE(r_ESYS);
    CASE(r_ENONE);
    CASE(r_EENCODING);
    CASE(r_EARGS);
    CASE(r_EDEVICE);
    CASE(r_EMEM);
    CASE(r_EBOUNDS);
    CASE(r_ETIMEOUT);
    CASE(r_DATA_READY);
    default:
      return "UNKNOWN";
  }
#undef CASE
}

