/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "rtthread.h"
#include "rtdevice.h"
#include <rthw.h>
#include "board.h" 
#include "drv_uart.h" 

// xboot 可以参考代码 https://github.com/xboot/xboot/blob/master/src/arch/arm64/mach-px30/driver/uart-8250.c
// https://github.com/xboot/xboot/blob/master/src/arch/arm64/mach-px30/sys-uart.c
// 时钟：https://github.com/xboot/xboot/blob/02aa27f707f8a39954127f93b1aee79157244ec2/src/arch/arm64/mach-px30/romdisk/boot/x30.json
// https://www.cnblogs.com/lialong1st/p/8477847.html
// uboot 时钟配置 arch\arm\include\asm\arch-rockchip\cru_px30.h
// uboot lcd common\lcd.c
// drivers\video\drm\rockchip_display.c:
//      755  static int display_logo(struct display_state *state)
//      1040 void rockchip_show_fbbase(ulong fbbase)
//      1270 static int rockchip_display_probe(struct udevice *dev)

// load mmc 1:1 0x300000 logo2.bmp
// bmp info 0x300000
// bmp display 0x300000
// cp 0x300000 0x3DE00000 307200
// fb: 0x3DE00000

#define UART0_BASE 0xFF030000
#define UART1_BASE 0xFF158000
#define UART2_BASE 0xFF160000
#define UART3_BASE 0xFF168000
#define UART4_BASE 0xFF170000
#define UART5_BASE 0xFF178000

#define UART0_IRQ 47 
#define UART1_IRQ 48
#define UART2_IRQ 49
#define UART3_IRQ 50
#define UART4_IRQ 51
#define UART5_IRQ 52

#define UART_RBR   0x0000 // W 0x00000000 Receive Buffer Register
#define UART_THR   0x0000 // W 0x00000000 Transmit Holding Register
#define UART_DLL   0x0000 // W 0x00000000 Divisor Latch (Low)
#define UART_DLH   0x0004 // W 0x00000000 Divisor Latch (High)
#define UART_IER   0x0004 // W 0x00000000 Interrupt Enable Register
#define UART_IIR   0x0008 // W 0x00000001 Interrupt Identification Register
#define UART_FCR   0x0008 // W 0x00000000 FIFO Control Register
#define UART_LCR   0x000c // W 0x00000000 Line Control Register
#define UART_MCR   0x0010 // W 0x00000000 Modem Control Register
#define UART_LSR   0x0014 // W 0x00000060 Line Status Register
#define UART_MSR   0x0018 // W 0x00000000 Modem Status Register
#define UART_SCR   0x001c // W 0x00000000 Scratchpad Register
#define UART_SRBR  0x0030 // W 0x00000000 Shadow Receive Buffer Register
#define UART_STHR  0x006c // W 0x00000000 Shadow Transmit Holding Register
#define UART_FAR   0x0070 // W 0x00000000 FIFO Access Register
#define UART_TFR   0x0074 // W 0x00000000 Transmit FIFO Read
#define UART_RFW   0x0078 // W 0x00000000 Receive FIFO // Write
#define UART_USR   0x007c // W 0x00000006 UART Status Register
#define UART_TFL   0x0080 // W 0x00000000 Transmit FIFO Level
#define UART_RFL   0x0084 // W 0x00000000 Receive FIFO Level
#define UART_SRR   0x0088 // W 0x00000000 Software Reset Register
#define UART_SRTS  0x008c // W 0x00000000 Shadow Request to Send
#define UART_SBCR  0x0090 // W 0x00000000 Shadow Break Control Register
#define UART_SDMAM 0x0094 // W 0x00000000 Shadow DMA Mode
#define UART_SFE   0x0098 // W 0x00000000 Shadow FIFO Enable
#define UART_SRT   0x009c // W 0x00000000 Shadow RCVR Trigger
#define UART_STET  0x00a0 // W 0x00000000 Shadow TX Empty Trigger
#define UART_HTX   0x00a4 // W 0x00000000 Halt TX
#define UART_DMASA 0x00a8 // W 0x00000000 DMA Software Acknowledge
#define UART_CPR   0x00f4 // W 0x00000000 Component Parameter Register
#define UART_UCV   0x00f8 // W 0x3330382a UART Component Version
#define UART_CTR   0x00fc // W 0x44570110 Component Type Register

struct hw_uart_device
{
    rt_uint32_t hw_base;
    rt_uint32_t irqno;
};

static rt_err_t rk3326_uart_init(rt_base_t uart_base, rt_uint32_t baudrate)
{
    // UINT32 uwBaudRate;
    // UINT32 divider;
    // UINT32 remainder;
    // UINT32 fraction;

    // /* First, disable everything */
    // WRITE_UINT32(0x0, uart_base + UART_CR);

    // /* set baud rate */
    // uwBaudRate = 16 * baudrate;
    // divider = CONFIG_UART_CLK_INPUT / uwBaudRate;
    // remainder = CONFIG_UART_CLK_INPUT % uwBaudRate;
    // uwBaudRate = (8 * remainder) / baudrate;
    // fraction = (uwBaudRate >> 1) + (uwBaudRate & 1);

    // WRITE_UINT32(divider, uart_base + UART_IBRD);
    // WRITE_UINT32(fraction, uart_base + UART_FBRD);

    // /* Set the UART to be 8 bits, 1 stop bit, no parity, fifo enabled. */
    // WRITE_UINT32((3 << 5) | (1 << 4), uart_base + UART_LCR_H);

    // /*set the fifo threshold of recv interrupt >= 1/8 full */
    // WRITE_UINT32((2 << 3) | (4 << 0), uart_base + UART_IFLS);

    // /* set nonblock of recv interrupt and recv timeout interrupt */
    // WRITE_UINT32((1 << 4) | (1 << 6), uart_base + UART_IMSC);
    
    // /* enable the UART */
    // WRITE_UINT32((1 << 0) | (1 << 8) | (1 << 9), uart_base + UART_CR);

    return RT_EOK; 
}

#ifdef BSP_USING_UART2
static struct rt_serial_device _serial2;
#endif

static void rt_hw_uart_isr(int irqno, void *param)
{ 
    struct rt_serial_device *serial = NULL;
    // struct hw_uart_device *uart = NULL;

    (void)param;

    rt_interrupt_enter();

    extern void rt_hw_debug_puts(const char *str); 
    rt_hw_debug_puts("rt_hw_uart_isr\n"); 

    if(irqno == UART2_IRQ)
    {
        serial = &_serial2; 
        // uart = (struct hw_uart_device *)serial->parent.user_data;
    }
    
    // CLR RX INR
    // WRITE_UINT32((1 << 4) | (1 << 6), uart->hw_base + UART_CLR);
    rt_hw_serial_isr(serial, RT_SERIAL_EVENT_RX_IND);
    rt_interrupt_leave();
} 

static rt_err_t uart_configure(struct rt_serial_device *serial, struct serial_configure *cfg)
{
    struct hw_uart_device *uart;

    RT_ASSERT(serial != RT_NULL);
    uart = (struct hw_uart_device *)serial->parent.user_data;
    rk3326_uart_init(uart->hw_base, cfg->baud_rate); 
    return RT_EOK; 
}

static rt_err_t uart_control(struct rt_serial_device *serial, int cmd, void *arg)
{
    struct hw_uart_device *uart; 

    RT_ASSERT(serial != RT_NULL);
    uart = (struct hw_uart_device *)serial->parent.user_data;

    switch(cmd)
    {
    case RT_DEVICE_CTRL_CLR_INT:
        /* Disable the UART Interrupt */
        // WRITE_UINT32(0, uart->hw_base + UART_IMSC);
        rt_hw_interrupt_mask(uart->irqno);
        break;

    case RT_DEVICE_CTRL_SET_INT:
        /* Enable the UART Interrupt */
        // WRITE_UINT32((1 << 4) | (1 << 6), uart->hw_base + UART_IMSC);
        rt_hw_interrupt_umask(uart->irqno);
        break;
    }
    return RT_EOK;
}

static int uart_putc(struct rt_serial_device *serial, char c)
{
    struct hw_uart_device *uart;
    
    RT_ASSERT(serial != RT_NULL);
    uart = (struct hw_uart_device *)serial->parent.user_data;
    rt_uint32_t base = uart->hw_base;

    /* Wait for Tx FIFO not full */
    rt_uint32_t status = 0;
    do {
        READ_UINT32(status, base + UART_USR);
    } while ((status & (0x01 << 1)) == 0); 

    WRITE_UINT8(c, base + UART_THR);

    return 1;
}

static int uart_getc(struct rt_serial_device *serial)
{
    rt_uint8_t ch = -1;
    struct hw_uart_device *uart;

    RT_ASSERT(serial != RT_NULL);
    uart = (struct hw_uart_device *)serial->parent.user_data;
    rt_uint32_t base = uart->hw_base;

    rt_uint32_t status;
    READ_UINT32(status, base + UART_USR);
    if ((status & (0x01 << 3)) == 0) 
    {
        return -1; 
    }
    READ_UINT8(ch, base + UART_RBR);

    return ch; 
}

static const struct rt_uart_ops _uart_ops =
{
    uart_configure,
    uart_control,
    uart_putc,
    uart_getc,
};

#ifdef BSP_USING_UART2
/* UART device driver structure */
static struct hw_uart_device _uart2_device =
{
    UART2_BASE,
    UART2_IRQ,
};
#endif

int rt_hw_uart_init(void)
{
    struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;

#ifdef BSP_USING_UART2
    struct hw_uart_device *uart2;
    uart2 = &_uart2_device;

    _serial2.ops    = &_uart_ops;
    _serial2.config = config;

    // #define PIN_74_UART0_RX_FUNC 0x01
    // #define PIN_73_UART0_TX_FUNC 0x01

    // rt_gpio_set_func(73, 1, 0, 0, PIN_73_UART0_TX_FUNC, 1); 
    // rt_gpio_set_func(74, 1, 0, 0, PIN_74_UART0_RX_FUNC, 0); 

    /* register UART2 device */
    rt_hw_serial_register(&_serial2, "uart2",
                          RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX,
                          uart2);
    rt_hw_interrupt_install(_uart2_device.irqno, rt_hw_uart_isr, &_serial2, "uart2");
    rt_hw_interrupt_umask(_uart2_device.irqno); 

    rt_uint32_t status = 0;
    READ_UINT32(status, UART2_BASE + UART_IER);
    rt_kprintf("UART2_BASE + UART_IER = 0x%.8x\n", status); 

    status = 0x01; 
    WRITE_UINT32(status, UART2_BASE + UART_IER); 

    READ_UINT32(status, UART2_BASE + UART_IER);
    rt_kprintf("UART2_BASE + UART_IER = 0x%.8x\n", status); 

#endif 

    return 0; 
}

#define DUMP_UART_REG(base, reg)                     \
do {                                                 \
    rt_uint32_t status = 0x00000000;                 \
    READ_UINT32(status, base + reg);                 \
    rt_kprintf(#reg ":\n");                          \
    rt_kprintf("\t%p 0x%.8x\n", base + reg, status); \
} while(0); 

int rk3326_uart_dump(void)
{
    rt_kprintf("############ rk3326_uart_dump\r\n"); 

    DUMP_UART_REG(UART2_BASE, UART_RBR); 
    DUMP_UART_REG(UART2_BASE, UART_THR); 
    DUMP_UART_REG(UART2_BASE, UART_DLL); 
    DUMP_UART_REG(UART2_BASE, UART_DLH); 
    DUMP_UART_REG(UART2_BASE, UART_IER); 
    DUMP_UART_REG(UART2_BASE, UART_IIR); 
    DUMP_UART_REG(UART2_BASE, UART_FCR); 
    DUMP_UART_REG(UART2_BASE, UART_LCR); 
    DUMP_UART_REG(UART2_BASE, UART_MCR); 
    DUMP_UART_REG(UART2_BASE, UART_LSR); 
    DUMP_UART_REG(UART2_BASE, UART_MSR); 
    DUMP_UART_REG(UART2_BASE, UART_SCR); 
    DUMP_UART_REG(UART2_BASE, UART_SRBR); 
    DUMP_UART_REG(UART2_BASE, UART_STHR); 
    DUMP_UART_REG(UART2_BASE, UART_FAR); 
    DUMP_UART_REG(UART2_BASE, UART_TFR); 
    DUMP_UART_REG(UART2_BASE, UART_RFW); 
    DUMP_UART_REG(UART2_BASE, UART_USR); 
    DUMP_UART_REG(UART2_BASE, UART_TFL); 
    DUMP_UART_REG(UART2_BASE, UART_RFL); 
    DUMP_UART_REG(UART2_BASE, UART_SRR); 
    DUMP_UART_REG(UART2_BASE, UART_SRTS); 
    DUMP_UART_REG(UART2_BASE, UART_SBCR); 
    DUMP_UART_REG(UART2_BASE, UART_SDMAM); 
    DUMP_UART_REG(UART2_BASE, UART_SFE); 
    DUMP_UART_REG(UART2_BASE, UART_SRT); 
    DUMP_UART_REG(UART2_BASE, UART_STET); 
    DUMP_UART_REG(UART2_BASE, UART_HTX); 
    DUMP_UART_REG(UART2_BASE, UART_DMASA); 
    DUMP_UART_REG(UART2_BASE, UART_CPR); 
    DUMP_UART_REG(UART2_BASE, UART_UCV); 
    DUMP_UART_REG(UART2_BASE, UART_CTR); 

    return 0; 
}
MSH_CMD_EXPORT_ALIAS(rk3326_uart_dump, rk3326_uart_dump , rk3326_uart_dump); 
