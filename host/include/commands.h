#ifndef COMMANDS_H
#define COMMANDS_H

#include <stddef.h>

int cmd_execute(char* cmd);

#define COMMANDS \
  X(cmd_help, "help", "h") \
  X(cmd_quit, "quit", "q") \
  X(cmd_scan_devices, "scan", "s") \
  X(cmd_connect, "connect", "c") \
  X(cmd_disconnect, "disconnect", "dis") \
  X(cmd_status, "status", "st") \
  X(cmd_ping, "ping", "p") \
  X(cmd_compat_code, "compat", "cpt") \
  X(cmd_version, "version", "v") \
  X(cmd_show_debug_logs, "logs", "log") \
  X(cmd_clear_gui_logs, "clear", "cl") \
  X(cmd_ext_clock_en, "clke", "ce") \
  X(cmd_read_bus_state, "rbus", "rb") \
  X(cmd_step_clock, "cstep", "cs") \
  X(cmd_step_instruction, "step", "stp") \
  X(cmd_reset_cpu, "reset", "rs") \
  X(cmd_mem_read, "read", "r") \
  X(cmd_mem_write, "write", "w") \
  X(cmd_mem_dump, "dump", "dmp") \
  X(cmd_mem_flash, "flash", NULL) \

#define X(CMD, NAME, ALT) \
  int CMD(size_t arg_cnt, const char** tokens);
COMMANDS
#undef X


/**
 * The debugger is very confusing. It exists, but it's very confusing.
 * I don't understand my debugger.
 * It's a low-level debugger and if you're not familiar with
 * a low-level debugger you don't know what you're talking about.
 * - Terry A. Davis
*/

#endif
