#include "cli.h"
#include "utils.h"
#include "commands.h"
#include "result.h"
#include <stdarg.h>
#include <ctype.h>
#include <ncurses.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <stddef.h>

#define USER_INPUT_BUF_SIZE				256
#define MAX_INPUT_TOKENS					      ((USER_INPUT_BUF_SIZE + 1) / 2)

#define CLI_PROMPT_NORMAL	        ">>>"

#define CTRL(k) ((k) & 0x1F)

static char* prompt = CLI_PROMPT_NORMAL;
static bool prompt_okay = false;

static int tokenize(char* str, char** tokens) {
  char* new_token = strtok(str, " \t");
  int token_cnt = 0;
  
  while (new_token != NULL) {
    tokens[token_cnt] = new_token;
    token_cnt++;

    new_token = strtok(NULL, " \t");
  }

  return token_cnt;
}

static void rel_move(int dx, int dy) {
  int x, y;
  getyx(stdscr, y, x);
  x += dx;
  y += dy;

  if (x < 0) x = 0;
  if (y < 0) y = 0;

  int mx, my;
  getmaxyx(stdscr, my, mx);
  my--;
  mx--;
  if (x > mx) x = mx;
  if (y > my) y = my;

  move(y, x);
}

static int read_str(char* str) {
  static int input_len = 0;
  static int input_pos = 0;
  static int column = 0;
  static char input_buff[USER_INPUT_BUF_SIZE];
  int _int;
  (void)_int;
  
  nodelay(stdscr, true);
  noecho();
  cbreak();
  keypad(stdscr, true);

  int retx, rety;
  getyx(stdscr, rety, retx);
  move(0, column);

  if (!prompt_okay) {
    printw("%s", prompt);
    getyx(stdscr, _int, column);
  }
  prompt_okay = true;

  int c = getch();
  if (c == ERR) {
    RES_RETURN(r_ENONE, 0);
  }

  switch (c) {
    case CTRL('a'): {
      rel_move(-input_pos, 0);
      input_pos = 0;
    } break;
    case KEY_LEFT: {
      if (input_pos) {
        input_pos--;
        rel_move(-1, 0);
      }
    } break;
    case KEY_RIGHT: {
      if (input_pos < input_len) {
        input_pos++;
        rel_move(1, 0);
      }
    } break;
    case '\r':
    case '\n':
    case KEY_ENTER: {
      printw("%.*s\n", input_len - input_pos, input_buff + input_pos);

      int _input_len = input_len;
      input_len = 0;
      input_pos = 0;

      strncpy(str, input_buff, _input_len);
      str[_input_len] = '\0';

      prompt_okay = false;

      move(rety, retx);
      refresh();
      RES_RETURN(r_DATA_READY, _input_len);
    }
    case KEY_BACKSPACE: {
      if (input_pos) {
        memmove(input_buff + input_pos - 1, input_buff + input_pos, input_len - input_pos);
        input_pos--;
        input_len--;
        
        int x, y;
        getyx(stdscr, y, x);
        move(y, x - 1);
        printw(" ");
        move(y, x - 1);
      }
    } break;
    case KEY_DC: {
      if (input_len - input_pos) {
        memmove(input_buff + input_pos, input_buff + input_pos + 1, input_len - input_pos);
        input_len--;
        printw("%.*s ", input_len - input_pos, input_buff + input_pos);
        rel_move(input_pos - input_len - 1, 0);
      }
    } break;
    default: {
      if (input_pos >= USER_INPUT_BUF_SIZE) input_pos = USER_INPUT_BUF_SIZE;
      else if (isprint(c)) {
        if (input_len > input_pos) {
          memmove(input_buff + input_pos + 1, input_buff + input_pos, input_len - input_pos);
        }
        input_buff[input_pos] = c;
        input_len++;
        printw("%.*s", input_len - input_pos, input_buff + input_pos);
        input_pos++;
        rel_move(input_pos - input_len, 0);
      }
    } break;
  }


  getyx(stdscr, _int, column);
  
  move(rety, retx);

  refresh();
  RES_RETURN(r_ENONE, 0);
}

void print(const char* str, ...) {
  int x, y;
  getyx(stdscr, y, x);

  move(y, 0);
  clrtoeol();

  va_list args;
  va_start(args, str);
  vw_printw(stdscr, str, args);
  va_end(args);

  int nx, ny;
  getyx(stdscr, ny, nx);

  if (ny == y) move(y + 1, 0);

  prompt_okay = false;

  refresh();
}

int cli_update(void) {
  char user_cmd[USER_INPUT_BUF_SIZE];
  if (read_str(user_cmd)) print("Said: %s", user_cmd);
	refresh();

  int cmd_res = 0;
  int token_cnt = 0;
  char* input_tokens[MAX_INPUT_TOKENS];

	if (token_cnt == 0) RES_RETURN(r_ENONE, 0);
#define X(CMD_FUNC, CMD, ALT) \
	else if (strcmp(CMD, input_tokens[0]) == 0 || strcmp(ALT, input_tokens[0]) == 0) \
	  cmd_res = CMD_FUNC(token_cnt - 1, (const char**)input_tokens);

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

