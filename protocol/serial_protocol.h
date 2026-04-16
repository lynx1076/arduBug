#ifndef SERIAL_PROTOCOL_H
#define SERIAL_PROTOCOL_H

#define SP_BAUD_RATE            B115200
#define SP_MAX_ARG_CNT          12
#define SP_COMPAT_CODE          0x02

#define SP_SIG_ESC              0xA2
#define SP_SIG_ERR              0xA3
#define SP_SIG_OK               0xA4
#define SP_SIG_END_CMD          0xA5

#define SP_CMD_RESET            0xC0
#define SP_CMD_PING             0xC1

#define SP_CMD_WRITE            0xC2
#define SP_CMD_WRITE_IODIR      0xC3
#define SP_CMD_WRITE_PULL_UP    0xC4

#define SP_CMD_READ             0xC5
#define SP_CMD_READ_IODIR       0xC6
#define SP_CMD_READ_PULL_UP     0xC7

#define SP_CMD_COMPAT_CODE      0xC8
#define SP_CMD_VERSION_TEXT     0xC9

#endif
