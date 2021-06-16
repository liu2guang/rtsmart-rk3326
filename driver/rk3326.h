#ifndef __RK3326_H__
#define __RK3326_H__


#define __REG32(x) (*((volatile unsigned int*)((rt_uint64_t)x)))
#define COREMB_INTCTL(n)            __REG32(0xFF210000 + 0x50 + 4*n)  /* Coren Mailboxes Interrupt control */
#define CORETIMER_INTCTL(n)         __REG32(0xFF210000 + 0x40 + n*4)  /* Coren timers Interrupt control */
#define IPI_MAILBOX_INT_MASK        (0x01)

#define CORE_MAILBOX3_SET(n)        __REG32(0xFF210000 + 0x8C + n*0x10)
#define CORE_MAILBOX3_CLEAR(n)      __REG32(0xFF210000 + 0xCC + n*0x10)
#define CORE_MAILBOX2_SET(n)        __REG32(0xFF210000 + 0x88 + n*0x10)
#define CORE_MAILBOX2_CLEAR(n)      __REG32(0xFF210000 + 0xC8 + n*0x10)
#define CORE_MAILBOX1_SET(n)        __REG32(0xFF210000 + 0x84 + n*0x10)
#define CORE_MAILBOX1_CLEAR(n)      __REG32(0xFF210000 + 0xC4 + n*0x10)
#define CORE_MAILBOX0_SET(n)        __REG32(0xFF210000 + 0x80 + n*0x10)
#define CORE_MAILBOX0_CLEAR(n)      __REG32(0xFF210000 + 0xC0 + n*0x10)
#define IPI_MAILBOX_SET             CORE_MAILBOX0_SET
#define IPI_MAILBOX_CLEAR           CORE_MAILBOX0_CLEAR

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

#endif /* __RK3326_H__ */ 
