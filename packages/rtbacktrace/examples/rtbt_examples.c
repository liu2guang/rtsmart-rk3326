/*
 * RT-Thread Secure
 * 
 * Copyright (c) 2021, Shanghai Real-Thread Electronic Technology Co., Ltd.
 *
 * All rights reserved.
 */
#include <rt_backtrace.h> 

extern rt_base_t __text_start; 
extern rt_base_t __text_end; 

static struct rt_backtrace_text texts = {
    (rt_base_t)&__text_start, 
    (rt_base_t)&__text_end
}; 

void __rtbt_c(void)
{
	RT_ASSERT(0);
}

void __rtbt_b(void)
{
	__rtbt_c();
}

void __rtbt_a(void)
{
	__rtbt_b();
}

void rt_backtrace_test(void)
{
    rt_backtrace_init(&texts, 1); 
	__rtbt_a(); 
}
MSH_CMD_EXPORT(rt_backtrace_test, rtbacktrace test cmd.); 
