/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "drv_clk.h" 
#include "rk3326.h"
#include <stdio.h>

#define DBG_TAG "drv.clk"
#define BSP_ENBALE_CLK_DEBUG 1

#define DBG_ENABLE
#if (BSP_ENBALE_CLK_DEBUG == 1)
#define DBG_LVL DBG_LOG
#else
#define DBG_LVL DBG_INFO
#endif 
#define DBG_COLOR
#include <rtdbg.h>

#define RK3326_XIN_OSC0 24000000 /* Hz */ 

/* PLL频率限制 */ 
#define RK3326_VCO_MAX_HZ 3200000000
#define RK3326_VCO_MIN_HZ 800000000
#define RK3326_PLL_MAX_HZ 3200000000
#define RK3326_PLL_MIN_HZ 24000000

static rt_uint32_t rk3326_pll_bases[RK3326_PLL_MAXNUM] = 
{
    CRU_APLL_BASE, CRU_DPLL_BASE, CRU_CPLL_BASE, 
    CRU_NPLL_BASE, CRU_GPLL_BASE
}; 

struct rk3326_pll_rate_table 
{
    rt_uint32_t rate;
    rt_uint32_t dsmpd;
    rt_uint32_t refdiv;
    rt_uint32_t fbdiv;
	rt_uint32_t postdiv1;
	rt_uint32_t postdiv2;
    rt_uint32_t frac;
};
typedef struct rk3326_pll_rate_table rk3326_pll_rate_table_t; 

static struct rk3326_pll_rate_table rk3326_pll_rates[] = {
	/*     rate, dsmpd, refdiv, fbdiv, postdiv1, postdiv2, frac */
    {1200000000,     1,      1,    50,        1,        1,   0}, /* 1200000000 */ 
    {1188000000,     1,      2,    99,        1,        1,   0}, /* 1188000000 */ 
    {1100000000,     1,     12,   550,        1,        1,   0}, /* 1100000000 */ 
    {1008000000,     1,      1,    84,        2,        1,   0}, /* 1008000000 */ 
    {1000000000,     1,      6,   500,        2,        1,   0}, /* 1000000000 */ 
    { 816000000,     1,      1,    68,        2,        1,   0}, /*  816000000 */ 
    { 600000000,     1,      1,    75,        3,        1,   0}, /*  600000000 */  
};

/* 显示频率 */ 
static void show_freq(const char *show, rt_uint32_t clk) 
{
#define FREQ_1KHZ 1000
#define FREQ_1MHZ 1000000
#define FREQ_1GHZ 1000000000

    float f; 

    if(clk >= FREQ_1GHZ)
    {
        f = (float)clk / FREQ_1GHZ; 
        printf("%12s: %fGHz\n", show, f); 
    }
    else if(clk >= FREQ_1MHZ)
    {
        f = (float)clk / FREQ_1MHZ; 
        printf("%12s: %fMHz\n", show, f); 
    }
    else if(clk >= FREQ_1KHZ)
    {
        f = (float)clk / FREQ_1KHZ; 
        printf("%12s: %fKHz\n", show, f); 
    }
    else
    {
        printf("%12s: %dHz\n", show, clk); 
    }
}

rt_uint32_t rk3326_xin_get_rate(void)
{
    return RK3326_XIN_OSC0; 
}

rt_uint32_t rk3326_pll_get_rate(rk3326_pll_type_t pll_type)
{
    rt_uint32_t output_hz = 0; 
    rt_uint32_t base = 0; 
    rt_uint32_t con[5] = {0}; 
    rt_uint8_t refdiv;
    rt_uint16_t fbdiv;
    rt_uint8_t postdiv1, postdiv2;
    rt_uint32_t frac;
    rt_uint8_t dsmpd; 

    RT_ASSERT(pll_type < RK3326_PLL_MAXNUM); 

    base = rk3326_pll_bases[pll_type]; 

    con[0] = readl(base + CRU_PLL_CON0); 
    con[1] = readl(base + CRU_PLL_CON1); 
    con[2] = readl(base + CRU_PLL_CON2); 
    con[3] = readl(base + CRU_PLL_CON3); 
    con[4] = readl(base + CRU_PLL_CON4); 

    dsmpd = (con[1] & CRU_PLL_DSMPD_MASK) >> CRU_PLL_DSMPD_SHIFT; 
    refdiv = (con[1] & CRU_PLL_REFDIV_MASK) >> CRU_PLL_REFDIV_SHIFT; 
    fbdiv = (con[0] & CRU_PLL_FBDIV_MASK) >> CRU_PLL_FBDIV_SHIFT; 
    postdiv1 = (con[0] & CRU_PLL_POSTDIV1_MASK) >> CRU_PLL_POSTDIV1_SHIFT; 
    postdiv2 = (con[1] & CRU_PLL_POSTDIV2_MASK) >> CRU_PLL_POSTDIV2_SHIFT; 
    frac = (con[2] & CRU_PLL_FRAC_MASK) >> CRU_PLL_FRAC_SHIFT;

    if(dsmpd)
    {
        output_hz = (RK3326_XIN_OSC0/refdiv*fbdiv)/(postdiv1*postdiv2); 
    }
    else
    {
        output_hz = (RK3326_XIN_OSC0/refdiv*(fbdiv+frac/0x1000000))/(postdiv1*postdiv2); 
    }

    return output_hz; 
}

static rt_uint32_t pll_set_rate(rk3326_pll_type_t pll_type, rk3326_pll_rate_table_t *table)
{
    rt_uint32_t foutvco, output_hz;
    rt_uint32_t base = 0; 

    RT_ASSERT(pll_type < RK3326_PLL_MAXNUM); 

    base = rk3326_pll_bases[pll_type]; 

    foutvco = RK3326_XIN_OSC0/table->refdiv*table->fbdiv; 
    output_hz = foutvco/(table->postdiv1*table->postdiv2); 

    RT_ASSERT(foutvco >= RK3326_VCO_MIN_HZ && foutvco <= RK3326_VCO_MAX_HZ); 
    RT_ASSERT(output_hz >= RK3326_PLL_MIN_HZ && foutvco <= RK3326_PLL_MAX_HZ); 

    /* 在改变锁相环设置时, 必须强制锁相环进入慢模式，以确保输出稳定时钟 */ 
    rk_clrsetreg(CRU_MODE_BASE, CRU_APLL_MODE_MASK, CRU_PLLMUX_FROM_XIN24M << CRU_APLL_MODE_SHIFT); 

    /* 使用整数模式 */
    rk_setreg(base + CRU_PLL_CON1, 1 << CRU_PLL_DSMPD_SHIFT);

    /* PLL下电 */
    rk_setreg(base + CRU_PLL_CON1, 1 << CRU_PLL_PD_SHIFT);

    /* 配置PLL参数 */ 
    rk_clrsetreg(base + CRU_PLL_CON0, CRU_PLL_POSTDIV1_MASK | CRU_PLL_FBDIV_MASK, 
        ((table->postdiv1 << CRU_PLL_POSTDIV1_SHIFT) | (table->fbdiv << CRU_PLL_FBDIV_SHIFT)));
    rk_clrsetreg(base + CRU_PLL_CON1, CRU_PLL_POSTDIV2_MASK | CRU_PLL_REFDIV_MASK, 
        ((table->postdiv2 << CRU_PLL_POSTDIV2_SHIFT) | (table->refdiv << CRU_PLL_REFDIV_SHIFT)));

    /* PLL上电 */
    rk_clrreg(base + CRU_PLL_CON1, 1 << CRU_PLL_PD_SHIFT);

    /* 等待PLL锁相环稳定锁定 */
	while (!(readl(base + CRU_PLL_CON1) & (1 << CRU_PLL_LOCK_STATUS_SHIFT))); 

    /* PLL进入快模式 */ 
    rk_clrsetreg(CRU_MODE_BASE, CRU_APLL_MODE_MASK, CRU_PLLMUX_FROM_PLL << CRU_APLL_MODE_SHIFT);

    /* 读取频率是否一致 */ 
    output_hz = rk3326_pll_get_rate(pll_type); 

    return output_hz; 
}

rt_uint32_t rk3326_pll_set_rate(rk3326_pll_type_t pll_type, rt_uint32_t rate)
{
    int index = 0; 
    rt_uint32_t real_rate = 0; 

    for(index = 0; index < sizeof(rk3326_pll_rates)/sizeof(rk3326_pll_rates[0]); index++)
    {
        if(rk3326_pll_rates[index].rate == rate)
        {
            real_rate = pll_set_rate(pll_type, &rk3326_pll_rates[index]); 
            break; 
        }
    }

    return real_rate; 
}

rt_uint32_t rk3326_clk_get_rate_aclk_core(void)
{
    rt_uint32_t clk_source = 0, clk_core = 0, aclk_core = 0; 
    rt_uint32_t clksel_con0 = 0; 
    rt_uint8_t clk_core_div_con = 0; 
    rt_uint8_t core_clk_pll_sel = 0; 
    rt_uint8_t aclk_core_div_con = 0; 

    clksel_con0 = readl(CRU_CLKSEL_CON0_BASE); 

    clk_core_div_con = (clksel_con0 & CRU_CORE_DIV_CON_MASK) >> CRU_CORE_DIV_CON_SHIFT; 
    core_clk_pll_sel = (clksel_con0 & CRU_CORE_CLK_PLL_SEL_MASK) >> CRU_CORE_CLK_PLL_SEL_SHIFT; 
    aclk_core_div_con = (clksel_con0 & CRU_CORE_ACLK_DIV_MASK) >> CRU_CORE_ACLK_DIV_SHIFT; 

    // LOG_D("clk_core_div_con = %d", clk_core_div_con); 
    // LOG_D("core_clk_pll_sel = %d", core_clk_pll_sel); 
    // LOG_D("aclk_core_div_con = %d", aclk_core_div_con); 

    switch(core_clk_pll_sel)
    {
    case CRU_CORE_CLK_PLL_SEL_APLL: 
        clk_source = rk3326_pll_get_rate(RK3326_PLL_APLL); 
        break; 
    case CRU_CORE_CLK_PLL_SEL_GPLL: 
        clk_source = rk3326_pll_get_rate(RK3326_PLL_GPLL); 
        break; 
    }

    clk_core = clk_source / (clk_core_div_con + 1); 
    aclk_core = clk_core / (aclk_core_div_con + 1); 

    // LOG_D("clk_source = %d", clk_source); 
    // LOG_D("clk_core = %d", clk_core); 
    // LOG_D("aclk_core = %d", aclk_core); 

    return aclk_core; 
}

rt_uint32_t rk3326_clk_get_rate_pclk_dbg(void)
{
    rt_uint32_t clk_source = 0, clk_core = 0, pclk_dbg = 0; 
    rt_uint32_t clksel_con0 = 0; 
    rt_uint8_t clk_core_div_con = 0; 
    rt_uint8_t core_clk_pll_sel = 0; 
    rt_uint8_t core_dbg_div_con = 0; 

    clksel_con0 = readl(CRU_CLKSEL_CON0_BASE); 

    clk_core_div_con = (clksel_con0 & CRU_CORE_DIV_CON_MASK) >> CRU_CORE_DIV_CON_SHIFT; 
    core_clk_pll_sel = (clksel_con0 & CRU_CORE_CLK_PLL_SEL_MASK) >> CRU_CORE_CLK_PLL_SEL_SHIFT; 
    core_dbg_div_con = (clksel_con0 & CRU_CORE_DBG_DIV_MASK) >> CRU_CORE_DBG_DIV_SHIFT; 

    // LOG_D("clk_core_div_con = %d", clk_core_div_con); 
    // LOG_D("core_clk_pll_sel = %d", core_clk_pll_sel); 
    // LOG_D("core_dbg_div_con = %d", core_dbg_div_con); 

    switch(core_clk_pll_sel)
    {
    case CRU_CORE_CLK_PLL_SEL_APLL: 
        clk_source = rk3326_pll_get_rate(RK3326_PLL_APLL); 
        break; 
    case CRU_CORE_CLK_PLL_SEL_GPLL: 
        clk_source = rk3326_pll_get_rate(RK3326_PLL_GPLL); 
        break; 
    }

    clk_core = clk_source / (clk_core_div_con + 1); 
    pclk_dbg = clk_core / (core_dbg_div_con + 1); 

    // LOG_D("clk_source = %d", clk_source); 
    // LOG_D("clk_core = %d", clk_core); 
    // LOG_D("pclk_dbg = %d", pclk_dbg); 

    return pclk_dbg; 
}

void rk3326_clk_set_rate_clk_core(rt_uint8_t clk_core_div_con, rt_uint8_t core_dbg_div_con, rt_uint8_t aclk_core_div_con)
{
    RT_ASSERT(clk_core_div_con <= 0x0F); 
    RT_ASSERT(core_dbg_div_con <= 0x0F); 
    RT_ASSERT(aclk_core_div_con <= 0x07); 

    rk_clrsetreg(CRU_CLKSEL_CON0_BASE, CRU_CORE_DIV_CON_MASK | CRU_CORE_DBG_DIV_MASK | CRU_CORE_ACLK_DIV_MASK, 
        clk_core_div_con << CRU_CORE_DIV_CON_SHIFT | core_dbg_div_con << CRU_CORE_DBG_DIV_SHIFT | 
        aclk_core_div_con << CRU_CORE_ACLK_DIV_SHIFT); 
}

rt_uint32_t rk3326_clk_get_rate_clk_uart(rt_uint8_t index)
{
    rt_uint32_t clk_source = 0, clk_uart2 = 0; 
    rt_uint32_t clksel_con37 = 0; 
    rt_uint32_t clksel_con38 = 0; 
    rt_uint32_t clksel_con39 = 0; 

    rt_uint8_t clk_uart2_pll_sel = 0; 
    rt_uint8_t clk_uart2_div_con = 0; 
    rt_uint8_t clk_uart2_sel = 0; 
    rt_uint8_t clk_uart2_divnp5_div_con = 0; 
    rt_uint16_t clk_uart2_frac_div_con_h16 = 0; 
    rt_uint16_t clk_uart2_frac_div_con_l16 = 0; 
    rt_uint32_t clk_uart2_np5 = 0; 
    rt_uint32_t clk_uart2_frac_out = 0; 

    clksel_con37 = readl(CRU_CLKSEL_CON37_BASE); 
    clksel_con38 = readl(CRU_CLKSEL_CON38_BASE); 
    clksel_con39 = readl(CRU_CLKSEL_CON39_BASE); 

    LOG_I("clksel_con37 = 0x%.8x", clksel_con37); 
    LOG_I("clksel_con38 = 0x%.8x", clksel_con38); 
    LOG_I("clksel_con39 = 0x%.8x", clksel_con39); 

    clk_uart2_frac_div_con_l16 = (clksel_con39 & CRU_UART2_FRAC_L_CON_MASK) >> CRU_UART2_FRAC_L_CON_SHIFT; 
    clk_uart2_frac_div_con_h16 = (clksel_con39 & CRU_UART2_FRAC_H_CON_MASK) >> CRU_UART2_FRAC_H_CON_SHIFT;
    clk_uart2_divnp5_div_con = (clksel_con38 & CRU_UART2_DIVNP5_MASK) >> CRU_UART2_DIVNP5_SHIFT; 
    clk_uart2_sel = (clksel_con38 & CRU_UART2_CLK_SEL_MASK) >> CRU_UART2_CLK_SEL_SHIFT; 
    clk_uart2_div_con = (clksel_con37 & CRU_UART2_DIV_CON_MASK) >> CRU_UART2_DIV_CON_SHIFT; 
    clk_uart2_pll_sel = (clksel_con37 & CRU_UART2_PLL_SEL_MASK) >> CRU_UART2_PLL_SEL_SHIFT; 

    LOG_I("clk_uart2_frac_div_con_l16 = %d", clk_uart2_frac_div_con_l16); 
    LOG_I("clk_uart2_frac_div_con_h16 = %d", clk_uart2_frac_div_con_h16); 
    LOG_I("clk_uart2_divnp5_div_con = %d", clk_uart2_divnp5_div_con); 
    LOG_I("clk_uart2_sel = %d", clk_uart2_sel); 
    LOG_I("clk_uart2_div_con = %d", clk_uart2_div_con); 
    LOG_I("clk_uart2_pll_sel = %d", clk_uart2_pll_sel); 

    switch (clk_uart2_pll_sel)
    {
    case CRU_UART2_PLL_SEL_GPLL:
        LOG_I("CRU_UART2_PLL_SEL_GPLL"); 
        clk_source = rk3326_pll_get_rate(RK3326_PLL_APLL); 
        break;
    case CRU_UART2_PLL_SEL_24M:
        LOG_I("CRU_UART2_PLL_SEL_24M"); 
        clk_source = RK3326_XIN_OSC0; 
        break;
    case CRU_UART2_PLL_SEL_480M: 
        LOG_I("CRU_UART2_PLL_SEL_480M"); 
        break;
    case CRU_UART2_PLL_SEL_NPLL:
        LOG_I("CRU_UART2_PLL_SEL_NPLL"); 
        clk_source = rk3326_pll_get_rate(RK3326_PLL_NPLL); 
        break;
    }

    clk_uart2 = clk_source / (clk_uart2_div_con + 1); 
    clk_uart2_np5 = 2 * clk_uart2 / (2 * clk_uart2_divnp5_div_con + 3); 
    clk_uart2_frac_out = clk_uart2 / (clk_uart2_frac_div_con_h16 / clk_uart2_frac_div_con_l16); 

    LOG_I("clk_uart2 = %d", clk_uart2); 
    LOG_I("clk_uart2_np5 = %d", clk_uart2_np5); 
    LOG_I("clk_uart2_frac_out = %d", clk_uart2_frac_out); 

    switch (clk_uart2_sel)
    {
    case CRU_UART2_CLK_SEL_UART2:
        LOG_I("CRU_UART2_CLK_SEL_UART2"); 
        clk_uart2 = clk_uart2; 
        break;
    case CRU_UART2_CLK_SEL_UART2_NP5:
        LOG_I("CRU_UART2_CLK_SEL_UART2_NP5"); 
        clk_uart2 = clk_uart2_np5; 
        break;
    case CRU_UART2_CLK_SEL_UART2_FRAC: 
        LOG_I("CRU_UART2_CLK_SEL_UART2_FRAC"); 
        clk_uart2 = clk_uart2_frac_out; 
        break;
    }

    return clk_uart2; 
}

int rk3326_clk_test(void)
{
    rt_uint32_t apll = rk3326_pll_get_rate(RK3326_PLL_APLL); 
    rt_uint32_t dpll = rk3326_pll_get_rate(RK3326_PLL_DPLL); 
    rt_uint32_t cpll = rk3326_pll_get_rate(RK3326_PLL_CPLL); 
    rt_uint32_t npll = rk3326_pll_get_rate(RK3326_PLL_NPLL); 
    rt_uint32_t gpll = rk3326_pll_get_rate(RK3326_PLL_GPLL); 
    show_freq("apll", apll); 
    show_freq("dpll", dpll); 
    show_freq("cpll", cpll); 
    show_freq("npll", npll); 
    show_freq("gpll", gpll); 

    apll = rk3326_pll_set_rate(RK3326_PLL_APLL, 600000000); 
    show_freq("apll", apll); 

    rt_uint32_t pclk_dbg  = rk3326_clk_get_rate_pclk_dbg(); 
    rt_uint32_t aclk_core = rk3326_clk_get_rate_aclk_core(); 
    show_freq("pclk_dbg", pclk_dbg);
    show_freq("aclk_core", aclk_core);

    // rk3326_clk_set_rate_clk_core(0, 5, 1); 
    // pclk_dbg  = rk3326_clk_get_rate_pclk_dbg(); 
    // aclk_core = rk3326_clk_get_rate_aclk_core(); 
    // show_freq("pclk_dbg", pclk_dbg); 
    // show_freq("aclk_core", aclk_core); 

    rt_uint32_t clk_uart2 = rk3326_clk_get_rate_clk_uart(2); 
    show_freq("clk_uart2", clk_uart2); 

    // arch\arm\mach-rockchip\px30\px30.c
    // drivers\serial\ns16550.c
    // baud_rate = (serial clock freq) / (16 * divisor)
    // baud_rate = 24000000 / (16 * divisor) = 115200; 
    // divisor = 13
    // baud_rate = 24000000 / (16 * 13) = 115384; 
    // divisor = 0x0D
    // ######### _debug_uart_init clock = 24000000, baudrate = 115200, divisor = 13

    return RT_EOK; 
}
MSH_CMD_EXPORT_ALIAS(rk3326_clk_test, rk3326_clk_test, rk3326_clk_test); 
