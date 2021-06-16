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

#define READ_UINT8(ucValue, Addr)   (ucValue)  = *((volatile rt_uint8_t  *)((rt_uint32_t)(Addr)))
#define READ_UINT16(usValue, Addr)  (usValue)  = *((volatile rt_uint16_t *)((rt_uint32_t)(Addr)))
#define READ_UINT32(uwValue, Addr)  (uwValue)  = *((volatile rt_uint32_t *)((rt_uint32_t)(Addr)))
#define READ_UINT64(ullValue, Addr) (ullValue) = *((volatile rt_uint64_t *)((rt_uint32_t)(Addr)))

#define WRITE_UINT8(ucValue, Addr)   *((volatile rt_uint8_t *)((rt_uint32_t)(Addr))) = (ucValue)
#define WRITE_UINT16(usValue, Addr)  *((volatile rt_uint16_t *)((rt_uint32_t)(Addr))) = (usValue)
#define WRITE_UINT32(uwValue, Addr)  *((volatile rt_uint32_t *)((rt_uint32_t)(Addr))) = (uwValue)
#define WRITE_UINT64(ullValue, Addr) *((volatile rt_uint64_t *)((rt_uint32_t)(Addr))) = (ullValue)

#define GET_UINT8(Addr)  ({rt_uint8_t  r_ = *((volatile rt_uint8_t *)((rt_uint32_t)(Addr))); r_;})
#define GET_UINT16(Addr) ({rt_uint16_t r_ = *((volatile rt_uint16_t *)((rt_uint32_t)(Addr))); r_;})
#define GET_UINT32(Addr) ({rt_uint32_t r_ = *((volatile rt_uint32_t *)((rt_uint32_t)(Addr))); r_;})
#define GET_UINT64(Addr) ({rt_uint64_t r_ = *((volatile rt_uint64_t *)((rt_uint32_t)(Addr))); r_;})

#define writeb(value, address)  WRITE_UINT8(value, address)
#define writew(value, address)  WRITE_UINT16(value, address)
#define writel(value, address)  WRITE_UINT32(value, address)

#define readb(address)          GET_UINT8(address)
#define readw(address)          GET_UINT16(address)
#define readl(address)          GET_UINT32(address)

#define SET_UINT32(bit, Addr)   ((Addr) |= (1<<(bit)))
#define CLR_UINT32(bit, Addr)   ((Addr) &= ~(1<<(bit)))

void rt_hw_board_init(void);

#endif
