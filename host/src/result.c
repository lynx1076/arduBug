#include "result.h"
#include <threads.h>

thread_local result _res;

char* res_get_string(result res) {
  switch (res) {

#define X(RESULT) \
  case RESULT: return #RESULT;

    RESULTS

#undef X

    default:
      return "UNKNOWN";
  }
}

