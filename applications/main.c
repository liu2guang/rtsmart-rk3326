/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <rtthread.h>
#include <drv_clk.h>

int main(int argc, char** argv)
{
    rt_kprintf("Hi, This is RT-Thread !!\n"); 
    rt_kprintf("Hi, This is Rk3326/PX30!!\n"); 

    rt_uint32_t clk = rk3326_clk_get(RK3326_CLK_PLL_APLL); 
    rt_kprintf("APLL = %d\n", clk); 

    return 0;
}
