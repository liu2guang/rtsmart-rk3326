/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author         Notes
 * 2020-04-16     bigmagic       first version
 */

#include <rthw.h>
#include <rtthread.h>
#include "board.h"
#include "drv_debug.h"
#include "drv_uart.h"

#include "cp15.h"
#include "mmu.h"

static struct rt_memheap memheap; 

static rt_uint64_t timerStep;

int rt_hw_get_gtimer_frq(void);
void rt_hw_set_gtimer_val(rt_uint64_t value);
int rt_hw_get_gtimer_val(void);
int rt_hw_get_cntpct_val(void);
void rt_hw_gtimer_enable(void);

void core0_timer_enable_interrupt_controller(void)
{
    CORE0_TIMER_IRQ_CTRL |= NON_SECURE_TIMER_IRQ;
}

void rt_hw_timer_isr(int vector, void *parameter)
{
    rt_hw_set_gtimer_val(timerStep);
    rt_tick_increase();
}

void rt_hw_timer_init(void)
{
    rt_hw_interrupt_install(TIMER_IRQ, rt_hw_timer_isr, RT_NULL, "tick");
    rt_hw_interrupt_umask(TIMER_IRQ);
    __ISB();
    timerStep = rt_hw_get_gtimer_frq();
    __DSB();
    timerStep /= RT_TICK_PER_SECOND;

    rt_hw_gtimer_enable();
    rt_hw_set_gtimer_val(timerStep);
    core0_timer_enable_interrupt_controller();
}

void idle_wfi(void)
{
    asm volatile ("wfi");
}

#define PMU_CRU_BASE 0xFF2BC000
#define CRU_GLB_SRST_FST_BASE PMU_CRU_BASE + 0x00b8
#define CRU_GLB_SRST_SND_BASE PMU_CRU_BASE + 0x00bc

void rk3326_reboot(void)
{
    WRITE_UINT32(0xfdb9, CRU_GLB_SRST_FST_BASE); 
    WRITE_UINT32(0xeca8, CRU_GLB_SRST_SND_BASE); 
}
MSH_CMD_EXPORT_ALIAS(rk3326_reboot, reboot, reboot RT-Thread.);  

/**
 *  Initialize the Hardware related stuffs. Called from rtthread_startup()
 *  after interrupt disabled.
 */
void rt_hw_board_init(void)
{
    rt_hw_debug_puts("rt_hw_board_init 1\r\n"); 
    rt_hw_debug_puts("rt_hw_board_init 2\r\n"); 
    rt_hw_debug_puts("rt_hw_board_init 3\r\n"); 
 
    mmu_init();
    rt_hw_debug_puts("rt_hw_board_init 4\r\n"); 

    armv8_map(0x00000000, 0x00000000, 0xFF000000, MEM_ATTR_MEMORY);
    armv8_map(0xFF000000, 0xFF000000, 0x00FF0000, MEM_ATTR_IO);  

    rt_hw_debug_puts("rt_hw_board_init 7\r\n"); 
    mmu_enable();
    rt_hw_debug_puts("rt_hw_board_init 8\r\n"); 

    /* initialize hardware interrupt */
    rt_hw_debug_puts("rt_hw_board_init 9\r\n"); 
    rt_hw_interrupt_init(); // in libcpu/interrupt.c. Set some data structures, no operation on device

    rt_hw_debug_puts("rt_hw_board_init 10\r\n"); 
    rt_hw_vector_init();    // in libcpu/interrupt.c. == rt_cpu_vector_set_base((rt_ubase_t)&system_vectors);

    rt_hw_debug_puts("rt_hw_board_init 11\r\n"); 
    rt_hw_uart_init(); 
    rt_hw_debug_puts("rt_hw_board_init 11.1\r\n"); 

#ifdef RT_USING_CONSOLE
    /* set console device */
    rt_hw_debug_puts("rt_hw_board_init 12\r\n"); 
    rt_console_set_device(RT_CONSOLE_DEVICE_NAME);
    rt_hw_debug_puts("rt_hw_board_init 13\r\n"); 
#endif /* RT_USING_CONSOLE */

#ifdef RT_USING_HEAP
    /* initialize memory system */
    rt_kprintf("DDR31 P1: 0x%08x - 0x%08x\n", RT_HW_DDR_P1_BEGIN, RT_HW_DDR_P1_END);
    rt_kprintf("DDR31 P2: 0x%08x - 0x%08x\n", RT_HW_DDR_P2_BEGIN, RT_HW_DDR_P2_END);
    rt_hw_debug_puts("rt_hw_board_init 14\r\n"); 

    rt_system_heap_init(RT_HW_DDR_P1_BEGIN, RT_HW_DDR_P1_END); 
    rt_memheap_init(&memheap, "heap2", (void *)RT_HW_DDR_P2_BEGIN, RT_HW_DDR_P2_END-RT_HW_DDR_P2_BEGIN+1); 

    rt_hw_debug_puts("rt_hw_board_init 15\r\n"); 
#endif
    
    /* initialize timer for os tick */
    rt_hw_debug_puts("rt_hw_board_init 16\r\n"); 
    rt_hw_timer_init();
    rt_hw_debug_puts("rt_hw_board_init 17\r\n"); 
    rt_thread_idle_sethook(idle_wfi);
    rt_hw_debug_puts("rt_hw_board_init 18\r\n"); 

#ifdef RT_USING_COMPONENTS_INIT
    rt_hw_debug_puts("rt_hw_board_init 19\r\n"); 
    rt_components_board_init();
    rt_hw_debug_puts("rt_hw_board_init 20\r\n"); 
#endif
}
