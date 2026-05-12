#include "result.h"

char* res_get_string(result res) {
#define RES_CASE(RESULT) case RESULT: return #RESULT
  switch (res) {
    RES_CASE(r_ENONE);
    RES_CASE(r_DATA_READY);
    RES_CASE(r_EENCODING);
    RES_CASE(r_EMEM);
    RES_CASE(r_ESYS);
    RES_CASE(r_ETIMEOUT);
    RES_CASE(r_EBOUNDS);
    RES_CASE(r_EARGS);
    RES_CASE(r_EDEVICE);
    RES_CASE(r_ENOT_INIT);
    RES_CASE(r_EDOUBLE_INIT);
    RES_CASE(r_EUSER_CMD);
    RES_CASE(r_EUSER_PARSE);
    default:
      return "UNKNOWN";
  }
#undef RES_CASE
}

