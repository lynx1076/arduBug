#include "cli.h"
#include "utils.h"
#include "commands.h"
#include "result.h"
#include <ncurses.h>
#include <string.h>
#include <stddef.h>

#define MAX_USER_INPUT				256
#define MAX_TOKENS					  ((MAX_USER_INPUT + 1) / 2)

#define CLI_PROMPT_NORMAL	    ">>>"

CliState cliState = CliState_Normal;

static char new_input[MAX_USER_INPUT + 1];
static int new_input_len = 0;

static char input[MAX_USER_INPUT + 1];
static int input_len = 0;

static char* input_tokens[MAX_TOKENS];
static int input_tokens_cnt = 0;

static char* prompt = CLI_PROMPT_NORMAL;

void tokenize(void) {
  memcpy(input, new_input, new_input_len);
  input[new_input_len] = '\0';

  input_len = new_input_len;

  input_tokens_cnt = 0;

  char* new_token = strtok(input, " \t");
  
  while (new_token != NULL) {
    input_tokens[input_tokens_cnt] = new_token;
    input_tokens_cnt++;

    new_token = strtok(NULL, " \t");
  }
}

int cli_update(void) {
	printw("%s", prompt);
	refresh();

	if (getnstr(new_input, MAX_USER_INPUT) == ERR) RES_RETURN(r_ESYS, -1);
  new_input_len = strlen(new_input);
	if (new_input_len == ERR) RES_RETURN(r_ESYS, -1);

  if (new_input_len != 0) tokenize();
  else if (input_len) {
    int x, y;
    
    getyx(stdscr, y, x);
    move(y - 1, x);
    clrtoeol();

    printw("(repeated) %s%s\n", prompt, input);
  } else {
    RES_RETURN(r_ENONE, 0);
  }

  int cmd_res = 0;

	if (input_tokens_cnt == 0) RES_RETURN(r_ENONE, 0);
#define X(CMD_FUNC, CMD, ALT) \
	else if (strcmp(CMD, input_tokens[0]) == 0 || strcmp(ALT, input_tokens[0]) == 0) \
	  cmd_res = CMD_FUNC(input_tokens_cnt - 1, (const char**)input_tokens);

	COMMANDS
#undef X
	else {
	  printw("Command not recognized\n");
    cmd_res = -1;
	}

	if (_res != r_ENONE) {
	  debug_log(log_WARN, "Command returned error code: (%i)\n-> %s\n", _res, res_get_string(_res));
	  printw("Command returned error code: (%i)\n-> %s\n", _res, res_get_string(_res));
	}

  refresh();

  return cmd_res;
}

