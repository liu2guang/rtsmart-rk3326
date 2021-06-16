/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <rtthread.h>

#define BSP_DEBUG_UART2 0

#define UART2_BASE 0xFF160000

#define UART2_THR  (*(volatile unsigned int *)(UART2_BASE + 0x0000))  // Transmit Holding Register
#define UART2_DLL  (*(volatile unsigned int *)(UART2_BASE + 0x0000))  // Divisor Latch (Low)
#define UART2_DLH  (*(volatile unsigned int *)(UART2_BASE + 0x0004))  // Divisor Latch (High)
#define UART2_IER  (*(volatile unsigned int *)(UART2_BASE + 0x0004))  // Interrupt Enable Register
#define UART2_LCR  (*(volatile unsigned int *)(UART2_BASE + 0x000C))  // Line Control Register
#define UART2_MCR  (*(volatile unsigned int *)(UART2_BASE + 0x0010))  // Modem Control Register
#define UART2_SRR  (*(volatile unsigned int *)(UART2_BASE + 0x0088))  // Software Reset Register
#define UART2_USR  (*(volatile unsigned int *)(UART2_BASE + 0x007C))  // UART Status Register
#define UART2_SFE  (*(volatile unsigned int *)(UART2_BASE + 0x0098))  // Shadow FIFO Enable
#define UART2_SRT  (*(volatile unsigned int *)(UART2_BASE + 0x009C))  // Shadow RCVR Trigger
#define UART2_STET (*(volatile unsigned int *)(UART2_BASE + 0x00A0))  // Shadow TX Empty Trigger

void rt_hw_debug_putc(char c)
{
    while((UART2_USR & (0x01 << 1)) == 0);
    UART2_THR = c;
}

#if (BSP_DEBUG_UART2 == 0)
void rt_hw_debug_puts(const char *str) {} 
#else
void rt_hw_debug_puts(const char *str)
{
    while(*str)
    {
        if(*str == '\n')
        {
            rt_hw_debug_putc('\r'); 
        }

        while((UART2_USR & (0x01 << 1)) == 0);
        UART2_THR = *str;
        str++;
    }
}
#endif /* BSP_DEBUG_UART2 */ 

void rt_hw_debug_puts_asm(void)
{
    for(int index = 0; index < 10; index++)
    {
        rt_hw_debug_putc('A'); 
    }
    rt_hw_debug_putc('\r'); 
    rt_hw_debug_putc('\n');
}

void rt_hw_console_output(const char *str)
{
    while(*str)
    {
        if(*str == '\n')
        {
            rt_hw_debug_putc('\r'); 
        }

        while((UART2_USR & (0x01 << 1)) == 0);
        UART2_THR = *str;
        str++;
    }
}
