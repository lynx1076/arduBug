#include "result.h"
#include <threads.h>

result thread_local _res;

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

