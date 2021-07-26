/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author         Notes
 * 2020-04-16     bigmagic       first version
 */

#ifndef BOARD_H__
#define BOARD_H__

#include <stdint.h>
#include "rk3326.h"

extern unsigned char __bss_start;
extern unsigned char __bss_end;

// DDR_P1 + DDR_P2 = 994MBytes (uboot dump)
//     memory.rgn[0].addr     = 0x00200000 - 0x08400000 (size: 0x08200000)
//     memory.rgn[1].addr     = 0x0a200000 - 0x40000000 (size: 0x35e00000)
#define RT_HW_DDR_P1_BEGIN (void*)&__bss_end
#define RT_HW_DDR_P1_END   (void*)(0x08400000 - 1) // 132MBytes 

#define RT_HW_DDR_P2_BEGIN (void*)(0x0a200000) 
#define RT_HW_DDR_P2_END   (void*)(RT_HW_DDR_P2_BEGIN + 0x35e00000 - 1) // 862MBytes 

void rt_hw_board_init(void);

#endif /*  */ 
