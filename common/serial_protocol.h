#ifndef SERIAL_PROTOCOL_H
#define SERIAL_PROTOCOL_H

#define SP_COMPAT_CODE          0x04

#define SP_SIG_ERR              0xA3
#define SP_SIG_OK               0xA4
#define SP_SIG_END_CMD          0xA5

#define SP_LOW                  0
#define SP_HIGH                 1

#define SP_STATE_ERROR          0
#define SP_STATE_DEV_TBO        1
#define SP_STATE_VP             2
#define SP_STATE_SYNC           3
#define SP_STATE_ML             4
#define SP_STATE_WRITING        5
#define SP_STATE_NMI            6
#define SP_STATE_IRQ            7

// 0, TBO, VP, SYNC, ML, WRITING, NMI, IRQ
#define SP_CMD_GET_CPU_STATE        0xC0
#define SP_CMD_SET_CPU_EN           0xC1

#define SP_CMD_SET_EXT_CLOCK_EN     0xC2
#define SP_CMD_SET_EXT_CLOCK        0xC3

#define SP_CMD_READ_DATABUS         0xC4
#define SP_CMD_READ_ADDRBUS         0xC5

#define SP_CMD_MEM_READ             0xC6
#define SP_CMD_MEM_WRITE            0xC7

#define SP_CMD_MEM_BULK_READ        0xC8
#define SP_CMD_MEM_BULK_WRITE       0xC9

#define SP_CMD_SET_RESET            0xCA

#define SP_CMD_COMPAT_CODE          0xCB
#define SP_CMD_VERSION_TEXT         0xCC
#define SP_CMD_PING                 0xCD

#define SP_CMD_SDP_DISABLE          0xCE

#endif
