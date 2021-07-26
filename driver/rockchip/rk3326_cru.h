/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __RK3326_CLK_H__
#define __RK3326_CLK_H__

/* CRU寄存器物理地址 */ 
#define CRU_BASE                    0xFF2B0000
#define CRU_APLL_BASE               CRU_BASE + 0x0000
#define CRU_DPLL_BASE               CRU_BASE + 0x0020
#define CRU_CPLL_BASE               CRU_BASE + 0x0040
#define CRU_NPLL_BASE               CRU_BASE + 0x0060
#define CRU_GPLL_BASE               CRU_BASE + 0xC000
#define CRU_MODE_BASE               CRU_BASE + 0x00a0
#define CRU_CLKSEL_CON0_BASE        CRU_BASE + 0x0100
#define CRU_CLKSEL_CON37_BASE       CRU_BASE + 0x0194 /* uart2 */ 
#define CRU_CLKSEL_CON38_BASE       CRU_BASE + 0x0198 /* uart2 */ 
#define CRU_CLKSEL_CON39_BASE       CRU_BASE + 0x019c /* uart2 */ 
#define CRU_CLKGATE_CON11_BASE      CRU_BASE + 0x022c
#define CRU_CLKGATE_CON14_BASE      CRU_BASE + 0x0238
#define CRU_SOFTRST_CON8_BASE       CRU_BASE + 0x0320

/* CON寄存器偏移地址 */ 
#define CRU_PLL_CON0                0x0000 // W 0x00003064 APLL configuration register0
#define CRU_PLL_CON1                0x0004 // W 0x00001041 APLL configuration register1
#define CRU_PLL_CON2                0x0008 // W 0x00000001 APLL configuration register2
#define CRU_PLL_CON3                0x000c // W 0x00000007 APLL configuration register3
#define CRU_PLL_CON4                0x0010 // W 0x00007f00 APLL configuration register4

/* PLLCON0 */
#define CRU_PLL_BP_SHIFT		    15
#define CRU_PLL_POSTDIV1_SHIFT	    12
#define CRU_PLL_POSTDIV1_MASK	    7 << CRU_PLL_POSTDIV1_SHIFT
#define CRU_PLL_FBDIV_SHIFT		    0
#define CRU_PLL_FBDIV_MASK		    0xfff
  
/* PLLCON1 */  
#define CRU_PLL_PDSEL_SHIFT		    15
#define CRU_PLL_PD1_SHIFT		    14
#define CRU_PLL_PD_SHIFT		    13
#define CRU_PLL_PD_MASK		        1 << CRU_PLL_PD_SHIFT
#define CRU_PLL_DSMPD_SHIFT		    12
#define CRU_PLL_DSMPD_MASK		    1 << CRU_PLL_DSMPD_SHIFT
#define CRU_PLL_LOCK_STATUS_SHIFT   10
#define CRU_PLL_LOCK_STATUS_MASK    1 << CRU_PLL_LOCK_STATUS_SHIFT
#define CRU_PLL_POSTDIV2_SHIFT	    6
#define CRU_PLL_POSTDIV2_MASK	    7 << CRU_PLL_POSTDIV2_SHIFT
#define CRU_PLL_REFDIV_SHIFT	    0
#define CRU_PLL_REFDIV_MASK		    0x3f

/* PLLCON2 */
#define CRU_PLL_FOUT4PHASEPD_SHIFT  27
#define CRU_PLL_FOUTVCOPD_SHIFT	    26
#define CRU_PLL_FOUTPOSTDIVPD_SHIFT	25
#define CRU_PLL_DACPD_SHIFT		    24
#define CRU_PLL_FRAC_SHIFT	        0
#define CRU_PLL_FRAC_MASK           0xffffff

/* CRU_MODE */
#define CRU_PLLMUX_FROM_XIN24M      0 
#define CRU_PLLMUX_FROM_PLL         1
#define CRU_PLLMUX_FROM_RTC32K      2
          
#define CRU_USBPHY480M_MODE_SHIFT   8
#define CRU_USBPHY480M_MODE_MASK    3 << CRU_USBPHY480M_MODE_SHIFT
#define CRU_NPLL_MODE_SHIFT		    6
#define CRU_NPLL_MODE_MASK		    3 << CRU_NPLL_MODE_SHIFT
#define CRU_DPLL_MODE_SHIFT		    4
#define CRU_DPLL_MODE_MASK		    3 << CRU_DPLL_MODE_SHIFT
#define CRU_CPLL_MODE_SHIFT		    2
#define CRU_CPLL_MODE_MASK		    3 << CRU_CPLL_MODE_SHIFT
#define CRU_APLL_MODE_SHIFT		    0
#define CRU_APLL_MODE_MASK		    3 << CRU_APLL_MODE_SHIFT

/* CRU_CLK_SEL0_CON */
#define CRU_CORE_CLK_PLL_SEL_APLL	0
#define CRU_CORE_CLK_PLL_SEL_GPLL   1

#define CRU_CORE_ACLK_DIV_SHIFT	    12
#define CRU_CORE_ACLK_DIV_MASK	    0x07 << CRU_CORE_ACLK_DIV_SHIFT
#define CRU_CORE_DBG_DIV_SHIFT	    8
#define CRU_CORE_DBG_DIV_MASK	    0x07 << CRU_CORE_DBG_DIV_SHIFT
#define CRU_CORE_CLK_PLL_SEL_SHIFT	7
#define CRU_CORE_CLK_PLL_SEL_MASK	1 << CRU_CORE_CLK_PLL_SEL_SHIFT
#define CRU_CORE_DIV_CON_SHIFT	    0
#define CRU_CORE_DIV_CON_MASK	    0x0f << CRU_CORE_DIV_CON_SHIFT

/* CRU_CLK_SEL37_CON */
#define CRU_UART2_PLL_SEL_GPLL	    0
#define CRU_UART2_PLL_SEL_24M       1
#define CRU_UART2_PLL_SEL_480M      2 
#define CRU_UART2_PLL_SEL_NPLL      3

#define CRU_UART2_PLL_SEL_SHIFT	    14
#define CRU_UART2_PLL_SEL_MASK	    3 << CRU_UART2_PLL_SEL_SHIFT
#define CRU_UART2_DIV_CON_SHIFT	    0
#define CRU_UART2_DIV_CON_MASK	    0x1f << CRU_UART2_DIV_CON_SHIFT

/* CRU_CLK_SEL38_CON */
#define CRU_UART2_CLK_SEL_UART2	     0
#define CRU_UART2_CLK_SEL_UART2_NP5  1
#define CRU_UART2_CLK_SEL_UART2_FRAC 2

#define CRU_UART2_CLK_SEL_SHIFT	    14
#define CRU_UART2_CLK_SEL_MASK	    3 << CRU_UART2_PLL_SEL_SHIFT
#define CRU_UART2_DIVNP5_SHIFT	    0
#define CRU_UART2_DIVNP5_MASK	    0x1f << CRU_UART2_DIVNP5_SHIFT

/* CRU_CLK_SEL39_CON */
#define CRU_UART2_FRAC_L_CON_SHIFT  16
#define CRU_UART2_FRAC_L_CON_MASK   0xffff << CRU_UART2_FRAC_L_CON_SHIFT
#define CRU_UART2_FRAC_H_CON_SHIFT  0
#define CRU_UART2_FRAC_H_CON_MASK   0xffff << CRU_UART2_FRAC_H_CON_SHIFT

#endif /* __RK3326_CLK_H__ */ 