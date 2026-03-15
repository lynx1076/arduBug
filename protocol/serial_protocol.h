#ifndef SERIAL_PROTOCOL_H
#define SERIAL_PROTOCOL_H

#define SP_BAUD_RATE            9600

#define SP_CMD_WRITE            0xC0
#define SP_CMD_READ             0xC1
#define SP_CMD_WRITE_MODE       0xC2

#define SP_CMD_PING             0xCA
#define SP_CMD_TEXT             0xCB

#define SP_SIG_SYNC             0xA0
#define SP_SIG_ESC              0xA1
#define SP_SIG_ACK              0xA2
#define SP_SIG_NACK             0xA3

#define SP_CMD_VERSION_TXT      0xFA
#define SP_CMD_VERSION_CODE     0xFB


#define SP_BYTE_LIST \
  X(SP_CMD_WRITE) \
  X(SP_CMD_READ) \
  X(SP_CMD_WRITE_MODE) \
  X(SP_CMD_PING) \
  X(SP_CMD_TEXT) \
  X(SP_SIG_SYNC) \
  X(SP_SIG_ESC) \
  X(SP_SIG_ACK) \
  X(SP_SIG_NACK) \
  X(SP_CMD_VERSION_TXT) \
  X(SP_CMD_VERSION_CODE)

#endif
