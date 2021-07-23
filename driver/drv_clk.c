/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "drv_clk.h" 
#include "rthw.h"
#include "mmu.h"
#include "board.h"

#define DBG_TAG "drv.clk"
#define BSP_ENBALE_CLK_DEBUG 0

#define DBG_ENABLE
#if (BSP_ENBALE_CLK_DEBUG == 1)
#define DBG_LVL DBG_LOG
#else
#define DBG_LVL DBG_INFO
#endif 
#define DBG_COLOR
#include <rtdbg.h>

/* bit field helpers. */
#define __M(n)                    (~(~0<<(n)))
#define __RBF(number, n)          ((number)&__M(n))
#define __BF(number, n, m)        __RBF((number>>m), (n-m+1))
#define RK3326_REG_BF(number, n, m) (m<n ? __BF(number, n, m) : __BF(number, m, n)) 

#define RK3326_DUMP_REG(addr)      \
do {                               \
    rt_uint32_t val = 0x00000000;  \
    val = readl(addr);             \
    rt_kprintf(#addr ":\n");       \
    rt_kprintf("\t0x%.8x\n", val); \
} while(0); 

struct rk_pll
{
    /* 寄存器 */ 
    rt_uint64_t base; 
    rt_uint32_t regs[5]; 

    /* 模式 */ 
    rt_uint8_t dsmpd; 

    /* 分频倍频参数 */ 
    rt_uint32_t refdiv; 
    rt_uint32_t fbdiv; 
    rt_uint32_t postdiv1; 
    rt_uint32_t postdiv2; 
    rt_uint32_t frac; 

    /* 输出时钟 */ 
    rt_uint32_t foutvco; 
    rt_uint32_t foutpostdiv; 
};
typedef struct rk_pll rk_pll_t; 

static rt_uint32_t fref = 24000000; /* 24MHz */ 
static struct rk_pll apll, dpll, cpll, npll, gpll; 

/* 显示频率 */ 
static void show_frequency(const char *show, rt_uint32_t clk) 
{
#define FREQ_1KHZ 1000
#define FREQ_1MHZ 1000000
#define FREQ_1GHZ 1000000000

    rt_uint32_t integer; 
    rt_uint32_t decimals;

    if(clk >= FREQ_1GHZ)
    {
        integer = clk / FREQ_1GHZ; 
        decimals = clk % FREQ_1GHZ; 
        // LOG_I("%s:\n\t%dHz\n\t%d.%dGHz", show, clk, integer, decimals);
        LOG_I("%s:\n\t%d.%dGHz", show, integer, decimals); 
    }
    else if(clk >= FREQ_1MHZ)
    {
        integer = clk / FREQ_1MHZ; 
        decimals = clk % FREQ_1MHZ; 
        // LOG_I("%s:\n\t%dHz\n\t%d.%dMHz", show, clk, integer, decimals); 
        LOG_I("%s:\n\t%d.%dMHz", show, integer, decimals); 
    }
    else if(clk >= FREQ_1KHZ)
    {
        integer = clk / FREQ_1KHZ; 
        decimals = clk % FREQ_1KHZ; 
        // LOG_I("%s:\n\t%dHz\n\t%d.%dKHz", show, clk, integer, decimals); 
        LOG_I("%s:\n\t%d.%dKHz", show, integer, decimals); 
    }
    else
    {
        LOG_I("%s:\n\t%dHz", show, clk); 
    }
}

static int rk3326_pll_read(struct rk_pll *pll, rt_uint64_t base)
{
    int index = 0; 

    RT_ASSERT(pll != RT_NULL); 
    RT_ASSERT(base != RT_NULL); 

    pll->base = base; 

    for(index = 0; index < 5; index++)
    {
        pll->regs[index] = readl(pll->base + index*4); 
        LOG_D("cru_pll_con%d_reg = 0x%.8x", index, pll->regs[index]); 
    }

    pll->dsmpd = RK3326_REG_BF(pll->regs[1], CRU_PLL_CON1_DSMPD_L, CRU_PLL_CON1_DSMPD_H); 
    pll->refdiv = RK3326_REG_BF(pll->regs[1], CRU_PLL_CON1_REFDIV_L, CRU_PLL_CON1_REFDIV_H); 
    pll->fbdiv = RK3326_REG_BF(pll->regs[0], CRU_PLL_CON0_FBDIV_L, CRU_PLL_CON0_FBDIV_H); 
    pll->postdiv1 = RK3326_REG_BF(pll->regs[0], CRU_PLL_CON0_POSTDIV1_L, CRU_PLL_CON0_POSTDIV1_H); 
    pll->postdiv2 = RK3326_REG_BF(pll->regs[1], CRU_PLL_CON1_POSTDIV2_L, CRU_PLL_CON1_POSTDIV2_H); 
    pll->frac = RK3326_REG_BF(pll->regs[1], CRU_PLL_CON2_FRACDIV_L, CRU_PLL_CON2_FRACDIV_H); 

    LOG_D("dsmpd    = 0x%.8x", pll->dsmpd);
    LOG_D("refdiv   = 0x%.8x", pll->refdiv);
    LOG_D("fbdiv    = 0x%.8x", pll->fbdiv);
    LOG_D("postdiv1 = 0x%.8x", pll->postdiv1);
    LOG_D("postdiv2 = 0x%.8x", pll->postdiv2);
    LOG_D("frac     = 0x%.8x", pll->frac);

    if(pll->dsmpd)
    {
        LOG_D("DSM is disabled,integer mode"); 
        pll->foutvco = (fref/pll->refdiv)*(pll->fbdiv); 
        pll->foutpostdiv = pll->foutvco/(pll->postdiv1*pll->postdiv2); 
    }
    else
    {
        LOG_D("DSM is enabled, fractional mode"); 
        pll->foutvco = (fref/pll->refdiv)*(pll->fbdiv+pll->frac/0x1000000);
        pll->foutpostdiv = pll->foutvco/(pll->postdiv1*pll->postdiv2);  
    }

    LOG_D("foutvco     = %d", pll->foutvco);
    LOG_D("foutpostdiv = %d", pll->foutpostdiv);

    show_frequency("foutvco", pll->foutvco); 
    show_frequency("foutpostdiv", pll->foutpostdiv); 

    return RT_EOK; 
}

#define rk_clrsetreg(addr, clr, set) writel(((clr) | (set)) << 16 | (set), addr)
#define rk_clrreg(addr, clr)         writel((clr) << 16, addr)
#define rk_setreg(addr, set)         writel((set) << 16 | (set), addr)

static int rk3326_pll_write(struct rk_pll *pll, rt_uint64_t base)
{
#define RK3326_VCO_MAX_HZ 3200000000
#define RK3326_VCO_MIN_HZ 800000000
#define RK3326_PLL_MAX_HZ 3200000000
#define RK3326_PLL_MIN_HZ 24000000

    int index = 0; 
    rt_uint32_t vco_hz, output_hz;

    RT_ASSERT(pll != RT_NULL); 
    RT_ASSERT(base != RT_NULL); 

    pll->base = base; 

    for(index = 0; index < 5; index++)
    {
        pll->regs[index] = readl(pll->base + index*4); 
        LOG_D("cru_pll_con%d_reg = 0x%.8x", index, pll->regs[index]); 
    }

    pll->foutvco = (fref/pll->refdiv)*(pll->fbdiv); 
    pll->foutpostdiv = pll->foutvco/(pll->postdiv1*pll->postdiv2); 

	/*
	 * When power on or changing PLL setting,
	 * we must force PLL into slow mode to ensure output stable clock.
	 */
	rk_clrsetreg(CRU_MODE, APLL_MODE_MASK, PLLMUX_FROM_XIN24M << APLL_MODE_SHIFT); 

    /* use integer mode */
    rk_setreg(pll->base + CRU_PLL_CON1, 1 << PLL_DSMPD_SHIFT);

    /* Power down */
    rk_setreg(pll->base + CRU_PLL_CON1, 1 << PLL_PD_SHIFT);

    rk_clrsetreg(pll->base + CRU_PLL_CON0, PLL_POSTDIV1_MASK | PLL_FBDIV_MASK, (pll->postdiv1 << PLL_POSTDIV1_SHIFT) | pll->fbdiv);
    rk_clrsetreg(pll->base + CRU_PLL_CON1, PLL_POSTDIV2_MASK | PLL_REFDIV_MASK, (pll->postdiv2 << PLL_POSTDIV2_SHIFT | pll->refdiv << PLL_REFDIV_SHIFT));

    /* Power Up */
    rk_clrreg(pll->base + CRU_PLL_CON1, 1 << PLL_PD_SHIFT);

    /* waiting for pll lock */
	while (!(readl(pll->base + CRU_PLL_CON1) & (1 << PLL_LOCK_STATUS_SHIFT)))
    {
        // rt_thread_mdelay(1); 
    }

	rk_clrsetreg(CRU_MODE, APLL_MODE_MASK, PLLMUX_FROM_PLL << APLL_MODE_SHIFT);

    return 0; 
}

rt_uint32_t rk3326_clk_get(rk3326_cru_pll_type_t type)
{
    struct rk_pll *pll; 
    rt_uint32_t clk = 0; 

    switch (type)
    {
    case RK3326_CLK_PLL_APLL:
        rk3326_pll_read(&apll, CRU_APLL_BASE); 
        pll = &apll; 
        clk = pll->foutpostdiv; 
        break;
    case RK3326_CLK_PLL_DPLL:
        rk3326_pll_read(&dpll, CRU_DPLL_BASE); 
        pll = &dpll; 
        clk = pll->foutpostdiv; 
        break;
    case RK3326_CLK_PLL_CPLL:
        rk3326_pll_read(&cpll, CRU_CPLL_BASE); 
        pll = &cpll; 
        clk = pll->foutpostdiv; 
        break;
    case RK3326_CLK_PLL_NPLL:
        rk3326_pll_read(&npll, CRU_NPLL_BASE); 
        pll = &npll; 
        clk = pll->foutpostdiv; 
        break;
    case RK3326_CLK_PLL_GPLL:
        rk3326_pll_read(&gpll, CRU_GPLL_BASE); 
        pll = &gpll; 
        clk = pll->foutpostdiv; 
        break;
    default:
        LOG_E("rk3326_clk_get error type"); 
        break;
    }

    return clk; 
}

int rk3326_clk_init(void)
{
    rk3326_pll_read(&apll, CRU_APLL_BASE); 
    rk3326_pll_read(&dpll, CRU_DPLL_BASE);
    rk3326_pll_read(&cpll, CRU_CPLL_BASE); 
    rk3326_pll_read(&npll, CRU_NPLL_BASE); 
    rk3326_pll_read(&gpll, CRU_GPLL_BASE); 
    return RT_EOK; 
}
INIT_BOARD_EXPORT(rk3326_clk_init); 

int clk_test(void)
{
    LOG_I("apll:");
    rk3326_pll_read(&apll, CRU_APLL_BASE); 

    LOG_I("dpll:");
    rk3326_pll_read(&dpll, CRU_DPLL_BASE);
    
    LOG_I("cpll:");
    rk3326_pll_read(&cpll, CRU_CPLL_BASE); 

    LOG_I("npll:");
    rk3326_pll_read(&npll, CRU_NPLL_BASE); 

    LOG_I("gpll:");
    rk3326_pll_read(&gpll, CRU_GPLL_BASE); 

    return RT_EOK; 
}
MSH_CMD_EXPORT_ALIAS(clk_test, clk_test, clk_test); 

int clk_change(void)
{
    /* 600MHz */ 
    apll.refdiv = 1; 
    apll.fbdiv = 75; 
    apll.postdiv1 = 3; 
    apll.postdiv2 = 1; 
    apll.dsmpd = 1; 
    apll.frac = 0; 

    /*  */ 

    rk3326_pll_write(&apll, CRU_APLL_BASE); 
}
MSH_CMD_EXPORT_ALIAS(clk_change, clk_change, clk_change); 

// apll 600000 KHz
// dpll 664000 KHz
// cpll 24000 KHz
// npll 1188000 KHz
// gpll 1200000 KHz
// aclk_bus 200000 KHz
// hclk_bus 150000 KHz
// pclk_bus 100000 KHz
// aclk_peri 200000 KHz
// hclk_peri 150000 KHz
// pclk_pmu 100000 KHz
