#ifndef IO_H
#define IO_H

#include <stdint.h>
#include <stdbool.h>
#define IOX0_ADDR         0x20
#define IOX1_ADDR         0x21

#define IOX_IODIRA        0x00
#define IOX_IODIRB        0x01
#define IOX_GPIOA         0x12
#define IOX_GPIOB         0x13
#define IOX_OLATA         0x14
#define IOX_OLATB         0x15

#define IOX_OUTPUT        0
#define IOX_INPUT         1

#define IOX_PORT_OUTPUT   0x00
#define IOX_PORT_INPUT    0xFF

#define PIN_EXT_RESETB        0
#define PIN_EXT_CLK_EN        1
#define PIN_EXT_CPU_EN        2
#define PIN_DEV_TBO           3
#define PIN_VPB               4
#define PIN_SYNC              5
#define PIN_MLB               6

#define PIN_DEV_ENB           0
#define PIN_RWB               1
#define PIN_NMIB              2
#define PIN_IRQB              3

#define PIN_EXT_CLK           PD2

#define LOW               0
#define HIGH              1

uint8_t io_init(void);

uint8_t io_set_cpu_en(bool enable);

uint8_t io_set_ext_clk_en(bool enable);
void io_set_ext_clk(bool high);

uint8_t io_set_rw(bool writing);
uint8_t io_get_rw(bool* writing);
uint8_t io_highz_rw(void);

uint8_t io_set_dev_en(bool enable);

uint8_t io_set_reset(bool reset);

uint8_t io_get_dev_tbo(bool* owned);
uint8_t io_get_vp(bool* pull_vector);
uint8_t io_get_sync(bool* sync);
uint8_t io_get_ml(bool* memory_block);

uint8_t io_get_nmi(bool* active);
uint8_t io_set_nmi(bool active);

uint8_t io_get_irq(bool* active);
uint8_t io_set_irq(bool active);

uint8_t io_read_databus(void);
void io_write_databus(uint8_t value);
void io_highz_databus(void);

uint8_t io_read_addrbus(uint16_t* addr);
uint8_t io_write_addrbus(uint16_t addr);
uint8_t io_highz_addrbus(void);

#endif

