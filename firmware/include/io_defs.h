#ifndef PINS_H
#define PINS_H

#include "io.h"

extern IOPin pin_EXT_CPU_EN;
extern IOPin pin_EXT_RESETB;
extern IOPin pin_EXT_CLK_EN;
extern IOPin pin_EXT_CLK;
extern IOPin pin_DEV_TBO;
extern IOPin pin_VPB;
extern IOPin pin_MLB;

extern IOPin pin_SYNC;
extern IOPin pin_DEV_ENB;
extern IOPin pin_RWB;
extern IOPin pin_NMIB;
extern IOPin pin_IRQB;

extern IOPin pin_DATA_0;
extern IOPin pin_DATA_1;
extern IOPin pin_DATA_2;
extern IOPin pin_DATA_3;
extern IOPin pin_DATA_4;
extern IOPin pin_DATA_5;
extern IOPin pin_DATA_6;
extern IOPin pin_DATA_7;
extern IOPin* bus_data[8];

extern IOPin pin_ADDR_00;
extern IOPin pin_ADDR_01;
extern IOPin pin_ADDR_02;
extern IOPin pin_ADDR_03;
extern IOPin pin_ADDR_04;
extern IOPin pin_ADDR_05;
extern IOPin pin_ADDR_06;
extern IOPin pin_ADDR_07;
extern IOPin pin_ADDR_08;
extern IOPin pin_ADDR_09;
extern IOPin pin_ADDR_10;
extern IOPin pin_ADDR_11;
extern IOPin pin_ADDR_12;
extern IOPin pin_ADDR_13;
extern IOPin pin_ADDR_14;
extern IOPin pin_ADDR_15;
extern IOPin* bus_addr[16];

#endif

