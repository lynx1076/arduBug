#ifndef CLI_H
#define CLI_H


int cli_update(void);

typedef enum {
  CliState_Normal,
  CliState_Debug,
  CliState_Override,
  CliState_EmulateMemory
} CliState;


extern CliState cliState;

#endif
