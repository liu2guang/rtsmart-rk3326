/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __DRV_CLK_H__
#define __DRV_CLK_H__

#include "rtthread.h"
#include "rtdevice.h"

/* 寄存器地址 */ 
#define CRU_BASE             0xFF2B0000
#define CRU_APLL_BASE        CRU_BASE + 0x0000
#define CRU_DPLL_BASE        CRU_BASE + 0x0020
#define CRU_CPLL_BASE        CRU_BASE + 0x0040
#define CRU_NPLL_BASE        CRU_BASE + 0x0060
#define CRU_GPLL_BASE        CRU_BASE + 0xC000
#define CRU_PLL_CON0         0x0000 // W 0x00003064 APLL configuration register0
#define CRU_PLL_CON1         0x0004 // W 0x00001041 APLL configuration register1
#define CRU_PLL_CON2         0x0008 // W 0x00000001 APLL configuration register2
#define CRU_PLL_CON3         0x000c // W 0x00000007 APLL configuration register3
#define CRU_PLL_CON4         0x0010 // W 0x00007f00 APLL configuration register4

/* 位域 */
#define CRU_PLL_CON0_FBDIV_L               0
#define CRU_PLL_CON0_FBDIV_H              11
#define CRU_PLL_CON0_POSTDIV1_L           12
#define CRU_PLL_CON0_POSTDIV1_H           14
#define CRU_PLL_CON0_BYPASS_L             15
#define CRU_PLL_CON0_BYPASS_H             15
#define CRU_PLL_CON0_WRITE_MASK_L         16
#define CRU_PLL_CON0_WRITE_MASK_H         31
       
#define CRU_PLL_CON1_REFDIV_L              0
#define CRU_PLL_CON1_REFDIV_H              5
#define CRU_PLL_CON1_POSTDIV2_L            6
#define CRU_PLL_CON1_POSTDIV2_H            8
#define CRU_PLL_CON1_PLL_LOCK_L           10
#define CRU_PLL_CON1_PLL_LOCK_H           10
#define CRU_PLL_CON1_DSMPD_L              12
#define CRU_PLL_CON1_DSMPD_H              12
#define CRU_PLL_CON1_PLLPD0_L             13
#define CRU_PLL_CON1_PLLPD0_H             13
#define CRU_PLL_CON1_PLLPD1_L             14
#define CRU_PLL_CON1_PLLPD1_H             14
#define CRU_PLL_CON1_PLLPDSEL_L           15
#define CRU_PLL_CON1_PLLPDSEL_H           15
#define CRU_PLL_CON1_WRITE_MASK_L         16
#define CRU_PLL_CON1_WRITE_MASK_H         31
     
#define CRU_PLL_CON2_FRACDIV_L             0
#define CRU_PLL_CON2_FRACDIV_H            23
#define CRU_PLL_CON2_DACPD_L              24
#define CRU_PLL_CON2_DACPD_H              24
#define CRU_PLL_CON2_FOUTPOSTDIVPD_L      25
#define CRU_PLL_CON2_FOUTPOSTDIVPD_H      25
#define CRU_PLL_CON2_FOUTVCOPD_L          26
#define CRU_PLL_CON2_FOUTVCOPD_H          26
#define CRU_PLL_CON2_FOUT4PHASEPD_L       27
#define CRU_PLL_CON2_FOUT4PHASEPD_H       27

#define CRU_PLL_CON3_SSMOD_BP_L            0
#define CRU_PLL_CON3_SSMOD_BP_H            0
#define CRU_PLL_CON3_SSMOD_DISABLE_SSCG_L  1
#define CRU_PLL_CON3_SSMOD_DISABLE_SSCG_H  1
#define CRU_PLL_CON3_SSMOD_RESET_L         2
#define CRU_PLL_CON3_SSMOD_RESET_H         2
#define CRU_PLL_CON3_SSMOD_DOWNSPREAD_L    3
#define CRU_PLL_CON3_SSMOD_DOWNSPREAD_H    3
#define CRU_PLL_CON3_SSMOD_DIVVAL_L        4
#define CRU_PLL_CON3_SSMOD_DIVVAL_H        7
#define CRU_PLL_CON3_SSMOD_SPREAD_L        8
#define CRU_PLL_CON3_SSMOD_SPREAD_H       12
#define CRU_PLL_CON3_WRITE_MASK_L         16
#define CRU_PLL_CON3_WRITE_MASK_H         31

#define CRU_PLL_CON4_SSMOD_SEL_EXT_WAVE_L  0
#define CRU_PLL_CON4_SSMOD_SEL_EXT_WAVE_H  0
#define CRU_PLL_CON4_SSMOD_EXT_MAXADDR_L   8
#define CRU_PLL_CON4_SSMOD_EXT_MAXADDR_H  15
#define CRU_PLL_CON4_WRITE_MASK_L         16
#define CRU_PLL_CON4_WRITE_MASK_H         31

#define CRU_MODE              CRU_BASE + 0x00a0

#define PLLMUX_FROM_XIN24M    0 
#define PLLMUX_FROM_PLL       1
#define PLLMUX_FROM_RTC32K    2
#define USBPHY480M_MODE_SHIFT 8
#define USBPHY480M_MODE_MASK  3 << USBPHY480M_MODE_SHIFT
#define NPLL_MODE_SHIFT		  6
#define NPLL_MODE_MASK		  3 << NPLL_MODE_SHIFT
#define DPLL_MODE_SHIFT		  4
#define DPLL_MODE_MASK		  3 << DPLL_MODE_SHIFT
#define CPLL_MODE_SHIFT		  2
#define CPLL_MODE_MASK		  3 << CPLL_MODE_SHIFT
#define APLL_MODE_SHIFT		  0
#define APLL_MODE_MASK		  3 << APLL_MODE_SHIFT

/* PLLCON0*/
#define PLL_BP_SHIFT		  15
#define PLL_POSTDIV1_SHIFT	  12
#define PLL_POSTDIV1_MASK	  7 << PLL_POSTDIV1_SHIFT
#define PLL_FBDIV_SHIFT		  0
#define PLL_FBDIV_MASK		  0xfff

/* PLLCON1 */
#define PLL_PDSEL_SHIFT		  15
#define PLL_PD1_SHIFT		  14
#define PLL_PD_SHIFT		  13
#define PLL_PD_MASK		      1 << PLL_PD_SHIFT
#define PLL_DSMPD_SHIFT		  12
#define PLL_DSMPD_MASK		  1 << PLL_DSMPD_SHIFT
#define PLL_LOCK_STATUS_SHIFT 10
#define PLL_LOCK_STATUS_MASK  1 << PLL_LOCK_STATUS_SHIFT
#define PLL_POSTDIV2_SHIFT	  6
#define PLL_POSTDIV2_MASK	  7 << PLL_POSTDIV2_SHIFT
#define PLL_REFDIV_SHIFT	  0
#define PLL_REFDIV_MASK		  0x3f

/* PLLCON2 */
#define PLL_FOUT4PHASEPD_SHIFT  27
#define PLL_FOUTVCOPD_SHIFT	    26
#define PLL_FOUTPOSTDIVPD_SHIFT	25
#define PLL_DACPD_SHIFT		    24
#define PLL_FRAC_DIV	        0xffffff

enum rk3326_clk_type
{
    RK3326_CLK_PLL_APLL = 0, 
    RK3326_CLK_PLL_DPLL,
    RK3326_CLK_PLL_CPLL,
    RK3326_CLK_PLL_NPLL,
    RK3326_CLK_PLL_GPLL,
}; 
typedef int rk3326_cru_pll_type_t; 

rt_uint32_t rk3326_clk_get(rk3326_cru_pll_type_t type); 

#endif /* __DRV_CLK_H__ */ 