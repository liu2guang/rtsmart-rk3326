/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __RK3326_REG_H__
#define __RK3326_REG_H__

#include <rtthread.h>

#define DUMP_UART_REG(base, reg)                     \
do {                                                 \
    rt_uint32_t status = 0x00000000;                 \
    READ_UINT32(status, base + reg);                 \
    rt_kprintf(#reg ":\n");                          \
    rt_kprintf("\t%p 0x%.8x\n", base + reg, status); \
} while(0); 

#define READ_UINT8(ucValue, Addr)    (ucValue) = *((volatile rt_uint8_t *)((rt_uint32_t)(Addr)))
#define READ_UINT16(usValue, Addr)   (usValue) = *((volatile rt_uint16_t *)((rt_uint32_t)(Addr)))
#define READ_UINT32(uwValue, Addr)   (uwValue) = *((volatile rt_uint32_t *)((rt_uint32_t)(Addr)))
#define READ_UINT64(ullValue, Addr)  (ullValue) = *((volatile rt_uint64_t *)((rt_uint32_t)(Addr)))

#define WRITE_UINT8(ucValue, Addr)   *((volatile rt_uint8_t *)((rt_uint32_t)(Addr))) = (ucValue)
#define WRITE_UINT16(usValue, Addr)  *((volatile rt_uint16_t *)((rt_uint32_t)(Addr))) = (usValue)
#define WRITE_UINT32(uwValue, Addr)  *((volatile rt_uint32_t *)((rt_uint32_t)(Addr))) = (uwValue)
#define WRITE_UINT64(ullValue, Addr) *((volatile rt_uint64_t *)((rt_uint32_t)(Addr))) = (ullValue)

#define GET_UINT8(Addr)              ({rt_uint8_t  r_ = *((volatile rt_uint8_t  *)((rt_uint32_t)(Addr))); r_;})
#define GET_UINT16(Addr)             ({rt_uint16_t r_ = *((volatile rt_uint16_t *)((rt_uint32_t)(Addr))); r_;})
#define GET_UINT32(Addr)             ({rt_uint32_t r_ = *((volatile rt_uint32_t *)((rt_uint32_t)(Addr))); r_;})
#define GET_UINT64(Addr)             ({rt_uint64_t r_ = *((volatile rt_uint64_t *)((rt_uint32_t)(Addr))); r_;})

#define SET_UINT32(bit, Addr)        ((Addr) |= (1<<(bit)))
#define CLR_UINT32(bit, Addr)        ((Addr) &= ~(1<<(bit)))

#define writeb(value, address)       WRITE_UINT8(value, address)
#define writew(value, address)       WRITE_UINT16(value, address)
#define writel(value, address)       WRITE_UINT32(value, address)
     
#define readb(address)               GET_UINT8(address)
#define readw(address)               GET_UINT16(address)
#define readl(address)               GET_UINT32(address)

#define rk_clrsetreg(addr, clr, set) writel(((clr) | (set)) << 16 | (set), addr)
#define rk_clrreg(addr, clr)         writel((clr) << 16, addr)
#define rk_setreg(addr, set)         writel((set) << 16 | (set), addr)

#define rk_writeb(address, value)    WRITE_UINT8(value, address)
#define rk_writew(address, value)    WRITE_UINT16(value, address)
#define rk_writel(address, value)    WRITE_UINT32(value, address)
     
#define rk_readb(address)            GET_UINT8(address)
#define rk_readw(address)            GET_UINT16(address)
#define rk_readl(address)            GET_UINT32(address)

#endif /* __RK3326_REG_H__ */ 