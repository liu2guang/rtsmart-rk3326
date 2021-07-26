/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __DRV_CLK_H__
#define __DRV_CLK_H__

#include "rtthread.h"
#include "rtdevice.h"

enum rk3326_pll_type 
{
    RK3326_PLL_APLL = 0, 
    RK3326_PLL_DPLL,
    RK3326_PLL_CPLL,
    RK3326_PLL_NPLL,
    RK3326_PLL_GPLL,
    RK3326_PLL_MAXNUM,
}; 
typedef int rk3326_pll_type_t; 

rt_uint32_t rk3326_xin_get_rate(void); 
rt_uint32_t rk3326_pll_get_rate(rk3326_pll_type_t pll_type); 
rt_uint32_t rk3326_pll_set_rate(rk3326_pll_type_t pll_type, rt_uint32_t rate); 

// clk_core = APLL / (clk_core_div_con + 1)
// pclk_dbg = clk_core / (core_dbg_div_con + 1)
// aclk_core = clk_core / (aclk_core_div_con + 1)
rt_uint32_t rk3326_clk_get_rate_aclk_core(void); 
rt_uint32_t rk3326_clk_get_rate_pclk_dbg(void); 
void rk3326_clk_set_rate_clk_core(rt_uint8_t clk_core_div_con, rt_uint8_t core_dbg_div_con, rt_uint8_t aclk_core_div_con); 

rt_uint32_t rk3326_clk_get_rate_clk_uart(rt_uint8_t index); 

#endif /* __DRV_CLK_H__ */ 
