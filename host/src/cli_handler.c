#include "cli_handler.h"
#include "commands.h"
#include "result.h"
#include "utils.h"
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

void clh_update(void) {
  result _res;
  static bool prompt_intact = false;
  char user_input[CLH_MAX_USER_INPUT_LEN];
  char* tokens[(CLH_MAX_USER_INPUT_LEN + 1) / 2];
  user_input[0] = '\0';
  int token_cnt = 0;

  if (!prompt_intact) {
    printf(CLH_PROMPT);
    fflush(stdout);
    prompt_intact = true;
  }

  if (fgets(user_input, CLH_MAX_USER_INPUT_LEN, stdin) == NULL) {
    debug_log(log_ERR, "Failed to retrieve user input");
    return;
  }

  prompt_intact = false;

  char* token_r = user_input;
  char* token_w = user_input;
  char* token_start = user_input;

  while (*token_r != '\0') {
    while (*token_r != '\0' && !isspace(*token_r)) *token_w++ = *token_r++;
    *token_r++ = '\0';
    while (*token_r != '\0' && isspace(*token_r)) token_r++;
    token_w = token_r;
    tokens[token_cnt] = token_start;
    token_start = token_r;
    token_cnt++;
  }

  if (0);
#define X(CMD_FUNC, CMD) \
  else if (strcmp(CMD, user_input) == 0) _res = CMD_FUNC((const char**)tokens, token_cnt - 1);
  COMMANDS
#undef X
  else {
    printf("Command not recognized\n");
    return;
  }

  if (_res != r_ENONE) {
    printf("Command returned error code: (%i)\n-> %s\n", _res, res_get_string(_res));
    debug_log(log_ERR, "Command failed");
  }
}

