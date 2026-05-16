#include "cli_handler.h"
#include "commands.h"
#include "result.h"
#include "utils.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>

void clh_update(void) {
  result _res;
  static bool prompt_intact = false;
  char user_input[CLH_MAX_USER_INPUT_LEN];
  
  const int max_tokens = (CLH_MAX_USER_INPUT_LEN + 1) / 2;
  char* tokens[max_tokens];
  
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

  user_input[strcspn(user_input, "\r\n")] = '\0';

  char* token = strtok(user_input, " \t\r\n");
  while (token != NULL) {
    if (token_cnt >= max_tokens) {
      debug_log(log_ERR, "Too many input tokens entered");
      break; 
    }
    tokens[token_cnt] = token;
    token_cnt++;
    token = strtok(NULL, " \t\r\n");
  }

  if (token_cnt == 0) {
    return;
  }

  if (0);
#define X(CMD_FUNC, CMD) \
  else if (strcmp(CMD, tokens[0]) == 0) _res = CMD_FUNC((const char**)tokens, token_cnt - 1);
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

