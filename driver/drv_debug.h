/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __DRV_DEBUG_H__
#define __DRV_DEBUG_H__

#include <rtthread.h>

void rt_hw_debug_putc(char c); 
void rt_hw_debug_puts(const char *str); 
void rt_hw_debug_puts_asm(void); 

#endif /* __DRV_DEBUG_H__ */ 
