#include "serial.h"
#include "ucobs.h"
#include "result.h"
#include "utils.h"
#include "vector.h"
#include <raylib.h>
#include <ncurses.h>
#include <sched.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <dirent.h>
#include <string.h>

#ifdef DEBUG
#define RECV_TIMEOUT_MS               999999999
#else
#define RECV_TIMEOUT_MS               20000
#endif
#define READ_TIMEOUT_100MS            1

static int device_fd = -1;
static char device_path[SERIAL_PORT_PATH_MAX] = {0};

int ser_scan_ports(Vec* return_vec) {
  if (vec_init(return_vec, SERIAL_PORT_PATH_MAX)) return -1;

  DIR* dev_dir = opendir("/dev");
  if (!dev_dir) {
    RES_RETURN(r_ESYS, -1);
  }

  struct dirent* entry;

  while ((entry = readdir(dev_dir)) != NULL) {
    if (strncmp(entry->d_name, "ttyUSB", 6) == 0) {
      char full_path[PATH_MAX];
      snprintf(full_path, sizeof(full_path), "/dev/%s", entry->d_name);
      debug_log(log_INFO, "Discovered open port: %s", full_path);
      if (vec_push(return_vec, full_path)) {
        closedir(dev_dir);
        return -1;
      }
    }
  }

  closedir(dev_dir);

  RES_RETURN(r_ENONE, 0);
}

int ser_open(char* path) {
  ser_close();

  if (path == NULL) {
    return r_EARGS;
  }

  int fd = open(path, O_RDWR | O_NOCTTY);
  if (fd < 0) {
    RES_RETURN(r_ESYS, -1);
  }

  struct termios tty;
  if (tcgetattr(fd, &tty) != 0) {
    close(fd);
    RES_RETURN(r_ESYS, -1);
  }

  cfmakeraw(&tty);

  tty.c_cflag = CLOCAL | CREAD;   // ignore modem ctrl lines, enable read
  tty.c_cflag &= ~CSIZE;          // ensure size bits empty
  tty.c_cflag |= CS8;             // 8 data bits
  tty.c_cflag &= ~PARENB;         // no parity
  tty.c_cflag &= ~CSTOPB;         // 1 stop bit
  tty.c_cflag &= ~CRTSCTS;        // no hardware flow control

  tty.c_iflag = 0;
  tty.c_iflag &= ~(IXON | IXOFF | IXANY); // no software flow control

  tty.c_oflag = 0;

  tty.c_lflag = 0; // raw input (no echo, no canonical mode)

  tty.c_cc[VMIN]  = 0;
  tty.c_cc[VTIME] = READ_TIMEOUT_100MS;

  if (cfsetispeed(&tty, SER_BAUDRATE) != 0 || cfsetospeed(&tty, SER_BAUDRATE) != 0) {
    close(fd);
    RES_RETURN(r_ESYS, -1);
  }

  if (tcsetattr(fd, TCSANOW, &tty) != 0) {
    close(fd);
    RES_RETURN(r_ESYS, -1);
  }

  if (tcflush(fd, TCIOFLUSH)) {
    close(fd);
    RES_RETURN(r_ESYS, -1);
  }

  device_fd = fd;
  snprintf(device_path, SERIAL_PORT_PATH_MAX, "%s", path);

  RES_RETURN(r_ENONE, 0);
}

void ser_close(void) {
  close(device_fd);
  device_fd = -1;
  *device_path = '\0';
}

char* ser_get_device(void) {
  return device_path;
}

bool ser_is_open(void) {
  if (*device_path != '\0' && access(device_path, F_OK) != 0) {
    ser_close();
  }

  return *device_path != '\0';
}

int ser_write(size_t length, const uint8_t* data) {
  if (!ser_is_open()) {
    RES_RETURN(r_ENOT_CONNECTED, -1);
  }

  ssize_t result = write(device_fd, data, length);

  if (result <= 0 || (size_t)result != length) {
    ser_close();
    RES_RETURN(r_ESYS, -1);
  }

#ifdef DEBUG
  for (size_t i = 0; i < length; i++) {
    printf("Write 0x%02x\n", data[i]);
  }
#endif

  RES_RETURN(r_ENONE, 0);
}

int ser_read(size_t* length, uint8_t* data) {
  if (!ser_is_open()) {
    RES_RETURN(r_ENOT_CONNECTED, -1);
  }

  if (data == NULL) {
    RES_RETURN(r_EARGS, -1);
  }

  if (length == NULL) {
    RES_RETURN(r_EARGS, -1);
  }

  ssize_t read_res = read(device_fd, data, *length);

  if (read_res < 0) {
    ser_close();
    RES_RETURN(r_ESYS, -1);
  } else if (read_res >= 1) {
    *length = read_res;

    RES_RETURN(r_DATA_READY, 0);
  }

#ifdef DEBUG
  for (size_t i = 0; i < *length; i++) {
    printf("Read 0x%02x\n", data[i]);
  }
#endif

  RES_RETURN(r_ENONE, 0);
}

int ser_enc_read(uint8_t* length, uint8_t* data) {
  if (*length > UCOBS_MAX_DATA_LEN) RES_RETURN(r_EARGS, -1);

  long i = 0;
  uint32_t last_byte_time = millis();
  bool recv_sync = false;
  uint8_t data_buf[UCOBS_MAX_PACKET_LEN];

  while (i < UCOBS_MAX_PACKET_LEN) {
    if (millis() - last_byte_time >= RECV_TIMEOUT_MS) {
      RES_RETURN(r_ENO_DATA, -1);
    }

    size_t bytes_to_read = 1;
    if (ser_read(&bytes_to_read, data_buf + i)) {
      return -1; 
    }

    if (bytes_to_read == 0) {
      continue;
    }

    last_byte_time = millis();

    if (data_buf[i] == 0x00) {
      if (i - 2 > *length) RES_RETURN(r_EDEVICE, -1);
      if (recv_sync) {
        int decoded = ucobs_decode(i - 1, data_buf + 1, data);
        
        if (decoded < 0) {
          RES_RETURN(r_EENCODING, -1);
        }

        *length = decoded;
        RES_RETURN(r_DATA_READY, 0);
      } else {
        recv_sync = true;
      }
    }

    i++;
  }

  RES_RETURN(r_ETIMEOUT, -1);
}

int ser_enc_write(size_t length, const uint8_t* data) {
  if (data == NULL) RES_RETURN(r_ENULL_PTR, -1);
  if (length > UCOBS_MAX_DATA_LEN) RES_RETURN(r_EPAYLOAD_SIZE, -1);

  if (length == 0) RES_RETURN(r_ENONE, 0);

  uint8_t buf[UCOBS_MAX_PACKET_LEN];

  int encoded_len = ucobs_encode(length, data, buf + 1);
  if (encoded_len < 0) RES_RETURN(r_EENCODING, -1);

  buf[0] = 0x00;
  buf[encoded_len + 1] = 0x00;

  if (ser_write(encoded_len + UCOBS_LEN_FRAME, buf)) return -1;

  RES_RETURN(r_ENONE, 0);
}

int ser_enc_write_va(unsigned int length, ...) {
  if (length > UCOBS_MAX_DATA_LEN) RES_RETURN(r_EPAYLOAD_SIZE, -1);

  va_list va_args;
  uint8_t buf[UCOBS_MAX_PACKET_LEN];

  va_start(va_args, length);

  for (size_t i = 1; i <= length; i++) {
    buf[i] = va_arg(va_args, int);
  }

  va_end(va_args);

  int encoded_len = ucobs_encode(length, buf + 1, buf + 1);
  if (encoded_len < 0) RES_RETURN(r_EENCODING, -1);

  buf[0] = 0x00;
  buf[encoded_len + 1] = 0x00;

  if (ser_write(encoded_len + UCOBS_LEN_FRAME, buf)) return -1;

  RES_RETURN(r_ENONE, 0);
}

int ser_enc_read_va(unsigned int expected_length, ...) {
  uint8_t read_length = expected_length;
  uint8_t read_data[UCOBS_MAX_DATA_LEN];

  if (ser_enc_read(&read_length, read_data)) return -1;
  if (_res != r_DATA_READY) RES_RETURN(r_ENO_DATA, -1);

  va_list va_args;

  va_start(va_args, expected_length);

  for (size_t i = 0; i < read_length; i++) {
    *va_arg(va_args, uint8_t*) = read_data[i];
  }

  va_end(va_args);

  if (read_length != expected_length) RES_RETURN(r_EMISSING_DATA, read_length);

  RES_RETURN(r_ENONE, 0);
}

