/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __RK3326_GIC_H__
#define __RK3326_GIC_H__

/* IRQ */ 
#define UART0_IRQ                   47 
#define UART1_IRQ                   48
#define UART2_IRQ                   49
#define UART3_IRQ                   50
#define UART4_IRQ                   51
#define UART5_IRQ                   52

// 0x40, 0x44, 0x48, 0x4c: Core 0~3 Timers interrupt control
#define CORE0_TIMER_IRQ_CTRL        HWREG32(0xFF210000)
#define TIMER_IRQ                   30
#define NON_SECURE_TIMER_IRQ        (1 << 1)

// gic max
#define ARM_GIC_NR_IRQS             (140)
#define INTC_BASE                   (0xFF130000)
#define GIC_V2_DISTRIBUTOR_BASE     (INTC_BASE + 0x1000)
#define GIC_V2_CPU_INTERFACE_BASE   (INTC_BASE + 0x2000)

#define GIC_PL400_DISTRIBUTOR_PPTR  GIC_V2_DISTRIBUTOR_BASE
#define GIC_PL400_CONTROLLER_PPTR   GIC_V2_CPU_INTERFACE_BASE

#endif /* __RK3326_GIC_H__ */ 