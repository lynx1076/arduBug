#ifndef SERIAL_PROTOCOL_H
#define SERIAL_PROTOCOL_H

#define SP_COMPAT_CODE          0x04

#define SP_SIG_ERR              0xA3
#define SP_SIG_OK               0xA4
#define SP_SIG_END_CMD          0xA5

#define SP_LOW                  0
#define SP_HIGH                 1

#define SP_INPUT                0
#define SP_OUTPUT               1

// CMD -> OK/ERR 0/1
#define SP_CMD_GET_CPU_READING  0xC0

// CMD 0/1 -> OK/ERR
#define SP_CMD_SET_EXT_CLOCK_EN 0xC4
// CMD 0/1 -> OK/ERR
#define SP_CMD_SET_CLOCK_LEVEL  0xC5

// CMD -> OK/ERR 0x00-0xFF
#define SP_CMD_READ_DATABUS     0xCA
// CMD 0x00-0xFF -> OK/ERR
#define SP_CMD_WRITE_DATABUS    0xCB

// CMD -> OK/ERR 0x0000-0xFFFF
#define SP_CMD_READ_ADDRBUS     0xCC
// CMD 0x0000-0xFFFF -> OK/ERR
#define SP_CMD_WRITE_ADDRBUS    0xCD

#define SP_CMD_COMPAT_CODE      0xC8
#define SP_CMD_VERSION_TEXT     0xC9
#define SP_CMD_PING             0xCA

#endif
