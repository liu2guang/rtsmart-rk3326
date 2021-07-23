/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "drv_clk.h" 
#include "rthw.h"
#include "mmu.h"
#include "board.h"

/* 
   看门狗：
   1. 看门狗挂在 APB 总线上
   2. 非安看门狗 WDT_NS
   3. 安全看门狗 WDT_S
   4. WDT计数器的时钟是 pclk 
   5. 32位WDT计数器宽度
   6. 向下计数器
   7. WDT在超时时可以执行两种操作：
       - 系统复位
       - 首先产生一个中断，如果这个中断没有被服务程序清除，第二次超时发生的时间，然后生成系统复位 
   8. 可编程复位脉冲长度
   9. 总共定义了16个主超时时间范围
 */ 

#define RK3326_DUMP_REG(addr)             \
do {                                    \
    rt_uint32_t val = 0x00000000;       \
    val = readl(addr);                  \
    rt_kprintf(#addr ":\n");            \
    rt_kprintf("\t0x%.8x\n", val);      \
} while(0); 

#define WDT_S_BASE    0xFF1F0000
#define WDT_NS_BASE   0xFF1E0000
#define WDT_CR        0x0000 // W 0x0000000a Control Register
#define WDT_TORR      0x0004 // W 0x00000000 Timeout range Register
#define WDT_CCVR      0x0008 // W 0x0000ffff Current counter value Register
#define WDT_CRR       0x000c // W 0x00000000 Counter restart Register
#define WDT_STAT      0x0010 // W 0x00000000 Interrupt status Register
#define WDT_EOI       0x0014 // W 0x00000000 Interrupt clear Register

int wdt_test(void)
{
    RK3326_DUMP_REG(WDT_S_BASE + WDT_CR); 
    RK3326_DUMP_REG(WDT_S_BASE + WDT_TORR); 
    RK3326_DUMP_REG(WDT_S_BASE + WDT_CCVR); 
    RK3326_DUMP_REG(WDT_S_BASE + WDT_CRR); 
    RK3326_DUMP_REG(WDT_S_BASE + WDT_STAT); 
    RK3326_DUMP_REG(WDT_S_BASE + WDT_EOI); 

    RK3326_DUMP_REG(WDT_NS_BASE + WDT_CR);
    RK3326_DUMP_REG(WDT_NS_BASE + WDT_TORR);
    RK3326_DUMP_REG(WDT_NS_BASE + WDT_CCVR);
    RK3326_DUMP_REG(WDT_NS_BASE + WDT_CRR);
    RK3326_DUMP_REG(WDT_NS_BASE + WDT_STAT);
    RK3326_DUMP_REG(WDT_NS_BASE + WDT_EOI);

    return 0; 
}
MSH_CMD_EXPORT(wdt_test, wdt_test);